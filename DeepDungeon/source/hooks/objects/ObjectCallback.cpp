#include "../../utils/Utils.h"
#include "../../patterns/MonsterPatterns.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

static void CheckForWhirlpool(CInstance* self)
{
	RValue ari = self->ToRValue();
	if (!StructVariableExists(ari, "fsm")) return;

	RValue fsm = ari.GetMember("fsm");
	if (!StructVariableExists(fsm, "state")) return;

	RValue state = fsm.GetMember("state");
	if (!StructVariableExists(state, "state_id")) return;
	if (state.GetMember("state_id").ToInt64() == static_cast<int>(MMAPI::Player::States::WhirlPool)) // TODO: Replace this debug block with the one-line return version after testing
	{
		in_whirl_pool = true;
		return;
	}

	in_whirl_pool = false;
}

// MMAPI Instance::Hooks::OnObjectCall("obj_ari", ...) callback.
// MMAPI's dispatcher already null-checks self/m_Object/m_Name and skips paused-game ticks.
void OnObjAriCall(CInstance* self)
{
	{
		CInstance* ari_instance = global_instance->GetRefMember("__ari")->ToInstance();

		RValue x;
		g_ModuleInterface->GetBuiltin("x", self, NULL_INDEX, x);
		ari_x = x.ToDouble();

		RValue y;
		g_ModuleInterface->GetBuiltin("y", self, NULL_INDEX, y);
		ari_y = y.ToDouble();

		// Progression Mode Biome Cleared Rewards
		if (!is_challenge_mode && Config::config.disable_dungeon_lift && drop_biome_reward && ari_x != 0 && ari_y != 0)
		{
			// Upper Mines
			if (ari_current_gm_room == "rm_water_seal")
			{
				drop_biome_reward = false;
				MMAPI::Item::Drop(item_name_to_id_map[CURSED_CHESTPIECE_NAME], ari_x, ari_y);
				MMAPI::Item::Drop(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], ari_x, ari_y);
			}
			// Tide Caverns
			else if (ari_current_gm_room == "rm_earth_seal")
			{
				drop_biome_reward = false;
				MMAPI::Item::Drop(item_name_to_id_map[CURSED_HELMET_NAME], ari_x, ari_y);
				MMAPI::Item::Drop(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], ari_x, ari_y);
			}
			// Deep Earth
			else if (ari_current_gm_room == "rm_fire_seal")
			{
				drop_biome_reward = false;
				MMAPI::Item::Drop(item_name_to_id_map[CURSED_GLOVES_NAME], ari_x, ari_y);
				MMAPI::Item::Drop(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], ari_x, ari_y);
			}
			// Lava Caves
			else if (ari_current_gm_room == "rm_ruins_seal")
			{
				drop_biome_reward = false;
				MMAPI::Item::Drop(item_name_to_id_map[CURSED_PANTS_NAME], ari_x, ari_y);
				MMAPI::Item::Drop(item_name_to_id_map[RUINS_KEY_F80_NAME], ari_x, ari_y);
			}
			// Ruins
			else if (ari_current_gm_room == "rm_seridias_chamber")
			{
				drop_biome_reward = false;
				MMAPI::Item::Drop(item_name_to_id_map[CURSED_BOOTS_NAME], ari_x, ari_y);
				MMAPI::Item::Drop(item_name_to_id_map[RUINS_KEY_F100_NAME], ari_x, ari_y);
			}
		}

		// Floor Traps
		if (active_traps.contains(Traps::EXPLODING))
		{
			// Apply damage to Ari
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			int penalty = current_health * Config::config.exploding_trap_current_health_damage_percent / 100;
			MMAPI::Player::SetHealth(current_health - penalty);

			// Apply damage to monsters
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "move"))
				{
					RValue monster_x;
					RValue monster_y;
					g_ModuleInterface->GetBuiltin("x", monster, NULL_INDEX, monster_x);
					g_ModuleInterface->GetBuiltin("y", monster, NULL_INDEX, monster_y);

					double distance = GetDistance(active_traps[Traps::EXPLODING].first, active_traps[Traps::EXPLODING].second, monster_x.ToInt64(), monster_y.ToInt64());
					if (distance <= 32)
					{
						double hit_points = monster->GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							int monster_hp_penalty = std::trunc(hit_points * Config::config.exploding_trap_current_health_damage_percent / 100);
							*monster->GetRefMember("hit_points") = std::max(0.0, hit_points - monster_hp_penalty);

							if (StructVariableExists(monster, "monster_id"))
							{
								RValue monster_id = monster->GetMember("monster_id");
								if (IsNumeric(monster_id))
									g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - The Exploding Trap damaged the monster: %s", MOD_NAME, MOD_VERSION, MMAPI::Monster::GetInternalName(static_cast<MMAPI::Monster::Ids>(monster_id.ToInt64())).c_str());
							}
						}
					}
				}
			}

			active_traps.erase(Traps::EXPLODING);
		}

		// Apply offering penalties.
		ApplyOfferingPenalties(ari_instance, self);

		// Item-use side effects are handled by AfterUseAction (Player::Hooks::AfterUseActionComplete)
		// in hooks/items/AfterUseAction.cpp, registered separately in Init.cpp.

		// Check for whirl pool movement.
		CheckForWhirlpool(self);

		// Restoration
		if (is_restoration_tracked_interval)
		{
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			if (current_health > 0)
				MMAPI::Player::ModifyHealth(1);
			is_restoration_tracked_interval = false;
		}

		// Fumigate
		if (is_fumigate_tracked_interval)
		{
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			if (current_health > 0)
				MMAPI::Player::ModifyHealth(-1);

			is_fumigate_tracked_interval = false;
		}

		// Deep Wounds
		if (is_deep_wounds_tracked_interval)
		{
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			if (current_health > 0 && deep_wounds_damage_pool > 0)
			{
				int damage = std::clamp(deep_wounds_damage_pool * 10 / 100, 1, 10);
				damage = std::min(damage, deep_wounds_damage_pool);

				deep_wounds_damage_pool -= damage;
				MMAPI::Player::ModifyHealth(-1 * damage);
			}

			is_deep_wounds_tracked_interval = false;
		}

		// Drain (Dark Knight Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN] > 0)
		{
			int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
			int recovery = max_health * GetDarkKnightDrainPotency();

			MMAPI::Player::ModifyHealth(recovery);
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN] = 0;
		}

		// Soul Eater (Dark Knight Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] > 0)
		{
			MMAPI::Player::ModifyHealth(-1 * class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER]);
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] = 0;
		}

		// Aspir (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR] > 0)
		{
			MMAPI::Player::ModifyMana(1);
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR] = 0;
		}

		// Mana Font (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT] >= 3)
		{
			MMAPI::Player::ModifyMana(4);
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT] = 0;
		}

		// Flood (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] > 0 && current_time_in_seconds > class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] + 90)
		{
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points > 0)
						*monster->GetRefMember("hit_points") = hit_points - 1;
				}
			}

			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = 0;
			MMAPI::Spell::Cast(MMAPI::Spell::Ids::SummonRain);
		}

		// Second Wind
		if (is_second_wind_tracked_interval)
		{
			MMAPI::Player::ModifyStamina(1);
			is_second_wind_tracked_interval = false;
		}

		// Fey
		if (active_floor_enchantments.contains(FloorEnchantments::FEY))
		{
			if (!fairy_buff_applied)
			{
				MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::Fairy), RValue(), 1, MMAPI::StatusEffect::InfiniteDuration);
				fairy_buff_applied = true;
			}
			ModifySpellCosts(false, true);
		}

		// Inner Fire
		if (active_offerings.contains(Offerings::INNER_FIRE) && !inner_fire_cast)
		{
			inner_fire_cast = true;
			MMAPI::Spell::Cast(MMAPI::Spell::Ids::FireBreath); // TODO: Make sure this works using name_to_id_map
		}

		// Reckoning
		if (active_offerings.contains(Offerings::RECKONING) && !reckoning_applied)
		{
			reckoning_applied = true;
			MMAPI::Player::SetHealth(1);
		}

		// Stoneskin
		if (active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && !stoneskin_applied)
		{
			stoneskin_applied = true;

			// TODO: Tune this
			if (floor_number < 20)
				stoneskin_shield_amount = 20;
			else if (floor_number < 40)
				stoneskin_shield_amount = 40;
			else if (floor_number < 60)
				stoneskin_shield_amount = 60;
			else if (floor_number < 60)
				stoneskin_shield_amount = 80;
			else
				stoneskin_shield_amount = 100;
		}

		// Chain Spell
		if (active_greater_sigils.contains(GreaterSigils::CHAIN_SPELL))
			ModifySpellCosts(false, true);

		TrackAriResources(ari_instance, self);
	}
}

