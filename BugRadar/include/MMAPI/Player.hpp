#pragma once

#include "Bark.hpp"
#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"

#include <optional>
#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Player
{
	/// Source: globalInstance.__player_state__
	enum class States : int
	{
		Default        = 0,
		Celebrate      = 1,
		Cutscene       = 2,
		Knockback      = 3,
		Jump           = 4,
		DownSmash      = 5,
		Hurt           = 6,
		Electrocute    = 7,
		AnimateAndThen = 8,
		Swim           = 9,
		ThrowItem      = 10,
		HoldToUse      = 11,
		Tool           = 12,
		Sword          = 13,
		Fishing        = 14,
		WhirlPool      = 15,
		Spell          = 16,
		Diving         = 17,
		Underwater     = 18,
		Resurface      = 19,
		Pathfind       = 20,
		Dummy          = 21,
		MountDefault   = 22,
		MountJump      = 23
	};

	/// Total number of enumerators in States. Iterating [0, StateCount) covers every States value.
	inline constexpr int StateCount = 24;

	/// Invokes fn with every States value, in ascending order.
	template <typename Fn>
	inline void ForEachState(Fn fn)
	{
		for (int i = 0; i < StateCount; ++i)
			fn(static_cast<States>(i));
	}

	struct Position
	{
		double x = 0.0;
		double y = 0.0;
	};

	struct MoveSpeedContext
	{
		double m_result = 0.0;
		double m_modifier_sum = 0.0;
		bool m_has_override = false;
		double m_override_value = 0.0;

		/// Returns the movement speed calculated by the game before any MMAPI modifications.
		double GetResult() const { return m_result; }

		/// Adds a signed value to the game's calculated movement speed. Accumulates across multiple callbacks.
		void AddModifier(double value) { m_modifier_sum += value; }

		/// Overrides the final movement speed to the given value, ignoring any accumulated modifiers.
		/// If multiple callbacks call SetOverride, the last one wins.
		void SetOverride(double value)
		{
			m_has_override = true;
			m_override_value = value;
		}
	};

	struct ModifyHealthChangeContext
	{
		double m_amount = 0.0;

		/// Returns the health change amount. Negative values are damage; positive values are healing.
		double GetAmount() const { return m_amount; }

		/// Overrides the health change amount passed to the game.
		void SetAmount(double amount) { m_amount = amount; }
	};

	struct ModifyStaminaChangeContext
	{
		double m_amount = 0.0;

		/// Returns the stamina change amount. Negative values are costs; positive values are recovery.
		double GetAmount() const { return m_amount; }

		/// Overrides the stamina change amount passed to the game.
		void SetAmount(double amount) { m_amount = amount; }
	};

	struct ModifyManaChangeContext
	{
		double m_amount = 0.0;

		/// Returns the mana change amount. Negative values are costs; positive values are recovery.
		double GetAmount() const { return m_amount; }

		/// Overrides the mana change amount passed to the game.
		void SetAmount(double amount) { m_amount = amount; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_HEALTH             = "gml_Script_get_health@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_SET_HEALTH             = "gml_Script_set_health@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_MODIFY_HEALTH          = "gml_Script_modify_health@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_GET_MAX_HEALTH         = "gml_Script_get_max_health@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_GET_STAMINA            = "gml_Script_get_stamina@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_MODIFY_STAMINA         = "gml_Script_modify_stamina@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_MODIFY_GOLD            = "gml_Script_modify_gold@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_MODIFY_RENOWN          = "gml_Script_modify_renown@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_GET_MANA               = "gml_Script_get_mana@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_MODIFY_MANA            = "gml_Script_modify_mana@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_GET_ESSENCE            = "gml_Script_get_essence@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_MODIFY_ESSENCE         = "gml_Script_modify_essence@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_GET_MOVE_SPEED         = "gml_Script_get_move_speed@Ari@Ari";
		inline constexpr const char* GML_SCRIPT_HELD_ITEM              = "gml_Script_held_item@Ari@Ari";
		using AfterMoveSpeedCallback     = void(*)(MMAPI::Player::MoveSpeedContext&);
		using BeforeHealthChangeCallback  = void(*)(MMAPI::Player::ModifyHealthChangeContext&);
		using BeforeStaminaChangeCallback = void(*)(MMAPI::Player::ModifyStaminaChangeContext&);
		using BeforeManaChangeCallback    = void(*)(MMAPI::Player::ModifyManaChangeContext&);

		inline AfterMoveSpeedCallback     after_move_speed_callback     = nullptr;
		inline BeforeHealthChangeCallback  before_health_change_callback  = nullptr;
		inline BeforeStaminaChangeCallback before_stamina_change_callback = nullptr;
		inline BeforeManaChangeCallback    before_mana_change_callback    = nullptr;

		inline YYTK::RValue GetStateId()
		{
			const auto& refs = MMAPI::Internal::instance_reference_map;
			if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
				return {};
			YYTK::CInstance* Ari = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];

			YYTK::RValue ari = Ari->ToRValue();
			YYTK::RValue fsm = ari.GetMember("fsm");
			YYTK::RValue state = fsm.GetMember("state");
			return state.GetMember("state_id");
		}

		inline YYTK::RValue& GmlScriptGetMoveSpeedCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_GET_MOVE_SPEED
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);

			if (after_move_speed_callback && MMAPI::Engine::IsNumeric(Result))
			{
				MMAPI::Player::MoveSpeedContext context{ Result.ToDouble() };
				after_move_speed_callback(context);
				Result = context.m_has_override ? context.m_override_value : context.m_result + context.m_modifier_sum;
			}

			return Result;
		}

		inline Aurie::AurieStatus RegisterMoveSpeedHook(AfterMoveSpeedCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_GET_MOVE_SPEED,
				reinterpret_cast<PVOID>(GmlScriptGetMoveSpeedCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_move_speed_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptModifyHealthCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_health_change_callback && Arguments && ArgumentCount >= 1 && Arguments[0] && MMAPI::Engine::IsNumeric(*Arguments[0]))
			{
				MMAPI::Player::ModifyHealthChangeContext context{ Arguments[0]->ToDouble() };
				before_health_change_callback(context);
				*Arguments[0] = context.m_amount;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_MODIFY_HEALTH
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline YYTK::RValue& GmlScriptModifyStaminaCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_stamina_change_callback && Arguments && ArgumentCount >= 1 && Arguments[0] && MMAPI::Engine::IsNumeric(*Arguments[0]))
			{
				MMAPI::Player::ModifyStaminaChangeContext context{ Arguments[0]->ToDouble() };
				before_stamina_change_callback(context);
				*Arguments[0] = context.m_amount;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_MODIFY_STAMINA
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline YYTK::RValue& GmlScriptModifyManaCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_mana_change_callback && Arguments && ArgumentCount >= 1 && Arguments[0] && MMAPI::Engine::IsNumeric(*Arguments[0]))
			{
				MMAPI::Player::ModifyManaChangeContext context{ Arguments[0]->ToDouble() };
				before_mana_change_callback(context);
				*Arguments[0] = context.m_amount;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_MODIFY_MANA
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline Aurie::AurieStatus RegisterHealthChangeHook(BeforeHealthChangeCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_MODIFY_HEALTH,
				reinterpret_cast<PVOID>(GmlScriptModifyHealthCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_health_change_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterStaminaChangeHook(BeforeStaminaChangeCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_MODIFY_STAMINA,
				reinterpret_cast<PVOID>(GmlScriptModifyStaminaCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_stamina_change_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterManaChangeHook(BeforeManaChangeCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_MODIFY_MANA,
				reinterpret_cast<PVOID>(GmlScriptModifyManaCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_mana_change_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Returns Ari's current facing direction in degrees, read from globalInstance.__ari.dir.
	/// Values are GameMaker direction degrees: 0 = right, 90 = up, 180 = left, 270 = down.
	/// @return Ari's facing direction in degrees.
	inline double GetFacingDirection()
	{
		YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
		return ari.GetMember("dir").ToDouble();
	}

	/// Returns the number of invulnerability hits remaining for Ari.
	/// Each hit absorbed while this value is greater than zero decrements it by one instead of dealing damage.
	/// @return The current value of MMAPI::Internal::global_instance.__ari.invulnerable_hits as an RValue.
	inline YYTK::RValue GetInvulnerabilityHits()
	{
		YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
		return ari.GetMember("invulnerable_hits");
	}

	/// Sets the number of invulnerability hits for Ari.
	/// Each hit absorbed while this value is greater than zero decrements it by one instead of dealing damage.
	/// @param value The number of invulnerability hits to assign.
	inline void SetInvulnerabilityHits(int value)
	{
		YYTK::RValue ari = *MMAPI::Internal::global_instance->GetRefMember("__ari");
		*ari.GetRefMember("invulnerable_hits") = value;
	}

	/// Adjusts the number of invulnerability hits for Ari by the given signed amount.
	/// Each hit absorbed while this value is greater than zero decrements it by one instead of dealing damage.
	/// @param value The amount to add to Ari's current invulnerability hits. Negative values reduce the count.
	inline void ModifyInvulnerabilityHits(int value)
	{
		YYTK::RValue ari = *MMAPI::Internal::global_instance->GetRefMember("__ari");
		int current = static_cast<int>(ari.GetMember("invulnerable_hits").ToInt64());
		*ari.GetRefMember("invulnerable_hits") = current + value;
	}

	/// Returns Ari's currently held item.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return The held item struct as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetHeldItem()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_HELD_ITEM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Returns the item_id of Ari's currently held item.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return The held item ID as an RValue, or undefined if no item or context is available.
	inline YYTK::RValue GetHeldItemId()
	{
		YYTK::RValue held_item = GetHeldItem();
		if (held_item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return held_item.GetMember("item_id");
	}

	/// Gets Ari's current player state.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param state Ari's current player state.
	/// @return True if the player state was resolved, false if the required context is unavailable.
	inline bool TryGetState(MMAPI::Player::States& state)
	{
		YYTK::RValue state_id = Internal::GetStateId();
		if (state_id.m_Kind == YYTK::VALUE_UNDEFINED || state_id.m_Kind == YYTK::VALUE_UNSET)
			return false;

		int player_state_id = static_cast<int>(state_id.ToInt64());
		if (player_state_id < static_cast<int>(MMAPI::Player::States::Default) ||
		    player_state_id > static_cast<int>(MMAPI::Player::States::MountJump))
			return false;

		state = static_cast<MMAPI::Player::States>(player_state_id);
		return true;
	}

	/// Returns true if Ari is currently in the given player state.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param state The player state to check.
	inline bool IsInState(MMAPI::Player::States state)
	{
		MMAPI::Player::States current_state;
		if (!TryGetState(current_state))
			return false;

		return current_state == state;
	}

	/// Gets Ari's current room position from the live obj_ari instance.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current x/y position, or std::nullopt if the required context is unavailable.
	inline std::optional<MMAPI::Player::Position> GetPosition()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return std::nullopt;
		YYTK::CInstance* Ari = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];

		YYTK::RValue x;
		MMAPI::Internal::module_interface->GetBuiltin("x", Ari, NULL_INDEX, x);

		YYTK::RValue y;
		MMAPI::Internal::module_interface->GetBuiltin("y", Ari, NULL_INDEX, y);

		return MMAPI::Player::Position{ x.ToDouble(), y.ToDouble() };
	}

	/// Returns Ari's current health.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current health as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetHealth()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Sets Ari's current health to the given value.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The health value to set.
	inline void SetHealth(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SET_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue health = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &health };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current maximum health.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current maximum health as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetMaxHealth()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_MAX_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Sets Ari's maximum health by writing directly to __ari.base_health.
	/// If Ari's current health exceeds the new maximum, current health is clamped to it.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The new maximum health value.
	inline void SetMaxHealth(int value)
	{
		YYTK::RValue ari = *MMAPI::Internal::global_instance->GetRefMember("__ari");
		*ari.GetRefMember("base_health") = value;

		if (GetHealth().ToDouble() > static_cast<double>(value))
			SetHealth(value);
	}

	/// Adjusts Ari's current health by the given signed amount.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The amount to add to Ari's current health. Negative values reduce health.
	inline void ModifyHealth(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Adjusts Ari's maximum health by the given signed amount, writing directly to __ari.base_health.
	/// Does not clamp current health — call SetMaxHealth if clamping is required.
	/// @param value The amount to add to Ari's maximum health. Negative values reduce the maximum.
	/// @return The resulting maximum health value as an RValue.
	inline YYTK::RValue ModifyMaxHealth(int value)
	{
		YYTK::RValue ari = *MMAPI::Internal::global_instance->GetRefMember("__ari");
		int max_health = static_cast<int>(ari.GetMember("base_health").ToInt64()) + value;
		*ari.GetRefMember("base_health") = max_health;
		return max_health;
	}

	/// Adjusts Ari's current stamina by the given signed amount.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The amount to add to Ari's current stamina. Negative values reduce stamina.
	inline void ModifyStamina(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_STAMINA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current stamina.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current stamina as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetStamina()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_STAMINA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Adjusts Ari's current gold by the given signed amount.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The amount to add to Ari's current gold. Negative values reduce gold.
	inline void ModifyGold(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_GOLD, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Adjusts Ari's current renown by the given signed amount.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The amount to add to Ari's current renown. Negative values reduce renown.
	inline void ModifyRenown(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_RENOWN, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current mana.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current mana as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetMana()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_MANA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Adjusts Ari's current mana by the given signed amount.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The amount to add to Ari's current mana. Negative values reduce mana.
	inline void ModifyMana(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_MANA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current essence.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current essence as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetEssence()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_ESSENCE, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Adjusts Ari's current essence by the given signed amount.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @param value The amount to add to Ari's current essence. Negative values reduce essence.
	inline void ModifyEssence(int value)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_ESSENCE, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current movement speed.
	/// @attention Requires MMAPI::Player::Enable() to have been called.
	/// @return Ari's current movement speed as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetMoveSpeed()
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_MOVE_SPEED, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Activates Player utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		Aurie::AurieStatus status = MMAPI::Bark::Enable();
		if (!Aurie::AurieSuccess(status))
			return status;
		return MMAPI::Instance::Enable();
	}

	namespace Hooks
	{
		/// Registers a callback that can modify Ari's movement speed after the game calculates it.
		/// Use ctx.AddModifier(value) to add a signed offset, or ctx.SetOverride(value) to force a specific speed.
		/// If multiple callbacks call SetOverride, the last registered callback wins.
		/// @param callback A function called with a mutable move speed context after the game calculates it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterMoveSpeed(Internal::AfterMoveSpeedCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_move_speed_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Player::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterMoveSpeedHook(callback);
		}

		/// Registers a callback that can modify the amount passed to the game's modify_health script.
		/// Use ctx.SetAmount(value) to change the health delta before the game applies it.
		/// @param callback A function called with a mutable health change context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeHealthChange(Internal::BeforeHealthChangeCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_health_change_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Player::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterHealthChangeHook(callback);
		}

		/// Registers a callback that can modify the amount passed to the game's modify_stamina script.
		/// Use ctx.SetAmount(value) to change the stamina delta before the game applies it.
		/// @param callback A function called with a mutable stamina change context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeStaminaChange(Internal::BeforeStaminaChangeCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_stamina_change_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Player::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterStaminaChangeHook(callback);
		}

		/// Registers a callback that can modify the amount passed to the game's modify_mana script.
		/// Use ctx.SetAmount(value) to change the mana delta before the game applies it.
		/// @param callback A function called with a mutable mana change context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeManaChange(Internal::BeforeManaChangeCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_mana_change_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Player::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterManaChangeHook(callback);
		}
	}

}
