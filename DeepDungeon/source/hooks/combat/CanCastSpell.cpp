#include "../../utils/Utils.h"

RValue& GmlScriptCanCastSpellCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CAN_CAST_SPELL));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Amnesia
	if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA))
		Result = 0;
	// Boss Fights
	else if (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
		Result = 0;
	// Dark Seal (Dark Knight Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0)
			Result = 0;
		else if (active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]] / 2))
			Result = 0;
		else if (!active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]]))
			Result = 0;
		else
			Result = 1;
	}
	// Flood (Mage Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["summon_rain"] && CountEquippedClassArmor()[Classes::MAGE] >= 2 && AriCurrentGmRoomIsDungeonFloor() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0)
		Result = 0;
	// Elemental Seal (Mage Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::MAGE] >= 3 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] > 0 || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] > 0 || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] > 0)
			Result = 0;
		else if (active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]] / 2))
			Result = 0;
		else if (!active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]]))
			Result = 0;
		else
			Result = 1;
	}
	// Quake (Mage Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::MAGE] >= 4 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0)
			Result = 0;
		else if (active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["growth"]] / 2))
			Result = 0;
		else if (!active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["growth"]]))
			Result = 0;
		else
			Result = 1;
	}
	// Predict (Oracle Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] > 0)
			Result = 0;
		else
			Result = 1;
	}
	// Condemn (Oracle Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (offering_chance_occurred || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0)
			Result = 0;
		else
			Result = 1;
	}

	return Result;
}
