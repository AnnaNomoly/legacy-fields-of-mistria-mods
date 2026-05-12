#include <filesystem>
#include <random>
#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "ChutesAndLadders";
static const char* const VERSION = "1.4.0";

// Config keys
static const char* const ACTIVATION_BUTTON_KEY = "activation_button";
static const char* const RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY = "ritual_chamber_additional_spawn_chance";
static const char* const SPAWN_LADDER_AT_PLAYER_POSITION_KEY = "spawn_ladder_at_player_position";

// Notification localization keys
static const std::string LADDER_SPAWNED_LOCALIZATION_KEY = "mods/ChutesAndLadders/ladder_spawned";
static const std::string LADDER_NOT_SPAWNED_LOCALIZATION_KEY = "mods/ChutesAndLadders/ladder_not_spawned";
static const std::string INVALID_LOCATION_LOCALIZATION_KEY = "mods/ChutesAndLadders/invalid_location";

// Defaults
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
	{ "rm_mines_tide_fork", { 544, 640 }},
	{ "rm_mines_tide_whirly", { 880, 480 }},
	{ "rm_mines_deep_runner", { 144, 144 }},
	{ "rm_mines_tide_islands", { 144, 528 }},
	{ "rm_mines_lava_bullseye", { 256, 368 }},
	{ "rm_mines_tide_paths", { 416, 576 }},
	{ "rm_mines_upper_slimetime", { 320, 272 }},
	{ "rm_mines_lava_generic", { 528, 672 }},
	{ "rm_mines_deep_winding", { 544, 336 }},
	{ "rm_mines_lava_world", { 288, 288 }},
	{ "rm_mines_deep_leap", { 528, 144 }},
	{ "rm_mines_ruins_secretfire", { 144, 624 }},
	{ "rm_mines_lava_treasure", { 160, 256 }},
	{ "rm_mines_tide_milestone", { 160, 272 }},
	{ "rm_mines_lava_elements", { 288, 560 }},
	{ "rm_mines_tide_oasis", { 160, 144 }},
	{ "rm_mines_tide_chamber", { 224, 480 }},
	{ "rm_mines_lava_arena", { 320, 128 }},
	{ "rm_mines_upper_treasure", { 144, 320 }},
	{ "rm_mines_upper_sock", { 256, 176 }},
	{ "rm_mines_upper_boomerang", { 368, 304 }},
	{ "rm_mines_upper_worm", { 176, 144 }},
	{ "rm_mines_upper_ponds", { 320, 336 }},
	{ "rm_mines_upper_pond", { 240, 416 }},
	{ "rm_mines_upper_snake", { 432, 128 }},
	{ "rm_mines_upper_four", { 176, 128 }},
	{ "rm_mines_upper_amoeba", { 176, 384 }},
	{ "rm_mines_upper_wishbone", { 336, 592 }},
	{ "rm_mines_upper_canada", { 192, 352 }},
	{ "rm_mines_upper_staple", { 480, 384 }},
	{ "rm_mines_upper_pillars", { 272, 128 }},
	{ "rm_mines_upper_elevator5", { 96, 224 }},
	{ "rm_mines_upper_elevator10", { 384, 336 }},
	{ "rm_mines_upper_elevator15", { 144, 64 }},
	{ "rm_mines_upper_floor1", { 96, 288 }},
	{ "rm_mines_upper_path", { 320, 352 }},
	{ "rm_mines_upper_stream", { 400, 128 }},
	{ "rm_mines_upper_muscle", { 224, 480 }},
	{ "rm_mines_upper_crossroad", { 320, 336 }},
	{ "rm_mines_upper_pits", { 240, 176 }},
	{ "rm_mines_basement_no_elevator", { 368, 304 }},
	{ "rm_mines_basement_elevator", { 96, 224 }},
	{ "rm_mines_tide_elevator25", { 432, 448 }},
	{ "rm_mines_tide_elevator30", { 352, 416 }},
	{ "rm_mines_tide_elevator35", { 448, 320 }},
	{ "rm_mines_tide_floor21", { 400, 160 }},
	{ "rm_mines_tide_basic1", { 528, 688 }},
	{ "rm_mines_tide_shrine1", { 208, 576 }},
	{ "rm_mines_tide_whirlpool1", { 864, 672 }},
	{ "rm_mines_tide_basic2", { 304, 160 }},
	{ "rm_mines_tide_switch2", { 144, 112 }},
	{ "rm_mines_tide_whirlpool2", { 144, 640 }},
	{ "rm_mines_tide_switch1", { 272, 304 }},
	{ "rm_mines_tide_shrine3", { 672, 624 }},
	{ "rm_mines_tide_shrine2", { 288, 464 }},
	{ "rm_mines_basement_treasure", { 144, 320 }},
	{ "rm_mines_tide_whirlpool3", { 160, 672 }},
	{ "rm_mines_tide_basic3", { 272, 288 }},
	{ "rm_mines_tide_basic4", { 448, 480 }},
	{ "rm_mines_tide_whirlpool4", { 128, 624 }},
	{ "rm_mines_tide_switch3", { 160, 128 }},
	{ "rm_mines_tide_switch4", { 464, 400 }},
	{ "rm_mines_deep_chambers", { 192, 560 }},
	{ "rm_mines_deep_spiral", { 240, 784 }},
	{ "rm_mines_deep_wishbone", { 656, 640 }},
	{ "rm_mines_deep_key", { 320, 160 }},
	{ "rm_mines_deep_scorpio", { 224, 192 }},
	{ "rm_mines_deep_butterfly", { 400, 288 }},
	{ "rm_mines_upper_formerelevator", { 320, 352 }},
	{ "rm_mines_deep_41", { 304, 256 }},
	{ "rm_mines_deep_45", { 384, 496 }},
	{ "rm_mines_deep_50", { 608, 480 }},
	{ "rm_mines_deep_55", { 544, 512 }},
	{ "rm_mines_deep_switch1", { 144, 144 }},
	{ "rm_mines_deep_switch2", { 112, 448 }},
	{ "rm_mines_deep_treasure1", { 144, 240 }},
	{ "rm_mines_deep_switch3", { 512, 320 }},
	{ "rm_mines_deep_switch4", { 688, 624 }},
	{ "rm_mines_deep_whirlpool1", { 528, 144 }},
	{ "rm_mines_deep_whirlpool2", { 208, 192 }},
	{ "rm_mines_deep_shrine2", { 336, 624 }},
	{ "rm_mines_deep_shrine1", { 816, 432 }},
	{ "rm_mines_deep_shrine3", { 480, 384 }},
	{ "rm_mines_deep_shrine4", { 448, 544 }},
	{ "rm_mines_tide_ritual_chamber", { 192, 128 }},
	{ "rm_mines_deep_ritual_chamber", { 192, 128 }},
	{ "rm_mines_lava_ritual_chamber", { 192, 128 }},
	{ "rm_mines_ruins_ritual_chamber", { 192, 128 }},
	{ "rm_mines_lava_61", { 288, 272 }},
	{ "rm_mines_lava_65", { 512, 144 }},
	{ "rm_mines_lava_70", { 400, 176 }},
	{ "rm_mines_lava_75", { 688, 176 }},
	{ "rm_mines_lava_switch1", { 784, 256 }},
	{ "rm_mines_lava_switch2", { 496, 240 }},
	{ "rm_mines_lava_switch3", { 448, 192 }},
	{ "rm_mines_lava_switch4", { 464, 240 }},
	{ "rm_mines_lava_switch5", { 448, 384 }},
	{ "rm_mines_lava_basic1", { 832, 288 }},
	{ "rm_mines_lava_basic2", { 336, 448 }},
	{ "rm_mines_lava_basic3", { 320, 208 }},
	{ "rm_mines_lava_basic4", { 544, 432 }},
	{ "rm_mines_lava_basic5", { 528, 672 }},
	{ "rm_mines_lava_shrine1", { 224, 480 }},
	{ "rm_mines_lava_shrine2", { 752, 624 }},
	{ "rm_mines_lava_shrine3", { 368, 528 }},
	{ "rm_mines_lava_shrine4", { 752, 608 }},
	{ "rm_mines_ruins_basic1", { 480, 544 }},
	{ "rm_mines_ruins_85", { 528, 112 }},
	{ "rm_mines_ruins_large_switch", { 320, 144 }},
	{ "rm_mines_ruins_81", { 608, 304 }},
	{ "rm_mines_ruins_95", { 592, 272 }},
	{ "rm_mines_ruins_arena1", { 576, 352 }},
	{ "rm_mines_ruins_basic2", { 528, 400 }},
	{ "rm_mines_ruins_basic3", { 688, 432 }},
	{ "rm_mines_ruins_basic4", { 704, 192 }},
	{ "rm_mines_ruins_basic5", { 288, 400 }},
	{ "rm_mines_ruins_switch2", { 448, 384 }},
	{ "rm_mines_ruins_switch3", { 496, 448 }},
	{ "rm_mines_ruins_switch4", { 496, 368 }},
	{ "rm_mines_ruins_switch5", { 624, 560 }},
	{ "rm_mines_ruins_arena2", { 576, 496 }},
	{ "rm_mines_ruins_whirlpool1", { 512, 256 }},
	{ "rm_mines_ruins_whirlpool2", { 480, 368 }},
	{ "rm_mines_ruins_whirlpool3", { 384, 256 }},
	{ "rm_mines_ruins_whirlpool4", { 688, 272 }},
	{ "rm_mines_ruins_shrine1", { 832, 512 }},
	{ "rm_mines_ruins_whirlpool5", { 592, 176 }},
	{ "rm_mines_ruins_shrine2", { 784, 464 }},
	{ "rm_mines_ruins_shrine3", { 464, 464 }},
	{ "rm_mines_ruins_shrine4", { 512, 400 }},
	{ "rm_mines_ruins_shrine5", { 368, 352 }},
	{ "rm_mines_ruins_arena3", { 1120, 864 }},
};

