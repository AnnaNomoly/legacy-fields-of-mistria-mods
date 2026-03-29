#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "SaveAnywhere";
static const char* const VERSION = "1.2.4";
static const char* const ACTIVATION_BUTTON_KEY = "activation_button";
static const char* const SAVE_LOCATION_KEY = "save_location";
static const char* const SAVE_X_POSITION_KEY = "save_x_position";
static const char* const SAVE_Y_POSITION_KEY = "save_y_position";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const char* const GML_SCRIPT_CREATE_SAVE_NOTIFICATION = "gml_Script_create_save_notification";
static const char* const GML_SCRIPT_SAVE_GAME = "gml_Script_save_game";
static const char* const GML_SCRIPT_LOAD_GAME = "gml_Script_load_game";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_SHOW_ROOM_TITLE = "gml_Script_show_room_title";
static const char* const GML_SCRIPT_TRY_LOCATION_ID_TO_STRING = "gml_Script_try_location_id_to_string";
static const char* const GML_SCRIPT_END_DAY = "gml_Script_end_day";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_ON_DRAW_GUI = "gml_Script_on_draw_gui@Display@Display";
static const std::string SAVING_DISABLED_NOTIFICATION_KEY = "Notifications/Mods/SaveAnywhere/saving_disabled";
static const std::string NEW_GAME_NOTIFICATION_KEY = "Notifications/Mods/SaveAnywhere/new_game_warning";
static const std::string DUNGEON_SAVE_NOTIFICATION_KEY = "Notifications/Mods/SaveAnywhere/location/dungeon";
static const std::string FARM_BUILDING_SAVE_NOTIFICATION_KEY = "Notifications/Mods/SaveAnywhere/location/farm_building";
static const std::string SMALL_BARN = "small_barn";
static const std::string MEDIUM_BARN = "medium_barn";
static const std::string LARGE_BARN = "large_barn";
static const std::string SMALL_COOP = "small_coop";
static const std::string MEDIUM_COOP = "medium_coop";
static const std::string LARGE_COOP = "large_coop";
static const std::string SMALL_GREENHOUSE = "small_greenhouse";
static const std::string LARGE_GREENHOUSE = "large_greenhouse";
static const std::string MINES_ENTRY = "mines_entry";
static const std::string DUNGEON = "dungeon";
static const std::string WATER_SEAL = "water_seal";
static const std::string EARTH_SEAL = "earth_seal";
static const std::string FIRE_SEAL = "fire_seal";
static const std::string RUINS_SEAL = "ruins_seal";
static const std::string VOID_SEAL = "void_seal";
static const std::string PRIESTESS_QUARTERS = "priestess_quarters";
static const std::string SERIDIAS_CHAMBER = "seridias_chamber";
static const std::string FARM = "farm";
static const double UNSET_INT = -1;
static const std::string DEFAULT_ACTIVATION_BUTTON = "HOME";
static const std::string ALLOWED_ACTIVATION_BUTTONS[] = {
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
	"NUMPAD_0", "NUMPAD_1", "NUMPAD_2", "NUMPAD_3", "NUMPAD_4", "NUMPAD_5", "NUMPAD_6", "NUMPAD_7", "NUMPAD_8", "NUMPAD_9",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"INSERT", "DELETE", "HOME", "PAGE_UP", "PAGE_DOWN", "NUM_LOCK", "SCROLL_LOCK", "CAPS_LOCK", "PAUSE_BREAK",
	"GAMEPAD_A", "GAMEPAD_B", "GAMEPAD_X", "GAMEPAD_Y", "GAMEPAD_LEFT_SHOULDER", "GAMEPAD_RIGHT_SHOULDER", "GAMEPAD_LEFT_TRIGGER", "GAMEPAD_RIGHT_TRIGGER", "GAMEPAD_DPAD_UP", "GAMEPAD_DPAD_DOWN", "GAMEPAD_DPAD_LEFT", "GAMEPAD_DPAD_RIGHT", "GAMEPAD_LEFT_STICK", "GAMEPAD_RIGHT_STICK", "GAMEPAD_SELECT", "GAMEPAD_START"
};

