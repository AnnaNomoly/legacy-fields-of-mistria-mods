#include <fstream>
#include <iostream>
#include <random>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "ChutesAndLadders";
static const char* const VERSION = "1.3.1";
static const char* const ACTIVATION_BUTTON_KEY = "activation_button";
static const char* const RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY = "ritual_chamber_additional_spawn_chance";
static const char* const SPAWN_LADDER_AT_PLAYER_POSITION_KEY = "spawn_ladder_at_player_position";
static const std::string LADDER_SPAWNED_LOCALIZATION_KEY = "mods/ChutesAndLadders/ladder_spawned";
static const std::string LADDER_NOT_SPAWNED_LOCALIZATION_KEY = "mods/ChutesAndLadders/ladder_not_spawned";
static const std::string INVALID_LOCATION_LOCALIZATION_KEY = "mods/ChutesAndLadders/invalid_location";
static const std::string LOST_TO_HISTORY_PERK_NAME = "lost_to_history";
static const char* const GML_SCRIPT_GET_MAX_HEALTH = "gml_Script_get_max_health@Ari@Ari";
static const char* const GML_SCRIPT_GET_HEALTH = "gml_Script_get_health@Ari@Ari";
static const char* const GML_SCRIPT_SET_HEALTH = "gml_Script_set_health@Ari@Ari";
static const char* const GML_SCRIPT_MODIFY_HEALTH = "gml_Script_modify_health@Ari@Ari";
static const char* const GML_SCRIPT_MOVE_ARI = "gml_Script_move_ari";
static const char* const GML_SCRIPT_SPAWN_LADDER = "gml_Script_spawn_ladder@DungeonRunner@DungeonRunner";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const char* const GML_SCRIPT_ARI_FACE_DIR = "gml_Script_face_dir@gml_Object_obj_ari_Create_0";
static const char* const GML_SCRIPT_ON_ROOM_START = "gml_Script_on_room_start@WeatherManager@Weather";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_ON_BEGIN_STEP = "gml_Script_on_begin_step@Anchor@Anchor";
static const char* const GML_SCRIPT_ON_DRAW_GUI = "gml_Script_on_draw_gui@Display@Display";
static const char* const GML_SCRIPT_GET_MINUTES = "gml_Script_update@Clock@Clock";
static const char* const GML_SCRIPT_CHECK_VALUE = "gml_Script_check_value@Input@Input";
static const char* const GML_SCRIPT_GO_TO_ROOM = "gml_Script_goto_gm_room";
static const int DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE = 0;
static const bool DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION = true;
static const std::string DEFAULT_ACTIVATION_BUTTON = "PAGE_DOWN";
static const std::string ALLOWED_ACTIVATION_BUTTONS[] = {
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
	"NUMPAD_0", "NUMPAD_1", "NUMPAD_2", "NUMPAD_3", "NUMPAD_4", "NUMPAD_5", "NUMPAD_6", "NUMPAD_7", "NUMPAD_8", "NUMPAD_9",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"INSERT", "DELETE", "HOME", "PAGE_UP", "PAGE_DOWN", "NUM_LOCK", "SCROLL_LOCK", "CAPS_LOCK", "PAUSE_BREAK",
	"GAMEPAD_A", "GAMEPAD_B", "GAMEPAD_X", "GAMEPAD_Y", "GAMEPAD_LEFT_SHOULDER", "GAMEPAD_RIGHT_SHOULDER", "GAMEPAD_LEFT_TRIGGER", "GAMEPAD_RIGHT_TRIGGER", "GAMEPAD_DPAD_UP", "GAMEPAD_DPAD_DOWN", "GAMEPAD_DPAD_LEFT", "GAMEPAD_DPAD_RIGHT", "GAMEPAD_LEFT_STICK", "GAMEPAD_RIGHT_STICK", "GAMEPAD_SELECT", "GAMEPAD_START"
};
static const std::map<std::string, std::vector<int>> LADDER_SPAWN_POINTS = { // As of 0.14.0
	{ "rm_mines_tide_fork", { 68, 80 }},
	{ "rm_mines_tide_whirly", { 110, 60 }},
	{ "rm_mines_deep_runner", { 18, 18 }},
	{ "rm_mines_tide_islands", { 18, 66 }},
	{ "rm_mines_lava_bullseye", { 32, 46 }},
	{ "rm_mines_tide_paths", { 52, 72 }},
	{ "rm_mines_upper_slimetime", { 40, 34 }},
	{ "rm_mines_lava_generic", { 66, 84 }},
	{ "rm_mines_deep_winding", { 68, 42 }},
	{ "rm_mines_lava_world", { 36, 36 }},
	{ "rm_mines_deep_leap", { 66, 18 }},
	{ "rm_mines_ruins_secretfire", { 18, 78 }},
	{ "rm_mines_lava_treasure", { 20, 32 }},
	{ "rm_mines_tide_milestone", { 20, 34 }},
	{ "rm_mines_lava_elements", { 36, 70 }},
	{ "rm_mines_tide_oasis", { 20, 18 }},
	{ "rm_mines_tide_chamber", { 28, 60 }},
	{ "rm_mines_lava_arena", { 40, 16 }},
	{ "rm_mines_upper_treasure", { 18, 40 }},
	{ "rm_mines_upper_sock", { 32, 22 }},
	{ "rm_mines_upper_boomerang", { 46, 38 }},
	{ "rm_mines_upper_worm", { 22, 18 }},
	{ "rm_mines_upper_ponds", { 40, 42 }},
	{ "rm_mines_upper_pond", { 30, 52 }},
	{ "rm_mines_upper_snake", { 54, 16 }},
	{ "rm_mines_upper_four", { 22, 16 }},
	{ "rm_mines_upper_amoeba", { 22, 48 }},
	{ "rm_mines_upper_wishbone", { 42, 74 }},
	{ "rm_mines_upper_canada", { 24, 44 }},
	{ "rm_mines_upper_staple", { 60, 48 }},
	{ "rm_mines_upper_pillars", { 34, 16 }},
	{ "rm_mines_upper_elevator5", { 12, 28 }},
	{ "rm_mines_upper_elevator10", { 48, 42 }},
	{ "rm_mines_upper_elevator15", { 18, 8 }},
	{ "rm_mines_upper_floor1", { 12, 36 }},
	{ "rm_mines_upper_path", { 40, 44 }},
	{ "rm_mines_upper_stream", { 50, 16 }},
	{ "rm_mines_upper_muscle", { 28, 60 }},
	{ "rm_mines_upper_crossroad", { 40, 42 }},
	{ "rm_mines_upper_pits", { 30, 22 }},
	{ "rm_mines_basement_no_elevator", { 46, 38 }},
	{ "rm_mines_basement_elevator", { 12, 28 }},
	{ "rm_mines_tide_elevator25", { 54, 56 }},
	{ "rm_mines_tide_elevator30", { 44, 52 }},
	{ "rm_mines_tide_elevator35", { 56, 40 }},
	{ "rm_mines_tide_floor21", { 50, 20 }},
	{ "rm_mines_tide_basic1", { 66, 86 }},
	{ "rm_mines_tide_shrine1", { 26, 72 }},
	{ "rm_mines_tide_whirlpool1", { 108, 84 }},
	{ "rm_mines_tide_basic2", { 38, 20 }},
	{ "rm_mines_tide_switch2", { 18, 14 }},
	{ "rm_mines_tide_whirlpool2", { 18, 80 }},
	{ "rm_mines_tide_switch1", { 34, 38 }},
	{ "rm_mines_tide_shrine3", { 84, 78 }},
	{ "rm_mines_tide_shrine2", { 36, 58 }},
	{ "rm_mines_basement_treasure", { 18, 40 }},
	{ "rm_mines_tide_whirlpool3", { 20, 84 }},
	{ "rm_mines_tide_basic3", { 34, 36 }},
	{ "rm_mines_tide_basic4", { 56, 60 }},
	{ "rm_mines_tide_whirlpool4", { 16, 78 }},
	{ "rm_mines_tide_switch3", { 20, 16 }},
	{ "rm_mines_tide_switch4", { 58, 50 }},
	{ "rm_mines_deep_chambers", { 24, 70 }},
	{ "rm_mines_deep_spiral", { 30, 98 }},
	{ "rm_mines_deep_wishbone", { 82, 80 }},
	{ "rm_mines_deep_key", { 40, 20 }},
	{ "rm_mines_deep_scorpio", { 28, 24 }},
	{ "rm_mines_deep_butterfly", { 50, 36 }},
	{ "rm_mines_upper_formerelevator", { 40, 44 }},
	{ "rm_mines_deep_41", { 38, 32 }},
	{ "rm_mines_deep_45", { 48, 62 }},
	{ "rm_mines_deep_50", { 76, 60 }},
	{ "rm_mines_deep_55", { 68, 64 }},
	{ "rm_mines_deep_switch1", { 18, 18 }},
	{ "rm_mines_deep_switch2", { 14, 56 }},
	{ "rm_mines_deep_treasure1", { 18, 30 }},
	{ "rm_mines_deep_switch3", { 64, 40 }},
	{ "rm_mines_deep_switch4", { 86, 78 }},
	{ "rm_mines_deep_whirlpool1", { 66, 18 }},
	{ "rm_mines_deep_whirlpool2", { 26, 24 }},
	{ "rm_mines_deep_shrine2", { 42, 78 }},
	{ "rm_mines_deep_shrine1", { 102, 54 }},
	{ "rm_mines_deep_shrine3", { 60, 48 }},
	{ "rm_mines_deep_shrine4", { 56, 68 }},
	{ "rm_mines_tide_ritual_chamber", { 24, 16 }},
	{ "rm_mines_deep_ritual_chamber", { 24, 16 }},
	{ "rm_mines_lava_ritual_chamber", { 24, 16 }},
	{ "rm_mines_ruins_ritual_chamber", { 24, 16 }},
	{ "rm_mines_lava_61", { 36, 34 }},
	{ "rm_mines_lava_65", { 64, 18 }},
	{ "rm_mines_lava_70", { 50, 22 }},
	{ "rm_mines_lava_75", { 86, 22 }},
	{ "rm_mines_lava_switch1", { 98, 32 }},
	{ "rm_mines_lava_switch2", { 62, 30 }},
	{ "rm_mines_lava_switch3", { 56, 24 }},
	{ "rm_mines_lava_switch4", { 58, 30 }},
	{ "rm_mines_lava_switch5", { 56, 48 }},
	{ "rm_mines_lava_basic1", { 104, 36 }},
	{ "rm_mines_lava_basic2", { 42, 56 }},
	{ "rm_mines_lava_basic3", { 40, 26 }},
	{ "rm_mines_lava_basic4", { 68, 54 }},
	{ "rm_mines_lava_basic5", { 66, 84 }},
	{ "rm_mines_lava_shrine1", { 28, 60 }},
	{ "rm_mines_lava_shrine2", { 94, 78 }},
	{ "rm_mines_lava_shrine3", { 46, 66 }},
	{ "rm_mines_lava_shrine4", { 94, 76 }},
	{ "rm_mines_ruins_basic1", { 60, 68 }},
	{ "rm_mines_ruins_85", { 48, 40 }},
	{ "rm_mines_ruins_90", { 74, 66 }}
};

