#include <map>
#include <set>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "StatueOfBoons";
static const char* const VERSION = "2.0.0";
static const char* const MANA_COST_KEY = "mana_cost"; // Used in mod config file
static const char* const ESSENCE_COST_KEY = "essence_cost"; // Used in mod config file
static const char* const PREVIOUS_BOONS_LIMIT_KEY = "previous_boons_limit"; // Used in mod config file
static const char* const ACTIVE_BOON_KEY = "active_boon"; // Used in mod save file
static const char* const PREVIOUS_BOONS_KEY = "previous_boons"; // Used in mod save file
static const char* const DRAGON_FAIRY_LOCATION_KEY = "dragon_fairy_location"; // Used in mod save file
static const char* const DRAGON_FAIRY_CAUGHT_KEY = "dragon_fairy_caught"; // Used in mod save file
static const char* const GML_SCRIPT_TRY_STRING_TO_ITEM_ID = "gml_Script_try_string_to_item_id";
static const char* const GML_SCRIPT_RENOWN_LEVEL_TO_INDIVIDUAL_COST = "gml_Script_renown_level_individual_cost";
static const char* const GML_SCRIPT_TRY_OBJECT_ID_TO_STRING = "gml_Script_try_object_id_to_string";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const char* const GML_SCRIPT_SPAWN_BUG = "gml_Script_spawn_bug";
static const char* const GML_SCRIPT_GET_MANA = "gml_Script_get_mana@Ari@Ari";
static const char* const GML_SCRIPT_GET_ESSENCE = "gml_Script_get_essence@Ari@Ari";
static const char* const GML_SCRIPT_MODIFY_MANA = "gml_Script_modify_mana@Ari@Ari";
static const char* const GML_SCRIPT_MODIFY_ESSENCE = "gml_Script_modify_essence@Ari@Ari";
static const char* const GML_SCRIPT_GET_FISHING_CELEBRATION_DATA = "gml_Script_get_celebration_data_essence_exp@anon@15053@Fish@Fish";
static const char* const GML_SCRIPT_GET_DIVING_CELEBRATION_DATA = "gml_Script_get_celebration_data@anon@15884@DiveSpot@Fish";
static const char* const GML_SCRIPT_GIVE_ARI_ITEM = "gml_Script_give_item@Ari@Ari";
static const char* const GML_SCRIPT_GET_MOVE_SPEED = "gml_Script_get_move_speed@Ari@Ari";
static const char* const GML_SCRIPT_INTERACT = "gml_Script_interact";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_SHOW_ROOM_TITLE = "gml_Script_show_room_title";
static const char* const GML_SCRIPT_ON_ROOM_START = "gml_Script_on_room_start@WeatherManager@Weather";
static const char* const GML_SCRIPT_CREATE_BUG = "gml_Script_setup@gml_Object_obj_bug_Create_0";
static const char* const GML_SCRIPT_ADD_HEART_POINTS = "gml_Script_add_heart_points@Npc@Npc";
static const char* const GML_SCRIPT_MODIFY_STAMINA = "gml_Script_modify_stamina@Ari@Ari";
static const char* const GML_SCRIPT_TRY_LOCATION_ID_TO_STRING = "gml_Script_try_location_id_to_string";
static const char* const GML_SCRIPT_END_DAY = "gml_Script_end_day";
static const char* const GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION = "gml_Script_write_furniture_to_location";
static const char* const GML_SCRIPT_ERASE_OBJECT_RENDERER = "gml_Script_erase_object_renderer";
static const char* const GML_SCRIPT_GET_LOCALIZER = "gml_Script_get@Localizer@Localizer";
static const char* const GML_SCRIPT_PLAY_TEXT = "gml_Script_play_text@TextboxMenu@TextboxMenu";
static const char* const GML_SCRIPT_PLAY_CONVERSATION = "gml_Script_play_conversation";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_SAVE_GAME = "gml_Script_save_game";
static const char* const GML_SCRIPT_LOAD_GAME = "gml_Script_load_game";
static const std::string NONE = "none";
static const std::string FARM_LOCATION_NAME = "farm";
static const std::string OBJECT_CATEGORY_BUSH = "bush";
static const std::string OBJECT_CATEGORY_CROP = "crop";
static const std::string OBJECT_CATEGORY_ROCK = "rock";
static const std::string OBJECT_CATEGORY_STUMP = "stump";
static const std::string OBJECT_CATEGORY_TREE = "tree";
static const std::string CUSTOM_ITEM_NAME = "dragon_fairy";
static const std::string UNIDENTIFIED_ARTIFACT_ITEM_NAME = "unidentified_artifact";
static const std::string CUSTOM_OBJECT_NAME = "statue_of_boons";
static const std::string BOON_OF_SPEED = "boon_of_speed";
static const std::string BOON_OF_FORAGE = "boon_of_forage";
static const std::string BOON_OF_FISHING = "boon_of_fishing";
static const std::string BOON_OF_BUTTERFLY = "boon_of_butterfly";
static const std::string BOON_OF_FRIENDSHIP = "boon_of_friendship";
static const std::string BOON_OF_STAMINA = "boon_of_stamina";
static const std::string BOON_OF_MANA = "boon_of_mana";
static const std::string WESTERN_RUINS = "western_ruins";
static const std::string EASTERN_ROAD = "eastern_road";
static const std::string NARROWS = "narrows";
static const std::string HAYDENS_FARM = "haydens_farm";
static const std::string BEACH = "beach";
static const std::string STATUE_OF_BOONS_INTERACT_KEY = "misc_local/Mods/Statue of Boons/interact";
static const std::string STATUE_OF_BOONS_CONVERSATION_KEY = "Conversations/Mods/Statue of Boons/statue_of_boons";
static const std::string STATUE_OF_BOONS_PLACEHOLDER_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/placeholder";
static const std::string STATUE_OF_BOONS_INSUFFICIENT_MANA_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/insufficient_mana";
static const std::string STATUE_OF_BOONS_INSUFFICIENT_ESSENCE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/insufficient_essence";
static const std::string STATUE_OF_BOONS_BOON_OF_SPEED_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_speed/granted"; // "Boon of the Wind"
static const std::string STATUE_OF_BOONS_BOON_OF_FORAGE_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_forage/granted"; // "Boon of the Land"
static const std::string STATUE_OF_BOONS_BOON_OF_FISHING_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_fishing/granted"; // "Boon of the Sea"
static const std::string STATUE_OF_BOONS_BOON_OF_BUTTERFLY_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_butterfly/granted"; // "Boon of the Dragons"
static const std::string STATUE_OF_BOONS_BOON_OF_FRIENDSHIP_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_friendship/granted"; // "Boon of the Stars"
static const std::string STATUE_OF_BOONS_BOON_OF_STAMINA_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_stamina/granted"; // "Boon of the Sun"
static const std::string STATUE_OF_BOONS_BOON_OF_MANA_GRANTED_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_mana/granted"; // "Boon of the Moon"
static const std::string STATUE_OF_BOONS_BOON_OF_SPEED_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_speed/active";
static const std::string STATUE_OF_BOONS_BOON_OF_FORAGE_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_forage/active";
static const std::string STATUE_OF_BOONS_BOON_OF_FISHING_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_fishing/active";
static const std::string STATUE_OF_BOONS_BOON_OF_BUTTERFLY_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_butterfly/active";
static const std::string STATUE_OF_BOONS_BOON_OF_FRIENDSHIP_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_friendship/active";
static const std::string STATUE_OF_BOONS_BOON_OF_STAMINA_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_stamina/active";
static const std::string STATUE_OF_BOONS_BOON_OF_MANA_ACTIVE_DIALOGUE_KEY = "Conversations/Mods/Statue of Boons/boon_of_mana/active";
static const std::string BOON_OF_BUTTERFLY_DETECTED_DIALOGUE_KEY = "Notifications/Mods/Statue of Boons/boon_of_butterfly/detected";
static const std::vector<std::string> LIST_OF_BOONS = { BOON_OF_SPEED, BOON_OF_FORAGE, BOON_OF_FISHING, BOON_OF_BUTTERFLY, BOON_OF_FRIENDSHIP, BOON_OF_STAMINA, BOON_OF_MANA };
static const std::vector<std::string> LIST_OF_LOCATIONS = { WESTERN_RUINS, EASTERN_ROAD, NARROWS, HAYDENS_FARM, BEACH };
static const std::map<std::string, std::vector<std::vector<int>>> CUSTOM_BUG_SPAWN_LOCATIONS = {
	{WESTERN_RUINS, {{145,128},{177,98},{177,166}}}, // working
	{EASTERN_ROAD, {{63,69},{131,69},{31,127},{190,95},{114,144},{39,186},{111,241},{32,259}}}, // working
	{NARROWS, {{138,52},{41,108},{130,117},{176,145},{114,189},{195,217}}}, // working
	{HAYDENS_FARM, {{73,45},{49,83},{70,105},{164,54},{185,115},{121,104}}}, // working
	{BEACH, {{303,77},{162,45},{175,75},{267,74},{259,37}}} // working
};
static const int DEFAULT_MANA_COST = 1;
static const int DEFAULT_ESSENCE_COST = 5;
static const int DEFAULT_PREVIOUS_BOONS_LIMIT = 7;

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static AurieStatus status = AURIE_SUCCESS;
static bool load_on_start = true;
static bool game_is_active = false;
static bool once_per_save_load = true;
static bool custom_object_used = false;
static int mana_cost = DEFAULT_MANA_COST;
static int essence_cost = DEFAULT_ESSENCE_COST;
static int previous_boons_limit = DEFAULT_PREVIOUS_BOONS_LIMIT;
static int ari_current_mana = -1;
static int ari_current_essence = -1;
static bool reduce_ari_mana = false;
static bool reduce_ari_essence = false;
static double ari_x = 0;
static double ari_y = 0;
static std::string ari_current_location = NONE;
static RValue custom_conversation_value;
static RValue* custom_conversation_value_ptr = nullptr;
static RValue custom_dialogue_value;
static RValue* custom_dialogue_value_ptr = nullptr;
static bool boon_of_speed = false;
static bool boon_of_forage = false;
static bool boon_of_fishing = false;
static bool boon_of_butterfly = false;
static bool boon_of_friendship = false;
static bool boon_of_stamina = false;
static bool boon_of_mana = false;
static bool modify_items_added = false;
static bool dragon_fairy_caught = false; // Used to track if the bug has been caught
static bool spawning_dragon_fairy = false; // Used to track when the bug is being spawned during script call stack
static std::string dragon_fairy_location = NONE; // Used to track the randomly selected location
static std::set<std::string> previous_boons = {};
static int dragon_fairy_item_id = -1;
static int unidentified_artifact_item_id = -1;
static std::map<int, int> renown_level_to_cumulative_required_points_map = {};
static std::map<int, int> spell_id_to_default_cost_map = {};
static std::map<std::string, int> object_category_to_id_map = {};
static std::vector<int> forage_boon_objects = {};
static std::map<int, std::string> object_id_to_name_map = {};
static std::set<std::pair<int, int>> statue_of_boons_positions = {};
static std::random_device rd;
static std::mt19937 gen(rd());
static std::string save_prefix = "";
static std::string mod_folder = "";
static RValue custom_interact_key;
static RValue* custom_interact_key_ptr = nullptr;