static YYTKInterface* g_ModuleInterface = nullptr;
static RValue __YYTK;
static bool load_on_start = true;
static bool game_is_active = false; // Used to indicate if the game is NOT on the title screen AND a file is being played. 
static bool mod_healthy = false;
static bool mod_save = false;
static int ari_x = UNSET_INT;
static int ari_y = UNSET_INT;
static int ari_room_id = UNSET_INT;
static int saved_x = UNSET_INT;
static int saved_y = UNSET_INT;
static std::string saved_room_name = "";
static bool wait_to_teleport_ari = false;
static bool ready_to_teleport_ari = false;
static bool wait_to_reposition_ari = false;
static bool ready_to_reposition_ari = false;
static bool activation_button_is_controller_key = false;
static int activation_button_int_value = -1;
static bool processing_user_input = false;
static std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
static std::string save_prefix = "";
static std::string save_folder = "";
static std::string mod_folder = "";
static std::map<int, std::string> location_id_to_name_map = {};
static std::map<std::string, int> location_name_to_id_map = {};

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

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return struct_exists.ToBoolean();
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return global_variable_exists.ToBoolean();
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists("__YYTK"))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet("__YYTK", __YYTK);
	}
	else
		__YYTK = GlobalVariableGet("__YYTK");
}

bool GameWindowHasFocus()
{
	RValue window_has_focus = g_ModuleInterface->CallBuiltin("window_has_focus", {});
	return RValueAsBool(window_has_focus);
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

void LogDefaultConfigValues()
{
	activation_button = DEFAULT_ACTIVATION_BUTTON;

	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON);
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

		// Try to find the mod_data/SaveAnywhere directory.
		std::string save_anywhere_folder = mod_data_folder + "\\SaveAnywhere";
		if (!std::filesystem::exists(save_anywhere_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"SaveAnywhere\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, save_anywhere_folder.c_str());
			std::filesystem::create_directory(save_anywhere_folder);
		}

		// Try to find the mod_data/SaveAnywhere/SaveAnywhere.json config file.
		std::string config_file = save_anywhere_folder + "\\" + "SaveAnywhere.json";
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
				}
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

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"SaveAnywhere.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());
			json default_json = {
				{ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON}
			};

			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_json << std::endl;
			out_stream.close();

			LogDefaultConfigValues();
		}

		mod_folder = save_anywhere_folder;
		mod_healthy = true;
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);
		mod_healthy = false;
	}
}

void CreateNotification(CInstance* Self, CInstance* Other, std::string notification_localization_str)
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

