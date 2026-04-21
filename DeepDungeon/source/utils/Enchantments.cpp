#include "Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

int CalculateMeteorDamage(double distance)
{
	const double lethalRadius = 32.0;
	const double maxDistance = 256.0;

	if (Config::config.meteor_trap_scaling_factor == 0)
		return 0;

	if (distance <= lethalRadius)
		return 100;

	if (distance >= maxDistance)
		return 1;

	double t = (distance - lethalRadius) / (maxDistance - lethalRadius);
	double damage = 100.0f * std::pow(1.0f - t, Config::config.meteor_trap_scaling_factor);

	return std::max(1, static_cast<int>(damage));
}

bool FacingTrap(int ariX, int ariY, int trapX, int trapY)
{
	static constexpr double PI = 3.14159265358979323846;

	std::pair<int, int> v = GetVector(ariX, ariY, trapX, trapY);
	double rad = ari_facing_dir * PI / 180.0;

	double forwardX = cos(rad);
	double forwardY = -sin(rad);

	float dot = forwardX * v.first + forwardY * v.second;
	bool inFrontHalfPlane = dot >= 0.0f;
	return inFrontHalfPlane;
}

std::unordered_set<FloorEnchantments> RandomFloorEnchantments(bool is_first_floor, DungeonBiomes dungeon_biome)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
	auto armor_set_bonuses = GetArmorSetBonuses();

	std::unordered_set<FloorEnchantments> random_floor_enchantments = {};

	if (is_first_floor)
	{
		const std::vector<FloorEnchantments> FIRST_FLOOR_POSSIBLE_ENCHANTMENTS = {
			FloorEnchantments::RESTORATION, FloorEnchantments::SECOND_WIND, FloorEnchantments::HASTE
		};

		std::uniform_int_distribution<size_t> first_floor_distribution(0, FIRST_FLOOR_POSSIBLE_ENCHANTMENTS.size() - 1);
		return { FIRST_FLOOR_POSSIBLE_ENCHANTMENTS[first_floor_distribution(random_generator)] };
	}

	if (dungeon_biome == DungeonBiomes::UPPER)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && armor_set_bonuses.oracle.FullSet())
		{
			// 70% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 70)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 45% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 45)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 20% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 20)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 50) // 50% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 50% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 50% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 50)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 25% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 25)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::TIDE_CAVERNS)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && armor_set_bonuses.oracle.FullSet())
		{
			// 85% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 60% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 60)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 35% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 35)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 60) // 60% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 40% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 40% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 40)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 15% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 15)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 60) // 60% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 40% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::DEEP_EARTH)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && armor_set_bonuses.oracle.FullSet())
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 85% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 40% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 40)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 70) // 70% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 30% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 45% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 45)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 65% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 65)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 20% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 20)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 70) // 70% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 30% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::LAVA_CAVES)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && armor_set_bonuses.oracle.FullSet())
		{
			// 80% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 80)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 95% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 95)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 45% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 45)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 80) // 80% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 20% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 60% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 60)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 75% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 75)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 25% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 25)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 80) // 80% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 20% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::RUINS)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && armor_set_bonuses.oracle.FullSet())
		{
			// 85% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 95% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 95)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 50% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 50)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 90) // 90% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 10% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 75% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 75)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 30% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 30)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 90) // 90% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 10% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	return random_floor_enchantments;
}

void GenerateFloorTraps()
{
	if (TRAP_SPAWN_POINTS.contains(ari_current_gm_room))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

		if (spawn_points.empty())
			return;

		int min_traps = 2;
		int biome_adjusted_max_traps = (floor_number / 20) + 2;

		std::uniform_int_distribution<size_t> traps_for_room_distribution(min_traps, biome_adjusted_max_traps);
		int random_trap_count = traps_for_room_distribution(random_generator);

		// Disarm Trap (Rogue Set Bonus)
		if (GetArmorSetBonuses().rogue.DisarmTrap())
			random_trap_count -= 2;

		// Peril
		if (active_offerings.contains(Offerings::PERIL))
			random_trap_count += 2;

		int count = 0;
		for (int i = 0; i < random_trap_count; i++)
		{
			std::uniform_int_distribution<size_t> random_number_of_traps_for_current_room(0, spawn_points.size() - 1);
			int random_index = random_number_of_traps_for_current_room(random_generator);
			floor_trap_positions.insert(spawn_points[random_index]);
			spawn_points.erase(spawn_points.begin() + random_index);
		}
	}
}

