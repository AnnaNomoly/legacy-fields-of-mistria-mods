#include "../../utils/Utils.h"

RValue& GmlScriptModifyHealthCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Cache the original value for blocks that read before any modification.
	// Stoneskin and Phalanx modify *Arguments[0] in sequence, so they read the live value.
	const int amount = Arguments[0]->ToInt64();
	const bool is_damage = amount < 0;

	// Afflatus Misery (Cleric Set Bonus) — accumulates incoming damage into a pool.
	if (is_damage && CountEquippedClassArmor()[Classes::CLERIC] == 5 && AriCurrentGmRoomIsDungeonFloor())
		class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY] += abs(amount);

	// Frailty — counts incoming hits to scale damage.
	if (is_damage && active_floor_enchantments.contains(FloorEnchantments::FRAILTY) && !is_fumigate_tracked_interval && !is_deep_wounds_tracked_interval) // Need to check for Fumigate and Deep Wounds since Frailty is a Group 2 enchant
		frailty_hit_counter++;

	// Deep Wounds — accumulates incoming damage into a pool.
	if (is_damage && active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS) && !is_deep_wounds_tracked_interval)
		deep_wounds_damage_pool += abs(amount);

	// Stoneskin — absorbs damage with a shield pool, modifies *Arguments[0] with remainder.
	if (is_damage && active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && stoneskin_shield_amount > 0)
	{
		int damage = abs(amount);
		if (stoneskin_shield_amount >= damage)
		{
			stoneskin_shield_amount -= damage;
			damage = 0;
		}
		else
		{
			damage -= stoneskin_shield_amount;
			stoneskin_shield_amount = 0;
		}
		*Arguments[0] = -1 * damage;
	}

	// Phalanx — reduces remaining damage by 20% (reads live value after Stoneskin).
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::PHALANX))
	{
		int damage = abs(Arguments[0]->ToInt64());
		int modifier = damage * 20 / 100;
		*Arguments[0] = -1 * (damage - modifier);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_HEALTH));
	original(Self, Other, Result, ArgumentCount, Arguments);

	// Flee (Rogue Set Bonus) — triggers a speed buff when Ari drops to or below 30% HP.
	if (Arguments[0]->ToInt64() < 0 && CountEquippedClassArmor()[Classes::ROGUE] >= 1 && AriCurrentGmRoomIsDungeonFloor() && class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] == 0)
	{
		int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
		int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

		if (current_health <= max_health * 30 / 100)
		{
			int unified_time = GetUnifiedTime(script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][0], script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][1]).ToInt64();
			RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["mine_time"], 2.0, unified_time, unified_time + 900); // 15m duration
			class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] = 1;
		}
	}

	return Result;
}