void WriteModSaveFile(CInstance* Self, CInstance* Other)
{
	// Write the custom mod data file.
	if (save_prefix.length() != 0 && mod_folder.length() != 0 && location_id_to_name_map.count(ari_room_id) > 0)
	{
		json mod_save_data = {};

		// Override dungeon locations.
		if (location_id_to_name_map[ari_room_id] == DUNGEON ||
			location_id_to_name_map[ari_room_id] == WATER_SEAL ||
			location_id_to_name_map[ari_room_id] == EARTH_SEAL ||
			location_id_to_name_map[ari_room_id] == FIRE_SEAL ||
			location_id_to_name_map[ari_room_id] == RUINS_SEAL ||
			location_id_to_name_map[ari_room_id] == VOID_SEAL ||
			location_id_to_name_map[ari_room_id] == PRIESTESS_QUARTERS ||
			location_id_to_name_map[ari_room_id] == SERIDIAS_CHAMBER)
		{
			mod_save_data[SAVE_LOCATION_KEY] = MINES_ENTRY;
			mod_save_data[SAVE_X_POSITION_KEY] = 216;
			mod_save_data[SAVE_Y_POSITION_KEY] = 198;

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are currently in the dungeon! Your location will be saved as \"mines_entry\" to avoid errors.", MOD_NAME, VERSION);
			g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Saving your game at the fallback location: \"mines_entry\".", MOD_NAME, VERSION);
			CreateNotification(Self, Other, DUNGEON_SAVE_NOTIFICATION_KEY);
		}
		// Farm building locations.
		else if (location_id_to_name_map[ari_room_id] == SMALL_BARN ||
			location_id_to_name_map[ari_room_id] == MEDIUM_BARN ||
			location_id_to_name_map[ari_room_id] == LARGE_BARN ||
			location_id_to_name_map[ari_room_id] == SMALL_COOP ||
			location_id_to_name_map[ari_room_id] == MEDIUM_COOP ||
			location_id_to_name_map[ari_room_id] == LARGE_COOP ||
			location_id_to_name_map[ari_room_id] == SMALL_GREENHOUSE ||
			location_id_to_name_map[ari_room_id] == LARGE_GREENHOUSE)
		{
			mod_save_data[SAVE_LOCATION_KEY] = FARM;
			mod_save_data[SAVE_X_POSITION_KEY] = 1032;
			mod_save_data[SAVE_Y_POSITION_KEY] = 87;

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are currently in a farm building! Your location will be saved as \"farm\" to avoid errors.", MOD_NAME, VERSION);
			g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Saving your game at the fallback location: \"farm\".", MOD_NAME, VERSION);
			CreateNotification(Self, Other, FARM_BUILDING_SAVE_NOTIFICATION_KEY);
		}
		else
		{
			mod_save_data[SAVE_LOCATION_KEY] = location_id_to_name_map[ari_room_id];
			mod_save_data[SAVE_X_POSITION_KEY] = ari_x;
			mod_save_data[SAVE_Y_POSITION_KEY] = ari_y;

			g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Saving your game at your current location: \"%s\".", MOD_NAME, VERSION, location_id_to_name_map[ari_room_id].c_str());
		}

		std::exception_ptr eptr;
		try
		{
			std::ofstream out_stream(mod_folder + "\\" + save_prefix + ".json");
			out_stream << std::setw(4) << mod_save_data << std::endl;
			out_stream.close();
			g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Successfully saved the mod file!", MOD_NAME, VERSION);
		}
		catch (...)
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to write the mod file! Your game was saved but the custom location was NOT. You will load in the Farmhouse.", MOD_NAME, VERSION);

			eptr = std::current_exception();
			PrintError(eptr);
		}
	}
	else
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to write the mod file! Your game was saved but the custom location was NOT. You will load in the Farmhouse.", MOD_NAME, VERSION);
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
			saved_room_name = mod_save_data[SAVE_LOCATION_KEY];
			saved_x = mod_save_data[SAVE_X_POSITION_KEY];
			saved_y = mod_save_data[SAVE_Y_POSITION_KEY];

			wait_to_teleport_ari = true;
			g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Successfully loaded your saved location: \"%s\".", MOD_NAME, VERSION, saved_room_name.c_str());
		}
	}
	catch (...)
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred reading the mod file.", MOD_NAME, VERSION);

		eptr = std::current_exception();
		PrintError(eptr);
	}
}

void ResetStaticFields(bool returnedToTitleScreen)
{
	if (returnedToTitleScreen)
	{
		save_prefix = "";
		game_is_active = false;
	}

	mod_save = false;
	ari_x = UNSET_INT;
	ari_y = UNSET_INT;
	ari_room_id = UNSET_INT;
	saved_x = UNSET_INT;
	saved_y = UNSET_INT;
	saved_room_name = "";
	wait_to_teleport_ari = false;
	ready_to_teleport_ari = false;
	wait_to_reposition_ari = false;
	ready_to_reposition_ari = false;
}

void SaveGame(CInstance* Self, CInstance* Other)
{
	mod_save = true;

	CScript* gml_script_save_game = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SAVE_GAME,
		(PVOID*)&gml_script_save_game
	);

	std::string file_name = save_folder + "\\" + "game-" + save_prefix + "-autosave.sav";
	RValue result;
	RValue argument = RValue(file_name);
	RValue* argument_ptr = &argument;

	gml_script_save_game->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &argument_ptr }
	);
}