RValue TryStringToItemId(CInstance* Self, CInstance* Other, std::string item_name_str)
{
	CScript* gml_script_try_string_to_item_id = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TRY_STRING_TO_ITEM_ID,
		(PVOID*)&gml_script_try_string_to_item_id
	);

	RValue item_id;
	RValue item_name = RValue(item_name_str);
	RValue* item_name_ptr = &item_name;

	gml_script_try_string_to_item_id->m_Functions->m_ScriptFunction(
		Self,
		Other,
		item_id,
		1,
		{ &item_name_ptr }
	);

	return item_id;
}

RValue RenownLevelToIndividualCost(CInstance* Self, CInstance* Other, double value)
{
	CScript* gml_script_renown_level_to_individual_cost = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_RENOWN_LEVEL_TO_INDIVIDUAL_COST,
		(PVOID*)&gml_script_renown_level_to_individual_cost
	);

	RValue renown_points_required;
	RValue renown_level = value;
	RValue* renown_level_ptr = &renown_level;

	gml_script_renown_level_to_individual_cost->m_Functions->m_ScriptFunction(
		Self,
		Other,
		renown_points_required,
		1,
		{ &renown_level_ptr }
	);

	return renown_points_required;
}

bool ItemHasBeenAcquired(int item_id)
{
	RValue __ari = global_instance->GetMember("__ari");
	RValue items_acquired = __ari.GetMember("items_acquired");
	RValue item_acquired = g_ModuleInterface->CallBuiltin("array_get", { items_acquired, item_id });
	return item_acquired.ToBoolean();
}

void LoadRenownData(CInstance* Self, CInstance* Other)
{
	for (int i = 1; i <= 100; i++)
	{
		RValue renown_points_required = RenownLevelToIndividualCost(Self, Other, i);
		renown_level_to_cumulative_required_points_map[i] = i == 1 ? renown_points_required.m_Real : renown_points_required.m_Real + renown_level_to_cumulative_required_points_map[i - 1];
	}
}

void ModifyDragonFairyPrice()
{
	// Get the current renown level.
	int current_renown_level = 1;
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue renown = *__ari.GetRefMember("renown");
	for (int i = 100; i > 0; i--)
	{
		int cumulative_renown_required = renown_level_to_cumulative_required_points_map[i];
		if (renown.ToInt64() >= cumulative_renown_required)
		{
			current_renown_level = i;
			break;
		}
	}

	// Dragon Fairy item.
	RValue* dragon_fairy_item;
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	g_ModuleInterface->GetArrayEntry(__item_data, dragon_fairy_item_id, dragon_fairy_item);

	// Modify the item's value.
	*dragon_fairy_item->GetRefMember("value")->GetRefMember("bin") = current_renown_level * 500;
}