static YYTKInterface* g_ModuleInterface = nullptr;
static bool load_on_start = true;
static bool game_is_active = false;
static bool activation_button_is_controller_key = false;
static int activation_button_int_value = -1;
static bool processing_user_input = false;
static std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
static std::string ari_current_gm_room = "";
static int ari_x = -1;
static int ari_y = -1;
static double ari_facing_dir = -1;
static bool teleport_ari = false;
static double meteor_sprite_speed = 0.7;
static bool ari_is_teleporting = false;
static bool create_ritual_altar = false;
static bool meteor_fall = false;
static int ritual_chamber_additional_spawn_chance = DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE;
static bool spawn_ladder_at_player_position = DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION;
static std::mt19937 generator(std::random_device{}());
static std::map<std::string, bool> active_perk_map = {};
static std::map<std::string, int> location_name_to_id_map = {};
static std::map<std::string, int64_t> perk_name_to_id_map = {};
static std::map<std::string, uint64_t> notification_name_to_last_display_time_map = {};
static std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map; // Vector<CInstance*> holds references to Self and Other for each script.

static enum class Trap {
	GAZE,
	METEOR
};
static struct Meteor {
	int x;
	int y;
	int spawned_time;
	int duration;
	bool is_active;
	RValue instance;
	Trap trap;
};
static std::vector<Meteor> meteors = {};

