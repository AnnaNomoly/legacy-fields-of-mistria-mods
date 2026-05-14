#include "Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

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
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::GriffinStatue)), candidate_monsters.end());

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

	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Stalagmite)), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::StalagmiteGreen)), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::StalagmitePurple)), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Spirit)), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::SpiritPurple)), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Mimic)), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::GriffinStatue)), candidate_monsters.end());

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
		std::vector<MMAPI::Monster::Ids> possible_dread_beast_monsters = {};
		if (floor_number < 20)
			possible_dread_beast_monsters = { MMAPI::Monster::Ids::Rockclod, MMAPI::Monster::Ids::Sapling, MMAPI::Monster::Ids::SaplingCool, MMAPI::Monster::Ids::Mushroom, MMAPI::Monster::Ids::Enchantern };
		else if (floor_number < 40)
			possible_dread_beast_monsters = { MMAPI::Monster::Ids::RockclodBlue, MMAPI::Monster::Ids::SaplingBlue, MMAPI::Monster::Ids::MushroomGreen, MMAPI::Monster::Ids::EnchanternBlue, MMAPI::Monster::Ids::Stalagmite, MMAPI::Monster::Ids::Bat };
		else if (floor_number < 60)
			possible_dread_beast_monsters = { MMAPI::Monster::Ids::RockclodGreen, MMAPI::Monster::Ids::SaplingPurple, MMAPI::Monster::Ids::MushroomBlue, MMAPI::Monster::Ids::StalagmiteGreen, MMAPI::Monster::Ids::BatBlue };
		else if (floor_number < 80)
			possible_dread_beast_monsters = { MMAPI::Monster::Ids::RockclodRed, MMAPI::Monster::Ids::SaplingOrange, MMAPI::Monster::Ids::MushroomPurple, MMAPI::Monster::Ids::StalagmitePurple, MMAPI::Monster::Ids::Spirit, MMAPI::Monster::Ids::Cat };
		else
			possible_dread_beast_monsters = { MMAPI::Monster::Ids::SaplingPink, MMAPI::Monster::Ids::SpiritPurple, MMAPI::Monster::Ids::CatVoid, MMAPI::Monster::Ids::RockStack, MMAPI::Monster::Ids::Tome }; // TODO: RockclodPurple if/when implemented

		std::uniform_int_distribution<size_t> random_dread_beast_distribution(0, possible_dread_beast_monsters.size() - 1);
		random_index = random_dread_beast_distribution(random_generator);
		MMAPI::Monster::Ids monster = possible_dread_beast_monsters[random_index];

		dread_beast_monster_id = static_cast<int>(monster);
		MMAPI::Monster::SpawnMonster(spawn_point.first, spawn_point.second, monster);
	}
}

void SelectDreadBeast(CInstance* Self, CInstance* Other)
{
	// TODO: Update this as Dread Beasts are implemented by removing the prune statements below.
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Barrel)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Copperclod)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Goldclod)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Ironclod)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Mimic)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Mistrilclod)), initial_floor_monsters.end());
	// rock_stack_lava isn't in MMAPI::Monster::Ids yet — resolve at runtime from __monster_id__.
	if (auto lava = MMAPI::Monster::TryFromInternalName("rock_stack_lava"))
		initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(*lava)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::SaplingOrangeMini)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::Silverclod)), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), static_cast<int>(MMAPI::Monster::Ids::RockclodPurple)), initial_floor_monsters.end()); // TODO: don't remove if/when implemented (MONSTER NOT IMPLEMENTED)

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
			if (IsNumeric(monster_id) && (monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::SpiritPurple)) && std::isfinite(hit_points) && hit_points > 0)
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
