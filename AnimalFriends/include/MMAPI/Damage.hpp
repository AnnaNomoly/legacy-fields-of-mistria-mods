#pragma once

#include "Core.hpp"
#include "Engine.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Damage
{
	struct Context
	{
		YYTK::RValue* damage_data = nullptr;

		/// Returns true if the damage packet is available.
		bool IsValid() const
		{
			return damage_data != nullptr;
		}

		/// Gets the raw damage packet passed to the game's damage script.
		YYTK::RValue GetRawDamageData() const
		{
			if (!damage_data)
				return {};

			return *damage_data;
		}

		/// Gets the current damage amount.
		double GetAmount() const
		{
			if (!damage_data || !MMAPI::Engine::StructVariableExists(*damage_data, "damage"))
				return 0.0;

			return damage_data->GetMember("damage").ToDouble();
		}

		/// Sets the damage amount.
		void SetAmount(double amount)
		{
			if (!damage_data)
				return;

			MMAPI::Engine::StructVariableSet(*damage_data, "damage", amount);
		}

		/// Returns true if the damage packet is marked as a critical hit.
		bool IsCritical() const
		{
			if (!damage_data || !MMAPI::Engine::StructVariableExists(*damage_data, "critical"))
				return false;

			return damage_data->GetMember("critical").ToBoolean();
		}

		/// Sets whether the damage packet is marked as a critical hit.
		void SetCritical(bool critical)
		{
			if (!damage_data)
				return;

			MMAPI::Engine::StructVariableSet(*damage_data, "critical", critical);
		}

		/// Gets the target value from the damage packet.
		YYTK::RValue GetTarget() const
		{
			if (!damage_data || !MMAPI::Engine::StructVariableExists(*damage_data, "target"))
				return {};

			return damage_data->GetMember("target");
		}

		/// Returns true if the damage target appears to be Ari.
		bool IsTargetAri() const
		{
			YYTK::RValue target = GetTarget();
			if (!MMAPI::Engine::IsNumeric(target))
				return false;

			return target.ToInt64() == 1;
		}

		/// Returns true if the damage amount is zero.
		bool IsMiss() const
		{
			return GetAmount() == 0.0;
		}
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_DAMAGE = "gml_Script_damage@gml_Object_obj_damage_receiver_Create_0";

		using OnDamageCallback = void(*)(MMAPI::Damage::Context&);

		inline OnDamageCallback on_damage_callback = nullptr;

		inline YYTK::RValue& GmlScriptDamageCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Arguments && ArgumentCount >= 1 && Arguments[0] && Arguments[0]->m_Kind == YYTK::VALUE_OBJECT)
			{
				MMAPI::Damage::Context context{ Arguments[0] };
				on_damage_callback(context);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_DAMAGE
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline Aurie::AurieStatus RegisterDamageHook(OnDamageCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_DAMAGE,
				reinterpret_cast<PVOID>(GmlScriptDamageCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			on_damage_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	namespace Hooks
	{
		/// Registers a callback that can modify a damage packet before the game applies it.
		/// @attention Requires MMAPI to be initialized with the AurieModule pointer via Initialize.
		/// @param callback A function called with a mutable damage context.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus OnDamage(Internal::OnDamageCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::on_damage_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterDamageHook(callback);
		}
	}
}