void DisplaySaveNotification(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_save_game_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_SAVE_NOTIFICATION,
		(PVOID*)&gml_script_save_game_notification
	);

	RValue result;
	gml_script_save_game_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

bool LoadLocationIds()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	// Load locations.
	size_t array_length;
	RValue location_ids = *global_instance->GetRefMember("__location_id__");
	g_ModuleInterface->GetArraySize(location_ids, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(location_ids, i, array_element);

		location_id_to_name_map[i] = array_element->ToString();
		location_name_to_id_map[array_element->ToString()] = i;
	}

	if (location_id_to_name_map.size() > 0 && location_name_to_id_map.size() > 0)
	{
		return true;
	}
	else {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to load location IDs!", MOD_NAME, VERSION);
		return false;
	}
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

	if (mod_healthy)
	{
		RValue x;
		g_ModuleInterface->GetBuiltin("x", self, NULL_INDEX, x);
		ari_x = x.m_Real;

		RValue y;
		g_ModuleInterface->GetBuiltin("y", self, NULL_INDEX, y);
		ari_y = y.m_Real;

		if (wait_to_teleport_ari && ready_to_teleport_ari)
		{
			CScript* gml_script_goto_location_id = nullptr;
			g_ModuleInterface->GetNamedRoutinePointer(
				"gml_Script_goto_location_id",
				(PVOID*)&gml_script_goto_location_id
			);

			RValue result;
			RValue location_id = location_name_to_id_map[saved_room_name];
			RValue* location_id_ptr = &location_id;
			gml_script_goto_location_id->m_Functions->m_ScriptFunction(
				self,
				other,
				result,
				1,
				{ &location_id_ptr }
			);

			saved_room_name = "";
			wait_to_teleport_ari = false;
			ready_to_teleport_ari = false;
			wait_to_reposition_ari = true;
		}
		else if (wait_to_reposition_ari && ready_to_reposition_ari) {
			RValue x = saved_x;
			RValue y = saved_y;

			g_ModuleInterface->SetBuiltin("x", self, NULL_INDEX, x);
			g_ModuleInterface->SetBuiltin("y", self, NULL_INDEX, y);

			saved_x = UNSET_INT;
			saved_y = UNSET_INT;
			wait_to_reposition_ari = false;
			ready_to_reposition_ari = false;
		}
	}
}