struct ChutesAndLaddersConfig
{
	std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
	int ritual_chamber_additional_spawn_chance = DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE;
	bool spawn_ladder_at_player_position = DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION;
};

void to_json(json& json_object, const ChutesAndLaddersConfig& config)
{
	json_object = json{
		{ ACTIVATION_BUTTON_KEY, config.activation_button },
		{ RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, config.ritual_chamber_additional_spawn_chance },
		{ SPAWN_LADDER_AT_PLAYER_POSITION_KEY, config.spawn_ladder_at_player_position }
	};
}

void from_json(const json& json_object, ChutesAndLaddersConfig& config)
{
	config.activation_button = MMAPI::Config::GetValue<std::string>(json_object, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON);
	auto allowed_button = std::find(std::begin(ALLOWED_ACTIVATION_BUTTONS), std::end(ALLOWED_ACTIVATION_BUTTONS), config.activation_button);
	if (allowed_button == std::end(ALLOWED_ACTIVATION_BUTTONS))
	{
		MMAPI::Log::Error("Invalid \"%s\" value (%s) — not one of the supported keys", ACTIVATION_BUTTON_KEY, config.activation_button.c_str());
		config.activation_button = DEFAULT_ACTIVATION_BUTTON;
	}

	config.ritual_chamber_additional_spawn_chance = MMAPI::Config::GetValue<int>(
		json_object, RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, DEFAULT_RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE, 0, 100);
	config.spawn_ladder_at_player_position = MMAPI::Config::GetValue<bool>(
		json_object, SPAWN_LADDER_AT_PLAYER_POSITION_KEY, DEFAULT_SPAWN_LADDER_AT_PLAYER_POSITION);
}

