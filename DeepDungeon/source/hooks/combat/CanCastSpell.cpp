#include "../../utils/Utils.h"

using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

// MMAPI Spell::Hooks::AfterCanCastSpell callback.
void AfterCanCastSpell(MMAPI::Spell::CanCastSpellContext& ctx)
{
	if (!AriCurrentGmRoomIsDungeonFloor())
		return;

	// Amnesia and boss fight restrictions block all spells regardless of set bonuses.
	if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA))
	{
		ctx.SetResult(false);
		return;
	}
	if (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
	{
		ctx.SetResult(false);
		return;
	}

	const MMAPI::Spell::Ids spell = ctx.GetSpell();
	auto armor_set_bonuses = GetArmorSetBonuses();

	if (spell == MMAPI::Spell::Ids::FullRestore)
	{
		// Dark Seal (Dark Knight, 3+ pieces)
		if (armor_set_bonuses.dark_knight.DarkSeal())
		{
			bool on_cooldown = class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0;
			ctx.SetResult(!on_cooldown && CanAffordSpell(MMAPI::Spell::Ids::FullRestore));
		}
		// Elemental Seal (Mage, 3+ pieces)
		else if (armor_set_bonuses.mage.ElementalSeal())
		{
			auto& mage = class_name_to_set_bonus_effect_value_map[Classes::MAGE];
			bool en_active = mage[ManagedSetBonuses::ENFIRE] > 0
				|| mage[ManagedSetBonuses::ENBLIZZARD] > 0
				|| mage[ManagedSetBonuses::ENPOISON] > 0;
			ctx.SetResult(!en_active && CanAffordSpell(MMAPI::Spell::Ids::FullRestore));
		}
		// Predict (Oracle, 5 pieces) — cost set to 0 by ModifySpellCosts, so no mana check needed.
		else if (armor_set_bonuses.oracle.FullSet())
		{
			ctx.SetResult(class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 0);
		}
	}
	else if (spell == MMAPI::Spell::Ids::SummonRain)
	{
		// Flood (Mage, 2+ pieces) — blocks recast while active.
		if (armor_set_bonuses.mage.Flood() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0)
			ctx.SetResult(false);
	}
	else if (spell == MMAPI::Spell::Ids::Growth)
	{
		// Quake (Mage, 4+ pieces)
		if (armor_set_bonuses.mage.Quake())
		{
			bool on_cooldown = class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0;
			ctx.SetResult(!on_cooldown && CanAffordSpell(MMAPI::Spell::Ids::Growth));
		}
		// Condemn (Oracle, 5 pieces)
		else if (armor_set_bonuses.oracle.FullSet())
		{
			bool used = offering_chance_occurred || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0;
			ctx.SetResult(!used);
		}
	}
}
