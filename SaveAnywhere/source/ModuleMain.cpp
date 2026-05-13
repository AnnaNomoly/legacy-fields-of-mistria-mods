#include <cstdint>
#include <optional>
#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>
#include <MMAPI/ModSave.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "SaveAnywhere";
static const char* const VERSION  = "1.3.0";

// ----- Config -----

static const char* const CONFIG_KEY_ACTIVATION_BUTTON = "activation_button";
static const std::string DEFAULT_ACTIVATION_BUTTON    = "HOME";

static std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
static std::optional<MMAPI::Input::Keybind> keybind;

// ----- Mod save JSON keys -----

static const char* const SAVE_LOCATION_KEY   = "save_location";
static const char* const SAVE_X_POSITION_KEY = "save_x_position";
static const char* const SAVE_Y_POSITION_KEY = "save_y_position";

// ----- Localization keys -----

static const std::string SAVING_DISABLED_NOTIFICATION    = "Notifications/Mods/SaveAnywhere/saving_disabled";
static const std::string NEW_GAME_NOTIFICATION           = "Notifications/Mods/SaveAnywhere/new_game_warning";
static const std::string DUNGEON_SAVE_NOTIFICATION       = "Notifications/Mods/SaveAnywhere/location/dungeon";
static const std::string FARM_BUILDING_SAVE_NOTIFICATION = "Notifications/Mods/SaveAnywhere/location/farm_building";

// ----- Cross-mod -----

// Set by Deep Dungeon mod on its __YYTK sub-struct. When non-zero, the player is on a dungeon
// floor and saving must be disabled (the save state isn't compatible with mid-run interruption).
static const char* const DEEP_DUNGEON_MOD_NAME = "DeepDungeon";
static const char* const DEEP_DUNGEON_FLOOR_KEY = "floor";

// ----- GML scripts called directly -----

// Transitions Ari to the given location id using the location's default spawn coordinates.
// Distinct from Location::TeleportAri (which uses ari_teleport_to_room with custom x/y) — the
// original SaveAnywhere uses goto_location_id and overrides x/y on a later tick, which appears
// to be necessary because ari_teleport_to_room's x/y get clobbered by the destination room's
// own spawn init. Inlining the call here preserves the original's exact teleport semantics.
static const char* const GML_SCRIPT_GOTO_LOCATION_ID         = "gml_Script_goto_location_id";
// The game's "Game Saved!" UI banner.
static const char* const GML_SCRIPT_CREATE_SAVE_NOTIFICATION = "gml_Script_create_save_notification";
// The game's save_game entry point — called directly so we can pass a custom autosave path
// instead of waiting for the game to trigger save on its own schedule.
static const char* const GML_SCRIPT_SAVE_GAME                = "gml_Script_save_game";

// ----- State -----

static bool startup_loaded = false;
// True between AfterGameActive and the next BeforeSetupMainScreen — gates the hotkey check.
static bool game_is_active = false;
// Stable per-save identifier extracted from the game's save_path. Populated from AfterLoadGame
// (and topped up from BeforeSaveGame so a new game becomes save-able once the player saves at
// the bed for the first time). Empty if the player is on a brand-new file with no save yet.
static std::string save_prefix = "";
// Directory part of the save_path. Captured the same way and used to compose our custom
// autosave path so we save into the same directory as the game's normal saves.
static std::string save_folder = "";
// Latched true around our own SaveGameToDisk call so the BeforeSaveGame observer can tell
// "mod-initiated save" apart from "game-initiated save" and skip re-parsing the prefix from
// our own custom path. Defensive — in practice the mod path parses to the same prefix, but
// the original mod deliberately only trusts game-initiated saves to set the canonical value.
static bool mod_initiated_save = false;

// Teleport state machine: after AfterLoadGame reads the mod save file, we sequence
//   wait_to_teleport → AfterShowRoomTitle → ready_to_teleport → obj_ari tick → goto_location_id
//   → wait_to_reposition → AfterShowRoomTitle → ready_to_reposition → obj_ari tick → SetPosition
// The two-stage dance is needed because setting x/y before the destination room loads gets
// clobbered by the destination's own spawn-init.
static bool wait_to_teleport    = false;
static bool ready_to_teleport   = false;
static bool wait_to_reposition  = false;
static bool ready_to_reposition = false;
static std::optional<MMAPI::Location::Ids> saved_location;
static double saved_x = 0.0;
static double saved_y = 0.0;

