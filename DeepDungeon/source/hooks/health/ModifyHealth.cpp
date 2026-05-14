#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

// MMAPI Player::Hooks::BeforeHealthChange callback.
// Applies dungeon damage modifiers (Stoneskin / Phalanx / Frailty / Deep Wounds / Afflatus Misery)
// before the game's modify_health script runs.
void BeforeHealthChange(MMAPI::Player::BeforeHealthChangeContext& ctx)
{
	const double amount = ctx.GetAmount();
	const bool is_damage = amount < 0;

	// Afflatus Misery (Cleric Set Bonus) — accumulates incoming damage into a pool.
	if (is_damage && GetArmorSetBonuses().cleric.AfflatusMisery() && AriCurrentGmRoomIsDungeonFloor())
		class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY] += static_cast<int>(std::abs(amount));

	// Frailty — counts incoming hits to scale damage.
	if (is_damage && active_floor_enchantments.contains(FloorEnchantments::FRAILTY) && !is_fumigate_tracked_interval && !is_deep_wounds_tracked_interval) // Need to check for Fumigate and Deep Wounds since Frailty is a Group 2 enchant
		frailty_hit_counter++;

	// Deep Wounds — accumulates incoming damage into a pool.
	if (is_damage && active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS) && !is_deep_wounds_tracked_interval)
		deep_wounds_damage_pool += static_cast<int>(std::abs(amount));

	double mutable_amount = amount;

	// Stoneskin — absorbs damage with a shield pool.
	if (is_damage && active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && stoneskin_shield_amount > 0)
	{
		int damage = static_cast<int>(std::abs(mutable_amount));
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
		mutable_amount = -1.0 * damage;
	}

	// Phalanx — reduces remaining damage by 20% (reads live value after Stoneskin).
	if (mutable_amount < 0 && active_floor_enchantments.contains(FloorEnchantments::PHALANX))
	{
		int damage = static_cast<int>(std::abs(mutable_amount));
		int modifier = damage * 20 / 100;
		mutable_amount = -1.0 * (damage - modifier);
	}

	if (mutable_amount != amount)
		ctx.SetAmount(mutable_amount);
}

// MMAPI Player::Hooks::AfterHealthChange callback.
// Triggers the Flee (Rogue Set Bonus) speed buff when Ari drops to or below 30% HP.
void AfterHealthChange(MMAPI::Player::AfterHealthChangeContext& ctx)
{
	if (ctx.GetAmount() >= 0)
		return;
	if (!GetArmorSetBonuses().rogue.Flee())
		return;
	if (!AriCurrentGmRoomIsDungeonFloor())
		return;
	if (class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] != 0)
		return;

	int current_health = MMAPI::Player::GetHealth().ToInt64();
	int max_health = MMAPI::Player::GetMaxHealth().ToInt64();

	if (current_health <= max_health * 30 / 100)
	{
		int unified_time = MMAPI::Calendar::GetUnifiedTime().ToInt64();
		MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::MineTime), 2.0, unified_time, unified_time + 900); // 15m duration
		class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] = 1;
	}
}
