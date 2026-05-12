#pragma once

#include "Core.hpp"
#include "Hook.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Input
{
	struct TakePressContext
	{
		int  m_input_id = 0;
		bool m_result   = false;

		/// Returns the input ID being checked (the first argument the game passed to take_press).
		int GetInputId() const { return m_input_id; }

		/// Returns whether the input was reported as pressed this frame, as computed by the game.
		bool GetResult() const { return m_result; }

		/// Overrides whether the game considers the input pressed. Set false to swallow the press.
		void SetResult(bool pressed) { m_result = pressed; }
	};

	struct CheckValueContext
	{
		int m_input_id = 0;

		/// Returns the input ID the game is about to check.
		int GetInputId() const { return m_input_id; }

		/// Overrides the input ID passed to the game's check_value script. Useful for input remapping
		/// (e.g. swapping direction inputs to implement a confusion effect).
		void SetInputId(int input_id) { m_input_id = input_id; }
	};

	namespace Internal
	{
		inline bool enabled = false;

		inline constexpr const char* GML_SCRIPT_TAKE_PRESS  = "gml_Script_take_press@Input@Input";
		inline constexpr const char* GML_SCRIPT_CHECK_VALUE = "gml_Script_check_value@Input@Input";

		using AfterTakePressCallback   = void(*)(MMAPI::Input::TakePressContext&);
		using BeforeCheckValueCallback = void(*)(MMAPI::Input::CheckValueContext&);

		inline AfterTakePressCallback   after_take_press_callback    = nullptr;
		inline BeforeCheckValueCallback before_check_value_callback  = nullptr;

		inline YYTK::RValue& GmlScriptAfterTakePressCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_TAKE_PRESS)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			if (after_take_press_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Input::TakePressContext context{
					static_cast<int>(Arguments[0]->ToInt64()),
					Result.ToBoolean()
				};
				after_take_press_callback(context);
				Result = context.m_result;
			}

			return Result;
		}

		inline YYTK::RValue& GmlScriptBeforeCheckValueCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_check_value_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Input::CheckValueContext context{
					static_cast<int>(Arguments[0]->ToInt64())
				};
				before_check_value_callback(context);
				*Arguments[0] = context.m_input_id;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CHECK_VALUE)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}
	}

	/// Activates Input hooks. Installs the take_press and check_value hooks so registered callbacks
	/// can observe input-press results and remap inputs before the game evaluates them. Safe to call
	/// before any Hooks::* registration — each callback no-ops until a user callback is bound.
	/// The existing pull-style helpers (KeyboardCheckPressed, GamepadIsConnected, etc.) do not require Enable().
	/// @return Status::Success if the hooks are installed (or already were); otherwise a failure status.
	inline MMAPI::Status Enable()
	{
		if (Internal::enabled)
			return MMAPI::Status::Success;

		MMAPI::Log::Debug("MMAPI::Input::Enable() called");

		MMAPI::Status status = MMAPI::Internal::InstallScriptHooks({
			{ Internal::GML_SCRIPT_TAKE_PRESS,  reinterpret_cast<PVOID>(Internal::GmlScriptAfterTakePressCallback) },
			{ Internal::GML_SCRIPT_CHECK_VALUE, reinterpret_cast<PVOID>(Internal::GmlScriptBeforeCheckValueCallback) },
		});
		if (!MMAPI::IsSuccess(status))
			return status;

		Internal::enabled = true;
		return MMAPI::Status::Success;
	}

	/// Returns true if the keyboard key was pressed this frame.
	/// @param key The GameMaker virtual key code to check.
	inline bool KeyboardCheckPressed(int key)
	{
		MMAPI_REQUIRE_ENABLED("Input", false);
		YYTK::RValue pressed = MMAPI::Internal::module_interface->CallBuiltin("keyboard_check_pressed", { key });
		return pressed.ToBoolean();
	}

	/// Returns true if a gamepad is connected in the given slot.
	/// @param gamepad_slot The gamepad slot to check.
	inline bool GamepadIsConnected(int gamepad_slot)
	{
		MMAPI_REQUIRE_ENABLED("Input", false);
		YYTK::RValue connected = MMAPI::Internal::module_interface->CallBuiltin("gamepad_is_connected", { gamepad_slot });
		return connected.ToBoolean();
	}

	/// Returns the first connected gamepad slot, or -1 if no gamepad is connected.
	inline int GetFirstConnectedGamepadSlot()
	{
		MMAPI_REQUIRE_ENABLED("Input", -1);
		for (int slot = 0; slot < 12; slot++)
		{
			if (GamepadIsConnected(slot))
				return slot;
		}

		return -1;
	}

	/// Returns true if the gamepad button was pressed this frame.
	/// @param gamepad_slot The gamepad slot to check.
	/// @param button The GameMaker gamepad button constant to check.
	inline bool GamepadButtonCheckPressed(int gamepad_slot, int button)
	{
		MMAPI_REQUIRE_ENABLED("Input", false);
		YYTK::RValue pressed = MMAPI::Internal::module_interface->CallBuiltin("gamepad_button_check_pressed", { gamepad_slot, button });
		return pressed.ToBoolean();
	}

	namespace Hooks
	{
		/// Registers a callback that runs after the game's `take_press` script.
		/// Read `ctx.GetInputId()` to identify the input being checked, `ctx.GetResult()` to see whether
		/// the game considered it pressed, and `ctx.SetResult(false)` to swallow the press.
		/// @param callback A function called with a mutable `MMAPI::Input::TakePressContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status AfterTakePress(Internal::AfterTakePressCallback callback)
		{
			MMAPI::Status status = MMAPI::Input::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Input::AfterTakePress",
				Internal::after_take_press_callback,
				callback
			);
		}

		/// Registers a callback that runs before the game's `check_value` script.
		/// Read `ctx.GetInputId()` to see which input the game is about to evaluate, and
		/// `ctx.SetInputId(int)` to remap it (e.g. swap direction inputs to implement a confusion effect).
		/// @param callback A function called with a mutable `MMAPI::Input::CheckValueContext`.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status BeforeCheckValue(Internal::BeforeCheckValueCallback callback)
		{
			MMAPI::Status status = MMAPI::Input::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Input::BeforeCheckValue",
				Internal::before_check_value_callback,
				callback
			);
		}
	}
}
