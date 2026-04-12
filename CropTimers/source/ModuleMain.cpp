#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "CropTimers";
static const char* const VERSION = "1.0.1";
static const char* const GML_SCRIPT_ON_DRAW_GUI = "gml_Script_on_draw_gui@Display@Display";
static const char* const GML_SCRIPT_NODE_OBJECT_SET_SPRITE = "gml_Script_set_sprite@gml_Object_obj_node_renderer_Create_0";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const TIMERS_START_VISIBLE_JSON_KEY = "timers_start_visible";
static const char* const ONLY_SHOW_TIMER_ZERO_JSON_KEY = "only_show_timer_zero";
static const char* const RED_TIMER_ZERO_JSON_KEY = "red_timer_zero";
static const char* const ACTIVATION_BUTTON_JSON_KEY = "activation_button";
static const bool DEFAULT_TIMERS_START_VISIBLE = true;
static const bool DEFAULT_ONLY_SHOW_TIMER_ZERO = false;
static const bool DEFAULT_RED_TIMER_ZERO = true;
static const std::string DEFAULT_ACTIVATION_BUTTON = "T";
static const std::string ALLOWED_ACTIVATION_BUTTONS[] = {
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
	"NUMPAD_0", "NUMPAD_1", "NUMPAD_2", "NUMPAD_3", "NUMPAD_4", "NUMPAD_5", "NUMPAD_6", "NUMPAD_7", "NUMPAD_8", "NUMPAD_9",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"INSERT", "DELETE", "HOME", "PAGE_UP", "PAGE_DOWN", "NUM_LOCK", "SCROLL_LOCK", "CAPS_LOCK", "PAUSE_BREAK",
	"GAMEPAD_A", "GAMEPAD_B", "GAMEPAD_X", "GAMEPAD_Y", "GAMEPAD_LEFT_SHOULDER", "GAMEPAD_RIGHT_SHOULDER", "GAMEPAD_LEFT_TRIGGER", "GAMEPAD_RIGHT_TRIGGER", "GAMEPAD_DPAD_UP", "GAMEPAD_DPAD_DOWN", "GAMEPAD_DPAD_LEFT", "GAMEPAD_DPAD_RIGHT", "GAMEPAD_LEFT_STICK", "GAMEPAD_RIGHT_STICK", "GAMEPAD_SELECT", "GAMEPAD_START"
};

static struct Configuration {
	bool timers_visible = DEFAULT_TIMERS_START_VISIBLE;
	bool timers_start_visible = DEFAULT_TIMERS_START_VISIBLE;
	bool only_show_timer_zero = DEFAULT_ONLY_SHOW_TIMER_ZERO;
	bool red_timer_zero = DEFAULT_RED_TIMER_ZERO;
	std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
	bool activation_button_is_controller_key = false;
	int activation_button_int_value = -1;
};

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static Configuration configuration = Configuration();
static bool load_on_start = true;
static bool processing_user_input = false;
static std::map<std::string, int> object_category_to_id_map = {};

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

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, std::string variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, RValue(variable_name) }
	);

	return RValueAsBool(struct_exists);
}

RValue StructVariableGet(RValue the_struct, std::string variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, RValue(variable_name) }
	);
}

void LoadObjectCategories()
{
	RValue object_categories = global_instance->GetMember("__object_category__");
	RValue object_categories_length = g_ModuleInterface->CallBuiltin("array_length", { object_categories });

	for (size_t i = 0; i < object_categories_length.ToInt64(); ++i)
	{
		RValue object_category = g_ModuleInterface->CallBuiltin("array_get", { object_categories, i });
		object_category_to_id_map[object_category.ToString()] = i;
	}
}

bool GameWindowHasFocus()
{
	RValue window_has_focus = g_ModuleInterface->CallBuiltin("window_has_focus", {});
	return RValueAsBool(window_has_focus);
}

