#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "EnemyHealthBars";
static const char* const VERSION = "1.0.0";
static const char* const GML_SCRIPT_DRAW_MONSTER = "gml_Script_draw@gml_Object_par_monster_Create_0";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const SMALLER_HEALTH_BAR_JSON_KEY = "smaller_health_bar";
static const char* const RED_HEALTH_BAR_JSON_KEY = "red_health_bar";
static const bool DEFAULT_SMALLER_HEALTH_BAR = false;
static const bool DEFAULT_RED_HEALTH_BAR = false;
static const int HEALTH_BAR_HEIGHT = 9;
static const int HEALTH_BAR_WIDTH = 40;
static const int SMALL_HEALTH_BAR_HEIGHT = 9;
static const int SMALL_HEALTH_BAR_WIDTH = 30;
static const std::map<std::string, int> MONSTER_NAME_TO_SPRITE_SIZE_MAP = { // Uses the monster's "idle south" sprite, divided by 2 or 3
	{ "bat", 40 * 0.65 }, // spr_monster_essence_bat_main_idle_south
	{ "bat_blue", 40 * 0.65 }, // spr_monster_essence_bat_blue_main_idle_south
	{ "cat", 48 * 0.50 }, // spr_monster_lava_cat_main_idle_south
	{ "cat_void", 48 * 0.50 }, // spr_monster_void_cat_main_idle_south
	{ "copperclod", 40 * 0.55 }, // spr_monster_copperclod_main_idle_south
	{ "enchantern", 64 * 0.55 }, // spr_monster_enchantern_off_idle_south
	{ "enchantern_blue", 64 * 0.55 }, // spr_monster_enchantern_blue_off_idle_south
	{ "goldclod", 40 * 0.50 }, // spr_monster_goldclod_main_idle_south
	{ "griffin_statue", 80 * 0.65 }, // spr_monster_living_griffin_statue_main_idle_south
	{ "ironclod", 40 * 0.55 }, // spr_monster_ironclod_main_idle_south
	{ "mimic", 40 * 0.50 }, // spr_monster_mimic_main_idle_south
	{ "mistrilclod", 40 * 0.55 }, // spr_monster_mistrilclod_main_idle_south
	{ "mushroom", 40 * 0.55 }, // spr_monster_mushroom_main_idle_south
	{ "mushroom_blue", 40 * 0.55 }, // spr_monster_mushroom_blue_main_idle_south
	{ "mushroom_green", 40 * 0.55 }, // spr_monster_mushroom_green_main_idle_south
	{ "mushroom_purple", 40 * 0.55 }, // spr_monster_mushroom_purple_main_idle_south
	{ "rock_stack", 40 * 0.55 }, // spr_monster_rock_stack_main_idle_south
	{ "rockclod", 40 * 0.55 }, // spr_monster_rockclod_main_idle_south
	{ "rockclod_blue", 40 * 0.55 }, // spr_monster_rockclod_blue_main_idle_south
	{ "rockclod_green", 40 * 0.55 }, // spr_monster_rockclod_green_main_idle_south
	{ "rockclod_purple", 40 * 0.55 }, // spr_monster_rockclod_purple_main_idle_south
	{ "rockclod_red", 40 * 0.55 }, // spr_monster_rockclod_red_main_idle_south
	{ "sapling", 40 * 0.55 }, // spr_monster_sapling_main_idle_south
	{ "sapling_blue", 40 * 0.55 }, // spr_monster_sapling_blue_main_idle_south
	{ "sapling_cool", 40 * 0.55 }, // spr_monster_sapling_cool_main_idle_south
	{ "sapling_orange", 40 * 0.55 }, // spr_monster_sapling_orange_main_idle_south
	{ "sapling_orange_mini", 40 * 0.50 }, // spr_monster_sapling_orange_mini_main_idle_south
	{ "sapling_pink", 40 * 0.55 }, // spr_monster_sapling_pink_main_idle_south
	{ "sapling_purple", 40 * 0.55 }, // spr_monster_sapling_purple_main_idle_south
	{ "silverclod", 40 * 0.55 }, // spr_monster_silverclod_main_idle_south
	{ "spirit", 40 * 0.70 }, // spr_monster_flame_spirit_main_idle_south
	{ "spirit_purple", 40 * 0.70 }, // spr_monster_flame_spirit_purple_main_idle_south
	{ "stalagmite", 40 * 0.50 }, // spr_monster_stalagmite_blue_tired
	{ "stalagmite_green", 40 * 0.50 }, // spr_monster_stalagmite_green_tired
	{ "stalagmite_purple", 40 * 0.50 }, // spr_monster_stalagmite_purple_tired
	{ "tome", 64 * 0.60 }, // spr_monster_flying_tome_main_idle_south
	//{ "",  }, // 
};

