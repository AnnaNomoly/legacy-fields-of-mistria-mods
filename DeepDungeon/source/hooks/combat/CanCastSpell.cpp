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
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (!AriCurrentGmRoomIsDungeonFloor())
		return Result;

	// Amnesia and boss fight restrictions block all spells regardless of set bonuses.
	if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA))
	{
		Result = 0;
		return Result;
	}
	if (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
	{
		Result = 0;
		return Result;
	}

	const int spell_id = Arguments[0]->ToInt64();
	const int mana = ari_resource_to_value_map[AriResources::MANA];
	const bool fey_active = active_floor_enchantments.contains(FloorEnchantments::FEY);
	auto armor_counts = CountEquippedClassArmor();

	auto CanAffordSpell = [&](const std::string& spell_name) -> bool {
		int cost = spell_id_to_default_cost_map[spell_name_to_id_map[spell_name]];
		return mana >= (fey_active ? cost / 2 : cost);
	};

	if (spell_id == spell_name_to_id_map["full_restore"])
	{
		// Dark Seal (Dark Knight, 3+ pieces) — takes priority over Mage/Oracle full_restore replacements.
		if (armor_counts[Classes::DARK_KNIGHT] >= 3)
		{
			bool on_cooldown = class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0;
			Result = (!on_cooldown && CanAffordSpell("full_restore")) ? 1 : 0;
		}
		// Elemental Seal (Mage, 3+ pieces)
		else if (armor_counts[Classes::MAGE] >= 3)
		{
			auto& mage = class_name_to_set_bonus_effect_value_map[Classes::MAGE];
			bool en_active = mage[ManagedSetBonuses::ENFIRE] > 0
				|| mage[ManagedSetBonuses::ENBLIZZARD] > 0
				|| mage[ManagedSetBonuses::ENPOISON] > 0;
			Result = (!en_active && CanAffordSpell("full_restore")) ? 1 : 0;
		}
		// Predict (Oracle, 5 pieces) — cost set to 0 by ModifySpellCosts, so no mana check needed.
		else if (armor_counts[Classes::ORACLE] >= 5)
		{
			Result = (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 0) ? 1 : 0;
		}
	}
	else if (spell_id == spell_name_to_id_map["summon_rain"])
	{
		// Flood (Mage, 2+ pieces) — blocks recast while active.
		if (armor_counts[Classes::MAGE] >= 2 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0)
			Result = 0;
	}
	else if (spell_id == spell_name_to_id_map["growth"])
	{
		// Quake (Mage, 4+ pieces)
		if (armor_counts[Classes::MAGE] >= 4)
		{
			bool on_cooldown = class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0;
			Result = (!on_cooldown && CanAffordSpell("growth")) ? 1 : 0;
		}
		// Condemn (Oracle, 5 pieces)
		else if (armor_counts[Classes::ORACLE] >= 5)
		{
			bool used = offering_chance_occurred || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0;
			Result = used ? 0 : 1;
		}
	}

	return Result;
}