// MMAPI Instance::Hooks::OnObjectCall("obj_monster", ...) callback.
void OnObjMonsterCall(CInstance* self)
{
	{
		RValue monster = self->ToRValue();
		if (StructVariableExists(monster, "monster_id"))
		{
			RValue monster_id = *monster.GetRefMember("monster_id");

			// Valid if monster_id resolves to a name in __monster_id__ (i.e. it's a known monster, not a sentinel).
			bool is_valid_monster_object = !MMAPI::Monster::GetInternalName(
				static_cast<MMAPI::Monster::Ids>(monster_id.ToInt64())).empty();

			if (is_valid_monster_object)
			{
				auto armor_set_bonuses = GetArmorSetBonuses();

				// Mimic Loot
				if (monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::Mimic) && !StructVariableExists(monster, "__deep_dungeon__mimic_drop_sigil") && StructVariableExists(monster, "fsm"))
				{
					RValue fsm = monster.GetMember("fsm");
					if (StructVariableExists(fsm, "state"))
					{
						RValue state = fsm.GetMember("state");
						if (StructVariableExists(state, "state_id"))
						{
							RValue state_id = state.GetMember("state_id");
							if (state_id.ToInt64() == static_cast<int64_t>(MMAPI::Monster::States::Mimic::Gobble))
							{
								static thread_local pcg32 random_generator([] {
									std::random_device rd;
									return pcg32(
										(static_cast<uint64_t>(rd()) << 32) | rd(),
										(static_cast<uint64_t>(rd()) << 32) | rd()
									);
								}());
								std::uniform_int_distribution<size_t> random_sigil_distribution(0, magic_enum::enum_count<Sigils>() - 1);

								Sigils random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));
								MMAPI::Item::Drop(sigil_to_item_id_map[random_sigil], ari_x, ari_y);
								StructVariableSet(monster, "__deep_dungeon__mimic_drop_sigil", true);
							}
						}
					}
				}

				// Boss Battles — TIDE_CAVERNS_ORB scales HP by 20x; all others by 3x.
				if (boss_battle != BossBattle::NONE)
				{
					const double hp_multiplier = (boss_battle == BossBattle::TIDE_CAVERNS_ORB) ? 20.0 : 3.0;
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * hp_multiplier;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}

				// Dread Beasts
				if (!dread_beast_configured && monster_id.ToInt64() == dread_beast_monster_id && !StructVariableExists(monster, "__deep_dungeon__dread_beast") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points))
					{
						*monster.GetRefMember("hit_points") = static_cast<int>(hit_points * Config::config.dread_beast_health_modifier);
						dread_beasts_configured++;
						if (dread_beast_monster_id != static_cast<int>(MMAPI::Monster::Ids::RockStack) || dread_beasts_configured == 2)
							dread_beast_configured = true;

						StructVariableSet(monster, "__deep_dungeon__dread_beast", true);
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Configured Dread Beast: %s", MOD_NAME, MOD_VERSION, MMAPI::Monster::GetInternalName(static_cast<MMAPI::Monster::Ids>(monster_id.ToInt64())).c_str());
					}
				}
				else if (StructVariableExists(monster, "__deep_dungeon__dread_beast"))
					ModifyDreadBeastAttackPatterns(false, false, monster);

				// Track the monster
				if (!StructVariableExists(monster, "__deep_dungeon__current_floor_monsters") && StructVariableExists(monster, "hit_points"))
				{
					current_floor_monsters.push_back(self);
					StructVariableSet(monster, "__deep_dungeon__current_floor_monsters", true);
					StructVariableSet(monster, "__deep_dungeon__default_hit_points", monster.GetMember("hit_points").ToDouble());
				}

				// Grudge
				if (active_floor_enchantments.contains(FloorEnchantments::GRUDGE) && !StructVariableExists(monster, "__deep_dungeon__grudge_tracked") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						++grudge_counter;
						StructVariableSet(monster, "__deep_dungeon__grudge_tracked", true);
					}
				}

				// Regular loot drops
				if (!is_challenge_mode && boss_battle == BossBattle::NONE && !ari_current_gm_room.contains("seal") && !StructVariableExists(monster, "__deep_dungeon__loot_drop") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						static thread_local pcg32 random_generator([] {
							std::random_device rd;
							return pcg32(
								(static_cast<uint64_t>(rd()) << 32) | rd(),
								(static_cast<uint64_t>(rd()) << 32) | rd()
							);
						}());
						std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
						bool drop_lift_key = zero_to_ninety_nine_distribution(random_generator) < Config::config.lift_key_drop_chance;

						if (floor_number < 20) // Upper Mines
						{
							MMAPI::Item::Drop(item_name_to_id_map["beast_coin_tiny"], ari_x, ari_y);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance;
								if (drop_soul_stone)
									MMAPI::Item::Drop(GetRandomSoulStone(), ari_x, ari_y);
							}
						}
						else if (floor_number < 40) // Tide Caverns
						{
							MMAPI::Item::Drop(item_name_to_id_map["beast_coin_small"], ari_x, ari_y);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance;
								if (drop_soul_stone)
									MMAPI::Item::Drop(GetRandomSoulStone(), ari_x, ari_y);
							}
						}
						else if (floor_number < 60) // Deep Earth
						{
							MMAPI::Item::Drop(item_name_to_id_map["beast_coin_medium"], ari_x, ari_y);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance;
								if (drop_soul_stone)
									MMAPI::Item::Drop(GetRandomSoulStone(), ari_x, ari_y);
							}
						}
						else if (floor_number < 80) // Lava Caves
						{
							MMAPI::Item::Drop(item_name_to_id_map["beast_coin_large"], ari_x, ari_y);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance;
								if (drop_soul_stone)
									MMAPI::Item::Drop(GetRandomSoulStone(), ari_x, ari_y);
							}
						}
						else if (floor_number < 100) // Ruins
						{
							MMAPI::Item::Drop(item_name_to_id_map["beast_coin_giant"], ari_x, ari_y);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance;
								if (drop_soul_stone)
									MMAPI::Item::Drop(GetRandomSoulStone(), ari_x, ari_y);
							}
						}

						StructVariableSet(monster, "__deep_dungeon__loot_drop", true);
					}
				}

				// Boss loot drops
				if (boss_battle != BossBattle::NONE && boss_battle != BossBattle::CLEARED && boss_monsters_configured > 0)
				{
					int boss_monsters_defeated = 0;
					for (CInstance* m : current_floor_monsters)
					{
						if (StructVariableExists(m, "hit_points"))
						{
							double m_hit_points = m->GetMember("hit_points").ToDouble();
							if (std::isfinite(m_hit_points) && m_hit_points <= 0)
								boss_monsters_defeated++;
						}
					}

					if (boss_monsters_defeated == boss_monsters_configured)
					{
						MMAPI::Item::Drop(GetRandomSoulStone(), ari_x, ari_y);
						boss_battle = BossBattle::CLEARED;
						ResetCustomDrawFields();
					}
				}

				// Aspir (Mage Set Bonus)
				if (armor_set_bonuses.mage.Aspir() && !StructVariableExists(monster, "__deep_dungeon__aspir_proc") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						static thread_local pcg32 random_generator([] {
							std::random_device rd;
							return pcg32(
								(static_cast<uint64_t>(rd()) << 32) | rd(),
								(static_cast<uint64_t>(rd()) << 32) | rd()
							);
						}());
						std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

						bool aspir_proc = zero_to_ninety_nine_distribution(random_generator) < 15; // TODO: Tune this. Should Aspir have a 15% chance to proc?
						if (aspir_proc)
							class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR]++;

						StructVariableSet(monster, "__deep_dungeon__aspir_proc", true);
					}
				}

				// Holy Circle (Paladin Set Bonus)
				if (armor_set_bonuses.paladin.HolyCircle() && !StructVariableExists(monster, "__deep_dungeon__holy_circle_proc") && StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						double default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToDouble();
						*monster.GetRefMember("hit_points") = hit_points - std::trunc(default_hit_points * GetPaladinHolyCirclePotency());
						StructVariableSet(monster, "__deep_dungeon__holy_circle_proc", true);

						for (CInstance* current_floor_monster : current_floor_monsters)
						{
							if (StructVariableExists(current_floor_monster, "monster_id") && StructVariableExists(current_floor_monster, "hit_points") && StructVariableExists(current_floor_monster, "__deep_dungeon__default_hit_points"))
							{
								RValue current_floor_monster_id = current_floor_monster->GetMember("monster_id");
								double current_floor_monster_hit_points = current_floor_monster->GetMember("hit_points").ToDouble();
								if (IsNumeric(current_floor_monster_id) && current_floor_monster_id.ToInt64() != static_cast<int>(MMAPI::Monster::Ids::Mimic) && IsNumeric(current_floor_monster_hit_points) && std::isfinite(current_floor_monster_hit_points) && current_floor_monster_hit_points > 0)
								{
									double current_floor_monster_default_hit_points = current_floor_monster->GetMember("__deep_dungeon__default_hit_points").ToDouble();
									*current_floor_monster->GetRefMember("hit_points") = current_floor_monster_hit_points - std::trunc(current_floor_monster_default_hit_points * GetPaladinHolyCirclePotency());
								}
							}
						}
					}
				}

				// Gloom
				if (active_floor_enchantments.contains(FloorEnchantments::GLOOM)
					&& !StructVariableExists(monster, "__deep_dungeon__gloom_applied") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points))
					{
						*monster.GetRefMember("hit_points") = std::trunc(hit_points * Config::config.gloom_health_modifier);
						StructVariableSet(monster, "__deep_dungeon__default_hit_points", std::trunc(hit_points * Config::config.gloom_health_modifier));
						StructVariableSet(monster, "__deep_dungeon__gloom_applied", true);
					}
				}

				// Reckoning
				if (active_offerings.contains(Offerings::RECKONING)
					&& !StructVariableExists(monster, "__deep_dungeon__reckoning_applied") && StructVariableExists(monster, "hit_points"))
				{
					if (current_time_in_seconds < floor_start_time + 30) // All enemies seem to be created with HP var initialized before floor starts
					{
						*monster.GetRefMember("hit_points") = 1;
						StructVariableSet(monster, "__deep_dungeon__reckoning_applied", true);
					}
				}

				// Spirit Link
				if (active_offerings.contains(Offerings::SPIRIT_LINK) && monster_id.ToInt64() != static_cast<int>(MMAPI::Monster::Ids::Mimic))
				{
					if (StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							if (StructVariableExists(monster, "__deep_dungeon__spirit_link_applied") && spirit_link_combined_health_pool <= 0)
								StructVariableSet(monster, "hit_points", 0);
							else if (!StructVariableExists(monster, "__deep_dungeon__spirit_link_applied"))
							{
								spirit_link_combined_health_pool += hit_points;
								StructVariableSet(monster, "__deep_dungeon__spirit_link_applied", true);
								StructVariableSet(monster, "__deep_dungeon__spirit_link_damage", 0);

							}
							else if (StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
							{
								int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
								int spirit_link_damage = monster.GetMember("__deep_dungeon__spirit_link_damage").ToInt64();
								if (hit_points < default_hit_points)
								{
									spirit_link_damage += (default_hit_points - hit_points);
									spirit_link_combined_health_pool -= (default_hit_points - hit_points);

									StructVariableSet(monster, "__deep_dungeon__spirit_link_damage", spirit_link_damage);
									StructVariableSet(monster, "hit_points", default_hit_points);
								}
							}
						}
					}
				}
				else if (StructVariableExists(monster, "__deep_dungeon__spirit_link_applied"))
				{
					if (StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
							int spirit_link_damage = monster.GetMember("__deep_dungeon__spirit_link_damage").ToInt64();

							if(default_hit_points - spirit_link_damage > 0)
								StructVariableSet(monster, "hit_points", default_hit_points - spirit_link_damage);
							else
								StructVariableSet(monster, "hit_points", 0);
						}

						StructVariableRemove(monster, "__deep_dungeon__spirit_link_applied");
					}
				}

				// Sigil of Concealment
				if (active_sigils.contains(Sigils::CONCEALMENT))
				{
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue hit_points = monster.GetMember("hit_points");
						if (!StructVariableExists(monster, "__deep_dungeon__conceal_hit_points"))
							StructVariableSet(monster, "__deep_dungeon__conceal_hit_points", hit_points);

						RValue original_hit_points = monster.GetMember("__deep_dungeon__conceal_hit_points");
						if (hit_points.ToDouble() == original_hit_points.ToDouble())
						{
							StructVariableSet(monster, "aggro", false);
							if (monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::Cat) || monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::CatVoid) || monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::Tome) || monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::GriffinStatue))
							{
								StructVariableSet(monster, "friction_coefficient", 1);
								StructVariableSet(monster, "slippery_coefficient", 1);
							}
						}
						else
						{
							active_sigils.erase(Sigils::CONCEALMENT);
							MMAPI::Game::CreateNotification(false, CONCEALMENT_LOST_NOTIFICATION_KEY);

							// Sneak Attack (Rogue Set Bonus)
							if (armor_set_bonuses.rogue.Hide())
								*monster.GetRefMember("hit_points") = 0;

							MMAPI::ToolbarMenu::ForceUpdate();
						}
					}
				}
				else
				{
					StructVariableRemove(monster, "__deep_dungeon__conceal_hit_points");
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points) && hit_points > 0 && (monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::Cat) || monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::CatVoid) || monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::Tome) || monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::GriffinStatue)))
						{
							StructVariableSet(monster, "friction_coefficient", 0.1);
							StructVariableSet(monster, "slippery_coefficient", 0.1);
						}
					}
				}


				// Sigil of Rage
				if (active_sigils.contains(Sigils::RAGE))
				{
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue hit_points = monster.GetMember("hit_points");
						if (!StructVariableExists(monster, "__deep_dungeon__rage_hit_points"))
							StructVariableSet(monster, "__deep_dungeon__rage_hit_points", hit_points);

						RValue original_hit_points = monster.GetMember("__deep_dungeon__rage_hit_points");
						if (hit_points.ToDouble() != original_hit_points.ToDouble())
							*monster.GetRefMember("hit_points") = 0;
					}
				}
			}
		}
	}
}