void LoadSpells()
{
	size_t array_length = 0;
	RValue spells = global_instance->GetMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		spell_id_to_default_cost_map[i] = array_element->GetMember("cost").ToInt64();
	}
}

void ModifySpellCosts(bool boon_of_mana) {
	size_t array_length = 0;
	RValue spells = *global_instance->GetRefMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		int cost = boon_of_mana ? 0 : spell_id_to_default_cost_map[i];
		*array_element->GetRefMember("cost") = cost;
	}
}

void LoadObjectCategories()
{
	size_t array_length = 0;
	RValue object_categories = global_instance->GetMember("__object_category__");
	g_ModuleInterface->GetArraySize(object_categories, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* object_category;
		g_ModuleInterface->GetArrayEntry(object_categories, i, object_category);

		std::string object_category_str = object_category->ToString();
		object_category_to_id_map[object_category_str] = i;
	}
}

void LoadObjectIds(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_try_object_id_to_string = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TRY_OBJECT_ID_TO_STRING,
		(PVOID*)&gml_script_try_object_id_to_string
	);

	for (int i = 0; i < 5000; i++)
	{
		RValue result;
		RValue object_id = i;
		RValue* object_id_ptr = &object_id;
		gml_script_try_object_id_to_string->m_Functions->m_ScriptFunction(
			Self,
			Other,
			result,
			1,
			{ &object_id_ptr }
		);

		if (result.IsString()) // switch to type checking methods in v5
			object_id_to_name_map[i] = result.ToString();
	}

	DbgPrintEx(LOG_SEVERITY_TRACE, "[%s %s] - Loaded %d objects!", MOD_NAME, VERSION, static_cast<int>(object_id_to_name_map.size()));
}

void LoadObjectItemData()
{
	size_t array_length = 0;
	RValue node_prototypes = global_instance->GetMember("__node_prototypes");
	g_ModuleInterface->GetArraySize(node_prototypes, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* node_prototype;
		g_ModuleInterface->GetArrayEntry(node_prototypes, i, node_prototype);

		bool object_is_eligible = false;
		int object_id = node_prototype->GetMember("object_id").ToInt64(); //RValueAsInt(StructVariableGet(*node_prototype, "object_id"));
		int category_id = node_prototype->GetMember("category_id").ToInt64(); //RValueAsInt(StructVariableGet(*node_prototype, "category_id"));

		if (category_id == object_category_to_id_map[OBJECT_CATEGORY_BUSH] || category_id == object_category_to_id_map[OBJECT_CATEGORY_CROP])		
			object_is_eligible = true;
		if (category_id == object_category_to_id_map[OBJECT_CATEGORY_ROCK] || category_id == object_category_to_id_map[OBJECT_CATEGORY_STUMP] || category_id == object_category_to_id_map[OBJECT_CATEGORY_TREE])
			object_is_eligible = true;

		if (object_is_eligible)
		{
			auto it = std::find(forage_boon_objects.begin(), forage_boon_objects.end(), object_id);
			if (it == forage_boon_objects.end())
				forage_boon_objects.push_back(object_id);
		}
	}
}

void CreateNotification(std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_NOTIFICATION,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = RValue(notification_localization_str);
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);
}

RValue SpawnBug(CInstance* Self, CInstance* Other, int x_coord, int y_coord)
{
	CScript* gml_script_spawn_bug = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_BUG,
		(PVOID*)&gml_script_spawn_bug
	);

	RValue result;
	RValue x = x_coord;
	RValue y = y_coord;
	RValue undefined;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* undefined_ptr = &undefined;
	RValue* argument_array[3] = { x_ptr, y_ptr, undefined_ptr };

	spawning_dragon_fairy = true;
	gml_script_spawn_bug->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		argument_array
	);

	return result;
}

RValue GetCurrentMana(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MANA,
		(PVOID*)&gml_script_get_mana
	);

	RValue current_mana;
	gml_script_get_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		current_mana,
		0,
		nullptr
	);

	return current_mana;
}

RValue GetCurrentEssence(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_essence = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_ESSENCE,
		(PVOID*)&gml_script_get_essence
	);

	RValue current_essence;
	gml_script_get_essence->m_Functions->m_ScriptFunction(
		Self,
		Other,
		current_essence,
		0,
		nullptr
	);

	return current_essence;
}

void ModifyMana(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_MANA,
		(PVOID*)&gml_script_modify_mana
	);

	RValue result;
	RValue mana_modifier = value;
	RValue* mana_modifier_ptr = &mana_modifier;

	gml_script_modify_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &mana_modifier_ptr }
	);
}

void ModifyEssence(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_essence = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_ESSENCE,
		(PVOID*)&gml_script_modify_essence
	);

	RValue result;
	RValue essence_modifier = value;
	RValue* essence_modifier_ptr = &essence_modifier;

	gml_script_modify_essence->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &essence_modifier_ptr }
	);
}

bool AnyBoonIsActive()
{
	if (boon_of_speed || boon_of_forage || boon_of_fishing || boon_of_butterfly || boon_of_friendship || boon_of_stamina || boon_of_mana)
		return true;
	return false;
}

std::string GetActiveBoonString()
{
	if (boon_of_speed)
		return BOON_OF_SPEED;
	if (boon_of_forage)
		return BOON_OF_FORAGE;
	if (boon_of_fishing)
		return BOON_OF_FISHING;
	if (boon_of_butterfly)
		return BOON_OF_BUTTERFLY;
	if (boon_of_friendship)
		return BOON_OF_FRIENDSHIP;
	if (boon_of_stamina)
		return BOON_OF_STAMINA;
	if (boon_of_mana)
		return BOON_OF_MANA;
	return NONE;
}

void LoadActiveBoonString(std::string input)
{
	if (input == BOON_OF_SPEED)
		boon_of_speed = true;
	if (input == BOON_OF_FORAGE)
		boon_of_forage = true;
	if (input == BOON_OF_FISHING)
		boon_of_fishing = true;
	if (input == BOON_OF_BUTTERFLY)
		boon_of_butterfly = true;
	if (input == BOON_OF_FRIENDSHIP)
		boon_of_friendship = true;
	if (input == BOON_OF_STAMINA)
		boon_of_stamina = true;
	if (input == BOON_OF_MANA)
		boon_of_mana = true;
}

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}