static ChutesAndLaddersConfig config = {};
static bool activation_button_is_controller_key = false;
static int activation_button_int_value = -1;
static bool processing_user_input = false;
static std::string ari_current_gm_room = "";
static int ari_x = -1;
static int ari_y = -1;
static bool teleport_ari = false;
static bool ari_is_teleporting = false;
static bool create_ritual_altar = false;
static bool lost_to_history_active = false;
static std::mt19937 generator(std::random_device{}());

int ActivationButtonToVirtualKey()
{
	// Function Keys
	if (config.activation_button == "F1")  return VK_F1;
	if (config.activation_button == "F2")  return VK_F2;
	if (config.activation_button == "F3")  return VK_F3;
	if (config.activation_button == "F4")  return VK_F4;
	if (config.activation_button == "F5")  return VK_F5;
	if (config.activation_button == "F6")  return VK_F6;
	if (config.activation_button == "F7")  return VK_F7;
	if (config.activation_button == "F8")  return VK_F8;
	if (config.activation_button == "F9")  return VK_F9;
	if (config.activation_button == "F10") return VK_F10;
	if (config.activation_button == "F11") return VK_F11;
	if (config.activation_button == "F12") return VK_F12;

	// Numpad
	if (config.activation_button == "NUMPAD_0") return VK_NUMPAD0;
	if (config.activation_button == "NUMPAD_1") return VK_NUMPAD1;
	if (config.activation_button == "NUMPAD_2") return VK_NUMPAD2;
	if (config.activation_button == "NUMPAD_3") return VK_NUMPAD3;
	if (config.activation_button == "NUMPAD_4") return VK_NUMPAD4;
	if (config.activation_button == "NUMPAD_5") return VK_NUMPAD5;
	if (config.activation_button == "NUMPAD_6") return VK_NUMPAD6;
	if (config.activation_button == "NUMPAD_7") return VK_NUMPAD7;
	if (config.activation_button == "NUMPAD_8") return VK_NUMPAD8;
	if (config.activation_button == "NUMPAD_9") return VK_NUMPAD9;

	// Numbers / Letters — single-character buttons map to their ASCII codepoint (matches GameMaker's vk_* convention)
	if (config.activation_button.size() == 1)
	{
		char c = config.activation_button[0];
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z'))
			return static_cast<int>(c);
	}

	// Special
	if (config.activation_button == "INSERT")      return VK_INSERT;
	if (config.activation_button == "DELETE")      return VK_DELETE;
	if (config.activation_button == "HOME")        return VK_HOME;
	if (config.activation_button == "PAGE_UP")     return VK_PRIOR;
	if (config.activation_button == "PAGE_DOWN")   return VK_NEXT;
	if (config.activation_button == "NUM_LOCK")    return VK_NUMLOCK;
	if (config.activation_button == "SCROLL_LOCK") return VK_SCROLL;
	if (config.activation_button == "CAPS_LOCK")   return VK_CAPITAL;
	if (config.activation_button == "PAUSE_BREAK") return VK_PAUSE;

	return -1;
}