int ActivationButtonToVirtualKey()
{
	// Function Keys
	if (configuration.activation_button == "F1")
		return VK_F1;
	if (configuration.activation_button == "F2")
		return VK_F2;
	if (configuration.activation_button == "F3")
		return VK_F3;
	if (configuration.activation_button == "F4")
		return VK_F4;
	if (configuration.activation_button == "F5")
		return VK_F5;
	if (configuration.activation_button == "F6")
		return VK_F6;
	if (configuration.activation_button == "F7")
		return VK_F7;
	if (configuration.activation_button == "F8")
		return VK_F8;
	if (configuration.activation_button == "F9")
		return VK_F9;
	if (configuration.activation_button == "F10")
		return VK_F10;
	if (configuration.activation_button == "F11")
		return VK_F11;
	if (configuration.activation_button == "F12")
		return VK_F12;

	// Numpad
	if (configuration.activation_button == "NUMPAD_0")
		return VK_NUMPAD0;
	if (configuration.activation_button == "NUMPAD_1")
		return VK_NUMPAD1;
	if (configuration.activation_button == "NUMPAD_2")
		return VK_NUMPAD2;
	if (configuration.activation_button == "NUMPAD_3")
		return VK_NUMPAD3;
	if (configuration.activation_button == "NUMPAD_4")
		return VK_NUMPAD4;
	if (configuration.activation_button == "NUMPAD_5")
		return VK_NUMPAD5;
	if (configuration.activation_button == "NUMPAD_6")
		return VK_NUMPAD6;
	if (configuration.activation_button == "NUMPAD_7")
		return VK_NUMPAD7;
	if (configuration.activation_button == "NUMPAD_8")
		return VK_NUMPAD8;
	if (configuration.activation_button == "NUMPAD_9")
		return VK_NUMPAD9;

	// Numbers
	if (configuration.activation_button == "0")
		return '0';
	if (configuration.activation_button == "1")
		return '1';
	if (configuration.activation_button == "2")
		return '2';
	if (configuration.activation_button == "3")
		return '3';
	if (configuration.activation_button == "4")
		return '4';
	if (configuration.activation_button == "5")
		return '5';
	if (configuration.activation_button == "6")
		return '6';
	if (configuration.activation_button == "7")
		return '7';
	if (configuration.activation_button == "8")
		return '8';
	if (configuration.activation_button == "9")
		return '9';

	// Letters
	if (configuration.activation_button == "A")
		return 'A';
	if (configuration.activation_button == "B")
		return 'B';
	if (configuration.activation_button == "C")
		return 'C';
	if (configuration.activation_button == "D")
		return 'D';
	if (configuration.activation_button == "E")
		return 'E';
	if (configuration.activation_button == "F")
		return 'F';
	if (configuration.activation_button == "G")
		return 'G';
	if (configuration.activation_button == "H")
		return 'H';
	if (configuration.activation_button == "I")
		return 'I';
	if (configuration.activation_button == "J")
		return 'J';
	if (configuration.activation_button == "K")
		return 'K';
	if (configuration.activation_button == "L")
		return 'L';
	if (configuration.activation_button == "M")
		return 'M';
	if (configuration.activation_button == "N")
		return 'N';
	if (configuration.activation_button == "O")
		return 'O';
	if (configuration.activation_button == "P")
		return 'P';
	if (configuration.activation_button == "Q")
		return 'Q';
	if (configuration.activation_button == "R")
		return 'R';
	if (configuration.activation_button == "S")
		return 'S';
	if (configuration.activation_button == "T")
		return 'T';
	if (configuration.activation_button == "U")
		return 'U';
	if (configuration.activation_button == "V")
		return 'V';
	if (configuration.activation_button == "W")
		return 'W';
	if (configuration.activation_button == "X")
		return 'X';
	if (configuration.activation_button == "Y")
		return 'Y';
	if (configuration.activation_button == "Z")
		return 'Z';

	// Special
	if (configuration.activation_button == "INSERT")
		return VK_INSERT;
	if (configuration.activation_button == "DELETE")
		return VK_DELETE;
	if (configuration.activation_button == "HOME")
		return VK_HOME;
	if (configuration.activation_button == "PAGE_UP")
		return VK_PRIOR;
	if (configuration.activation_button == "PAGE_DOWN")
		return VK_NEXT;
	if (configuration.activation_button == "NUM_LOCK")
		return VK_NUMLOCK;
	if (configuration.activation_button == "SCROLL_LOCK")
		return VK_SCROLL;
	if (configuration.activation_button == "CAPS_LOCK")
		return VK_CAPITAL;
	if (configuration.activation_button == "PAUSE_BREAK")
		return VK_PAUSE;
}

