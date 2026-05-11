#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Anchor
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_ON_BEGIN_STEP = "gml_Script_on_begin_step@Anchor@Anchor";

		using BeforeBeginStepCallback = void(*)();
		inline BeforeBeginStepCallback before_begin_step_callback = nullptr;

		inline YYTK::RValue& GmlScriptBeforeBeginStepCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_begin_step_callback)
				before_begin_step_callback();

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_ON_BEGIN_STEP
				)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterBeforeBeginStepHook(BeforeBeginStepCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_ON_BEGIN_STEP,
				reinterpret_cast<PVOID>(GmlScriptBeforeBeginStepCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_begin_step_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Activates Anchor hooks. Installs the Anchor on_begin_step hook so registered callbacks
	/// run before the game's per-frame physics tick. Safe to call before any Hooks::* registration —
	/// the callback no-ops until a user callback is bound.
	/// @return AURIE_SUCCESS if the hook is installed (or already was); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_ON_BEGIN_STEP,
			reinterpret_cast<PVOID>(Internal::GmlScriptBeforeBeginStepCallback)
		);
	}

	namespace Hooks
	{
		/// Registers a callback that runs before each Anchor `on_begin_step` — the game's per-frame
		/// world physics tick. Fires roughly 60 times per second.
		/// The callback receives no context; use it for per-frame state updates (e.g. applying sprite
		/// overrides to tracked instances).
		/// @param callback A parameterless function called every frame before the game's begin-step.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeBeginStep(Internal::BeforeBeginStepCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_begin_step_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Anchor::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterBeforeBeginStepHook(callback);
		}
	}
}
