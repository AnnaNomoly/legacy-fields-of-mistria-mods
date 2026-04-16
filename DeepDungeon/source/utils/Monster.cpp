#include "Utils.h"

std::vector<int> GenerateRandomMonstersIdsForCurrentFloor(int monsters_to_spawn, const int monster_id_to_exclude)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::vector<int> candidate_monsters(dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].begin(), dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].end());

	if (monster_id_to_exclude != -1)
		candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_id_to_exclude), candidate_monsters.end());

	// Don't spawn griffin statues for luring traps.
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["griffin_statue"]), candidate_monsters.end());

	std::vector<int> random_monsters = {};
	for (int i = 0; i < monsters_to_spawn; i++)
	{
		std::uniform_int_distribution<size_t> random_monster_distribution(0, candidate_monsters.size() - 1);
		random_monsters.push_back(candidate_monsters[random_monster_distribution(random_generator)]);
	}

	return random_monsters;
}

int SelectRandomMonsterForAlteration()
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::vector<int> candidate_monsters(dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].begin(), dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].end());

	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["stalagmite"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["stalagmite_green"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["stalagmite_purple"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["spirit"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["spirit_purple"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["mimic"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["griffin_statue"]), candidate_monsters.end());

	std::uniform_int_distribution<size_t> random_monster_distribution(0, candidate_monsters.size() - 1);
	return candidate_monsters[random_monster_distribution(random_generator)];
}

void SpawnDreadBeast(CInstance* Self, CInstance* Other)
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

		std::uniform_int_distribution<size_t> trap_spawn_points_distribution(0, spawn_points.size() - 1);
		int random_index = trap_spawn_points_distribution(random_generator);
		std::pair<int, int> spawn_point = spawn_points[random_index];

		// TODO: Update this as dread beast logic is implemented
		std::vector<std::string> possible_dread_beast_monsters = {};
		if (floor_number < 20)
			possible_dread_beast_monsters = { "rockclod", "sapling", "sapling_cool", "mushroom", "enchantern" };
		else if (floor_number < 40)
			possible_dread_beast_monsters = { "rockclod_blue", "sapling_blue", "mushroom_green", "enchantern_blue", "stalagmite", "bat" };
		else if (floor_number < 60)
			possible_dread_beast_monsters = { "rockclod_green", "sapling_purple", "mushroom_blue", "stalagmite_green", "bat_blue" };
		else if (floor_number < 80)
			possible_dread_beast_monsters = { "rockclod_red", "sapling_orange", "mushroom_purple", "stalagmite_purple", "spirit", "cat" };
		else
			possible_dread_beast_monsters = { "sapling_pink", "spirit_purple", "cat_void", "rock_stack", "tome" }; // TODO: "rockclod_purple" if/when implemented

		std::uniform_int_distribution<size_t> random_dread_beast_distribution(0, possible_dread_beast_monsters.size() - 1);
		random_index = random_dread_beast_distribution(random_generator);
		int monster_id = monster_name_to_id_map[possible_dread_beast_monsters[random_index]];

		dread_beast_monster_id = monster_id;
		SpawnMonster(Self, Other, spawn_point.first, spawn_point.second, monster_id);
	}
}

void SelectDreadBeast(CInstance* Self, CInstance* Other)
{
	// TODO: Update this as Dread Beasts are implemented by removing the prune statements below.
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["barrel"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["copperclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["goldclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["ironclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["mimic"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["mistrilclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["rock_stack_lava"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["sapling_orange_mini"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["silverclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["rockclod_purple"]), initial_floor_monsters.end()); // TODO: don't remove if/when implemented (MONSTER NOT IMPLEMENTED)

	if (initial_floor_monsters.size() > 0)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<int> initial_floor_monster_distribution(0, initial_floor_monsters.size() - 1);

		dread_beast_monster_id = initial_floor_monsters[initial_floor_monster_distribution(random_generator)];
	}
	else
		SpawnDreadBeast(Self, Other);
}

void ProcessSpiritConcealment()
{
	// TODO: Update as more spirits get added
	for (CInstance* monster : current_floor_monsters)
	{
		if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points"))
		{
			RValue monster_id = monster->GetMember("monster_id");
			double hit_points = monster->GetMember("hit_points").ToDouble();
			if (IsNumeric(monster_id) && (monster_id.ToInt64() == monster_name_to_id_map["spirit_purple"]) && std::isfinite(hit_points) && hit_points > 0)
			{
				if (active_sigils.contains(Sigils::CONCEALMENT) && !StructVariableExists(monster, "__deep_dungeon__deactivated"))
				{
					StructVariableSet(monster, "__deep_dungeon__deactivated", true);
					g_ModuleInterface->CallBuiltin("instance_deactivate_object", { monster });
				}

				else if (!active_sigils.contains(Sigils::CONCEALMENT) && StructVariableExists(monster, "__deep_dungeon__deactivated"))
				{
					StructVariableRemove(monster, "__deep_dungeon__deactivated");
					g_ModuleInterface->CallBuiltin("instance_activate_object", { monster });
				}
			}
		}
	}
}