int ActivationButtonToControllerKey()
{
	if (configuration.activation_button == "GAMEPAD_A")
		return 0x8001;
	if (configuration.activation_button == "GAMEPAD_B")
		return 0x8002;
	if (configuration.activation_button == "GAMEPAD_X")
		return 0x8003;
	if (configuration.activation_button == "GAMEPAD_Y")
		return 0x8004;
	if (configuration.activation_button == "GAMEPAD_LEFT_SHOULDER")
		return 0x8005;
	if (configuration.activation_button == "GAMEPAD_RIGHT_SHOULDER")
		return 0x8006;
	if (configuration.activation_button == "GAMEPAD_LEFT_TRIGGER")
		return 0x8007;
	if (configuration.activation_button == "GAMEPAD_RIGHT_TRIGGER")
		return 0x8008;
	if (configuration.activation_button == "GAMEPAD_SELECT")
		return 0x8009;
	if (configuration.activation_button == "GAMEPAD_START")
		return 0x800A;
	if (configuration.activation_button == "GAMEPAD_LEFT_STICK")
		return 0x800B;
	if (configuration.activation_button == "GAMEPAD_RIGHT_STICK")
		return 0x800C;
	if (configuration.activation_button == "GAMEPAD_DPAD_UP")
		return 0x800D;
	if (configuration.activation_button == "GAMEPAD_DPAD_DOWN")
		return 0x800E;
	if (configuration.activation_button == "GAMEPAD_DPAD_LEFT")
		return 0x800F;
	if (configuration.activation_button == "GAMEPAD_DPAD_RIGHT")
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
		RValue button_pressed = g_ModuleInterface->CallBuiltin("gamepad_button_check_pressed", { gamepad_slot, configuration.activation_button_int_value });
		return button_pressed.ToBoolean();
	}

	return false;
}

bool CheckVirtualKeyInput()
{
	RValue key_pressed = g_ModuleInterface->CallBuiltin("keyboard_check_pressed", { configuration.activation_button_int_value });
	if (RValueAsBool(key_pressed))
	{
		return true;
	}
	return false;
}