// Locations that map to "you're in a dungeon — save under the fallback location instead".
static bool IsDungeonLikeLocation(MMAPI::Location::Ids loc)
{
	return loc == MMAPI::Location::Ids::Dungeon
	    || loc == MMAPI::Location::Ids::WaterSeal
	    || loc == MMAPI::Location::Ids::EarthSeal
	    || loc == MMAPI::Location::Ids::FireSeal
	    || loc == MMAPI::Location::Ids::RuinsSeal
	    || loc == MMAPI::Location::Ids::VoidSeal
	    || loc == MMAPI::Location::Ids::PriestessQuarters
	    || loc == MMAPI::Location::Ids::SeridiasChamber;
}

// Locations that map to "you're inside a farm building — save under the farm fallback instead".
static bool IsFarmBuilding(MMAPI::Location::Ids loc)
{
	return loc == MMAPI::Location::Ids::SmallBarn
	    || loc == MMAPI::Location::Ids::MediumBarn
	    || loc == MMAPI::Location::Ids::LargeBarn
	    || loc == MMAPI::Location::Ids::SmallCoop
	    || loc == MMAPI::Location::Ids::MediumCoop
	    || loc == MMAPI::Location::Ids::LargeCoop
	    || loc == MMAPI::Location::Ids::SmallGreenhouse
	    || loc == MMAPI::Location::Ids::LargeGreenhouse;
}

// ----- Config -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	activation_button = MMAPI::Config::GetValue<std::string>(config, CONFIG_KEY_ACTIVATION_BUTTON, DEFAULT_ACTIVATION_BUTTON);

	keybind = MMAPI::Input::TryParseKeybind(activation_button);
	if (!keybind)
	{
		MMAPI::Log::Warn("Invalid %s value (%s) — falling back to default %s",
			CONFIG_KEY_ACTIVATION_BUTTON, activation_button.c_str(), DEFAULT_ACTIVATION_BUTTON.c_str());
		activation_button = DEFAULT_ACTIVATION_BUTTON;
		keybind = MMAPI::Input::TryParseKeybind(activation_button);
	}

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_ACTIVATION_BUTTON] = activation_button;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config: %s=%s", CONFIG_KEY_ACTIVATION_BUTTON, activation_button.c_str());
}

// ----- Save / load -----

// Returns true if Deep Dungeon is in a non-zero floor — i.e., saving should be disabled.
static bool DeepDungeonInProgress()
{
	YYTK::RValue dd = MMAPI::CrossMod::TryGetModStruct(DEEP_DUNGEON_MOD_NAME);
	if (dd.m_Kind != YYTK::VALUE_OBJECT) return false;
	if (!MMAPI::Engine::StructVariableExists(dd, DEEP_DUNGEON_FLOOR_KEY)) return false;

	YYTK::RValue floor = dd.GetMember(DEEP_DUNGEON_FLOOR_KEY);
	if (!MMAPI::Engine::IsNumeric(floor)) return false;

	return floor.ToInt64() != 0;
}