int ActivationButtonToControllerKey()
{
	if (config.activation_button == "GAMEPAD_A")              return 0x8001;
	if (config.activation_button == "GAMEPAD_B")              return 0x8002;
	if (config.activation_button == "GAMEPAD_X")              return 0x8003;
	if (config.activation_button == "GAMEPAD_Y")              return 0x8004;
	if (config.activation_button == "GAMEPAD_LEFT_SHOULDER")  return 0x8005;
	if (config.activation_button == "GAMEPAD_RIGHT_SHOULDER") return 0x8006;
	if (config.activation_button == "GAMEPAD_LEFT_TRIGGER")   return 0x8007;
	if (config.activation_button == "GAMEPAD_RIGHT_TRIGGER")  return 0x8008;
	if (config.activation_button == "GAMEPAD_SELECT")         return 0x8009;
	if (config.activation_button == "GAMEPAD_START")          return 0x800A;
	if (config.activation_button == "GAMEPAD_LEFT_STICK")     return 0x800B;
	if (config.activation_button == "GAMEPAD_RIGHT_STICK")    return 0x800C;
	if (config.activation_button == "GAMEPAD_DPAD_UP")        return 0x800D;
	if (config.activation_button == "GAMEPAD_DPAD_DOWN")      return 0x800E;
	if (config.activation_button == "GAMEPAD_DPAD_LEFT")      return 0x800F;
	if (config.activation_button == "GAMEPAD_DPAD_RIGHT")     return 0x8010;
	return -1;
}