RValue StructVariableGet(RValue the_struct, const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, variable_name }
	);
}

int RValueAsInt(RValue value)
{
	if (value.m_Kind == VALUE_REAL)
		return static_cast<int>(value.m_Real);
	if (value.m_Kind == VALUE_INT64)
		return static_cast<int>(value.m_i64);
	if (value.m_Kind == VALUE_INT32)
		return static_cast<int>(value.m_i32);
}

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_REAL && value.m_Real == 1)
		return true;
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
}

bool GameWindowHasFocus()
{
	RValue window_has_focus = g_ModuleInterface->CallBuiltin("window_has_focus", {});
	return RValueAsBool(window_has_focus);
}

bool GameIsPaused()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	RValue paused = global_instance->GetMember("__pause_status");
	return paused.m_i64 > 0;
}

void LoadPerks()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	size_t array_length;
	RValue perks = global_instance->GetMember("__perk__");
	g_ModuleInterface->GetArraySize(perks, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(perks, i, array_element);
		perk_name_to_id_map[array_element->ToString()] = i;
	}
}

void LoadLocationIds()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	// Load locations.
	size_t array_length;
	RValue location_ids = global_instance->GetMember("__location_id__");
	g_ModuleInterface->GetArraySize(location_ids, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(location_ids, i, array_element);

		location_name_to_id_map[array_element->ToString()] = i;
	}

	if (location_name_to_id_map.size() == 0)
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to load location IDs!", MOD_NAME, VERSION);
}

std::pair<int, int> GetVector(int x1, int y1, int x2, int y2)
{
	return { x2 - x1, y2 - y1 };
}