void RevealFloorTraps()
{
	if (active_sigils.contains(Sigils::SIGHT) && revealed_floor_traps.empty())
	{
		for (auto floor_trap = floor_trap_positions.begin(); floor_trap != floor_trap_positions.end(); floor_trap++) {
			RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
			if (instance_layer_exists)
			{
				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

				RevealedFloorTrap revealed_floor_trap = RevealedFloorTrap(floor_trap->first, floor_trap->second, true, instance);
				revealed_floor_traps.push_back(revealed_floor_trap);
			}
		}
	}

	for (RevealedFloorTrap revealed_floor_trap : revealed_floor_traps)
	{
		if (revealed_floor_trap.is_active && !floor_trap_positions.contains({ revealed_floor_trap.x, revealed_floor_trap.y }))
		{
			revealed_floor_trap.is_active = false;

			RValue revealed_floor_trap_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { revealed_floor_trap.instance });
			if (revealed_floor_trap_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { revealed_floor_trap.instance });
		}
	}
}

void ApplyFloorTraps(CInstance* Self, CInstance* Other)
{
	// Prune traps that have fully applied.
	if (active_traps.contains(Traps::CONFUSING) && active_traps_to_value_map[Traps::CONFUSING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect ended at: %d", MOD_NAME, MOD_VERSION, current_time_in_seconds);
		active_traps.erase(Traps::CONFUSING);
		active_traps_to_value_map.erase(Traps::CONFUSING);
	}
	if (active_traps.contains(Traps::DISORIENTING) && active_traps_to_value_map[Traps::DISORIENTING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect ended at: %d", MOD_NAME, MOD_VERSION, current_time_in_seconds);
		active_traps.erase(Traps::DISORIENTING);
		active_traps_to_value_map.erase(Traps::DISORIENTING);
	}
	if (active_traps.contains(Traps::INHIBITING) && active_traps_to_value_map[Traps::INHIBITING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect ended at: %d", MOD_NAME, MOD_VERSION, current_time_in_seconds);
		active_traps.erase(Traps::INHIBITING);
		active_traps_to_value_map.erase(Traps::INHIBITING);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	// Ignore traps if Ari is in whirlpool movement.
	if (in_whirl_pool)
		return;

	// Process traps in range.
	for (auto floor_trap = floor_trap_positions.begin(); floor_trap != floor_trap_positions.end();) {
		double distance = GetDistance(ari_x, ari_y, floor_trap->first, floor_trap->second);
		if (distance <= 16)
		{
			static thread_local pcg32 random_generator([] {
				std::random_device rd;
				return pcg32(
					(static_cast<uint64_t>(rd()) << 32) | rd(),
					(static_cast<uint64_t>(rd()) << 32) | rd()
				);
			}());
			std::uniform_int_distribution<size_t> random_trap_distribution(0, magic_enum::enum_count<Traps>() - 1);
			std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

			Traps trap = magic_enum::enum_value<Traps>(random_trap_distribution(random_generator));

			// Hallowed Ground (Paladin Set Bonus)
			bool malfunction = zero_to_ninety_nine_distribution(random_generator) < 50 ? true : false;
			if (GetArmorSetBonuses().paladin.HallowedGround() && malfunction)
			{
				PlaySoundEffect("snd_bark_heart603", 100, 1);
				CreateNotification(true, MALFUNCTION_TRAP_NOTIFICATION_KEY, Self, Other);
			}
			else
			{
				g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Trap Triggered: %s", MOD_NAME, MOD_VERSION, magic_enum::enum_name(trap).data());
				active_traps.insert({ trap, { floor_trap->first, floor_trap->second } });

				if (trap == Traps::CONFUSING)
				{
					PlaySoundEffect("snd_bark_o_o", 100, 1);
					CreateNotification(true, CONFUSING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["question_mark"], 0);

					if (!active_traps_to_value_map.contains(Traps::CONFUSING))
					{
						active_traps_to_value_map[Traps::CONFUSING] = current_time_in_seconds + Config::config.confusing_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect started at: %d", MOD_NAME, MOD_VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::CONFUSING] += Config::config.confusing_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect extended by: %d", MOD_NAME, MOD_VERSION, Config::config.confusing_trap_duration_seconds);
					}
				}
				else if (trap == Traps::DISORIENTING)
				{
					PlaySoundEffect("snd_interactable_scan", 100, 1);
					CreateNotification(true, DISORIENTING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["mist"], 0);

					if (!active_traps_to_value_map.contains(Traps::DISORIENTING))
					{
						active_traps_to_value_map[Traps::DISORIENTING] = current_time_in_seconds + Config::config.disorienting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect started at: %d", MOD_NAME, MOD_VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::DISORIENTING] += Config::config.disorienting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect extended by: %d", MOD_NAME, MOD_VERSION, Config::config.disorienting_trap_duration_seconds);
					}
				}
				else if (trap == Traps::EXPLODING)
				{
					PlaySoundEffect("snd_Explosion_CaveReverb", 100, 0.35);
					CreateNotification(true, EXPLODING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["angry"], 0);
				}
				else if (trap == Traps::INHIBITING)
				{
					PlaySoundEffect("snd_bark_surprised", 100, 1);
					CreateNotification(true, INHIBITING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["no_coin"], 0);

					if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
						UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

					if (!active_traps_to_value_map.contains(Traps::INHIBITING))
					{
						active_traps_to_value_map[Traps::INHIBITING] = current_time_in_seconds + Config::config.inhibiting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect started at: %d", MOD_NAME, MOD_VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::INHIBITING] += Config::config.inhibiting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect extended by: %d", MOD_NAME, MOD_VERSION, Config::config.inhibiting_trap_duration_seconds);
					}
				}
				else if (trap == Traps::LURING)
				{
					std::uniform_int_distribution<int> random_position_offset_distribution(-12, 12);

					PlaySoundEffect("snd_ScrollRaise", 100, 1);
					CreateNotification(true, LURING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["fishing"], 0);

					// TODO: Restrict monster spawns as necessary (stalagmite_pink? TBD)
					std::vector<int> random_monsters;
					if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite"]);
					else if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite_green"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite_green"]);
					else if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite_purple"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite_purple"]);
					else
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count);

					for (int i = 0; i < random_monsters.size(); i++)
						SpawnMonster(Self, Other, floor_trap->first + random_position_offset_distribution(random_generator), floor_trap->second + random_position_offset_distribution(random_generator), random_monsters[i]);

					active_traps.erase(Traps::LURING);
				}
				else if (trap == Traps::METEOR)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_VoidPortalSpawn", 100, 0.7);
						CreateNotification(true, METEOR_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

						CustomAOE meteor = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 600, current_time_in_seconds, true, instance, CustomAOETypes::METEOR);
						meteor_aoes.push_back(meteor);
					}

					active_traps.erase(Traps::METEOR);
				}
				else if (trap == Traps::GAZE)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_VoidMassAppear", 100, 0.7);
						CreateNotification(true, GAZE_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

						CustomAOE gaze = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 600, current_time_in_seconds, true, instance, CustomAOETypes::GAZE);
						gaze_aoes.push_back(gaze);
					}

					active_traps.erase(Traps::GAZE);
				}
				else if (trap == Traps::_VOID)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_MagicVoidLightSpell", 100, 0.3);
						CreateNotification(true, VOID_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

						CustomAOE void_aoe = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, Config::config.void_trap_duration_seconds, current_time_in_seconds, true, instance, CustomAOETypes::_VOID);
						void_aoes.push_back(void_aoe);
					}

					active_traps.erase(Traps::_VOID);
				}
			}

			floor_trap = floor_trap_positions.erase(floor_trap);
		}
		else
			++floor_trap;
	}
}