void WriteModSaveFile()
{
	// Write the custom mod data file.
	if (save_prefix.length() != 0 && mod_folder.length() != 0)
	{
		json mod_save_data = {};
		mod_save_data[ACTIVE_BOON_KEY] = GetActiveBoonString();
		mod_save_data[PREVIOUS_BOONS_KEY] = previous_boons;
		mod_save_data[DRAGON_FAIRY_LOCATION_KEY] = dragon_fairy_location;
		mod_save_data[DRAGON_FAIRY_CAUGHT_KEY] = dragon_fairy_caught;

		std::exception_ptr eptr;
		try
		{
			std::ofstream out_stream(mod_folder + "\\" + save_prefix + ".json");
			out_stream << std::setw(4) << mod_save_data << std::endl;
			out_stream.close();
			DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Successfully saved the mod file!", MOD_NAME, VERSION);
		}
		catch (...)
		{
			DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - An error occurred writing the mod file.", MOD_NAME, VERSION);

			eptr = std::current_exception();
			PrintError(eptr);
		}
	}
}

void ReadModSaveFile()
{
	std::exception_ptr eptr;
	try
	{
		std::ifstream in_stream(mod_folder + "\\" + save_prefix + ".json");
		if (in_stream.good())
		{
			json mod_save_data = json::parse(in_stream);
			LoadActiveBoonString(mod_save_data[ACTIVE_BOON_KEY]);
			previous_boons = mod_save_data[PREVIOUS_BOONS_KEY];
			dragon_fairy_location = mod_save_data[DRAGON_FAIRY_LOCATION_KEY];
			dragon_fairy_caught = mod_save_data[DRAGON_FAIRY_CAUGHT_KEY];
			DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Successfully loaded the mod file!", MOD_NAME, VERSION);
		}
	}
	catch (...)
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - An error occurred reading the mod file.", MOD_NAME, VERSION);

		eptr = std::current_exception();
		PrintError(eptr);
	}
}

json CreateModConfigJson(bool use_defaults)
{
	json config_json = {
		{ MANA_COST_KEY, use_defaults ? DEFAULT_MANA_COST : mana_cost },
		{ ESSENCE_COST_KEY, use_defaults ? DEFAULT_ESSENCE_COST : essence_cost },
		{ PREVIOUS_BOONS_LIMIT_KEY, use_defaults ? DEFAULT_PREVIOUS_BOONS_LIMIT : previous_boons_limit }
	};
	return config_json;
}

void LogDefaultConfigValues()
{
	mana_cost = DEFAULT_MANA_COST;
	essence_cost = DEFAULT_ESSENCE_COST;
	previous_boons_limit = DEFAULT_PREVIOUS_BOONS_LIMIT;

	DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, MANA_COST_KEY, DEFAULT_MANA_COST);
	DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, ESSENCE_COST_KEY, DEFAULT_ESSENCE_COST);
	DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, PREVIOUS_BOONS_LIMIT_KEY, DEFAULT_PREVIOUS_BOONS_LIMIT);
}

void CreateOrLoadModConfigFile()
{
	// Load config file.
	std::exception_ptr eptr;
	try
	{
		// Try to find the mod_data directory.
		std::string current_dir = std::filesystem::current_path().string();
		std::string mod_data_folder = current_dir + "\\mod_data";
		if (!std::filesystem::exists(mod_data_folder))
		{
			DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
			std::filesystem::create_directory(mod_data_folder);
		}

		// Try to find the mod_data/StatueOfBoons directory.
		std::string statue_of_boons_folder = mod_data_folder + "\\StatueOfBoons";
		if (!std::filesystem::exists(statue_of_boons_folder))
		{
			DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - The \"StatueOfBoons\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, statue_of_boons_folder.c_str());
			std::filesystem::create_directory(statue_of_boons_folder);
		}

		mod_folder = statue_of_boons_folder;

		// Try to find the mod_data/StatueOfBoons/StatueOfBoons.json config file.
		bool update_config_file = false;
		std::string config_file = statue_of_boons_folder + "\\" + "StatueOfBoons.json";
		std::ifstream in_stream(config_file);
		if (in_stream.good())
		{
			try
			{
				json json_object = json::parse(in_stream);

				// Check if the json_object is empty.
				if (json_object.empty())
				{
					DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.c_str());
					DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Add your desired values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
					LogDefaultConfigValues();
				}
				else
				{
					// Try loading the mana_cost value.
					if (json_object.contains(MANA_COST_KEY))
					{
						mana_cost = json_object[MANA_COST_KEY];
						if (mana_cost < 0 || mana_cost > 16)
						{
							DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Invalid \"%s\" value (%d) in mod configuration file: %s", MOD_NAME, VERSION, MANA_COST_KEY, mana_cost, config_file.c_str());
							DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - This value MUST be a valid integer between 0 and 16 (inclusive)!", MOD_NAME, VERSION);
							DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, MANA_COST_KEY, DEFAULT_MANA_COST);
							mana_cost = DEFAULT_MANA_COST;
						}
						else
						{
							DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using CUSTOM \"%s\" value: %d!", MOD_NAME, VERSION, MANA_COST_KEY, mana_cost);
						}
					}
					else
					{
						DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, MANA_COST_KEY, config_file.c_str());
						DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, MANA_COST_KEY, DEFAULT_MANA_COST);
						mana_cost = DEFAULT_MANA_COST;
					}

					// Try loading the essence_cost value.
					if (json_object.contains(ESSENCE_COST_KEY))
					{
						essence_cost = json_object[ESSENCE_COST_KEY];
						if (essence_cost < 0 || essence_cost > 1000)
						{
							DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Invalid \"%s\" value (%d) in mod configuration file: %s", MOD_NAME, VERSION, ESSENCE_COST_KEY, essence_cost, config_file.c_str());
							DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - This value MUST be a valid integer between 0 and 1000 (inclusive)!", MOD_NAME, VERSION);
							DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, ESSENCE_COST_KEY, DEFAULT_ESSENCE_COST);
							essence_cost = DEFAULT_ESSENCE_COST;
						}
						else
						{
							DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using CUSTOM \"%s\" value: %d!", MOD_NAME, VERSION, ESSENCE_COST_KEY, essence_cost);
						}
					}
					else
					{
						DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, ESSENCE_COST_KEY, config_file.c_str());
						DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, ESSENCE_COST_KEY, DEFAULT_ESSENCE_COST);
						essence_cost = DEFAULT_ESSENCE_COST;
					}

					// Try loading the previous_boons_limit value.
					if (json_object.contains(PREVIOUS_BOONS_LIMIT_KEY))
					{
						previous_boons_limit = json_object[PREVIOUS_BOONS_LIMIT_KEY];
						if (previous_boons_limit < 0 || previous_boons_limit > 7)
						{
							DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Invalid \"%s\" value (%d) in mod configuration file: %s", MOD_NAME, VERSION, PREVIOUS_BOONS_LIMIT_KEY, previous_boons_limit, config_file.c_str());
							DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - This value MUST be a valid integer between 0 and 7 (inclusive)!", MOD_NAME, VERSION);
							DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, PREVIOUS_BOONS_LIMIT_KEY, DEFAULT_PREVIOUS_BOONS_LIMIT);
							previous_boons_limit = DEFAULT_PREVIOUS_BOONS_LIMIT;
						}
						else
						{
							DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using CUSTOM \"%s\" value: %d!", MOD_NAME, VERSION, PREVIOUS_BOONS_LIMIT_KEY, previous_boons_limit);
						}
					}
					else
					{
						DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, PREVIOUS_BOONS_LIMIT_KEY, config_file.c_str());
						DbgPrintEx(LOG_SEVERITY_INFO, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, PREVIOUS_BOONS_LIMIT_KEY, DEFAULT_PREVIOUS_BOONS_LIMIT);
						previous_boons_limit = DEFAULT_PREVIOUS_BOONS_LIMIT;
					}
				}

				update_config_file = true;
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
				LogDefaultConfigValues();
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			DbgPrintEx(LOG_SEVERITY_WARNING, "[%s %s] - The \"StatueOfBoons.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateModConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();

			LogDefaultConfigValues();
		}

		if (update_config_file)
		{
			json config_json = CreateModConfigJson(false);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << config_json << std::endl;
			out_stream.close();
		}
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);
		LogDefaultConfigValues();
	}
}

