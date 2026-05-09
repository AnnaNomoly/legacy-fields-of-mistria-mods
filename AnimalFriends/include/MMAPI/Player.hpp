#pragma once

#include "Bark.hpp"
#include "Core.hpp"
#include "Instance.hpp"
#include "Spell.hpp"

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

	struct Position
	{
		double x = 0.0;
		double y = 0.0;
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
		inline constexpr const char* GML_SCRIPT_CAST_SPELL             = "gml_Script_cast_spell";
		inline constexpr const char* GML_SCRIPT_BARK_EMITTER_EMIT      = "gml_Script_emit@BarkEmitter@BarkEmitter";
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
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return The held item struct as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetHeldItem()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_HELD_ITEM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Returns the item_id of Ari's currently held item.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return The held item ID as an RValue, or undefined if no item or context is available.
	inline YYTK::RValue GetHeldItemId()
	{
		YYTK::RValue held_item = GetHeldItem();
		if (held_item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return held_item.GetMember("item_id");
	}

	/// Returns Ari's current player state ID.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return The current player state ID as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetStateId()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Ari = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::RValue ari = Ari->ToRValue();
		YYTK::RValue fsm = ari.GetMember("fsm");
		YYTK::RValue state = fsm.GetMember("state");
		return state.GetMember("state_id");
	}

	/// Returns true if Ari is currently in the given player state.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param state The player state to check.
	inline bool IsInState(MMAPI::Player::States state)
	{
		YYTK::RValue state_id = GetStateId();
		if (state_id.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		return state_id.ToInt64() == static_cast<int>(state);
	}

	/// Gets Ari's current room position from the live obj_ari instance.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current x/y position, or std::nullopt if the required context is unavailable.
	inline std::optional<MMAPI::Player::Position> GetPosition()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return std::nullopt;
		YYTK::CInstance* Ari = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::RValue x;
		MMAPI::Internal::module_interface->GetBuiltin("x", Ari, NULL_INDEX, x);

		YYTK::RValue y;
		MMAPI::Internal::module_interface->GetBuiltin("y", Ari, NULL_INDEX, y);

		return MMAPI::Player::Position{ x.ToDouble(), y.ToDouble() };
	}

	/// Returns Ari's current health.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current health as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetHealth()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Sets Ari's current health to the given value.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The health value to set.
	inline void SetHealth(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SET_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue health = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &health };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current maximum health.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current maximum health as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetMaxHealth()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_MAX_HEALTH, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Sets Ari's maximum health by writing directly to __ari.base_health.
	/// If Ari's current health exceeds the new maximum, current health is clamped to it.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The new maximum health value.
	inline void SetMaxHealth(int value)
	{
		YYTK::RValue ari = *MMAPI::Internal::global_instance->GetRefMember("__ari");
		*ari.GetRefMember("base_health") = value;

		if (GetHealth().ToDouble() > static_cast<double>(value))
			SetHealth(value);
	}

	/// Adjusts Ari's current health by the given signed amount.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The amount to add to Ari's current health. Negative values reduce health.
	inline void ModifyHealth(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

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
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The amount to add to Ari's current stamina. Negative values reduce stamina.
	inline void ModifyStamina(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_STAMINA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current stamina.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current stamina as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetStamina()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_STAMINA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Adjusts Ari's current gold by the given signed amount.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The amount to add to Ari's current gold. Negative values reduce gold.
	inline void ModifyGold(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_GOLD, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Adjusts Ari's current renown by the given signed amount.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The amount to add to Ari's current renown. Negative values reduce renown.
	inline void ModifyRenown(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_RENOWN, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current mana.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current mana as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetMana()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_MANA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Adjusts Ari's current mana by the given signed amount.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The amount to add to Ari's current mana. Negative values reduce mana.
	inline void ModifyMana(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_MANA, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current essence.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current essence as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetEssence()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_ESSENCE, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Adjusts Ari's current essence by the given signed amount.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param value The amount to add to Ari's current essence. Negative values reduce essence.
	inline void ModifyEssence(int value)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_MODIFY_ESSENCE, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue modifier = value;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &modifier };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Returns Ari's current movement speed.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return Ari's current movement speed as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetMoveSpeed()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_MOVE_SPEED, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Casts the given spell as Ari.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param spell The spell to cast.
	inline void CastSpell(MMAPI::Spell::Ids spell)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_CAST_SPELL, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue spell_id = static_cast<int>(spell);
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &spell_id };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
	}

	/// Emits a bark (voiced dialogue bubble) from Ari via the BarkEmitter.
	/// @attention Requires MMAPI::Bark::Internal::GML_SCRIPT_BARK_EMITTER to be registered via RegisterScriptContext.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param bark_icon The bark icon to display.
	/// @param bark_type The numeric type of the bark (controls display style or trigger behavior).
	inline void EmitBark(MMAPI::Bark::Icons bark_icon, int bark_type)
	{
		const auto& script_refs   = MMAPI::Internal::script_reference_map;
		const auto& instance_refs = MMAPI::Internal::instance_reference_map;
		if (!script_refs.contains(MMAPI::Bark::Internal::GML_SCRIPT_BARK_EMITTER) ||
		    !instance_refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = script_refs.at(MMAPI::Bark::Internal::GML_SCRIPT_BARK_EMITTER)[0];
		YYTK::CInstance* Other = instance_refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_BARK_EMITTER_EMIT, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue id   = static_cast<int>(bark_icon);
		YYTK::RValue type = bark_type;
		YYTK::RValue result;
		YYTK::RValue* args[2] = { &id, &type };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 2, args);
	}

	/// Returns true if Ari has the named cosmetic equipped in the currently selected preset.
	/// @param cosmetic_name The internal cosmetic name to check.
	inline bool HasCosmeticEquipped(std::string cosmetic_name)
	{
		YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
		int preset_index = static_cast<int>(ari.GetMember("preset_index_selected").ToInt64());

		YYTK::RValue presets = ari.GetMember("presets");
		YYTK::RValue preset_buffer = presets.GetMember("__buffer");

		size_t preset_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(preset_buffer, preset_count);
		if (preset_index < 0 || static_cast<size_t>(preset_index) >= preset_count)
			return false;

		YYTK::RValue* selected_preset = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(preset_buffer, static_cast<size_t>(preset_index), selected_preset);

		YYTK::RValue assets = selected_preset->GetMember("assets");
		YYTK::RValue asset_buffer = assets.GetMember("__buffer");

		size_t asset_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(asset_buffer, asset_count);

		for (size_t i = 0; i < asset_count; i++)
		{
			YYTK::RValue* equipped_cosmetic = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(asset_buffer, i, equipped_cosmetic);

			if (equipped_cosmetic->GetMember("name").ToString() == cosmetic_name)
				return true;
		}

		return false;
	}
}