void ProcessCustomAOEs()
{
	for (CustomAOE& meteor : meteor_aoes)
	{
		if (meteor.is_active && current_time_in_seconds >= meteor.spawned_time + meteor.duration)
		{
			meteor.is_active = false;
			PlaySoundEffect("snd_EarthquakeImpact", 1, 1);
			PlaySoundEffect("snd_AriLowHealthWarning", 1, 1);

			double distance = GetDistance(ari_x, ari_y, meteor.x, meteor.y);
			double modifier = CalculateMeteorDamage(distance) / 100.0;

			double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
			ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);

			RValue meteor_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { meteor.instance });
			if (meteor_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { meteor.instance });
		}
	}

	for (CustomAOE& gaze : gaze_aoes)
	{
		if (gaze.is_active && current_time_in_seconds >= gaze.spawned_time + gaze.duration)
		{
			gaze.is_active = false;
			PlaySoundEffect("snd_CosmicImpact", 1, 0.30);

			bool facing_trap = FacingTrap(ari_x, ari_y, gaze.x, gaze.y);
			if (facing_trap)
			{
				double modifier = Config::config.gaze_trap_max_health_damage_percent / 100.0;
				double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);
				PlaySoundEffect("snd_AriLowHealthWarning", 1, 1);
			}

			RValue gaze_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { gaze.instance });
			if (gaze_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { gaze.instance });
		}
	}

	for (CustomAOE& void_aoe : void_aoes)
	{
		if (void_aoe.is_active && current_time_in_seconds >= void_aoe.last_application + 15) // TODO: Make tick rate configurable
		{
			void_aoe.last_application = current_time_in_seconds;

			double distance = GetDistance(ari_x, ari_y, void_aoe.x, void_aoe.y);
			if (distance > 96 && distance <= 298)
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1);
		}

		if (void_aoe.is_active && current_time_in_seconds >= void_aoe.spawned_time + void_aoe.duration)
		{
			void_aoe.is_active = false;

			RValue void_trap_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { void_aoe.instance });
			if (void_trap_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { void_aoe.instance });
		}
	}
}