double CalculateDistance(int x1, int y1, int x2, int y2) {
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

void FindStatueOfBoonsPositions()
{
	CRoom* current_room = nullptr;
	if (!AurieSuccess(g_ModuleInterface->GetCurrentRoomData(current_room)))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Failed to get data for the current room!", MOD_NAME, VERSION);
		return;
	}

	for (CInstance* inst = current_room->GetMembers().m_ActiveInstances.m_First; inst != nullptr; inst = inst->GetMembers().m_Flink)
	{
		auto map = inst->ToRValue().ToRefMap();
		if (!map.contains("node")) continue;

		RValue* nodeValue = map["node"];
		if (!nodeValue || nodeValue->GetKindName() != "struct") continue;

		auto nodeRefMap = nodeValue->ToRefMap();
		if (!nodeRefMap.contains("prototype")) continue;

		RValue* protoVal = nodeRefMap["prototype"];
		if (!protoVal || protoVal->GetKindName() != "struct") continue;

		auto protoMap = protoVal->ToRefMap();
		if (!protoMap.contains("object_id")) continue;

		int object_id = protoMap["object_id"]->ToInt64();
		if (object_id_to_name_map[object_id] == CUSTOM_OBJECT_NAME)
		{
			int x = nodeRefMap["top_left_x"]->ToInt64();
			int y = nodeRefMap["top_left_y"]->ToInt64();
			statue_of_boons_positions.emplace(x, y);
		}
	}
}

void ResetStaticFields(bool returned_to_title_screen)
{
	if (returned_to_title_screen)
	{
		game_is_active = false;
		once_per_save_load = true;
		ari_current_mana = -1;
		ari_current_essence = -1;
		ari_x = 0;
		ari_y = 0;
		ari_current_location = NONE;
		custom_conversation_value = RValue();
		custom_conversation_value_ptr = nullptr;
		custom_dialogue_value = RValue();
		custom_dialogue_value_ptr = nullptr;
		previous_boons = {};
		statue_of_boons_positions = {};
		save_prefix = "";
		custom_interact_key = "";
		custom_interact_key_ptr = nullptr;
	}

	custom_object_used = false;
	reduce_ari_mana = false;
	reduce_ari_essence = false;
	boon_of_speed = false;
	boon_of_forage = false;
	boon_of_fishing = false;
	boon_of_butterfly = false;
	boon_of_friendship = false;
	boon_of_stamina = false;
	boon_of_mana = false;
	modify_items_added = false;
	dragon_fairy_caught = false;
	spawning_dragon_fairy = false;
	dragon_fairy_location = NONE;
	ModifySpellCosts(boon_of_mana);
}

void ObjectCallback(
	IN FWCodeEvent& CodeEvent
)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self)
		return;

	if (!self->m_Object)
		return;

	if (strstr(self->m_Object->m_Name, "obj_ari"))
	{
		RValue x;
		g_ModuleInterface->GetBuiltin("x", self, NULL_INDEX, x);
		ari_x = x.ToDouble();

		RValue y;
		g_ModuleInterface->GetBuiltin("y", self, NULL_INDEX, y);
		ari_y = y.ToDouble();

		if (reduce_ari_mana)
		{
			reduce_ari_mana = false;
			ModifyMana(global_instance->GetRefMember("__ari")->ToInstance(), self, static_cast<double>(-1 * mana_cost));
		}
			
		if (reduce_ari_essence)
		{
			reduce_ari_essence = false;
			ModifyEssence(global_instance->GetRefMember("__ari")->ToInstance(), self, static_cast<double>(-1 * essence_cost));
		}

		ari_current_mana = GetCurrentMana(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
		ari_current_essence = GetCurrentEssence(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
	}
}

RValue& GmlScriptGetFishingCelebrationDataCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if(boon_of_fishing)
		modify_items_added = true;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_FISHING_CELEBRATION_DATA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetDivingCelebrationDataCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (boon_of_fishing)
		modify_items_added = true;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_DIVING_CELEBRATION_DATA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGiveItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (boon_of_butterfly && !dragon_fairy_caught && ari_current_location == dragon_fairy_location)
	{
		RValue item = Arguments[0]->m_Object;
		int item_id = item.GetMember("item_id").ToInt64();

		if(item_id == dragon_fairy_item_id)
			dragon_fairy_caught = true;
	}

	if (modify_items_added)
	{
		modify_items_added = false;
		if (ari_current_location != FARM_LOCATION_NAME)
		{
			RValue item = Arguments[0]->m_Object;
			int item_id = item.GetMember("item_id").ToInt64();

			// Prevent giving more than one of an unidentified artifact or unacquired item.
			if (item_id != unidentified_artifact_item_id && ItemHasBeenAcquired(item_id))
			{
				const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GIVE_ARI_ITEM));
				original(
					Self,
					Other,
					Result,
					ArgumentCount,
					Arguments
				);
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GIVE_ARI_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetMoveSpeedCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MOVE_SPEED));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (boon_of_speed)
		Result.m_Real += 0.5;
		
	return Result; // 2.0 is default run speed, max run speed boost from gear is 20% => 2.4
}

RValue& GmlScriptInteractCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	RValue object = Arguments[0]->m_Object;
	int object_id = object.GetMember("object_id").ToInt64();
	std::string object_name = object_id_to_name_map[object_id];

	// Statue of Boons
	if (object_name.find(CUSTOM_OBJECT_NAME) != std::string::npos)
		custom_object_used = true;
	else 
	{
		// Objects affected by Statue of Boons
		auto it = std::find(forage_boon_objects.begin(), forage_boon_objects.end(), object_id);
		if (it != forage_boon_objects.end())
		{
			if (boon_of_forage)
				modify_items_added = true;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_INTERACT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);
	return Result;
}

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	game_is_active = true;
	ModifySpellCosts(boon_of_mana);
	ModifyDragonFairyPrice();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_WEATHER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptShowRoomTitleCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SHOW_ROOM_TITLE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (boon_of_butterfly && !dragon_fairy_caught)
	{
		if (ari_current_location == dragon_fairy_location)
		{
			std::vector<std::vector<int>> possible_spawn_points = CUSTOM_BUG_SPAWN_LOCATIONS.at(dragon_fairy_location);
			std::uniform_int_distribution<> choose_random_spawn_point(0, static_cast<int>(possible_spawn_points.size() - 1));
			std::vector<int> random_spawn_point = possible_spawn_points[choose_random_spawn_point(gen)];
			SpawnBug(Self, Other, random_spawn_point[0], random_spawn_point[1]);
			CreateNotification(BOON_OF_BUTTERFLY_DETECTED_DIALOGUE_KEY, Self, Other);
		}
	}

	return Result;
}

RValue& GmlScriptOnRoomStartCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && once_per_save_load && ari_current_location == FARM_LOCATION_NAME)
	{
		once_per_save_load = false;
		FindStatueOfBoonsPositions();
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_ROOM_START));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptCreateBugCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (spawning_dragon_fairy)
	{
		spawning_dragon_fairy = false;
		Arguments[0]->m_i64 = dragon_fairy_item_id;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CREATE_BUG));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;  
}

RValue& GmlScriptAddHeartPointsCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (boon_of_friendship)
		Arguments[0]->m_Real = ceil(1.5 * Arguments[0]->m_Real);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ADD_HEART_POINTS));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptModifyStaminaCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (boon_of_stamina && Arguments[0]->m_Real < 0)
		Arguments[0]->m_Real = 0;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_STAMINA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptTryLocationIdToStringCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "gml_Script_try_location_id_to_string"));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && Result.m_Kind == VALUE_STRING)
		ari_current_location = Result.ToString();

	return Result;
}

RValue& GmlScriptEndDayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_END_DAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	std::string active_boon_str = GetActiveBoonString();
	if (active_boon_str != NONE)
		previous_boons.insert(active_boon_str);

	ResetStaticFields(false);
	
	return Result;
}

RValue& GmlScriptWriteFurnitureToLocationCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active)
	{
		if (Result.m_Kind != VALUE_UNDEFINED && Result.m_Kind != VALUE_UNSET && Result.m_Kind != VALUE_NULL)
		{
			RValue object_id = Result.GetMember("object_id");
			if (object_id_to_name_map[object_id.ToInt64()] == CUSTOM_OBJECT_NAME)
			{
				RValue top_left_x = Result.GetMember("top_left_x");
				RValue top_left_y = Result.GetMember("top_left_y");

				RValue write_size_x = Result.GetMember("write_size_x");
				RValue write_size_y = Result.GetMember("write_size_y");

				std::pair<int64_t, int64_t> position = { top_left_x.ToInt64(), top_left_y.ToInt64() };
				statue_of_boons_positions.insert(position);
			}
		}
	}

	return Result;
}

RValue& GmlScriptEraseObjectRendererCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ERASE_OBJECT_RENDERER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && ari_current_location == FARM_LOCATION_NAME)
	{
		int object_id = Arguments[0]->GetMember("object_id").ToInt64();
		if (object_id_to_name_map[object_id] == CUSTOM_OBJECT_NAME)
		{
			int x = Arguments[0]->GetMember("top_left_x").ToInt64();
			int y = Arguments[0]->GetMember("top_left_y").ToInt64();

			auto it = std::find_if(statue_of_boons_positions.begin(), statue_of_boons_positions.end(),
				[x, y](const std::pair<int, int>& p) {
					return p.first == x && p.second == y;
				});

			if (it != statue_of_boons_positions.end()) {
				statue_of_boons_positions.erase(it);
			}
		}
	}

	return Result;
}

RValue& GmlScriptGetLocalizerCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active)
	{
		if (ari_current_location == FARM_LOCATION_NAME && Arguments[0]->ToString() == "misc_local/crystal_ball")
		{
			for (auto it : statue_of_boons_positions)
			{
				int temp = CalculateDistance(ari_x, ari_y, it.first * 8 + 8, it.second * 8 + 8);
				if (CalculateDistance(ari_x, ari_y, it.first * 8 + 8, it.second * 8 + 8) <= 44)
				{
					custom_interact_key = RValue(STATUE_OF_BOONS_INTERACT_KEY);
					custom_interact_key_ptr = &custom_interact_key;
					Arguments[0] = custom_interact_key_ptr;
				}
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_LOCALIZER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptPlayTextCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	std::string conversation_name = Arguments[0]->ToString();
	if (conversation_name.find(STATUE_OF_BOONS_PLACEHOLDER_DIALOGUE_KEY) != std::string::npos)
	{
		// Check if a boon is already active.
		if (AnyBoonIsActive())
		{
			if (boon_of_speed)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_SPEED_ACTIVE_DIALOGUE_KEY);
			if (boon_of_forage)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_FORAGE_ACTIVE_DIALOGUE_KEY);
			if (boon_of_fishing)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_FISHING_ACTIVE_DIALOGUE_KEY);
			if (boon_of_butterfly)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_BUTTERFLY_ACTIVE_DIALOGUE_KEY);
			if (boon_of_friendship)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_FRIENDSHIP_ACTIVE_DIALOGUE_KEY);
			if (boon_of_stamina)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_STAMINA_ACTIVE_DIALOGUE_KEY);
			if (boon_of_mana)
				custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_MANA_ACTIVE_DIALOGUE_KEY);
		}
		else
		{
			// Check if mana or essence is required and that Ari has enough to activate the statue.
			bool statue_activation_requirements_met = true;
			if (ari_current_mana < mana_cost)
			{
				statue_activation_requirements_met = false;
				custom_dialogue_value = RValue(STATUE_OF_BOONS_INSUFFICIENT_MANA_DIALOGUE_KEY);
			}
			if (ari_current_essence < essence_cost)
			{
				statue_activation_requirements_met = false;
				custom_dialogue_value = RValue(STATUE_OF_BOONS_INSUFFICIENT_ESSENCE_DIALOGUE_KEY);
			}

			if (statue_activation_requirements_met)
			{
				if (mana_cost > 0)
					reduce_ari_mana = true;
				if (essence_cost > 0)
					reduce_ari_essence = true;

				if (previous_boons.size() > previous_boons_limit || previous_boons.size() == LIST_OF_BOONS.size())
					previous_boons = {};

				std::string random_boon = NONE;
				if (previous_boons.empty())
				{
					// Choose a random boon.
					std::uniform_int_distribution<> choose_random_boon(0, static_cast<int>(LIST_OF_BOONS.size() - 1));
					random_boon = LIST_OF_BOONS[choose_random_boon(gen)];
				}
				else
				{
					// Choose a random boon excluding the previous ones.
					std::vector<std::string> modified_boon_list = LIST_OF_BOONS;
					for(std::string previous_boon : previous_boons)
						modified_boon_list.erase(std::remove(modified_boon_list.begin(), modified_boon_list.end(), previous_boon), modified_boon_list.end());

					std::uniform_int_distribution<> choose_random_boon(0, static_cast<int>(modified_boon_list.size() - 1));
					random_boon = modified_boon_list[choose_random_boon(gen)];
				}

				if (random_boon == BOON_OF_SPEED)
				{
					boon_of_speed = true;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_SPEED_GRANTED_DIALOGUE_KEY);
				}
				if (random_boon == BOON_OF_FORAGE)
				{
					boon_of_forage = true;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_FORAGE_GRANTED_DIALOGUE_KEY);
				}
				if (random_boon == BOON_OF_FISHING)
				{
					boon_of_fishing = true;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_FISHING_GRANTED_DIALOGUE_KEY);
				}
				if (random_boon == BOON_OF_BUTTERFLY)
				{
					boon_of_butterfly = true;
					dragon_fairy_caught = false;
					spawning_dragon_fairy = false;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_BUTTERFLY_GRANTED_DIALOGUE_KEY);

					std::uniform_int_distribution<> choose_random_location(0, static_cast<int>(LIST_OF_LOCATIONS.size() - 1));
					dragon_fairy_location = LIST_OF_LOCATIONS[choose_random_location(gen)];
				}
				if (random_boon == BOON_OF_FRIENDSHIP)
				{
					boon_of_friendship = true;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_FRIENDSHIP_GRANTED_DIALOGUE_KEY);
				}
				if (random_boon == BOON_OF_STAMINA)
				{
					boon_of_stamina = true;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_STAMINA_GRANTED_DIALOGUE_KEY);
				}
				if (random_boon == BOON_OF_MANA)
				{
					boon_of_mana = true;
					custom_dialogue_value = RValue(STATUE_OF_BOONS_BOON_OF_MANA_GRANTED_DIALOGUE_KEY);

					ModifySpellCosts(boon_of_mana);
				}
			}
		}

		custom_dialogue_value_ptr = &custom_dialogue_value;
		Arguments[0] = custom_dialogue_value_ptr;
	}
	
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_TEXT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptPlayConversationCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (custom_object_used)
	{
		custom_object_used = false;
		custom_conversation_value = RValue(STATUE_OF_BOONS_CONVERSATION_KEY);
		custom_conversation_value_ptr = &custom_conversation_value;
		Arguments[1] = custom_conversation_value_ptr;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_CONVERSATION));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (load_on_start)
	{
		load_on_start = false;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		dragon_fairy_item_id = TryStringToItemId(Self, Other, CUSTOM_ITEM_NAME).ToInt64();
		unidentified_artifact_item_id = TryStringToItemId(Self, Other, UNIDENTIFIED_ARTIFACT_ITEM_NAME).ToInt64();
		LoadObjectCategories();
		LoadObjectIds(Self, Other);
		LoadObjectItemData();
		LoadRenownData(Self, Other);
		LoadSpells();
		CreateOrLoadModConfigFile();
	}
	else
		ResetStaticFields(true);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptSaveGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// No save prefix has been detected. This should only happen when a new game is started.
	if (save_prefix.size() == 0)
	{
		// Get the save file name.
		std::string save_file = Arguments[0]->ToString();
		std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
		std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);

		// Check it's a valid value.
		if (save_name.find("undefined") == std::string::npos)
		{
			// Get the save prefix.
			std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
			std::size_t second_hyphen_index = save_name.find_last_of("-");
			save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));
		}
	}

	// Write the custom mod data file.
	WriteModSaveFile();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SAVE_GAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptLoadGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Get the save file name.
	std::string save_file = Arguments[0]->ToInstance()->GetMember("save_path").ToString();
	std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
	std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);

	// Get the save prefix.
	std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
	std::size_t second_hyphen_index = save_name.find_last_of("-");
	save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));

	// Read from the custom mod data file.
	ReadModSaveFile();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_LOAD_GAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

void CreateObjectCallback(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook (EVENT_OBJECT_CALL)!", MOD_NAME, VERSION);
	}
}

void CreateHookGmlScriptGetFishingCelebrationData(AurieStatus& status)
{
	CScript* gml_script_get_fishing_celebration_data = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_FISHING_CELEBRATION_DATA,
		(PVOID*)&gml_script_get_fishing_celebration_data
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_FISHING_CELEBRATION_DATA);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_FISHING_CELEBRATION_DATA,
		gml_script_get_fishing_celebration_data->m_Functions->m_ScriptFunction,
		GmlScriptGetFishingCelebrationDataCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_FISHING_CELEBRATION_DATA);
	}
}

void CreateHookGmlScriptGetDivingCelebrationData(AurieStatus& status)
{
	CScript* gml_script_get_diving_celebration_data = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_DIVING_CELEBRATION_DATA,
		(PVOID*)&gml_script_get_diving_celebration_data
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_DIVING_CELEBRATION_DATA);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_DIVING_CELEBRATION_DATA,
		gml_script_get_diving_celebration_data->m_Functions->m_ScriptFunction,
		GmlScriptGetDivingCelebrationDataCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_DIVING_CELEBRATION_DATA);
	}
}

void CreateHookGmlScriptGiveItem(AurieStatus& status)
{
	CScript* gml_script_give_item = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GIVE_ARI_ITEM,
		(PVOID*)&gml_script_give_item
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GIVE_ARI_ITEM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GIVE_ARI_ITEM,
		gml_script_give_item->m_Functions->m_ScriptFunction,
		GmlScriptGiveItemCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GIVE_ARI_ITEM);
	}
}

void CreateHookGmlScriptGetMoveSpeed(AurieStatus& status)
{
	CScript* gml_script_get_move_speed = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MOVE_SPEED,
		(PVOID*)&gml_script_get_move_speed
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MOVE_SPEED);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_MOVE_SPEED,
		gml_script_get_move_speed->m_Functions->m_ScriptFunction,
		GmlScriptGetMoveSpeedCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MOVE_SPEED);
	}
}

void CreateHookGmlScriptInteract(AurieStatus& status)
{
	CScript* gml_script_interact = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INTERACT,
		(PVOID*)&gml_script_interact
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_INTERACT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_INTERACT,
		gml_script_interact->m_Functions->m_ScriptFunction,
		GmlScriptInteractCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_INTERACT);
	}
}

void CreateHookGmlScriptGetWeather(AurieStatus& status)
{
	CScript* gml_script_get_weather = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_WEATHER,
		(PVOID*)&gml_script_get_weather
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_WEATHER,
		gml_script_get_weather->m_Functions->m_ScriptFunction,
		GmlScriptGetWeatherCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}
}

void CreateHookGmlScriptShowRoomTitle(AurieStatus& status)
{
	CScript* gml_script_create_bug = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SHOW_ROOM_TITLE,
		(PVOID*)&gml_script_create_bug
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SHOW_ROOM_TITLE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SHOW_ROOM_TITLE,
		gml_script_create_bug->m_Functions->m_ScriptFunction,
		GmlScriptShowRoomTitleCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SHOW_ROOM_TITLE);
	}
}