RValue& GmlScriptSaveGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		if (!mod_save)
		{
			// No save prefix has been detected. This should only happen when a new game is started.
			if (save_prefix.size() == 0)
			{
				// Get the save folder and file name.
				std::string save_file = Arguments[0]->ToString();
				std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
				std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);
				save_folder = save_file.substr(0, save_file_name_delimiter_index);

				// Check it's a valid value.
				if (save_name.find("undefined") == std::string::npos)
				{
					// Get the save prefix.
					std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
					std::size_t second_hyphen_index = save_name.find_last_of("-");
					save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));
				}
			}
		}
		else
		{
			mod_save = false;
		}
	}

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
	if (mod_healthy)
	{
		// Get the save folder and file name.
		std::string save_file = Arguments[0]->GetRefMember("save_path")->ToString();
		std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
		std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);
		save_folder = save_file.substr(0, save_file_name_delimiter_index);

		// Get the save prefix.
		std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
		std::size_t second_hyphen_index = save_name.find_last_of("-");
		save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));

		if (save_name.find("autosave") != std::string::npos)
		{
			ReadModSaveFile();
		}
	}

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

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		game_is_active = true;
	}

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
	if (mod_healthy)
	{
		if (wait_to_reposition_ari) {
			ready_to_reposition_ari = true;
		}
		if (wait_to_teleport_ari) {
			ready_to_teleport_ari = true;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SHOW_ROOM_TITLE));
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
	if (mod_healthy)
	{
		if(Arguments[0]->m_Kind == VALUE_REAL)
			ari_room_id = Arguments[0]->m_Real;
		if (Arguments[0]->m_Kind == VALUE_INT64)
			ari_room_id = Arguments[0]->m_i64;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

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
	if (mod_healthy)
	{
		// Remove the mod file for this save (if one exists).
		std::string file_name = mod_folder + "\\" + save_prefix + ".json";
		std::remove(file_name.c_str());

		ResetStaticFields(false);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_END_DAY));
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
		CreateOrLoadConfigFile();
		ConfigureActivationButton();
		CreateOrGetGlobalYYTKVariable();

		mod_healthy = mod_healthy && LoadLocationIds();
		if (!mod_healthy)
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - The mod is not healthy and will not function!", MOD_NAME, VERSION);
		}

		load_on_start = false;
	}
	else
	{
		if (mod_healthy)
		{
			ResetStaticFields(true);
		}
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

RValue& GmlScriptOnDrawGuiCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		if (GameWindowHasFocus() && !processing_user_input && game_is_active)
		{
			bool activate = false;
			if (activation_button_is_controller_key && CheckControllerInput())
				activate = true;
			if (!activation_button_is_controller_key && CheckVirtualKeyInput())
				activate = true;

			if (activate)
			{
				if (save_prefix.size() != 0)
				{
					// Check if the Deep Dungeon mod is being used.
					boolean save_disabled = false;
					if (StructVariableExists(__YYTK, "DeepDungeon"))
					{
						RValue deep_dungeon = __YYTK.GetMember("DeepDungeon");
						if (StructVariableExists(deep_dungeon, "floor"))
						{
							RValue floor = deep_dungeon.GetMember("floor");
							if (IsNumeric(floor))
							{
								int floor_number = floor.ToInt64();
								if (floor_number != 0)
								{
									CreateNotification(Self, Other, SAVING_DISABLED_NOTIFICATION_KEY);
									save_disabled = true;
								}
							}
						}
					}

					if (!save_disabled)
					{
						WriteModSaveFile(Self, Other);
						SaveGame(Self, Other);
						DisplaySaveNotification(Self, Other);
					}
				}
				else
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Game was NOT saved! An autosave file has not been created. If this is a new file you must save at the bed at least once.", MOD_NAME, VERSION);
					CreateNotification(Self, Other, NEW_GAME_NOTIFICATION_KEY);
				}
			}

			processing_user_input = false;
		}
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

void CreateHookObject(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook (ObjectCallback)!", MOD_NAME, VERSION);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LOAD_GAME);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LOAD_GAME);
	}
}

void CreateHookGmlScriptGetWeather(AurieStatus& status)
{
	CScript* gml_script_show_room_title = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_WEATHER,
		(PVOID*)&gml_script_show_room_title
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_WEATHER,
		gml_script_show_room_title->m_Functions->m_ScriptFunction,
		GmlScriptGetWeatherCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}
}

void CreateHookGmlScriptShowRoomTitle(AurieStatus& status)
{
	CScript* gml_script_show_room_title = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SHOW_ROOM_TITLE,
		(PVOID*)&gml_script_show_room_title
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SHOW_ROOM_TITLE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SHOW_ROOM_TITLE,
		gml_script_show_room_title->m_Functions->m_ScriptFunction,
		GmlScriptShowRoomTitleCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SHOW_ROOM_TITLE);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_END_DAY);
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
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_END_DAY);
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

void CreateHookGmlScriptOnDrawGui(AurieStatus& status)
{
	CScript* gml_script_on_draw_gui = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_DRAW_GUI,
		(PVOID*)&gml_script_on_draw_gui
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DRAW_GUI);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_DRAW_GUI,
		gml_script_on_draw_gui->m_Functions->m_ScriptFunction,
		GmlScriptOnDrawGuiCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DRAW_GUI);
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

	CreateHookObject(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSaveGame(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptLoadGame(status);
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
	
	CreateHookGmlScriptShowRoomTitle(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptTryLocationIdToString(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptEndDay(status);
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

	CreateHookGmlScriptOnDrawGui(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}