struct Configuration {
	bool smaller_health_bar = DEFAULT_SMALLER_HEALTH_BAR;
	bool red_health_bar = DEFAULT_RED_HEALTH_BAR;
};

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static Configuration configuration = Configuration();
static bool load_on_start = true;
static std::map<std::string, int> monster_name_to_id_map = {};
static std::map<int, std::string> monster_id_to_name_map = {};

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}

json CreateConfigJson(bool use_defaults)
{
	json config_json = {
		{ SMALLER_HEALTH_BAR_JSON_KEY, use_defaults ? DEFAULT_SMALLER_HEALTH_BAR : configuration.smaller_health_bar },
		{ RED_HEALTH_BAR_JSON_KEY, use_defaults ? DEFAULT_RED_HEALTH_BAR : configuration.red_health_bar }
	};
	return config_json;
}

void CreateOrLoadConfigFile()
{
	std::exception_ptr eptr;
	try
	{
		std::string current_dir = std::filesystem::current_path().string();
		std::string mod_data_folder = current_dir + "\\mod_data";
		if (!std::filesystem::exists(mod_data_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
			std::filesystem::create_directory(mod_data_folder);
		}

		std::string enemy_health_bars_folder = mod_data_folder + "\\EnemyHealthBars";
		if (!std::filesystem::exists(enemy_health_bars_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"EnemyHealthBars\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, enemy_health_bars_folder.c_str());
			std::filesystem::create_directory(enemy_health_bars_folder);
		}

		bool update_config_file = false;
		std::string config_file = enemy_health_bars_folder + "\\" + "EnemyHealthBars.json";
		std::ifstream in_stream(config_file);
		if (in_stream.good())
		{
			try
			{
				json json_object = json::parse(in_stream);

				if (json_object.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.c_str());
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Add your desired values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
				}
				else
				{
					if (json_object.contains(SMALLER_HEALTH_BAR_JSON_KEY))
					{
						configuration.smaller_health_bar = json_object[SMALLER_HEALTH_BAR_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, SMALLER_HEALTH_BAR_JSON_KEY, configuration.smaller_health_bar ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SMALLER_HEALTH_BAR_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, SMALLER_HEALTH_BAR_JSON_KEY, DEFAULT_SMALLER_HEALTH_BAR ? "true" : "false");
					}

					if (json_object.contains(RED_HEALTH_BAR_JSON_KEY))
					{
						configuration.red_health_bar = json_object[RED_HEALTH_BAR_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, RED_HEALTH_BAR_JSON_KEY, configuration.red_health_bar ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RED_HEALTH_BAR_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, RED_HEALTH_BAR_JSON_KEY, DEFAULT_RED_HEALTH_BAR ? "true" : "false");
					}
				}

				update_config_file = true;
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"EnemyHealthBars.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();
		}

		if (update_config_file)
		{
			json config_json = CreateConfigJson(false);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << config_json << std::endl;
			out_stream.close();
		}
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);
	}
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return struct_exists.ToBoolean();
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void LoadMonsters()
{
	size_t array_length;
	RValue monster_names = global_instance->GetMember("__monster_id__");
	g_ModuleInterface->GetArraySize(monster_names, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* monster_name;
		g_ModuleInterface->GetArrayEntry(monster_names, i, monster_name);

		monster_name_to_id_map[monster_name->ToString()] = i;
		monster_id_to_name_map[i] = monster_name->ToString();
	}
}

void DrawRectangle(int color, float x1, float y1, float x2, float y2, bool outline)
{
	g_ModuleInterface->CallBuiltin(
		"draw_set_color", {
		 color
		}
	);

	g_ModuleInterface->CallBuiltin(
		"draw_rectangle", {
			x1, y1, x2, y2, outline
		}
	);
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

	if (strstr(self->m_Object->m_Name, "obj_monster"))
	{
		RValue monster = self->ToRValue();
		if (StructVariableExists(monster, "__enemy_health_bars_max_health") || !StructVariableExists(monster, "monster_id") || !StructVariableExists(monster, "hit_points"))
			return;

		RValue monster_id = monster.GetMember("monster_id");
		if (!IsNumeric(monster_id) || !monster_id_to_name_map.contains(monster_id.ToInt64()) || !MONSTER_NAME_TO_SPRITE_SIZE_MAP.contains(monster_id_to_name_map[monster_id.ToInt64()]))
			return;

		RValue hit_points = monster.GetMember("hit_points");
		if (!IsNumeric(hit_points) || !std::isfinite(hit_points.ToDouble()))
			return;

		StructVariableSet(monster, "__enemy_health_bars_max_health", hit_points.ToInt64());
	}
}

RValue& GmlScriptDrawMonsterCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DRAW_MONSTER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (!StructVariableExists(Self, "monster_id") || !StructVariableExists(Self, "hit_points") || (!StructVariableExists(Self, "__enemy_health_bars_max_health") && !StructVariableExists(Self, "__deep_dungeon__default_hit_points")))
		return Result;
	
	RValue monster_id = Self->GetMember("monster_id");
	if (!IsNumeric(monster_id) || !MONSTER_NAME_TO_SPRITE_SIZE_MAP.contains(monster_id_to_name_map[monster_id.ToInt64()]))
		return Result;

	RValue health = Self->GetMember("hit_points");
	if (!IsNumeric(health) || !std::isfinite(health.ToDouble()))
		return Result;

	bool deep_dungeon = StructVariableExists(Self, "__deep_dungeon__default_hit_points");
	RValue max_health = deep_dungeon ? Self->GetMember("__deep_dungeon__default_hit_points") : Self->GetMember("__enemy_health_bars_max_health");

	RValue x, y;
	g_ModuleInterface->GetBuiltin("x", Self, NULL_INDEX, x);
	g_ModuleInterface->GetBuiltin("y", Self, NULL_INDEX, y);

	int sprite_height = MONSTER_NAME_TO_SPRITE_SIZE_MAP.at(monster_id_to_name_map[monster_id.ToInt64()]);
	int bar_x = x.ToInt64();
	int bar_y = y.ToInt64() - sprite_height;
	int health_bar_width = configuration.smaller_health_bar ? SMALL_HEALTH_BAR_WIDTH : HEALTH_BAR_WIDTH;
	int health_bar_height = configuration.smaller_health_bar ? SMALL_HEALTH_BAR_HEIGHT : HEALTH_BAR_HEIGHT;

	double hp_percent = health.ToDouble() / max_health.ToDouble();
	hp_percent = std::clamp(hp_percent, 0.0, 1.0);

	// Centered bounds
	int left = bar_x - health_bar_width / 2;
	int right = bar_x + health_bar_width / 2;
	int top = bar_y - health_bar_height / 2;
	int bottom = bar_y + health_bar_height / 2;

	// Fill only middle rows
	int fill_top = top + health_bar_height / 3;
	int fill_bottom = top + (health_bar_height * 2) / 3;

	// Compute fill width
	int fill_right = left + (int)(health_bar_width * hp_percent) - 1;

	// Draw fill (black background first)
	DrawRectangle(0, left, fill_top, right - 1, fill_bottom, false);

	// Draw HP
	int health_bar_color = configuration.red_health_bar ? 255 : 65280;
	DrawRectangle(health_bar_color, left, fill_top, fill_right, fill_bottom, false);

	RValue sprite_index = g_ModuleInterface->CallBuiltin(
		"asset_get_index", {
			"enemy_health_bar"
		}
	);

	// Draw sprite on top (centered)
	g_ModuleInterface->CallBuiltin(
		"draw_sprite_ext",
		{
			RValue(sprite_index),
			RValue(-1),
			RValue(left),
			RValue(top),
			RValue((double)health_bar_width / HEALTH_BAR_WIDTH),
			RValue((double)health_bar_height / HEALTH_BAR_HEIGHT),
			RValue(0),
			RValue(16777215),
			RValue(1.0)
		}
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
		LoadMonsters();
		CreateOrLoadConfigFile();
	}

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
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook (EVENT_OBJECT_CALL)!", MOD_NAME, VERSION);
	}
}

void CreateHookGmlScriptDrawMonster(AurieStatus& status)
{
	CScript* gml_script_draw_monster = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DRAW_MONSTER,
		(PVOID*)&gml_script_draw_monster
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DRAW_MONSTER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DRAW_MONSTER,
		gml_script_draw_monster->m_Functions->m_ScriptFunction,
		GmlScriptDrawMonsterCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DRAW_MONSTER);
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
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
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
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateObjectCallback(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptDrawMonster(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSetupMainScreen(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}