void CreateHookGmlScriptOnRoomStart(AurieStatus& status)
{
	CScript* gml_script_on_room_start = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_ROOM_START,
		(PVOID*)&gml_script_on_room_start
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_ROOM_START);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_ROOM_START,
		gml_script_on_room_start->m_Functions->m_ScriptFunction,
		GmlScriptOnRoomStartCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_ROOM_START);
	}
}

void CreateHookGmlScriptCreateBug(AurieStatus& status)
{
	CScript* gml_script_create_bug = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_BUG,
		(PVOID*)&gml_script_create_bug
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CREATE_BUG);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CREATE_BUG,
		gml_script_create_bug->m_Functions->m_ScriptFunction,
		GmlScriptCreateBugCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CREATE_BUG);
	}
}

void CreateHookGmlScriptAddHeartPoints(AurieStatus& status)
{
	CScript* gml_script_add_heart_points = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ADD_HEART_POINTS,
		(PVOID*)&gml_script_add_heart_points
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ADD_HEART_POINTS);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ADD_HEART_POINTS,
		gml_script_add_heart_points->m_Functions->m_ScriptFunction,
		GmlScriptAddHeartPointsCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ADD_HEART_POINTS);
	}
}

void CreateHookGmlScriptModifyStamina(AurieStatus& status)
{
	CScript* gml_script_modify_stamina = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_STAMINA);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_MODIFY_STAMINA,
		gml_script_modify_stamina->m_Functions->m_ScriptFunction,
		GmlScriptModifyStaminaCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_STAMINA);
	}
}

void CreateHookGmlScriptTryLocationIdToString(AurieStatus& status)
{
	CScript* gml_script_try_location_id_to_string = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
		(PVOID*)&gml_script_try_location_id_to_string
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
		gml_script_try_location_id_to_string->m_Functions->m_ScriptFunction,
		GmlScriptTryLocationIdToStringCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
	}
}

void CreateHookGmlScriptEndDay(AurieStatus& status)
{
	CScript* gml_script_end_day = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_END_DAY,
		(PVOID*)&gml_script_end_day
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_END_DAY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_END_DAY,
		gml_script_end_day->m_Functions->m_ScriptFunction,
		GmlScriptEndDayCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_END_DAY);
	}
}

void CreateHookGmlScriptWriteFurnitureToLocation(AurieStatus& status)
{
	CScript* gml_script_write_furniture_to_location = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION,
		(PVOID*)&gml_script_write_furniture_to_location
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION,
		gml_script_write_furniture_to_location->m_Functions->m_ScriptFunction,
		GmlScriptWriteFurnitureToLocationCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_WRITE_FURNITURE_TO_LOCATION);
	}
}

void CreateHookGmlScriptEraseObjectRenderer(AurieStatus& status)
{
	CScript* gml_script_erase_object_renderer = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ERASE_OBJECT_RENDERER,
		(PVOID*)&gml_script_erase_object_renderer
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ERASE_OBJECT_RENDERER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ERASE_OBJECT_RENDERER,
		gml_script_erase_object_renderer->m_Functions->m_ScriptFunction,
		GmlScriptEraseObjectRendererCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ERASE_OBJECT_RENDERER);
	}
}

void CreateHookGmlScriptGetLocalizer(AurieStatus& status)
{
	CScript* gml_script_get_localizer = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_LOCALIZER,
		gml_script_get_localizer->m_Functions->m_ScriptFunction,
		GmlScriptGetLocalizerCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}
}

void CreateHookGmlScriptPlayText(AurieStatus& status)
{
	CScript* gml_script_play_text = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_TEXT,
		(PVOID*)&gml_script_play_text
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_TEXT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_PLAY_TEXT,
		gml_script_play_text->m_Functions->m_ScriptFunction,
		GmlScriptPlayTextCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_TEXT);
	}
}

void CreateHookGmlScriptPlayConversation(AurieStatus& status)
{
	CScript* gml_script_play_conversation = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_CONVERSATION,
		(PVOID*)&gml_script_play_conversation
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_CONVERSATION);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_PLAY_CONVERSATION,
		gml_script_play_conversation->m_Functions->m_ScriptFunction,
		GmlScriptPlayConversationCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_CONVERSATION);
	}
}

void CreateHookGmlScriptSetupMainScreen(AurieStatus& status)
{
	CScript* gml_script_setup_main_screen = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		(PVOID*)&gml_script_setup_main_screen
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		gml_script_setup_main_screen->m_Functions->m_ScriptFunction,
		GmlScriptSetupMainScreenCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}
}

void CreateHookGmlScriptSaveGame(AurieStatus& status)
{
	CScript* gml_script_save_game = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SAVE_GAME,
		(PVOID*)&gml_script_save_game
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SAVE_GAME,
		gml_script_save_game->m_Functions->m_ScriptFunction,
		GmlScriptSaveGameCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
	}
}

void CreateHookGmlScriptLoadGame(AurieStatus& status)
{
	CScript* gml_script_load_game = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_LOAD_GAME,
		(PVOID*)&gml_script_load_game
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LOAD_GAME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_LOAD_GAME,
		gml_script_load_game->m_Functions->m_ScriptFunction,
		GmlScriptLoadGameCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LOAD_GAME);
	}
}

void RunnerInitCallback(
	IN FunctionWrapper<void(int)>& wrapper
)
{
	CreateHookGmlScriptGetFishingCelebrationData(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptGetDivingCelebrationData(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptGiveItem(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptGetMoveSpeed(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptInteract(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptGetWeather(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptShowRoomTitle(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptOnRoomStart(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptCreateBug(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptAddHeartPoints(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptModifyStamina(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptTryLocationIdToString(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptEndDay(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptWriteFurnitureToLocation(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptEraseObjectRenderer(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptGetLocalizer(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptPlayText(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptPlayConversation(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptSetupMainScreen(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptSaveGame(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	CreateHookGmlScriptLoadGame(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return;
	}

	DbgPrintEx(LOG_SEVERITY_DEBUG, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
}

void CreateRunnerInitCallback(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_RUNNER_INIT,
		RunnerInitCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_CRITICAL, "[%s %s] - Failed to hook (EVENT_RUNNER_INIT)!", MOD_NAME, VERSION);
	}
}

EXPORTED AurieStatus ModulePreInitialize(
	IN AurieModule* Module,
	IN const fs::path& ModulePath
)
{
	UNREFERENCED_PARAMETER(ModulePath);
	g_ModuleInterface = YYTK::GetInterface();

	DbgPrintEx(LOG_SEVERITY_DEBUG, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateObjectCallback(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateRunnerInitCallback(status);
	if (!AurieSuccess(status))
	{
		DbgPrintEx(LOG_SEVERITY_ERROR, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	return AURIE_SUCCESS;
}