double GetDistance(int x1, int y1, int x2, int y2)
{
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

int CalculateMeteorDamage(double distance)
{
	const double lethalRadius = 32.0;
	const double maxDistance = 256.0;

	if (distance <= lethalRadius)
		return 100;

	if (distance >= maxDistance)
		return 1;

	// Normalize distance outside lethal zone
	double t = (distance - lethalRadius) / (maxDistance - lethalRadius);

	// Exponential falloff (adjust exponent for feel)
	const double exponent = 2.5;
	double damage = 100.0f * std::pow(1.0f - t, exponent);

	return max(1, static_cast<int>(damage));
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

uint64_t GetCurrentSystemTime() {
	return duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int ActivationButtonToVirtualKey()
{
	// Function Keys
	if (activation_button == "F1")
		return VK_F1;
	if (activation_button == "F2")
		return VK_F2;
	if (activation_button == "F3")
		return VK_F3;
	if (activation_button == "F4")
		return VK_F4;
	if (activation_button == "F5")
		return VK_F5;
	if (activation_button == "F6")
		return VK_F6;
	if (activation_button == "F7")
		return VK_F7;
	if (activation_button == "F8")
		return VK_F8;
	if (activation_button == "F9")
		return VK_F9;
	if (activation_button == "F10")
		return VK_F10;
	if (activation_button == "F11")
		return VK_F11;
	if (activation_button == "F12")
		return VK_F12;

	// Numpad
	if (activation_button == "NUMPAD_0")
		return VK_NUMPAD0;
	if (activation_button == "NUMPAD_1")
		return VK_NUMPAD1;
	if (activation_button == "NUMPAD_2")
		return VK_NUMPAD2;
	if (activation_button == "NUMPAD_3")
		return VK_NUMPAD3;
	if (activation_button == "NUMPAD_4")
		return VK_NUMPAD4;
	if (activation_button == "NUMPAD_5")
		return VK_NUMPAD5;
	if (activation_button == "NUMPAD_6")
		return VK_NUMPAD6;
	if (activation_button == "NUMPAD_7")
		return VK_NUMPAD7;
	if (activation_button == "NUMPAD_8")
		return VK_NUMPAD8;
	if (activation_button == "NUMPAD_9")
		return VK_NUMPAD9;

	// Numbers
	if (activation_button == "0")
		return '0';
	if (activation_button == "1")
		return '1';
	if (activation_button == "2")
		return '2';
	if (activation_button == "3")
		return '3';
	if (activation_button == "4")
		return '4';
	if (activation_button == "5")
		return '5';
	if (activation_button == "6")
		return '6';
	if (activation_button == "7")
		return '7';
	if (activation_button == "8")
		return '8';
	if (activation_button == "9")
		return '9';

	// Letters
	if (activation_button == "A")
		return 'A';
	if (activation_button == "B")
		return 'B';
	if (activation_button == "C")
		return 'C';
	if (activation_button == "D")
		return 'D';
	if (activation_button == "E")
		return 'E';
	if (activation_button == "F")
		return 'F';
	if (activation_button == "G")
		return 'G';
	if (activation_button == "H")
		return 'H';
	if (activation_button == "I")
		return 'I';
	if (activation_button == "J")
		return 'J';
	if (activation_button == "K")
		return 'K';
	if (activation_button == "L")
		return 'L';
	if (activation_button == "M")
		return 'M';
	if (activation_button == "N")
		return 'N';
	if (activation_button == "O")
		return 'O';
	if (activation_button == "P")
		return 'P';
	if (activation_button == "Q")
		return 'Q';
	if (activation_button == "R")
		return 'R';
	if (activation_button == "S")
		return 'S';
	if (activation_button == "T")
		return 'T';
	if (activation_button == "U")
		return 'U';
	if (activation_button == "V")
		return 'V';
	if (activation_button == "W")
		return 'W';
	if (activation_button == "X")
		return 'X';
	if (activation_button == "Y")
		return 'Y';
	if (activation_button == "Z")
		return 'Z';

	// Special
	if (activation_button == "INSERT")
		return VK_INSERT;
	if (activation_button == "DELETE")
		return VK_DELETE;
	if (activation_button == "HOME")
		return VK_HOME;
	if (activation_button == "PAGE_UP")
		return VK_PRIOR;
	if (activation_button == "PAGE_DOWN")
		return VK_NEXT;
	if (activation_button == "NUM_LOCK")
		return VK_NUMLOCK;
	if (activation_button == "SCROLL_LOCK")
		return VK_SCROLL;
	if (activation_button == "CAPS_LOCK")
		return VK_CAPITAL;
	if (activation_button == "PAUSE_BREAK")
		return VK_PAUSE;
}

int ActivationButtonToControllerKey()
{
	if (activation_button == "GAMEPAD_A")
		return 0x8001;
	if (activation_button == "GAMEPAD_B")
		return 0x8002;
	if (activation_button == "GAMEPAD_X")
		return 0x8003;
	if (activation_button == "GAMEPAD_Y")
		return 0x8004;
	if (activation_button == "GAMEPAD_LEFT_SHOULDER")
		return 0x8005;
	if (activation_button == "GAMEPAD_RIGHT_SHOULDER")
		return 0x8006;
	if (activation_button == "GAMEPAD_LEFT_TRIGGER")
		return 0x8007;
	if (activation_button == "GAMEPAD_RIGHT_TRIGGER")
		return 0x8008;
	if (activation_button == "GAMEPAD_SELECT")
		return 0x8009;
	if (activation_button == "GAMEPAD_START")
		return 0x800A;
	if (activation_button == "GAMEPAD_LEFT_STICK")
		return 0x800B;
	if (activation_button == "GAMEPAD_RIGHT_STICK")
		return 0x800C;
	if (activation_button == "GAMEPAD_DPAD_UP")
		return 0x800D;
	if (activation_button == "GAMEPAD_DPAD_DOWN")
		return 0x800E;
	if (activation_button == "GAMEPAD_DPAD_LEFT")
		return 0x800F;
	if (activation_button == "GAMEPAD_DPAD_RIGHT")
		return 0x8010;
	return -1;
}

int GetGamepadSlotNumber()
{
	for (int i = 0; i < 12; i++)
	{
		RValue gamepad_is_connected = g_ModuleInterface->CallBuiltin("gamepad_is_connected", { i });
		if (gamepad_is_connected.ToBoolean())
		{
			return i;
		}
	}

	return -1;
}

bool CheckControllerInput()
{
	int gamepad_slot = GetGamepadSlotNumber();
	if (gamepad_slot != -1)
	{
		RValue button_pressed = g_ModuleInterface->CallBuiltin("gamepad_button_check_pressed", { gamepad_slot, activation_button_int_value });
		return button_pressed.ToBoolean();
	}
	
	return false;
}

bool CheckVirtualKeyInput()
{
	RValue key_pressed = g_ModuleInterface->CallBuiltin("keyboard_check_pressed", { activation_button_int_value });
	if (RValueAsBool(key_pressed))
	{
		processing_user_input = true;
		return true;
	}
	return false;
}

void ConfigureActivationButton()
{
	if (activation_button.find("GAMEPAD") != std::string::npos)
	{
		activation_button_is_controller_key = true;
		activation_button_int_value = ActivationButtonToControllerKey();
	}
	else
	{
		activation_button_is_controller_key = false;
		activation_button_int_value = ActivationButtonToVirtualKey();
	}
}

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
		{ ACTIVATION_BUTTON_KEY, use_defaults ? DEFAULT_ACTIVATION_BUTTON : activation_button },
		{ RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, use_defaults ? DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE : ritual_chamber_additional_spawn_chance },
		{ SPAWN_LADDER_AT_PLAYER_POSITION_KEY, use_defaults ? DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION : spawn_ladder_at_player_position }
	};
	return config_json;
}

void LogDefaultConfigValues()
{
	activation_button = DEFAULT_ACTIVATION_BUTTON;
	ritual_chamber_additional_spawn_chance = DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE;
	spawn_ladder_at_player_position = DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION;

	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, SPAWN_LADDER_AT_PLAYER_POSITION_KEY, DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION ? "true" : "false");
}

