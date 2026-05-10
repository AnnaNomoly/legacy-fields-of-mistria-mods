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
		inline constexpr const char* GML_SCRIPT_CANCEL_STATUS_EFFECT   = "gml_Script_cancel@StatusEffectManager@StatusEffectManager";
		inline constexpr const char* GML_SCRIPT_REGISTER_STATUS_EFFECT = "gml_Script_register@StatusEffectManager@StatusEffectManager";

		using OnRegisterStatusEffectCallback = void(*)(MMAPI::StatusEffect::RegisterStatusEffectContext&);
		using OnCancelStatusEffectCallback   = void(*)(MMAPI::StatusEffect::CancelStatusEffectContext&);

		inline OnRegisterStatusEffectCallback on_register_status_effect_callback = nullptr;
		inline OnCancelStatusEffectCallback   on_cancel_status_effect_callback   = nullptr;

		inline void RegisterById(int status_effect_id, YYTK::RValue amount, int start, int finish)
		{
			const auto& refs = MMAPI::Internal::script_reference_map;
			if (!refs.contains(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE))
				return;
			YYTK::CInstance* Self  = refs.at(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE)[0];
			YYTK::CInstance* Other = refs.at(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE)[1];

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
			const auto& refs = MMAPI::Internal::script_reference_map;
			if (!refs.contains(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE))
				return;
			YYTK::CInstance* Self  = refs.at(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE)[0];
			YYTK::CInstance* Other = refs.at(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE)[1];

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
			if (Arguments && ArgumentCount >= 4)
			{
				MMAPI::StatusEffect::RegisterStatusEffectContext context{
					static_cast<int>(Arguments[0]->ToInt64()),
					Arguments[1]->ToDouble(),
					static_cast<int>(Arguments[2]->ToInt64()),
					static_cast<int>(Arguments[3]->ToInt64())
				};
				on_register_status_effect_callback(context);

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

		inline Aurie::AurieStatus RegisterStatusEffectHook(OnRegisterStatusEffectCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_REGISTER_STATUS_EFFECT,
				reinterpret_cast<PVOID>(GmlScriptRegisterStatusEffectCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			on_register_status_effect_callback = callback;
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
			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::StatusEffect::CancelStatusEffectContext context{ static_cast<int>(Arguments[0]->ToInt64()) };
				on_cancel_status_effect_callback(context);

				if (context.m_cancelled)
					return Result;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CANCEL_STATUS_EFFECT)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterCancelStatusEffectHook(OnCancelStatusEffectCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_CANCEL_STATUS_EFFECT,
				reinterpret_cast<PVOID>(GmlScriptCancelStatusEffectCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			on_cancel_status_effect_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Registers a persistent status effect on Ari.
	/// @attention Requires MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE to be registered via RegisterScriptContext.
	/// @param status_effect The status effect to register.
	/// @param amount The magnitude of the status effect.
	inline void RegisterPersistent(MMAPI::StatusEffect::Ids status_effect, YYTK::RValue amount = YYTK::RValue())
	{
		Internal::RegisterById(static_cast<int>(status_effect), amount, 1, MMAPI::StatusEffect::InfiniteDuration);
	}

	/// Registers a status effect on Ari for the given duration.
	/// @attention Requires MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE to be registered via RegisterScriptContext.
	/// @attention Requires MMAPI::Calendar::Internal::GML_SCRIPT_GET_UNIFIED_TIME to be registered via RegisterScriptContext.
	/// @param status_effect The status effect to register.
	/// @param amount The magnitude of the status effect.
	/// @param duration The duration of the status effect in unified game-time seconds.
	inline void RegisterForDuration(MMAPI::StatusEffect::Ids status_effect, YYTK::RValue amount, int duration)
	{
		YYTK::RValue unified_time = MMAPI::Calendar::GetUnifiedTime();
		if (unified_time.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		int start = static_cast<int>(unified_time.ToInt64());
		Internal::RegisterById(static_cast<int>(status_effect), amount, start, start + duration);
	}

	/// Cancels an active status effect on Ari.
	/// @attention Requires MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE to be registered via RegisterScriptContext.
	/// @param status_effect The status effect to cancel.
	inline void Cancel(MMAPI::StatusEffect::Ids status_effect)
	{
		Internal::CancelById(static_cast<int>(status_effect));
	}

	/// Cancels all known status effects on Ari.
	/// @attention Requires MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE to be registered via RegisterScriptContext.
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
		/// @attention Requires MMAPI to be initialized with the AurieModule pointer via Initialize.
		/// @param callback A function called with a mutable register context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus OnRegisterStatusEffect(Internal::OnRegisterStatusEffectCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::on_register_status_effect_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterStatusEffectHook(callback);
		}

		/// Registers a callback that runs before the game cancels a status effect on Ari.
		/// Call ctx.Cancel() to prevent the status effect from being removed.
		/// @attention Requires MMAPI to be initialized with the AurieModule pointer via Initialize.
		/// @param callback A function called with a mutable cancel context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus OnCancelStatusEffect(Internal::OnCancelStatusEffectCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::on_cancel_status_effect_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterCancelStatusEffectHook(callback);
		}
	}
}