void GenerateTreasureSpot(CInstance* Self, CInstance* Other)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_seven_distribution(0, 7);

	int biome_adjusted_max_traps_with_peril = (floor_number / 20) + 4;
	int floors_to_descend = zero_to_seven_distribution(random_generator);

	if (floor_trap_positions.size() == biome_adjusted_max_traps_with_peril)
		floors_to_descend++;

	treasure_spot.floors_to_descend = floors_to_descend;
	treasure_spot.state = TreasureSpot::WAITING_TO_SPAWN;

	if (treasure_spot.floors_to_descend > 0)
		CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
}

void ProcessTreasureSpot(CInstance* Self, CInstance* Other)
{
	if (treasure_spot.state == TreasureSpot::WAITING_TO_SPAWN && treasure_spot.floors_to_descend == 0)
	{
		int biome_adjusted_max_traps_with_peril = (floor_number / 20) + 4;
		if (floor_trap_positions.size() < biome_adjusted_max_traps_with_peril)
		{
			// Find a position for the treasure spot on the current floor.
			std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

			std::pair<int, int> treasure_spot_coordinates;
			for (std::pair<int, int> spawn_point : spawn_points)
			{
				if (floor_trap_positions.contains(spawn_point))
					continue;

				treasure_spot_coordinates = spawn_point;
				break;
			}

			RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
			if (instance_layer_exists)
			{
				PlaySoundEffect("snd_TreasureChestSpawn", 100, 0.7);
				CreateNotification(true, TREASURE_SPOT_SPAWNED_NOTIFICATION_KEY, Self, Other);

				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { treasure_spot_coordinates.first, treasure_spot_coordinates.second, RValue("Instances"), obj_assetobject });

				treasure_spot.x = treasure_spot_coordinates.first;
				treasure_spot.y = treasure_spot_coordinates.second;
				treasure_spot.is_active = true;
				treasure_spot.instance = instance;
				treasure_spot.state = TreasureSpot::SPAWNED;
			}
			else
			{
				treasure_spot.floors_to_descend++;
				CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
			}
		}
		else
		{
			treasure_spot.floors_to_descend++;
			CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
		}

	}

	if (treasure_spot.state == TreasureSpot::SPAWNED)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> random_greater_sigil_distribution(0, magic_enum::enum_count<GreaterSigils>() - 1);

		double distance = GetDistance(ari_x, ari_y, treasure_spot.x, treasure_spot.y);
		if (distance <= 8)
		{
			treasure_spot.state = TreasureSpot::FOUND;
			CreateNotification(true, TREASURE_SPOT_FOUND_NOTIFICATION_KEY, Self, Other);

			//GreaterSigils random_greater_sigil = magic_enum::enum_value<GreaterSigils>(random_greater_sigil_distribution(random_generator));
			//DropItem(greater_sigil_to_item_id_map[random_greater_sigil], ari_x, ari_y, Self, Other);

			if (floor_number < 20)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE], ari_x, ari_y, Self, Other);
			else if (floor_number < 40)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI], ari_x, ari_y, Self, Other);
			else if (floor_number < 60)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION], ari_x, ari_y, Self, Other);
			else if (floor_number < 80)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL], ari_x, ari_y, Self, Other);
			else if (floor_number < 100)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW], ari_x, ari_y, Self, Other);

			if (treasure_spot.is_active)
			{
				treasure_spot.is_active = false;

				RValue treasure_spot_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { treasure_spot.instance });
				if (treasure_spot_instance_exists.ToBoolean())
					g_ModuleInterface->CallBuiltin("instance_destroy", { treasure_spot.instance });
			}
		}
	}

}