void CreateOrLoadConfigFile()
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
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
			std::filesystem::create_directory(mod_data_folder);
		}

		// Try to find the mod_data/ChutesAndLadders directory.
		std::string chutes_and_ladders_folder = mod_data_folder + "\\ChutesAndLadders";
		if (!std::filesystem::exists(chutes_and_ladders_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"ChutesAndLadders\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, chutes_and_ladders_folder.c_str());
			std::filesystem::create_directory(chutes_and_ladders_folder);
		}

		// Try to find the mod_data/ChutesAndLadders/ChutesAndLadders.json config file.
		bool update_config_file = false;
		std::string config_file = chutes_and_ladders_folder + "\\" + "ChutesAndLadders.json";
		std::ifstream in_stream(config_file);
		if (in_stream.good())
		{
			try
			{
				json json_object = json::parse(in_stream);

				// Check if the json_object is empty.
				if (json_object.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.c_str());
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Add your desired values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
					LogDefaultConfigValues();
				}
				else
				{
					// Try loading the activation_button value.
					if (json_object.contains(ACTIVATION_BUTTON_KEY))
					{
						activation_button = json_object[ACTIVATION_BUTTON_KEY];
						auto allowed_button = std::find(std::begin(ALLOWED_ACTIVATION_BUTTONS), std::end(ALLOWED_ACTIVATION_BUTTONS), activation_button);
						if (allowed_button == std::end(ALLOWED_ACTIVATION_BUTTONS))
						{
							g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value (%s) in mod configuration file: %s", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, activation_button, config_file.c_str());
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the value is one of the supported keys!", MOD_NAME, VERSION);
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON.c_str());
							activation_button = DEFAULT_ACTIVATION_BUTTON;
						}
						else
						{
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, activation_button.c_str());
						}
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON.c_str());
					}

					// Try loading the ritual_chamber_additional_spawn_chance value.
					if (json_object.contains(RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY))
					{
						ritual_chamber_additional_spawn_chance = json_object[RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY];
						if (ritual_chamber_additional_spawn_chance < 0 || ritual_chamber_additional_spawn_chance > 100)
						{
							g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value (%d) in mod configuration file: %s", MOD_NAME, VERSION, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, ritual_chamber_additional_spawn_chance, config_file.c_str());
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the value is a valid integer between 0 and 100 (inclusive)!", MOD_NAME, VERSION);
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE);
							ritual_chamber_additional_spawn_chance = DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE;
						}
						else
						{
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %d!", MOD_NAME, VERSION, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, ritual_chamber_additional_spawn_chance);
						}
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %d!", MOD_NAME, VERSION, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE);
					}

					// Try loading the spawn_ladder_at_player_position value.
					if (json_object.contains(SPAWN_LADDER_AT_PLAYER_POSITION_KEY))
					{
						spawn_ladder_at_player_position = json_object[SPAWN_LADDER_AT_PLAYER_POSITION_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, SPAWN_LADDER_AT_PLAYER_POSITION_KEY, spawn_ladder_at_player_position ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SPAWN_LADDER_AT_PLAYER_POSITION_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, SPAWN_LADDER_AT_PLAYER_POSITION_KEY, DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION ? "true" : "false");
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
				LogDefaultConfigValues();
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"ChutesAndLadders.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();

			LogDefaultConfigValues();
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
		LogDefaultConfigValues();
	}
}

void ModifyHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_HEALTH,
		(PVOID*)&gml_script_modify_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_modify_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

RValue GetMaxHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MAX_HEALTH,
		(PVOID*)&gml_script_get_max_health
	);

	RValue result;
	gml_script_get_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void CreateNotification(std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	uint64_t current_system_time = GetCurrentSystemTime();
	if (current_system_time > notification_name_to_last_display_time_map[notification_localization_str] + 5000)
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

		notification_name_to_last_display_time_map[notification_localization_str] = current_system_time;
	}
}

void SpawnLadder(CInstance* Self, CInstance* Other)
{
	if (LADDER_SPAWN_POINTS.count(ari_current_gm_room) > 0)
	{
		CScript* gml_Script_spawn_ladder = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			GML_SCRIPT_SPAWN_LADDER,
			(PVOID*)&gml_Script_spawn_ladder
		);

		int _x = spawn_ladder_at_player_position ? ((ari_x * 2) / 16) : RValueAsInt(LADDER_SPAWN_POINTS.at(ari_current_gm_room)[0]);
		int _y = spawn_ladder_at_player_position ? ((ari_y * 2) / 16) : RValueAsInt(LADDER_SPAWN_POINTS.at(ari_current_gm_room)[1]);
		RValue x = _x;
		RValue y = _y;
		RValue* x_ptr = &x;
		RValue* y_ptr = &y;
		RValue* rvalue_array[2] = { x_ptr, y_ptr };
		RValue retval;
		gml_Script_spawn_ladder->m_Functions->m_ScriptFunction(
			Self,
			Other,
			retval,
			2,
			rvalue_array
		);

		CreateNotification(LADDER_SPAWNED_LOCALIZATION_KEY, Self, Other);
	}
	else
	{
		CreateNotification(LADDER_NOT_SPAWNED_LOCALIZATION_KEY, Self, Other);
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The current GM room (%s) has no set ladder coordinates!", MOD_NAME, VERSION, ari_current_gm_room.c_str());
	}
}