// Writes the mod's per-save file recording the location + (x, y) Ari should restore to on load.
// Dungeon and farm-building locations get rewritten to fallback positions (mines_entry / farm)
// to avoid loading into a procedural or building-interior room where Ari can't safely spawn.
static void WriteModSaveFile()
{
	if (save_prefix.empty())
	{
		MMAPI::Log::Error("Cannot write mod save file: no save_prefix yet.");
		return;
	}

	MMAPI::Location::Ids loc;
	if (!MMAPI::Location::TryGetCurrentLocation(loc))
	{
		MMAPI::Log::Error("Cannot write mod save file: current location unresolved.");
		return;
	}

	nlohmann::json data;

	if (IsDungeonLikeLocation(loc))
	{
		data[SAVE_LOCATION_KEY]   = MMAPI::Location::LocationIdToString(MMAPI::Location::Ids::MinesEntry);
		data[SAVE_X_POSITION_KEY] = 216;
		data[SAVE_Y_POSITION_KEY] = 198;
		MMAPI::Log::Info("In a dungeon — saving location as fallback (mines_entry).");
		MMAPI::Game::CreateNotification(true, DUNGEON_SAVE_NOTIFICATION);
	}
	else if (IsFarmBuilding(loc))
	{
		data[SAVE_LOCATION_KEY]   = MMAPI::Location::LocationIdToString(MMAPI::Location::Ids::Farm);
		data[SAVE_X_POSITION_KEY] = 1032;
		data[SAVE_Y_POSITION_KEY] = 87;
		MMAPI::Log::Info("In a farm building — saving location as fallback (farm).");
		MMAPI::Game::CreateNotification(true, FARM_BUILDING_SAVE_NOTIFICATION);
	}
	else
	{
		auto position = MMAPI::Player::GetPosition();
		if (!position)
		{
			MMAPI::Log::Error("Cannot write mod save file: Ari's position unresolved.");
			return;
		}
		data[SAVE_LOCATION_KEY]   = MMAPI::Location::LocationIdToString(loc);
		data[SAVE_X_POSITION_KEY] = static_cast<int>(position->x);
		data[SAVE_Y_POSITION_KEY] = static_cast<int>(position->y);
		MMAPI::Log::Info("Saving at current location: %s",
			MMAPI::Location::LocationIdToString(loc).c_str());
	}

	MMAPI::ModSave::Write(save_prefix, data);
}

// Calls the game's save_game script with a custom autosave path, reusing the save folder we
// captured from the most recent natural save/load.
static void SaveGameToDisk()
{
	YYTK::CScript* gml_script = nullptr;
	MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_SAVE_GAME, reinterpret_cast<PVOID*>(&gml_script));
	if (!gml_script) return;

	YYTK::CInstance* Self  = nullptr;
	YYTK::CInstance* Other = nullptr;
	if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other)) return;

	std::string file_name = save_folder + "\\game-" + save_prefix + "-autosave.sav";
	YYTK::RValue arg = YYTK::RValue(file_name);
	YYTK::RValue result;
	YYTK::RValue* args[1] = { &arg };
	gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
}

static void DisplaySaveNotification()
{
	YYTK::CScript* gml_script = nullptr;
	MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_CREATE_SAVE_NOTIFICATION, reinterpret_cast<PVOID*>(&gml_script));
	if (!gml_script) return;

	YYTK::CInstance* Self  = nullptr;
	YYTK::CInstance* Other = nullptr;
	if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other)) return;

	YYTK::RValue result;
	gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
}

static void TriggerSave()
{
	if (save_prefix.empty())
	{
		MMAPI::Log::Warn("Game was NOT saved! An autosave doesn't exist yet — save at the bed at least once first.");
		MMAPI::Game::CreateNotification(true, NEW_GAME_NOTIFICATION);
		return;
	}

	if (DeepDungeonInProgress())
	{
		MMAPI::Log::Info("Saving disabled while in a Deep Dungeon run.");
		MMAPI::Game::CreateNotification(true, SAVING_DISABLED_NOTIFICATION);
		return;
	}

	WriteModSaveFile();
	mod_initiated_save = true;
	SaveGameToDisk();
	DisplaySaveNotification();
}

// Reads the mod's per-save file and primes the teleport state machine for the post-load tick.
static void ReadModSaveFile()
{
	if (save_prefix.empty()) return;

	nlohmann::json data = MMAPI::ModSave::Read(save_prefix);
	if (data.empty()) return;
	if (!data.contains(SAVE_LOCATION_KEY) || !data[SAVE_LOCATION_KEY].is_string()) return;

	std::string location_name = data[SAVE_LOCATION_KEY].get<std::string>();
	auto loc = MMAPI::Location::TryFromInternalName(location_name);
	if (!loc)
	{
		MMAPI::Log::Warn("Mod save file references unknown location: %s", location_name.c_str());
		return;
	}

	saved_location = *loc;
	saved_x = MMAPI::Config::GetValue<int>(data, SAVE_X_POSITION_KEY, 0);
	saved_y = MMAPI::Config::GetValue<int>(data, SAVE_Y_POSITION_KEY, 0);
	wait_to_teleport = true;

	MMAPI::Log::Info("Mod save loaded — will teleport to: %s", location_name.c_str());
}

// ----- Teleport -----