void GenerateTreasureChestLoot(std::string object_name, CInstance* Self, CInstance* Other)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
	std::uniform_int_distribution<size_t> random_sigil_distribution(0, magic_enum::enum_count<Sigils>() - 1);

	// Sigils
	std::vector<int> sigil_roll_success_thresholds = {};
	if (object_name == TREASURE_CHEST_WOOD_NAME)
		sigil_roll_success_thresholds = { 50, 25, 0, 0 };
	else if (object_name == TREASURE_CHEST_COPPER_NAME)
		sigil_roll_success_thresholds = { 75, 50, 10, 0 };
	else if (object_name == TREASURE_CHEST_SILVER_NAME)
		sigil_roll_success_thresholds = { 100, 50, 25, 0 };
	else if (object_name == TREASURE_CHEST_GOLD_NAME)
		sigil_roll_success_thresholds = { 100, 100, 25, 10 };

	// Treasure Hunter (Rogue Set Bonus)
	if (GetArmorSetBonuses().rogue.TreasureHunter())
		sigil_roll_success_thresholds.push_back(100);

	std::unordered_set<Sigils> sigils_spawned = {};
	for (size_t i = 0; i < sigil_roll_success_thresholds.size(); i++)
	{
		int roll_for_drop = zero_to_ninety_nine_distribution(random_generator);
		if (roll_for_drop < sigil_roll_success_thresholds[i])
		{
			Sigils random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));
			while (sigils_spawned.contains(random_sigil))
				random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));

			sigils_spawned.insert(random_sigil);
			DropItem(sigil_to_item_id_map[random_sigil], ari_x, ari_y, Self, Other);
		}
	}

	// Cursed Armor
	int cursed_armor_roll_success_threshold = 0;
	if (object_name == TREASURE_CHEST_WOOD_NAME)
		cursed_armor_roll_success_threshold = 1 * Config::config.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_COPPER_NAME)
		cursed_armor_roll_success_threshold = 2 * Config::config.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_SILVER_NAME)
		cursed_armor_roll_success_threshold = 3 * Config::config.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_GOLD_NAME)
		cursed_armor_roll_success_threshold = 4 * Config::config.cursed_armor_drop_chance_modifier;

	int roll_for_drop = zero_to_ninety_nine_distribution(random_generator);
	if (!is_challenge_mode && roll_for_drop < cursed_armor_roll_success_threshold)
	{
		if (floor_number < 20) // Upper Mines
			DropItem(item_name_to_id_map[CURSED_CHESTPIECE_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 40) // Tide Caverns
			DropItem(item_name_to_id_map[CURSED_HELMET_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 60) // Deep Earth
			DropItem(item_name_to_id_map[CURSED_GLOVES_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 80) // Lava Caves
			DropItem(item_name_to_id_map[CURSED_PANTS_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 100) // Ruins
			DropItem(item_name_to_id_map[CURSED_BOOTS_NAME], ari_x, ari_y, Self, Other);
	}
}

void ApplyOfferingPenalties(CInstance* Self, CInstance* Other)
{
	if (ari_resource_to_penalty_map[AriResources::HEALTH])
		ModifyHealth(Self, Other, Config::config.offering_health_requirement * -1);
	if (ari_resource_to_penalty_map[AriResources::STAMINA])
		ModifyStamina(Self, Other, Config::config.offering_stamina_requirement * -1);
	if (ari_resource_to_penalty_map[AriResources::MANA])
		ModifyMana(Self, Other, Config::config.offering_mana_requirement * -1);

	ari_resource_to_penalty_map.clear();
}

void TickTimeStoppedSystems(CInstance* Self, CInstance* Other, int64_t tick_delta)
{
	// Accumulate game-clock ticks. Each unit = one in-game second.
	time_stopped_tick_accumulator += tick_delta;
	if (time_stopped_tick_accumulator < 1)
		return;

	int seconds_to_advance = static_cast<int>(time_stopped_tick_accumulator);
	time_stopped_tick_accumulator -= seconds_to_advance;

	// Floor enchantment tick timestamps — decrement so elapsed time grows relative to the frozen clock.
	time_of_last_restoration_tick -= seconds_to_advance;
	time_of_last_second_wind_tick -= seconds_to_advance;
	time_of_last_fumigate_tick -= seconds_to_advance;
	time_of_last_deep_wounds_tick -= seconds_to_advance;
	time_of_last_outbreak_tick -= seconds_to_advance;

	// Timed trap expiry timestamps — stored as absolute expiry; decrement so they reach current_time_in_seconds sooner.
	for (auto& [trap, expiry] : active_traps_to_value_map)
		expiry -= seconds_to_advance;

	// Set bonus timestamp tracking.
	auto& bonus_map = class_name_to_set_bonus_effect_value_map;
	if (bonus_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] > 0)
		bonus_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] -= seconds_to_advance;
	if (bonus_map[Classes::MAGE][ManagedSetBonuses::FLOOD] > 0)
		bonus_map[Classes::MAGE][ManagedSetBonuses::FLOOD] -= seconds_to_advance;

	// Custom AOE timestamps — decrement spawned_time and last_application so durations and tick rates progress.
	for (CustomAOE& aoe : meteor_aoes)
	{
		if (aoe.is_active)
		{
			aoe.spawned_time -= seconds_to_advance;
			aoe.last_application -= seconds_to_advance;
		}
	}
	for (CustomAOE& aoe : gaze_aoes)
	{
		if (aoe.is_active)
		{
			aoe.spawned_time -= seconds_to_advance;
			aoe.last_application -= seconds_to_advance;
		}
	}
	for (CustomAOE& aoe : void_aoes)
	{
		if (aoe.is_active)
		{
			aoe.spawned_time -= seconds_to_advance;
			aoe.last_application -= seconds_to_advance;
		}
	}

	// Status effect timestamps — advance start and last_update toward finish so effects progress and expire.
	RValue effects_inner = global_instance
		->GetMember("__ari")
		.GetMember("status_effects")
		.GetMember("effects")
		.GetMember("inner");

	//int unified_seconds_to_advance = 10;
	for (const auto& [name, id] : status_effect_name_to_id_map)
	{
		std::string id_str = std::to_string(id);
		if (!StructVariableExists(effects_inner, id_str.c_str()))
			continue;

		RValue effect = effects_inner.GetMember(id_str.c_str());
		if (effect.m_Kind == VALUE_UNDEFINED)
			continue;

		int64_t finish = effect.GetMember("finish").ToInt64();
		if (finish == 2147483647)
			continue;

		int64_t start = effect.GetMember("start").ToInt64();
		if (!StructVariableExists(effect, "last_tick"))
			StructVariableSet(effect, "last_tick", start);

		int64_t last_tick = effect.GetMember("last_tick").ToInt64();
		if (last_tick < start)
			StructVariableSet(effect, "last_tick", start);

		int64_t last_update = effect.GetMember("last_update").ToInt64();
		if (last_update < start)
			StructVariableSet(effect, "last_update", start);

		last_tick += seconds_to_advance;
		StructVariableSet(effect, "last_tick", last_tick);

		if (name == "restorative" || name == "shrine_boon")
		{
			if (last_tick >= last_update + 600)
			{
				StructVariableSet(effect, "last_update", last_tick);
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 5);
				ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 5);
			}
		}

		if (last_tick >= finish)
		{
			if (name == "flame_breath")
				SetFireBreathTime(0);
			CancelStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], id);
		}

		//int64_t start = effect.GetMember("start").ToInt64();
		//int64_t new_start = start + seconds_to_advance;
		//StructVariableSet(effect, "start", static_cast<double>(new_start));
		//StructVariableSet(effect, "last_update", static_cast<double>(min(new_start, finish)));

		//if (new_start >= finish)
		//	CancelStatusEffect(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], RValue(id));
	}
}
