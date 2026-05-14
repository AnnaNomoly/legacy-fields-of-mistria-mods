#include "../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Maps;

static enum ArmorSlot
{
	HELMET,
	CHESTPIECE,
	GLOVES,
	PANTS,
	BOOTS
};

static fs::path GetChallengeModeFilePath()
{
	std::string filename = save_prefix + ".bin";
	return fs::current_path() / "mod_data" / "DeepDungeon" / filename;
}

static bool ItemInInventory(int item_id)
{
	int count = MMAPI::Inventory::CountItem(item_id).ToInt64();
	return count > 0;
}

static bool DeepDungeonArmorTypeEquipped(std::set<std::string> equipped_armor, ArmorSlot armor_slot)
{
	for (std::string armor : equipped_armor)
	{
		switch (armor_slot)
		{
			case HELMET:
				if (armor == MISTPOOL_HELMET_NAME || armor == CLERIC_HELMET_NAME || armor == DARK_KNIGHT_HELMET_NAME || armor == MAGE_HELMET_NAME || armor == PALADIN_HELMET_NAME || armor == ROGUE_HELMET_NAME || armor == ORACLE_HELMET_NAME)
					return true;
				break;
			case CHESTPIECE:
				if (armor == MISTPOOL_CHESTPIECE_NAME || armor == CLERIC_CHESTPIECE_NAME || armor == DARK_KNIGHT_CHESTPIECE_NAME || armor == MAGE_CHESTPIECE_NAME || armor == PALADIN_CHESTPIECE_NAME || armor == ROGUE_CHESTPIECE_NAME || armor == ORACLE_CHESTPIECE_NAME)
					return true;
				break;
			case GLOVES:
				if (armor == MISTPOOL_GLOVES_NAME || armor == CLERIC_GLOVES_NAME || armor == DARK_KNIGHT_GLOVES_NAME || armor == MAGE_GLOVES_NAME || armor == PALADIN_GLOVES_NAME || armor == ROGUE_GLOVES_NAME || armor == ORACLE_GLOVES_NAME)
					return true;
				break;
			case PANTS:
				if (armor == MISTPOOL_PANTS_NAME || armor == CLERIC_PANTS_NAME || armor == DARK_KNIGHT_PANTS_NAME || armor == MAGE_PANTS_NAME || armor == PALADIN_PANTS_NAME || armor == ROGUE_PANTS_NAME || armor == ORACLE_PANTS_NAME)
					return true;
				break;
			case BOOTS:
				if (armor == MISTPOOL_BOOTS_NAME || armor == CLERIC_BOOTS_NAME || armor == DARK_KNIGHT_BOOTS_NAME || armor == MAGE_BOOTS_NAME || armor == PALADIN_BOOTS_NAME || armor == ROGUE_BOOTS_NAME || armor == ORACLE_BOOTS_NAME)
					return true;
				break;
		}
	}

	return false;
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

		if (challenge_mode_progress.run_in_progress)
		{
			challenge_mode_progress = {};
			WriteChallengeModeFile();
		}

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
		int count = MMAPI::Inventory::CountItem(item_id).ToInt64();
		item_id_to_inventory_count_map[item_id] = count;
	}

	for (auto item_id : dread_contract_items)
	{
		int count = MMAPI::Inventory::CountItem(item_id).ToInt64();
		item_id_to_inventory_count_map[item_id] = count;
	}

	for (auto& pair : sigil_to_item_id_map)
	{
		int count = MMAPI::Inventory::CountItem(pair.second).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : greater_sigil_to_item_id_map)
	{
		int count = MMAPI::Inventory::CountItem(pair.second).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : item_id_to_inventory_count_map)
	{
		if (pair.second > 0)
			MMAPI::Inventory::RemoveItem(pair.first, pair.second);
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

		std::set<std::string> equipped_armor = GetEquippedArmor();

		if (!ItemInInventory(item_name_to_id_map[MISTPOOL_SWORD_NAME]))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_SWORD_NAME], item_spawn_point.first, item_spawn_point.second);
		if (!ItemInInventory(item_name_to_id_map[MISTPOOL_PICK_AXE_NAME]))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], item_spawn_point.first, item_spawn_point.second);
		if (!DeepDungeonArmorTypeEquipped(equipped_armor, ArmorSlot::HELMET))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_HELMET_NAME], item_spawn_point.first, item_spawn_point.second);
		if (!DeepDungeonArmorTypeEquipped(equipped_armor, ArmorSlot::CHESTPIECE))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME], item_spawn_point.first, item_spawn_point.second);
		if (!DeepDungeonArmorTypeEquipped(equipped_armor, ArmorSlot::GLOVES))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_GLOVES_NAME], item_spawn_point.first, item_spawn_point.second);
		if (!DeepDungeonArmorTypeEquipped(equipped_armor, ArmorSlot::PANTS))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_PANTS_NAME], item_spawn_point.first, item_spawn_point.second);
		if (!DeepDungeonArmorTypeEquipped(equipped_armor, ArmorSlot::BOOTS))
			MMAPI::Item::Drop(item_name_to_id_map[MISTPOOL_BOOTS_NAME], item_spawn_point.first, item_spawn_point.second);

		challenge_mode_bulk_given_item_ids.clear();
		challenge_mode_item_drop_quantities.clear();
		challenge_mode_item_drop_quantities[item_name_to_id_map[HEALTH_SALVE_NAME]] = 60;
		challenge_mode_item_drop_quantities[item_name_to_id_map[STAMINA_SALVE_NAME]] = 60;
		challenge_mode_item_drop_quantities[item_name_to_id_map[MANA_SALVE_NAME]] = 20;
		challenge_mode_item_drop_quantities[item_name_to_id_map[SUSTAINING_POTION_NAME]] = 60;
		for (auto& [name, count] : challenge_mode_progress.starting_inventory)
			challenge_mode_item_drop_quantities[item_name_to_id_map[name.c_str()]] = count;

		MMAPI::Item::Drop(item_name_to_id_map[HEALTH_SALVE_NAME], item_spawn_point.first, item_spawn_point.second);
		MMAPI::Item::Drop(item_name_to_id_map[STAMINA_SALVE_NAME], item_spawn_point.first, item_spawn_point.second);
		MMAPI::Item::Drop(item_name_to_id_map[MANA_SALVE_NAME], item_spawn_point.first, item_spawn_point.second);
		MMAPI::Item::Drop(item_name_to_id_map[SUSTAINING_POTION_NAME], item_spawn_point.first, item_spawn_point.second);
		for (auto& [name, count] : challenge_mode_progress.starting_inventory)
			MMAPI::Item::Drop(item_name_to_id_map[name.c_str()], item_spawn_point.first, item_spawn_point.second);
	}
}

void UpdateChallengeModeProgress()
{
	std::map<int, int> item_id_to_inventory_count_map = {};
	challenge_mode_progress.starting_inventory.clear();

	for (auto& pair : sigil_to_item_id_map)
	{
		int count = MMAPI::Inventory::CountItem(pair.second).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : greater_sigil_to_item_id_map)
	{
		int count = MMAPI::Inventory::CountItem(pair.second).ToInt64();
		item_id_to_inventory_count_map[pair.second] = count;
	}

	for (auto& pair : item_id_to_inventory_count_map)
		if (pair.second > 0)
			challenge_mode_progress.starting_inventory[item_id_to_name_map[pair.first]] = pair.second;

	challenge_mode_progress.highest_floor_reached = floor_number;
}