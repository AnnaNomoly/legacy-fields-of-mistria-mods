#pragma once

#include "Calendar.hpp"
#include "Core.hpp"
#include "Engine.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::StatusEffect
{
	inline constexpr int InfiniteDuration = 2147483647;

	/// Source: globalInstance.__status_effect_id__
	enum class Ids : int
	{
		Restorative     = 0,
		Speedy          = 1,
		Fairy           = 2,
		GuardiansShield = 3,
		MineTime        = 4,
		SlimeDash       = 5,
		ShrineBoon      = 6,
		KillHaste       = 7,
		FlameBreath     = 8,
		StackingSpeed   = 9,
		Venomous        = 10,
		VenomSword      = 11,
		Frozen          = 12,
		IceSword        = 13,
		FireSword       = 14,
		SureStrike      = 15,
		SacredLight     = 16
	};

	struct RegisterStatusEffectContext
	{
		int    m_status_id = 0;
		double m_amount    = 0.0;
		int    m_start     = 0;
		int    m_finish    = 0;
		bool   m_cancelled = false;

		MMAPI::StatusEffect::Ids GetStatusEffect() const { return static_cast<MMAPI::StatusEffect::Ids>(m_status_id); }
		double GetAmount() const { return m_amount; }
		void SetAmount(double amount) { m_amount = amount; }
		int GetStart() const { return m_start; }
		int GetFinish() const { return m_finish; }
		void SetFinish(int finish) { m_finish = finish; }
		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	struct CancelStatusEffectContext
	{
		int  m_status_id = 0;
		bool m_cancelled = false;

		MMAPI::StatusEffect::Ids GetStatusEffect() const { return static_cast<MMAPI::StatusEffect::Ids>(m_status_id); }
		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE = "gml_Script_update@StatusEffectManager@StatusEffectManager";
		inline constexpr const char* GML_SCRIPT_CANCEL_STATUS_EFFECT         = "gml_Script_cancel@StatusEffectManager@StatusEffectManager";
		inline constexpr const char* GML_SCRIPT_REGISTER_STATUS_EFFECT       = "gml_Script_register@StatusEffectManager@StatusEffectManager";

		// Live StatusEffectManager Self/Other, latched per-tick from the update hook.
		// Used by TryGetStatusEffectManagerContext for callers outside any hook frame.
		inline YYTK::CInstance* status_effect_manager_self  = nullptr;
		inline YYTK::CInstance* status_effect_manager_other = nullptr;

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by StatusEffect::Enable().
		inline void ClearStatusEffectManagerOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			status_effect_manager_self  = nullptr;
			status_effect_manager_other = nullptr;
		}

		inline YYTK::RValue& StatusEffectManagerUpdateContextCallback(IN YYTK::CInstance* Self, IN YYTK::CInstance* Other, OUT YYTK::RValue& Result, IN int ArgumentCount, IN YYTK::RValue** Arguments)
		{
			// Refresh the latched pair every tick; the manager is a long-lived singleton but the captured pointer
			// is invalidated on return-to-title (see ClearStatusEffectManagerOnReturnToTitle).
			status_effect_manager_self  = Self;
			status_effect_manager_other = Other;

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE));
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		/// Resolves the StatusEffectManager's GML calling context, latched from the most recent update tick.
		/// Cleared automatically when the game returns to the title menu.
		/// @return True if a StatusEffectManager tick has been observed this session, false otherwise.
		inline bool TryGetStatusEffectManagerContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			if (!status_effect_manager_self)
				return false;
			Self  = status_effect_manager_self;
			Other = status_effect_manager_other;
			return true;
		}

		inline constexpr double FireSwordAmount   = 1.25;
		inline constexpr double IceSwordAmount    = 1.0;
		inline constexpr double VenomSwordAmount  = 1.0;
		inline constexpr double MineTimeAmount    = 2.0;
		inline constexpr double SacredLightAmount = 0.0;

		inline YYTK::RValue GetDefaultAmount(MMAPI::StatusEffect::Ids status_effect)
		{
			switch (status_effect)
			{
				case MMAPI::StatusEffect::Ids::FireSword:   return FireSwordAmount;
				case MMAPI::StatusEffect::Ids::IceSword:    return IceSwordAmount;
				case MMAPI::StatusEffect::Ids::VenomSword:  return VenomSwordAmount;
				case MMAPI::StatusEffect::Ids::MineTime:    return MineTimeAmount;
				case MMAPI::StatusEffect::Ids::SacredLight: return SacredLightAmount;
				default:                                     return {};
			}
		}

		using BeforeRegisterStatusEffectCallback = void(*)(MMAPI::StatusEffect::RegisterStatusEffectContext&);
		using BeforeCancelStatusEffectCallback   = void(*)(MMAPI::StatusEffect::CancelStatusEffectContext&);

		inline BeforeRegisterStatusEffectCallback before_register_status_effect_callback = nullptr;
		inline BeforeCancelStatusEffectCallback   before_cancel_status_effect_callback   = nullptr;

		inline void RegisterById(int status_effect_id, YYTK::RValue amount, int start, int finish)
		{
			YYTK::CInstance* Self  = nullptr;
			YYTK::CInstance* Other = nullptr;
			if (!TryGetStatusEffectManagerContext(Self, Other))
				return;

			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_REGISTER_STATUS_EFFECT, reinterpret_cast<PVOID*>(&gml_script));

			YYTK::RValue id     = status_effect_id;
			YYTK::RValue st     = start;
			YYTK::RValue fn     = finish;
			YYTK::RValue result;
			YYTK::RValue* args[4] = { &id, &amount, &st, &fn };
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 4, args);
		}

		inline void CancelById(int status_effect_id)
		{
			YYTK::CInstance* Self  = nullptr;
			YYTK::CInstance* Other = nullptr;
			if (!TryGetStatusEffectManagerContext(Self, Other))
				return;

			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_CANCEL_STATUS_EFFECT, reinterpret_cast<PVOID*>(&gml_script));

			YYTK::RValue id = status_effect_id;
			YYTK::RValue result;
			YYTK::RValue* args[1] = { &id };
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		}

		inline YYTK::RValue GetActiveEffectById(int status_effect_id)
		{
			YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
			YYTK::RValue status_effects = ari.GetMember("status_effects");
			YYTK::RValue effects = status_effects.GetMember("effects");
			YYTK::RValue inner = effects.GetMember("inner");

			std::string id = std::to_string(status_effect_id);
			if (!MMAPI::Engine::StructVariableExists(inner, id.c_str()))
				return {};

			return inner.GetMember(id);
		}

		inline YYTK::RValue& GmlScriptRegisterStatusEffectCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_register_status_effect_callback && Arguments && ArgumentCount >= 4)
			{
				MMAPI::StatusEffect::RegisterStatusEffectContext context{
					static_cast<int>(Arguments[0]->ToInt64()),
					Arguments[1]->ToDouble(),
					static_cast<int>(Arguments[2]->ToInt64()),
					static_cast<int>(Arguments[3]->ToInt64())
				};
				before_register_status_effect_callback(context);

				if (context.m_cancelled)
					return Result;

				*Arguments[1] = YYTK::RValue(context.m_amount);
				*Arguments[3] = YYTK::RValue(context.m_finish);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_REGISTER_STATUS_EFFECT)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterStatusEffectHook(BeforeRegisterStatusEffectCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_REGISTER_STATUS_EFFECT,
				reinterpret_cast<PVOID>(GmlScriptRegisterStatusEffectCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_register_status_effect_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptCancelStatusEffectCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_cancel_status_effect_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::StatusEffect::CancelStatusEffectContext context{ static_cast<int>(Arguments[0]->ToInt64()) };
				before_cancel_status_effect_callback(context);

				if (context.m_cancelled)
					return Result;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CANCEL_STATUS_EFFECT)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterCancelStatusEffectHook(BeforeCancelStatusEffectCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_CANCEL_STATUS_EFFECT,
				reinterpret_cast<PVOID>(GmlScriptCancelStatusEffectCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_cancel_status_effect_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Activates StatusEffect utility functions that directly call game scripts.
	/// Installs the manager update hook so the live StatusEffectManager Self/Other are latched for
	/// TryGetStatusEffectManagerContext (cleared on return-to-title via the setup_main_screen pub/sub).
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		Aurie::AurieStatus status = MMAPI::Calendar::Enable();
		if (!Aurie::AurieSuccess(status))
			return status;

		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearStatusEffectManagerOnReturnToTitle);

		return MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN,        reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE,    reinterpret_cast<PVOID>(Internal::StatusEffectManagerUpdateContextCallback) },
		});
	}

	/// Registers a persistent status effect on Ari.
	/// @attention Requires MMAPI::StatusEffect::Enable() to have been called.
	/// @param status_effect The status effect to register.
	/// @return True if the effect was registered, false if the required context is unavailable.
	inline bool RegisterPersistent(MMAPI::StatusEffect::Ids status_effect)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetStatusEffectManagerContext(Self, Other))
			return false;

		Internal::RegisterById(static_cast<int>(status_effect), Internal::GetDefaultAmount(status_effect), 1, MMAPI::StatusEffect::InfiniteDuration);
		return true;
	}

	/// Registers a status effect on Ari for the given duration.
	/// @attention Requires MMAPI::StatusEffect::Enable() to have been called.
	/// @param status_effect The status effect to register.
	/// @param duration The duration of the status effect in unified game-time seconds.
	/// @return True if the effect was registered, false if the required context is unavailable.
	inline bool RegisterForDuration(MMAPI::StatusEffect::Ids status_effect, int duration)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetStatusEffectManagerContext(Self, Other))
			return false;

		YYTK::RValue unified_time = MMAPI::Calendar::GetUnifiedTime();
		if (unified_time.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		int start = static_cast<int>(unified_time.ToInt64());
		Internal::RegisterById(static_cast<int>(status_effect), Internal::GetDefaultAmount(status_effect), start, start + duration);
		return true;
	}

	/// Cancels an active status effect on Ari.
	/// @attention Requires MMAPI::StatusEffect::Enable() to have been called.
	/// @param status_effect The status effect to cancel.
	inline void Cancel(MMAPI::StatusEffect::Ids status_effect)
	{
		Internal::CancelById(static_cast<int>(status_effect));
	}

	/// Cancels all known status effects on Ari.
	/// @attention Requires MMAPI::StatusEffect::Enable() to have been called.
	inline void CancelAll()
	{
		for (int i = 0; i <= static_cast<int>(MMAPI::StatusEffect::Ids::SacredLight); i++)
			Internal::CancelById(i);
	}

	/// Gets Ari's active status effect data for the given status effect.
	/// @param status_effect The status effect to retrieve.
	/// @return The active status effect struct, or undefined if it is not active.
	inline YYTK::RValue GetActive(MMAPI::StatusEffect::Ids status_effect)
	{
		return Internal::GetActiveEffectById(static_cast<int>(status_effect));
	}

	/// Returns true if Ari currently has the given status effect active.
	/// @param status_effect The status effect to check.
	inline bool IsActive(MMAPI::StatusEffect::Ids status_effect)
	{
		return GetActive(status_effect).m_Kind == YYTK::VALUE_OBJECT;
	}

	namespace Hooks
	{
		/// Registers a callback that runs before the game registers a status effect on Ari.
		/// Use ctx.SetAmount() to modify the effect's magnitude, ctx.SetFinish() to adjust its end time, or ctx.Cancel() to prevent registration entirely.
		/// @param callback A function called with a mutable register context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeRegisterStatusEffect(Internal::BeforeRegisterStatusEffectCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_register_status_effect_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::StatusEffect::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterStatusEffectHook(callback);
		}

		/// Registers a callback that runs before the game cancels a status effect on Ari.
		/// Call ctx.Cancel() to prevent the status effect from being removed.
		/// @param callback A function called with a mutable cancel context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeCancelStatusEffect(Internal::BeforeCancelStatusEffectCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_cancel_status_effect_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::StatusEffect::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterCancelStatusEffectHook(callback);
		}
	}
}