void ConfigureActivationButton()
{
	if (config.activation_button.find("GAMEPAD") != std::string::npos)
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
	config = ChutesAndLaddersConfig{};
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", ACTIVATION_BUTTON_KEY, config.activation_button.c_str());
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %d!", RITUAL_CHAMBER_ADDITIONAL_SPAWN_CHANCE_KEY, config.ritual_chamber_additional_spawn_chance);
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", SPAWN_LADDER_AT_PLAYER_POSITION_KEY, config.spawn_ladder_at_player_position ? "true" : "false");
}

void LoadOrCreateConfigFile()
{
	try
	{
		std::filesystem::path config_file = MMAPI::Config::GetConfigPath(MOD_NAME);
		bool config_file_exists = std::filesystem::exists(config_file);
		json json_object = MMAPI::Config::Load(config_file);

		if (!config_file_exists)
			MMAPI::Log::Warn("Configuration file was not found. Creating file: %s", config_file.string().c_str());

		if (json_object.empty())
		{
			if (config_file_exists)
			{
				MMAPI::Log::Error("No readable values found in mod configuration file: %s!", config_file.string().c_str());
				MMAPI::Log::Warn("Defaults will be used and written back to the configuration file.");
			}
			LogDefaultConfigValues();
			MMAPI::Config::Save(config_file, config);
			return;
		}

		config = json_object.get<ChutesAndLaddersConfig>();
		MMAPI::Config::Save(config_file, config);
		MMAPI::Log::Info("Loaded configuration file: %s", config_file.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		LogDefaultConfigValues();
	}
}

void SpawnLadder()
{
	if (LADDER_SPAWN_POINTS.count(ari_current_gm_room) == 0)
	{
		MMAPI::Game::CreateNotification(false, LADDER_NOT_SPAWNED_LOCALIZATION_KEY);
		MMAPI::Log::Warn("The current GM room (%s) has no set ladder coordinates!", ari_current_gm_room.c_str());
		return;
	}

	// Both ari_x/ari_y and LADDER_SPAWN_POINTS values are in room pixels (matching UMT's room editor).
	// MMAPI::Dungeon::SpawnLadder handles the pixel→grid conversion internally.
	int x = config.spawn_ladder_at_player_position ? ari_x : LADDER_SPAWN_POINTS.at(ari_current_gm_room)[0];
	int y = config.spawn_ladder_at_player_position ? ari_y : LADDER_SPAWN_POINTS.at(ari_current_gm_room)[1];

	if (MMAPI::Dungeon::SpawnLadder(x, y))
		MMAPI::Game::CreateNotification(false, LADDER_SPAWNED_LOCALIZATION_KEY);
}

void HandleAri(CInstance* self)
{
	// Track player position from obj_ari's built-in x/y.
	ari_x = static_cast<int>(MMAPI::Engine::InstanceVariableGet(self, "x").ToDouble());
	ari_y = static_cast<int>(MMAPI::Engine::InstanceVariableGet(self, "y").ToDouble());

	// Track Lost-to-History perk status.
	lost_to_history_active = MMAPI::Perk::IsActive(MMAPI::Perk::Ids::LostToHistory);
}

void OnAfterRoomStart(MMAPI::Weather::AfterRoomStartContext& ctx)
{
	if (teleport_ari)
	{
		teleport_ari = false;
		ari_is_teleporting = true;
		MMAPI::Location::TeleportAri(MMAPI::Location::Ids::MinesEntry, 192, 352);
	}

	if (create_ritual_altar && MMAPI::Engine::LayerExists("Impl_Ritual"))
	{
		MMAPI::Engine::InstanceCreateLayer(192.0, 224.0, "Impl_Ritual", "obj_dungeon_ritual_altar");
		create_ritual_altar = false;
	}
}

void OnSetupMainScreen()
{
	ari_current_gm_room = "";
	teleport_ari = false;
	ari_is_teleporting = false;
	create_ritual_altar = false;
	ari_x = -1;
	ari_y = -1;
	lost_to_history_active = false;
}

void OnAfterDrawGui()
{
	if (!MMAPI::Game::WindowHasFocus() || MMAPI::Game::IsPaused() || processing_user_input)
		return;

	bool activate = false;
	if (activation_button_is_controller_key)
	{
		int slot = MMAPI::Input::GetFirstConnectedGamepadSlot();
		if (slot != -1 && MMAPI::Input::GamepadButtonCheckPressed(slot, activation_button_int_value))
			activate = true;
	}
	else if (MMAPI::Input::KeyboardCheckPressed(activation_button_int_value))
	{
		processing_user_input = true;
		activate = true;
	}

	if (!activate)
	{
		processing_user_input = false;
		return;
	}

	if (ari_current_gm_room.contains("rm_mines") && ari_current_gm_room != "rm_mines_entry" && !ari_current_gm_room.contains("seal"))
	{
		SpawnLadder();
	}
	else
	{
		MMAPI::Log::Warn("Ladders may only be spawned in the dungeon!");
		MMAPI::Game::CreateNotification(false, INVALID_LOCATION_LOCALIZATION_KEY);
	}

	processing_user_input = false;
}

void OnBeforeError(MMAPI::Game::BeforeErrorContext& ctx)
{
	if (ctx.GetMessage() == "Failed to spawn a ladder in the dungeon!")
		ctx.Cancel();
}

void OnBeforeGoToRoom(MMAPI::Location::BeforeGoToRoomContext& ctx)
{
	// Redirect to the biome-appropriate ritual chamber when a teleport is queued.
	if (ari_is_teleporting)
	{
		if (ari_current_gm_room.contains("rm_mines_tide"))
			ctx.SetTargetRoom("rm_mines_tide_ritual_chamber");
		else if (ari_current_gm_room.contains("rm_mines_deep"))
			ctx.SetTargetRoom("rm_mines_deep_ritual_chamber");
		else if (ari_current_gm_room.contains("rm_mines_lava"))
			ctx.SetTargetRoom("rm_mines_lava_ritual_chamber");
		else if (ari_current_gm_room.contains("rm_mines_ruins"))
			ctx.SetTargetRoom("rm_mines_ruins_ritual_chamber");
	}
}

void OnAfterGoToRoom(MMAPI::Location::AfterGoToRoomContext& ctx)
{
	std::string previous_room = ari_current_gm_room;
	ari_current_gm_room = ctx.GetRoomName();

	if (ari_is_teleporting)
	{
		ari_is_teleporting = false;
		create_ritual_altar = true;
	}
	else if (config.ritual_chamber_additional_spawn_chance > 0 && lost_to_history_active)
	{
		if (!create_ritual_altar &&
			ari_current_gm_room.contains("rm_mines") &&
			ari_current_gm_room != "rm_mines_entry" &&
			!ari_current_gm_room.contains("seal") &&
			!ari_current_gm_room.contains("ritual") &&
			!ari_current_gm_room.contains("treasure") &&
			!ari_current_gm_room.contains("milestone"))
		{
			if (ari_current_gm_room.find("rm_mines_tide") != std::string::npos ||
				ari_current_gm_room.find("rm_mines_deep") != std::string::npos ||
				ari_current_gm_room.find("rm_mines_lava") != std::string::npos ||
				ari_current_gm_room.find("rm_mines_ruins") != std::string::npos)
			{
				std::uniform_int_distribution<int> distribution(1, 100);
				if (distribution(generator) <= config.ritual_chamber_additional_spawn_chance)
					teleport_ari = true;
			}
		}
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(module_interface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Dungeon::Enable();
	MMAPI::Location::Enable();
	MMAPI::Perk::Enable();
	MMAPI::Input::Enable();
	MMAPI::Game::Enable();

	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, HandleAri);
	MMAPI::Weather::Hooks::AfterRoomStart(OnAfterRoomStart);
	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Game::Hooks::BeforeError(OnBeforeError);
	MMAPI::Location::Hooks::BeforeGoToRoom(OnBeforeGoToRoom);
	MMAPI::Location::Hooks::AfterGoToRoom(OnAfterGoToRoom);

	LoadOrCreateConfigFile();
	ConfigureActivationButton();

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