bool PerkActive(CInstance* Self, CInstance* Other, std::string perk_name)
{
	int64_t perk_id = perk_name_to_id_map[perk_name];

	CScript* gml_script_perk_active = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_perk_active@Ari@Ari",
		(PVOID*)&gml_script_perk_active
	);

	RValue result;
	RValue input = perk_id;
	RValue* input_ptr = &input;
	gml_script_perk_active->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	if (result.m_Kind == VALUE_BOOL && result.m_Real == 1.0)
		return true;
	return false;
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

	if (!strstr(self->m_Object->m_Name, "obj_ari"))
		return;

	if (game_is_active)
	{
		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);

		if (!script_name_to_reference_map.contains("obj_ari"))
			script_name_to_reference_map["obj_ari"] = { global_instance->GetRefMember("__ari")->ToInstance(), self };

		// Track player position.
		RValue x;
		g_ModuleInterface->GetBuiltin("x", self, NULL_INDEX, x);
		ari_x = x.ToDouble();

		RValue y;
		g_ModuleInterface->GetBuiltin("y", self, NULL_INDEX, y);
		ari_y = y.ToDouble();

		// Track active perks.
		//if (PerkActive(global_instance->GetMember("__ari").ToInstance(), self, LOST_TO_HISTORY_PERK_NAME))
		active_perk_map[LOST_TO_HISTORY_PERK_NAME] = true;
		//else
		//	active_perk_map[LOST_TO_HISTORY_PERK_NAME] = false;
	}
}

RValue& GmlScriptAriFaceDirCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ari_facing_dir = Arguments[0]->ToDouble();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FACE_DIR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

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
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_ROOM_START));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (teleport_ari)
	{
		teleport_ari = false;

		CScript* gml_script_register_status_effect = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			"gml_Script_ari_teleport_to_room",
			(PVOID*)&gml_script_register_status_effect
		);

		RValue result;
		RValue arg0 = location_name_to_id_map["mines_entry"];
		RValue arg1 = 192.0;
		RValue arg2 = 352.0;
		RValue* arg0_ptr = &arg0;
		RValue* arg1_ptr = &arg1;
		RValue* arg2_ptr = &arg2;
		RValue* argument_array[3] = { arg0_ptr, arg1_ptr, arg2_ptr };

		ari_is_teleporting = true;
		gml_script_register_status_effect->m_Functions->m_ScriptFunction(
			Self,
			Other,
			result,
			3,
			argument_array
		);
	}

	if (create_ritual_altar)
	{
		RValue layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Impl_Ritual" });
		if (RValueAsBool(layer_exists))
		{
			//RValue obj_dungeon_ritual_altar_index = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_dungeon_ritual_altar" });
			//double x = 192.0;
			//double y = 224.0;
			//std::string layer_name = "Impl_Ritual";
			//g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue(layer_name), obj_dungeon_ritual_altar_index });
			create_ritual_altar = false;
			
			// Meteor fall prep
			meteor_fall = true;

			//RValue spr_mines_level3_lavabubbles_1 = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_mines_level3_lavabubbles_1" });
			//RValue spr_mines_level3_lavabubbles_2 = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_mines_level3_lavabubbles_2" });
			//RValue spr_mines_level3_lavabubbles_3 = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_mines_level3_lavabubbles_3" });
			//RValue spr_meteor_effect = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_meteor_effect" });

			//g_ModuleInterface->CallBuiltin("sprite_assign", { spr_mines_level3_lavabubbles_1, spr_meteor_effect });
			//g_ModuleInterface->CallBuiltin("sprite_assign", { spr_mines_level3_lavabubbles_2, spr_meteor_effect });
			//g_ModuleInterface->CallBuiltin("sprite_assign", { spr_mines_level3_lavabubbles_3, spr_meteor_effect });

			// spr_debris
			//RValue spr_rain_splash = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_rain_splash" });
			//RValue spr_circle_aoe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_circle_aoe" });

			//g_ModuleInterface->CallBuiltin("sprite_assign", { spr_rain_splash, spr_circle_aoe });
		}
	}

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

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	game_is_active = false;
	ari_current_gm_room = "";
	teleport_ari = false;
	ari_is_teleporting = false;
	create_ritual_altar = false;
	ari_x = -1;
	ari_y = -1;
	ari_facing_dir = -1;
	active_perk_map = {};
	meteors.clear();
	meteor_fall = false;
	meteor_sprite_speed = 0.7;
	script_name_to_reference_map = {};

	if (load_on_start)
	{
		LoadPerks();
		LoadLocationIds();
		CreateOrLoadConfigFile();
		ConfigureActivationButton();

		load_on_start = false;
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

RValue& GmlScriptOnBeginStepCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	for (int i = 0; i < meteors.size(); i++)
	{
		if (meteors[i].is_active)
		{
			if (meteors[i].trap == Trap::GAZE)
			{
				RValue spr_trap_gaze = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_gaze" });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[i].instance, "sprite_index", spr_trap_gaze });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[i].instance, "image_speed", 1.0 });
			}
			else if (meteors[i].trap == Trap::METEOR)
			{
				RValue spr_trap_meteor = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_meteor" });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[i].instance, "sprite_index", spr_trap_meteor });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[i].instance, "image_speed", meteor_sprite_speed });
			}
		}
	}

	//if (meteors.size() >= 1 && meteors[0].is_active)
	//{
	//	if(meteors[0].trap == Trap::GAZE)
	//		
	//	else if
	//	
	//}
	//else if (meteors.size() >= 2 && meteors[1].is_active)
	//{
	//	RValue spr_trap_meteor = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_meteor" });
	//	g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[0].instance, "sprite_index", spr_trap_meteor });
	//}

	//if (meteors.size() == 5 && meteors[4].is_active)
	//{
	//	RValue spr_circle_aoe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_circle_aoe_32px" }); // "spr_circle_aoe"
	//	g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[4].instance, "sprite_index", spr_circle_aoe });
	//}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_BEGIN_STEP));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptOnDrawGuiCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (GameWindowHasFocus() && !GameIsPaused() && !processing_user_input && game_is_active)
	{
		bool activate = false;
		if (activation_button_is_controller_key && CheckControllerInput())
			activate = true;
		if (!activation_button_is_controller_key && CheckVirtualKeyInput())
			activate = true;

		if (activate)
		{
			if (ari_current_gm_room.contains("rm_mines") && ari_current_gm_room != "rm_mines_entry" && !ari_current_gm_room.contains("seal"))
			{
				SpawnLadder(Self, Other);
			}
			else {
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Ladders may only be spawned in the dungeon!", MOD_NAME, VERSION);
				CreateNotification(INVALID_LOCATION_LOCALIZATION_KEY, Self, Other);
			}
		}

		processing_user_input = false;
	}

	if (meteor_fall)
	{
		for (int i = 0; i < meteors.size(); i++)
		{
			if (meteors[i].is_active)
			{
				//if(meteors[i].trap == Trap::GAZE)
				//	g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[i].instance, "image_speed", 1.0});
				//if (meteors[i].trap == Trap::METEOR)
				//	g_ModuleInterface->CallBuiltin("variable_instance_set", { meteors[i].instance, "image_speed", 0.35});
			}
		}

		//for (Meteor meteor : meteors)
		//{
		//	if (meteor.is_active)
		//	{
		//		
		//	}
		//}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_DRAW_GUI));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetMinutesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MINUTES));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (meteor_fall)
	{
		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);

		RValue time = global_instance->GetMember("__clock").GetMember("time");
		int current_time_in_seconds = time.ToInt64();

		// TESTING - DEBUG
		RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
		if (instance_layer_exists.ToBoolean())
		{
			if (meteors.size() == 0)
			{
				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" }); // "obj_lavabubble"
				double x = 192.0 + 8;
				double y = 144.0 + 8;
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_assetobject });

				Meteor meteor = Meteor(x, y, current_time_in_seconds, 600, true, instance, Trap::GAZE);
				meteors.push_back(meteor);
			}
			if (meteors.size() == 1)
			{
				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" }); // "obj_lavabubble"
				double x = 192.0 + 8;
				double y = 352.0 + 8;
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_assetobject });

				Meteor meteor = Meteor(x, y, current_time_in_seconds, 600, true, instance, Trap::METEOR);
				meteors.push_back(meteor);
			}

			//if (meteors.size() == 0)
			//{
			//	RValue obj_lavabubble = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_lavabubble" }); // "obj_lavabubble"
			//	double x = 192.0 + 8;
			//	double y = 160.0 + 8;
			//	RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_lavabubble });
			//	
			//	Meteor meteor = Meteor(x, y, current_time_in_seconds, 600, true, instance);
			//	meteors.push_back(meteor);
			//}
			//else if (meteors.size() == 1 && current_time_in_seconds >= meteors[0].spawned_time + 150)
			//{
			//	RValue obj_lavabubble = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_lavabubble" }); // "obj_lavabubble"
			//	double x = 272.0 + 8;
			//	double y = 240.0 + 8;
			//	RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_lavabubble });

			//	Meteor meteor = Meteor(x, y, current_time_in_seconds, 600, true, instance);
			//	meteors.push_back(meteor);
			//}
			//else if (meteors.size() == 2 && current_time_in_seconds >= meteors[1].spawned_time + 150)
			//{
			//	RValue obj_lavabubble = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_lavabubble" }); // "obj_lavabubble"
			//	double x = 192.0 + 8;
			//	double y = 304.0 + 8;
			//	RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_lavabubble });

			//	Meteor meteor = Meteor(x, y, current_time_in_seconds, 600, true, instance);
			//	meteors.push_back(meteor);
			//}
			//else if (meteors.size() == 3 && current_time_in_seconds >= meteors[2].spawned_time + 150)
			//{
			//	RValue obj_lavabubble = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_lavabubble" }); // "obj_lavabubble"
			//	double x = 128.0 + 8;
			//	double y = 240.0 + 8;
			//	RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_lavabubble });

			//	Meteor meteor = Meteor(x, y, current_time_in_seconds, 600, true, instance);
			//	meteors.push_back(meteor);
			//}
			//else if (meteors.size() == 4 && current_time_in_seconds >= meteors[3].spawned_time + 150)
			//{
			//	RValue obj_lavabubble = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_lavabubble" }); // "obj_lavabubble"
			//	double x = 192.0 + 8;
			//	double y = 240.0 + 8;
			//	RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { x, y, RValue("Instances"), obj_lavabubble });

			//	Meteor meteor = Meteor(x, y, current_time_in_seconds, 1800, true, instance);
			//	meteors.push_back(meteor);
			//}

			//RValue sprite = g_ModuleInterface->CallBuiltin(
			//	"asset_get_index",
			//	{ "spr_abandoned_mines_ground_crack_animation" }
			//);

			//g_ModuleInterface->CallBuiltin(
			//	"variable_instance_set",
			//	{ inst, RValue("sprite_index"), sprite }
			//);
		}

		for (Meteor& meteor : meteors)
		{
			if (meteor.is_active && current_time_in_seconds >= meteor.spawned_time + meteor.duration)
			{
				meteor.is_active = false;

				if (meteor.trap == Trap::METEOR)
				{
					double distance = GetDistance(ari_x, ari_y, meteor.x, meteor.y);
					double modifier = CalculateMeteorDamage(distance) / 100.0;

					double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
					ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);
				}

				if (meteor.trap == Trap::GAZE)
				{
					bool facing_trap = FacingTrap(ari_x, ari_y, meteor.x, meteor.y);
					if (facing_trap)
						CreateNotification("mods/ExamplePlugin/gaze/pass", Self, Other);
					else
						CreateNotification("mods/ExamplePlugin/gaze/fail", Self, Other);
				}

				RValue meteor_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { meteor.instance } );
				if (meteor_instance_exists.ToBoolean())
					g_ModuleInterface->CallBuiltin("instance_destroy", { meteor.instance } );
			}
		}
	}

	return Result;
}

