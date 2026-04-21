#include "../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Maps;

static fs::path GetChallengeModeFilePath()
{
	std::string filename = save_prefix + ".bin";
	return fs::current_path() / "mod_data" / "DeepDungeon" / filename;
}

void WriteChallengeModeFile()
{
	try
	{
		fs::create_directories(GetChallengeModeFilePath().parent_path());
		cista::write<cista::mode::NONE>(GetChallengeModeFilePath(), challenge_mode_progress);
	}
	catch (const std::exception& e)
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error writing challenge mode file: %s", MOD_NAME, MOD_VERSION, e.what());
	}
}

bool ReadChallengeModeFile()
{
	if (!fs::exists(GetChallengeModeFilePath()))
		return false;

	try
	{
		auto wrapped = cista::read<ChallengeModeProgress, cista::mode::NONE>(GetChallengeModeFilePath());
		challenge_mode_progress = *wrapped;
		return true;
	}
	catch (const std::exception& e)
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error reading challenge mode file: %s", MOD_NAME, MOD_VERSION, e.what());
		return false;
	}
}

void RemoveItemsFromInventoryForChallengeMode()
{
	std::map<int, int> item_id_to_inventory_count_map = {};

	for (auto item_id : salve_items)
	{
		int count = InventoryCountItem(item_id, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
		item_id_to_inventory_count_map[item_id] = count;
	}

	for (auto item_id : dread_contract_items)
	{
		int count = InventoryCountItem(item_id, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
		item_id_to_inventory_count_map[item_id] = count;
	}

	for (auto& pair : sigil_to_item_id_map)
	{
		int count = InventoryCountItem(pair.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : greater_sigil_to_item_id_map)
	{
		int count = InventoryCountItem(pair.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : item_id_to_inventory_count_map)
	{
		if (pair.second > 0)
			InventoryRemoveItem(pair.first, pair.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]);
	}
}

void DropItemsForChallengeMode(CInstance* Self, CInstance* Other)
{
	static const std::pair<int, int> upper_mines_start_location = { 96 + 8, 272 + 8 };
	static const std::pair<int, int> tide_caverns_start_location = { 432 + 8, 176 + 8 };
	static const std::pair<int, int> deep_earth_start_location = { 304 + 8, 240 + 8 };
	static const std::pair<int, int> lava_caves_start_location = { 304 + 8, 272 + 8 };
	static const std::pair<int, int> ruins_start_location = { 576 + 8, 320 + 8 };

	if (floor_number % 20 == 1 && floor_number <= 81)
	{
		std::pair<int, int> item_spawn_point = {};
		if (floor_number == 1)
			item_spawn_point = upper_mines_start_location;
		else if (floor_number == 21)
			item_spawn_point = tide_caverns_start_location;
		else if (floor_number == 41)
			item_spawn_point = deep_earth_start_location;
		else if (floor_number == 61)
			item_spawn_point = lava_caves_start_location;
		else
			item_spawn_point = ruins_start_location;

		DropItem(item_name_to_id_map[MISTPOOL_SWORD_NAME], item_spawn_point.first, item_spawn_point.second, Self, Other);

		for (std::string mistpool_armor_name : MISTPOOL_ARMOR_NAMES)
			DropItem(item_name_to_id_map[mistpool_armor_name], item_spawn_point.first, item_spawn_point.second, Self, Other);

		for (size_t i = 0; i < 60; i++)
			DropItem(item_name_to_id_map[HEALTH_SALVE_NAME], item_spawn_point.first, item_spawn_point.second, Self, Other);

		for (size_t i = 0; i < 60; i++)
			DropItem(item_name_to_id_map[STAMINA_SALVE_NAME], item_spawn_point.first, item_spawn_point.second, Self, Other);

		for (size_t i = 0; i < 20; i++)
			DropItem(item_name_to_id_map[MANA_SALVE_NAME], item_spawn_point.first, item_spawn_point.second, Self, Other);

		for (size_t i = 0; i < 60; i++)
			DropItem(item_name_to_id_map[SUSTAINING_POTION_NAME], item_spawn_point.first, item_spawn_point.second, Self, Other);

		for (auto& [name, count] : challenge_mode_progress.starting_inventory)
			for (int i = 0; i < count; i++)
				DropItem(item_name_to_id_map[name.c_str()], item_spawn_point.first, item_spawn_point.second, Self, Other);
	}
}

void UpdateChallengeModeProgress()
{
	std::map<int, int> item_id_to_inventory_count_map = {};

	for (auto& pair : sigil_to_item_id_map)
	{
		int count = InventoryCountItem(pair.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : greater_sigil_to_item_id_map)
	{
		int count = InventoryCountItem(pair.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : item_id_to_inventory_count_map)
		if (pair.second > 0)
			challenge_mode_progress.starting_inventory[item_id_to_name_map[pair.first]] = pair.second;

	challenge_mode_progress.highest_floor_reached = floor_number;
}