// Invokes gml_Script_goto_location_id with the saved location id. The destination's spawn-init
// will clobber Ari's x/y; reposition runs in a later tick after the room finishes loading.
static void TeleportToSavedLocation(YYTK::CInstance* Self, YYTK::CInstance* Other)
{
	if (!saved_location) return;

	YYTK::CScript* gml_script = nullptr;
	MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_GOTO_LOCATION_ID, reinterpret_cast<PVOID*>(&gml_script));
	if (!gml_script) return;

	YYTK::RValue location_id = static_cast<int>(*saved_location);
	YYTK::RValue result;
	YYTK::RValue* args[1] = { &location_id };
	gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
}

// ----- Hooks -----

void OnAriTick(YYTK::CInstance* self)
{
	if (wait_to_teleport && ready_to_teleport)
	{
		TeleportToSavedLocation(self, self);
		wait_to_teleport   = false;
		ready_to_teleport  = false;
		wait_to_reposition = true;
		return;
	}

	if (wait_to_reposition && ready_to_reposition)
	{
		MMAPI::Player::SetPosition(saved_x, saved_y);
		wait_to_reposition  = false;
		ready_to_reposition = false;
		saved_location      = std::nullopt;
	}
}

void OnAfterShowRoomTitle()
{
	if (wait_to_teleport)   ready_to_teleport  = true;
	if (wait_to_reposition) ready_to_reposition = true;
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	// Skip our own save fires — the mod's custom autosave path reuses the prefix we already
	// captured, so re-parsing it would be redundant at best and corruption-prone at worst if
	// the path were ever malformed. Only game-initiated saves are trusted to set the canonical
	// prefix.
	if (mod_initiated_save)
	{
		mod_initiated_save = false;
		return;
	}

	// Only capture once. Subsequent natural saves (e.g. the game's periodic autosave) reuse
	// the prefix we already locked in. This is also how a brand-new game becomes save-able —
	// the first save-at-the-bed populates save_prefix, after which the hotkey works.
	if (!save_prefix.empty()) return;

	std::string prefix = ctx.GetSavePrefix();
	if (prefix.empty()) return;

	save_prefix = prefix;
	std::string path(ctx.GetSavePath());
	std::size_t slash = path.find_last_of("/\\");
	if (slash != std::string::npos)
		save_folder = path.substr(0, slash);
}

void OnAfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	std::string path(ctx.GetSavePath());
	save_prefix = ctx.GetSavePrefix();
	std::size_t slash = path.find_last_of("/\\");
	if (slash != std::string::npos)
		save_folder = path.substr(0, slash);

	// Only restore mod-saved location for autosave loads — manual saves (the player choosing a
	// specific named save) shouldn't snap them to wherever they last hit the hotkey.
	if (path.find("autosave") != std::string::npos)
		ReadModSaveFile();
}

void OnAfterEndDay()
{
	// Player slept through to the next day — they'll naturally be at the bed on the next load,
	// so the per-save mod file is no longer needed.
	if (!save_prefix.empty())
		MMAPI::ModSave::Delete(save_prefix);

	// Reset the teleport state machine in case end_day fires mid-restore.
	wait_to_teleport    = false;
	ready_to_teleport   = false;
	wait_to_reposition  = false;
	ready_to_reposition = false;
	saved_location      = std::nullopt;
}

void OnBeforeSetupMainScreen()
{
	game_is_active = false;
	save_prefix.clear();
	mod_initiated_save  = false;
	wait_to_teleport    = false;
	ready_to_teleport   = false;
	wait_to_reposition  = false;
	ready_to_reposition = false;
	saved_location      = std::nullopt;

	if (!startup_loaded)
	{
		LoadConfig();
		startup_loaded = true;
	}
}

void OnGameActive()
{
	game_is_active = true;
}

void OnAfterDrawGui()
{
	if (!game_is_active) return;
	if (!MMAPI::Game::WindowHasFocus()) return;
	if (!keybind || !MMAPI::Input::IsKeybindPressed(*keybind)) return;

	TriggerSave();
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)module_interface);
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Game::Enable();
	MMAPI::Player::Enable();
	MMAPI::Location::Enable();
	MMAPI::Instance::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::AfterEndDay(OnAfterEndDay);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Game::Hooks::AfterLoadGame(OnAfterLoadGame);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Location::Hooks::AfterShowRoomTitle(OnAfterShowRoomTitle);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, OnAriTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
