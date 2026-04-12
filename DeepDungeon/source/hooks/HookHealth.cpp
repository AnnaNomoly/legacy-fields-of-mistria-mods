#include "../utils/Utils.h"

RValue& GmlScriptCancelStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Protection
	if (active_sigils.contains(Sigils::PROTECTION) && Arguments[0]->ToInt64() == status_effect_name_to_id_map["guardians_shield"])
	{
		active_sigils.erase(Sigils::PROTECTION);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	// Redemption
	if (active_sigils.contains(Sigils::REDEMPTION) && Arguments[0]->ToInt64() == status_effect_name_to_id_map["fairy"])
	{
		active_sigils.erase(Sigils::REDEMPTION);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CANCEL_STATUS_EFFECT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptRegisterStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (AriCurrentGmRoomIsDungeonFloor() && held_item_id == item_name_to_id_map[SUSTAINING_POTION_NAME])
	{
		if (Arguments[0]->ToInt64() == status_effect_name_to_id_map["restorative"])
		{
			int finish = Arguments[3]->ToInt64();
			*Arguments[3] = finish - static_cast<int>(7200 * (1 - configuration.sustaining_potion_duration_modifier)); // Modify the duration of Restoration
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_REGISTER_STATUS_EFFECT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptVitalsMenuSetMaxHealthCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH))
		script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptModifyHealthCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Afflatus Misery (Cleric Set Bonus)
	if (Arguments[0]->ToInt64() < 0 && CountEquippedClassArmor()[Classes::CLERIC] == 5 && AriCurrentGmRoomIsDungeonFloor())
		class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY] += abs(Arguments[0]->ToInt64());

	// Frailty
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::FRAILTY) && !is_fumigate_tracked_interval && !is_deep_wounds_tracked_interval) // Need to check for Fumigate and Deep Wounds since Frailty is a Group 2 enchant
		frailty_hit_counter += 1;

	// Deep Wounds
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS) && !is_deep_wounds_tracked_interval)
		deep_wounds_damage_pool += abs(Arguments[0]->ToInt64());

	// Stoneskin
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && stoneskin_shield_amount > 0)
	{
		int damage = abs(Arguments[0]->ToInt64());
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

	// Phalanx
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::PHALANX))
	{
		int damage = abs(Arguments[0]->ToInt64());
		int modifier = damage * 20 / 100;
		*Arguments[0] = -1 * (damage - modifier);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_HEALTH));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Flee (Rogue Set Bonus)
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

RValue& GmlScriptModifyStaminaCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Exhaustion
	if (active_floor_enchantments.contains(FloorEnchantments::EXHAUSTION))
	{
		if (Arguments[0]->ToDouble() < 0)
		{
			double modified_stamina_cost = Arguments[0]->ToDouble() * 2;
			*Arguments[0] = modified_stamina_cost;
		}
	}

	// Spirit Surge
	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE) && Arguments[0]->ToDouble() < 0)
		*Arguments[0] = 0;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_STAMINA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptStatusEffectManagerUpdateCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE))
		script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
