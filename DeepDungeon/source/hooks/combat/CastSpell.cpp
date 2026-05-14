#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

// MMAPI Spell::Hooks::BeforeSpellCast callback.
void BeforeSpellCast(MMAPI::Spell::BeforeSpellCastContext& ctx)
{
	const bool in_dungeon = AriCurrentGmRoomIsDungeonFloor();
	if (!in_dungeon)
		return;

	const MMAPI::Spell::Ids spell = ctx.GetSpell();
	const auto armor_set_bonuses = GetArmorSetBonuses();

	// Mana Font (Mage, 5 pieces) — counts every spell cast except Flood recasts.
	if (armor_set_bonuses.mage.ManaFont()
		&& (spell != MMAPI::Spell::Ids::SummonRain
			|| class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] == -1))
	{
		class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT]++;
	}

	// Set bonuses that fully replace the original spell — cancel and skip original.
	if (spell == MMAPI::Spell::Ids::FullRestore)
	{
		// Dark Seal (Dark Knight, 3+ pieces) — Siphon Life: drain 15% of each monster's base HP.
		if (armor_set_bonuses.dark_knight.DarkSeal())
		{
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				{
					RValue monster_id = monster->GetMember("monster_id");
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (IsNumeric(monster_id) && monster_id.ToInt64() != static_cast<int>(MMAPI::Monster::Ids::Mimic) && IsNumeric(hit_points) && std::isfinite(hit_points) && hit_points > 0)
					{
						double default_hit_points = monster->GetMember("__deep_dungeon__default_hit_points").ToDouble();
						double siphon_life_amount = std::trunc(default_hit_points * 0.15);
						hit_points -= siphon_life_amount;

						*monster->GetRefMember("hit_points") = hit_points;
						int max_health = ModifyMaxHealth(siphon_life_amount);
						MMAPI::Player::ModifyHealth(siphon_life_amount);

						int current_health = MMAPI::Player::GetHealth().ToInt64();
						MMAPI::VitalsMenu::SetMaxHealth(max_health);
						MMAPI::VitalsMenu::SetHealth(current_health, max_health);
					}
				}
			}
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] = 1;
			ctx.Cancel();
			return;
		}
		// Elemental Seal (Mage, 3+ pieces) — applies weapon enchantment based on current seal element.
		else if (armor_set_bonuses.mage.ElementalSeal())
		{
			ElementalSealEffects effect = *magic_enum::enum_cast<ElementalSealEffects>(
				class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);

			switch (effect)
			{
			case ElementalSealEffects::FIRE:
				class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] = 1;
				MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::FireSword), 1.25, 1, MMAPI::StatusEffect::InfiniteDuration);
				break;
			case ElementalSealEffects::ICE:
				class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] = 1;
				MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::IceSword), 1.0, 1, MMAPI::StatusEffect::InfiniteDuration);
				break;
			case ElementalSealEffects::VENOM:
				class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] = 1;
				MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::VenomSword), 1.0, 1, MMAPI::StatusEffect::InfiniteDuration);
				break;
			}
			ctx.Cancel();
			return;
		}
		// Predict (Oracle, 5 pieces) — cost set to zero by ModifySpellCosts.
		else if (armor_set_bonuses.oracle.FullSet())
		{
			MMAPI::Game::CreateNotification(false, PREDICT_SPELL_CAST_NOTIFICATION_KEY);
			class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 1;
			ctx.Cancel();
			return;
		}
	}
	else if (spell == MMAPI::Spell::Ids::Growth)
	{
		// Quake (Mage, 4+ pieces) — deals 90% of max HP to Ari and all monsters.
		if (armor_set_bonuses.mage.Quake())
		{
			int ari_max_health = ari_resource_to_value_map[AriResources::MAX_HEALTH];
			int ari_quake_damage = std::trunc(ari_max_health * 0.9);
			MMAPI::Player::ModifyHealth(ari_quake_damage * -1);

			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				{
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points > 0)
					{
						int monster_quake_damage = std::trunc(hit_points * 0.9);
						*monster->GetRefMember("hit_points") = std::max(0.0, hit_points - monster_quake_damage);
					}
				}
			}

			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] = 1;
			ctx.Cancel();
			return;
		}
		// Condemn (Oracle, 5 pieces) — queues a random offering for the next floor.
		else if (armor_set_bonuses.oracle.FullSet())
		{
			static thread_local pcg32 random_generator([] {
				std::random_device rd;
				return pcg32(
					(static_cast<uint64_t>(rd()) << 32) | rd(),
					(static_cast<uint64_t>(rd()) << 32) | rd()
				);
			}());
			std::uniform_int_distribution<size_t> random_offering_distribution(0, magic_enum::enum_count<Offerings>() - 1);

			queued_offerings.insert(magic_enum::enum_value<Offerings>(random_offering_distribution(random_generator)));
			offering_chance_occurred = true;
			class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 1;
			MMAPI::Text::PlayConversation("Conversations/Mods/Deep Dungeon/condemn");
			ctx.Cancel();
			return;
		}
	}
}

// MMAPI Spell::Hooks::AfterSpellCast callback.
void AfterSpellCast(MMAPI::Spell::AfterSpellCastContext& ctx)
{
	const bool in_dungeon = AriCurrentGmRoomIsDungeonFloor();
	if (!in_dungeon)
		return;

	const MMAPI::Spell::Ids spell = ctx.GetSpell();
	const auto armor_set_bonuses = GetArmorSetBonuses();

	// Divine Seal (Cleric, 3+ pieces) — full_restore clears all floor enchantments.
	if (spell == MMAPI::Spell::Ids::FullRestore && armor_set_bonuses.cleric.DivineSeal())
	{
		if (active_floor_enchantments.contains(FloorEnchantments::FEY))
		{
			ModifySpellCosts(true, true);
			MMAPI::StatusEffect::Cancel(MMAPI::StatusEffect::Ids::Fairy);
		}

		active_floor_enchantments.clear();
		active_sigils.insert(Sigils::SERENITY);
		MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::Fairy), RValue(), 1, MMAPI::StatusEffect::InfiniteDuration);

		// Undo Blessed (Oracle Set Bonus) HP inflation from this floor.
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] > 0)
		{
			int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
			int adjusted_max_health = max_health - class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED];

			SetMaxHealth(adjusted_max_health);
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			MMAPI::VitalsMenu::SetMaxHealth(adjusted_max_health);
			MMAPI::VitalsMenu::SetHealth(current_health, adjusted_max_health);
			class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = 0;
		}

		// Undo HP Penalty floor enchantment.
		if (hp_penalty_amount > -1)
		{
			int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
			int adjusted_max_health = max_health + hp_penalty_amount;
			hp_penalty_amount = -1;

			SetMaxHealth(adjusted_max_health);
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			MMAPI::VitalsMenu::SetMaxHealth(adjusted_max_health);
			MMAPI::VitalsMenu::SetHealth(current_health, adjusted_max_health);
		}
	}

	// Flood (Mage, 2+ pieces) — records cast time to enforce recast cooldown.
	if (spell == MMAPI::Spell::Ids::SummonRain && armor_set_bonuses.mage.Flood()
		&& class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] <= 0)
	{
		class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = current_time_in_seconds;
	}
}