RValue& GmlScriptErrorCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CHECK_VALUE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);


	//if (game_is_active)
	//{
	//	if (Arguments[0]->ToInt64() == 3)
	//		Result = true;
	//	else
	//		Result = false;
	//}

	return Result;
}

RValue& GmlScriptGoToRoomCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (ari_is_teleporting)
	{
		if (ari_current_gm_room.contains("rm_mines_tide"))
			*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_tide_ritual_chamber" });
		if (ari_current_gm_room.contains("rm_mines_deep"))
			*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_deep_ritual_chamber" });
		if (ari_current_gm_room.contains("rm_mines_lava"))
			*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_lava_ritual_chamber" });
		if (ari_current_gm_room.contains("rm_mines_ruins"))
			*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_ruins_ritual_chamber" });
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GO_TO_ROOM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	RValue gm_room = StructVariableGet(Result, "gm_room");
	RValue room_name = g_ModuleInterface->CallBuiltin("room_get_name", { gm_room });
	ari_current_gm_room = room_name.ToString();

	if (ari_is_teleporting)
	{
		ari_is_teleporting = false;
		create_ritual_altar = true;
	}
	else if (ritual_chamber_additional_spawn_chance > 0 && active_perk_map[LOST_TO_HISTORY_PERK_NAME])
	{
		if (!create_ritual_altar && ari_current_gm_room.contains("rm_mines") && ari_current_gm_room != "rm_mines_entry" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure") && !ari_current_gm_room.contains("milestone"))
		{
			if (ari_current_gm_room.find("rm_mines_tide") != std::string::npos || ari_current_gm_room.find("rm_mines_deep") != std::string::npos || ari_current_gm_room.find("rm_mines_lava") != std::string::npos || ari_current_gm_room.find("rm_mines_ruins") != std::string::npos)
			{
				std::uniform_int_distribution<int> distribution(1, 100);
				int random = distribution(generator);
				if (random <= ritual_chamber_additional_spawn_chance)
					teleport_ari = true;

			}
		}
	}
	
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

void CreateHookGmlScriptAriFaceDir(AurieStatus& status)
{
	CScript* gml_script_ari_face_dir = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FACE_DIR,
		(PVOID*)&gml_script_ari_face_dir
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FACE_DIR);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FACE_DIR,
		gml_script_ari_face_dir->m_Functions->m_ScriptFunction,
		GmlScriptAriFaceDirCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FACE_DIR);
	}
}

void CreateHookGmlScriptOnRoomStart(AurieStatus& status)
{
	CScript* gml_script_show_room_title = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_ROOM_START,
		(PVOID*)&gml_script_show_room_title
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_ROOM_START);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_ROOM_START,
		gml_script_show_room_title->m_Functions->m_ScriptFunction,
		GmlScriptOnRoomStartCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_ROOM_START);
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
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
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
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
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

void CreateHookGmlScriptOnBeginStep(AurieStatus& status)
{
	CScript* gml_script_on_begin_step = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_BEGIN_STEP,
		(PVOID*)&gml_script_on_begin_step
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_BEGIN_STEP);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_BEGIN_STEP,
		gml_script_on_begin_step->m_Functions->m_ScriptFunction,
		GmlScriptOnBeginStepCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_BEGIN_STEP);
	}
}

void CreateHookGmlScriptOnDrawGui(AurieStatus& status)
{
	CScript* gml_script_try_location_id_to_string = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_DRAW_GUI,
		(PVOID*)&gml_script_try_location_id_to_string
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DRAW_GUI);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_DRAW_GUI,
		gml_script_try_location_id_to_string->m_Functions->m_ScriptFunction,
		GmlScriptOnDrawGuiCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DRAW_GUI);
	}
}

void CreateHookGmlScriptGetMinutes(AurieStatus& status)
{
	CScript* gml_script_get_minutes = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MINUTES,
		(PVOID*)&gml_script_get_minutes
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MINUTES);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_MINUTES,
		gml_script_get_minutes->m_Functions->m_ScriptFunction,
		GmlScriptGetMinutesCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MINUTES);
	}
}

void CreateHookGmlScriptError(AurieStatus& status)
{
	CScript* gml_script_error = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CHECK_VALUE,
		(PVOID*)&gml_script_error
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHECK_VALUE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CHECK_VALUE,
		gml_script_error->m_Functions->m_ScriptFunction,
		GmlScriptErrorCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHECK_VALUE);
	}
}

void CreateHookGmlScriptGoToRoom(AurieStatus& status)
{
	CScript* gml_script_go_to_room = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GO_TO_ROOM,
		(PVOID*)&gml_script_go_to_room
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GO_TO_ROOM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GO_TO_ROOM,
		gml_script_go_to_room->m_Functions->m_ScriptFunction,
		GmlScriptGoToRoomCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GO_TO_ROOM);
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

	CreateHookGmlScriptAriFaceDir(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptOnRoomStart(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetWeather(status);
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

	CreateHookGmlScriptOnBeginStep(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptOnDrawGui(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetMinutes(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptError(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGoToRoom(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}