void ConfigureActivationButton()
{
	if (configuration.activation_button.find("GAMEPAD") != std::string::npos)
	{
		configuration.activation_button_is_controller_key = true;
		configuration.activation_button_int_value = ActivationButtonToControllerKey();
	}
	else
	{
		configuration.activation_button_is_controller_key = false;
		configuration.activation_button_int_value = ActivationButtonToVirtualKey();
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
		{ TIMERS_START_VISIBLE_JSON_KEY, use_defaults ? DEFAULT_TIMERS_START_VISIBLE : configuration.timers_start_visible },
		{ ONLY_SHOW_TIMER_ZERO_JSON_KEY, use_defaults ? DEFAULT_ONLY_SHOW_TIMER_ZERO : configuration.only_show_timer_zero },
		{ RED_TIMER_ZERO_JSON_KEY, use_defaults ? DEFAULT_RED_TIMER_ZERO : configuration.red_timer_zero },
		{ ACTIVATION_BUTTON_JSON_KEY, use_defaults ? DEFAULT_ACTIVATION_BUTTON : configuration.activation_button }
	};
	return config_json;
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

		// Try to find the mod_data/CropTimers directory.
		std::string crop_timers_folder = mod_data_folder + "\\CropTimers";
		if (!std::filesystem::exists(crop_timers_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"CropTimers\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, crop_timers_folder.c_str());
			std::filesystem::create_directory(crop_timers_folder);
		}

		// Try to find the mod_data/CropTimers/CropTimers.json config file.
		bool update_config_file = false;
		std::string config_file = crop_timers_folder + "\\" + "CropTimers.json";
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
				}
				else
				{
					// Try loading the timers_start_visible value.
					if (json_object.contains(TIMERS_START_VISIBLE_JSON_KEY))
					{
						configuration.timers_start_visible = json_object[TIMERS_START_VISIBLE_JSON_KEY];
						configuration.timers_visible = configuration.timers_start_visible;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, TIMERS_START_VISIBLE_JSON_KEY, configuration.timers_start_visible ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, TIMERS_START_VISIBLE_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, TIMERS_START_VISIBLE_JSON_KEY, DEFAULT_TIMERS_START_VISIBLE ? "true" : "false");
					}

					// Try loading the only_show_timer_zero value.
					if (json_object.contains(ONLY_SHOW_TIMER_ZERO_JSON_KEY))
					{
						configuration.only_show_timer_zero = json_object[ONLY_SHOW_TIMER_ZERO_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, ONLY_SHOW_TIMER_ZERO_JSON_KEY, configuration.only_show_timer_zero ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, ONLY_SHOW_TIMER_ZERO_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ONLY_SHOW_TIMER_ZERO_JSON_KEY, DEFAULT_ONLY_SHOW_TIMER_ZERO ? "true" : "false");
					}

					// Try loading the red_timer_zero value.
					if (json_object.contains(RED_TIMER_ZERO_JSON_KEY))
					{
						configuration.red_timer_zero = json_object[RED_TIMER_ZERO_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, RED_TIMER_ZERO_JSON_KEY, configuration.red_timer_zero ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RED_TIMER_ZERO_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, RED_TIMER_ZERO_JSON_KEY, DEFAULT_RED_TIMER_ZERO ? "true" : "false");
					}

					// Try loading the activation_button value.
					if (json_object.contains(ACTIVATION_BUTTON_JSON_KEY))
					{
						configuration.activation_button = json_object[ACTIVATION_BUTTON_JSON_KEY];
						auto allowed_button = std::find(std::begin(ALLOWED_ACTIVATION_BUTTONS), std::end(ALLOWED_ACTIVATION_BUTTONS), configuration.activation_button);
						if (allowed_button == std::end(ALLOWED_ACTIVATION_BUTTONS))
						{
							g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value (%s) in mod configuration file: %s", MOD_NAME, VERSION, ACTIVATION_BUTTON_JSON_KEY, configuration.activation_button, config_file.c_str());
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the value is one of the supported keys!", MOD_NAME, VERSION);
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_JSON_KEY, DEFAULT_ACTIVATION_BUTTON.c_str());
							configuration.activation_button = DEFAULT_ACTIVATION_BUTTON;
						}
						else
						{
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_JSON_KEY, configuration.activation_button.c_str());
						}
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ACTIVATION_BUTTON_JSON_KEY, DEFAULT_ACTIVATION_BUTTON.c_str());
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

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"CropTimers.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

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

RValue& GmlScriptOnDrawGuiCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (GameWindowHasFocus() && !processing_user_input)
	{
		bool activate = false;
		if (configuration.activation_button_is_controller_key && CheckControllerInput())
			activate = true;
		if (!configuration.activation_button_is_controller_key && CheckVirtualKeyInput())
			activate = true;

		if (activate)
		{
			processing_user_input = true;
			configuration.timers_visible = !configuration.timers_visible;

			CRoom* current_room = nullptr;
			if (AurieSuccess(g_ModuleInterface->GetCurrentRoomData(current_room)))
			{
				for (CInstance* inst = current_room->GetMembers().m_ActiveInstances.m_First; inst != nullptr; inst = inst->GetMembers().m_Flink)
				{
					RValue sprite_index = g_ModuleInterface->CallBuiltin("variable_instance_get", { inst, "sprite_index" });
					RValue sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { sprite_index }); // spr_crop_wheat_stage3
					std::string sprite_name_str = sprite_name.ToString();

					if (sprite_name_str.contains("stage") && (sprite_name_str.contains("crop") || sprite_name_str.contains("forage")))
					{
						if (configuration.timers_visible)
						{
							RValue node = inst->GetMember("node");
							if (StructVariableExists(node, "prototype"))
							{
								RValue prototype = node.GetMember("prototype");
								if (StructVariableExists(prototype, "category_id") && IsNumeric(prototype.GetMember("category_id")))
								{
									int category_id = prototype.GetMember("category_id").ToInt64();
									if (category_id == 3) // don't hardcode this
									{
										int day_count = node.GetMember("day_count").ToInt64();
										bool stage_zero_is_seed = prototype.GetMember("stage_zero_is_seed").ToBoolean();
										bool regrow_cycle = node.GetMember("regrow_cycle").ToBoolean();

										RValue day_to_stage = prototype.GetMember("day_to_stage");
										RValue buffer = day_to_stage.GetMember("__buffer");

										int growth_days = g_ModuleInterface->CallBuiltin("array_length", { buffer }).ToInt64() - 1;
										growth_days = stage_zero_is_seed ? growth_days : growth_days - 1; // Adjust if stage 0 isn't a seed

										if (!regrow_cycle)
										{
											int days_remaining = growth_days - day_count;
											if (configuration.only_show_timer_zero && days_remaining != 0)
												continue;

											std::string replacement_sprite_name = sprite_name_str + "_timer_" + std::to_string(days_remaining);
											if (days_remaining == 0 && configuration.red_timer_zero)
												replacement_sprite_name = replacement_sprite_name + "_red";

											RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
											if (replacement_sprite.m_Kind == VALUE_REF)
												g_ModuleInterface->CallBuiltin("variable_instance_set", { inst, "sprite_index", replacement_sprite });
										}
										else
										{
											RValue post_harvest_day_to_stage = prototype.GetMember("post_harvest_day_to_stage");
											RValue post_harvest_day_to_stage_buffer = post_harvest_day_to_stage.GetMember("__buffer");

											int regrowth_days = g_ModuleInterface->CallBuiltin("array_length", { post_harvest_day_to_stage_buffer }).ToInt64() - 1;
											int days_remaining = regrowth_days - day_count;
											if (configuration.only_show_timer_zero && days_remaining != 0)
												continue;

											std::string replacement_sprite_name = sprite_name_str + "_timer_" + std::to_string(days_remaining);
											if (days_remaining == 0 && configuration.red_timer_zero)
												replacement_sprite_name = replacement_sprite_name + "_red";

											RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
											if (replacement_sprite.m_Kind == VALUE_REF)
												g_ModuleInterface->CallBuiltin("variable_instance_set", { inst, "sprite_index", replacement_sprite });
										}
									}
								}
							}
						}
						else
						{
							std::string _timer_ = "_timer_";
							if (sprite_name_str.contains(_timer_))
							{
								size_t pos = sprite_name_str.find(_timer_);
								if (pos != std::string::npos)
									sprite_name_str.erase(pos);

								RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { sprite_name_str.c_str() });
								if (replacement_sprite.m_Kind == VALUE_REF)
									g_ModuleInterface->CallBuiltin("variable_instance_set", { inst, "sprite_index", replacement_sprite });
							}
						}
					}
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

RValue& GmlScriptNodeObjectSetSpriteCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (configuration.timers_visible && ArgumentCount > 0 && Arguments[0]->m_Kind == VALUE_REF)
	{
		RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { *Arguments[0] });
		if (type.ToInt64() == 1) // asset_sprite
		{
			RValue sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { *Arguments[0] });
			std::string sprite_name_str = sprite_name.ToString();

			if (sprite_name_str.contains("stage") && (sprite_name_str.contains("crop") || sprite_name_str.contains("forage")))
			{
				RValue node = Self->GetMember("node");
				if (StructVariableExists(node, "prototype"))
				{
					RValue prototype = node.GetMember("prototype");
					if (StructVariableExists(prototype, "category_id") && IsNumeric(prototype.GetMember("category_id")))
					{
						int category_id = prototype.GetMember("category_id").ToInt64();
						if (category_id == 3) // don't hardcode this
						{
							int day_count = node.GetMember("day_count").ToInt64();
							bool stage_zero_is_seed = prototype.GetMember("stage_zero_is_seed").ToBoolean();
							bool regrow_cycle = node.GetMember("regrow_cycle").ToBoolean();

							RValue day_to_stage = prototype.GetMember("day_to_stage");
							RValue buffer = day_to_stage.GetMember("__buffer");

							int growth_days = g_ModuleInterface->CallBuiltin("array_length", { buffer }).ToInt64() - 1;
							growth_days = stage_zero_is_seed ? growth_days : growth_days - 1; // Adjust if stage 0 isn't a seed

							if (!regrow_cycle)
							{
								bool ignore_node = false;
								int days_remaining = growth_days - day_count;
								if (configuration.only_show_timer_zero && days_remaining != 0)
									ignore_node = true;

								if (!ignore_node)
								{
									std::string replacement_sprite_name = sprite_name_str + "_timer_" + std::to_string(days_remaining);
									if (days_remaining == 0 && configuration.red_timer_zero)
										replacement_sprite_name = replacement_sprite_name + "_red";

									RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
									if (replacement_sprite.m_Kind == VALUE_REF)
										*Arguments[0] = replacement_sprite;
								}
							}
							else
							{
								RValue post_harvest_day_to_stage = prototype.GetMember("post_harvest_day_to_stage");
								RValue post_harvest_day_to_stage_buffer = post_harvest_day_to_stage.GetMember("__buffer");

								bool ignore_node = false;
								int regrowth_days = g_ModuleInterface->CallBuiltin("array_length", { post_harvest_day_to_stage_buffer }).ToInt64() - 1;
								int days_remaining = regrowth_days - day_count;
								if (configuration.only_show_timer_zero && days_remaining != 0)
									ignore_node = true;

								if (!ignore_node)
								{
									std::string replacement_sprite_name = sprite_name_str + "_timer_" + std::to_string(days_remaining);
									if (days_remaining == 0 && configuration.red_timer_zero)
										replacement_sprite_name = replacement_sprite_name + "_red";

									RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
									if (replacement_sprite.m_Kind == VALUE_REF)
										*Arguments[0] = replacement_sprite;
								}
							}
						}
					}
				}
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_NODE_OBJECT_SET_SPRITE));
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
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		LoadObjectCategories();
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

void CreateHookGmlScriptNodeObjectSetSprite(AurieStatus& status)
{
	CScript* gml_script_node_object_set_sprite = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_NODE_OBJECT_SET_SPRITE,
		(PVOID*)&gml_script_node_object_set_sprite
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NODE_OBJECT_SET_SPRITE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_NODE_OBJECT_SET_SPRITE,
		gml_script_node_object_set_sprite->m_Functions->m_ScriptFunction,
		GmlScriptNodeObjectSetSpriteCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NODE_OBJECT_SET_SPRITE);
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

	CreateHookGmlScriptOnDrawGui(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptNodeObjectSetSprite(status);
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