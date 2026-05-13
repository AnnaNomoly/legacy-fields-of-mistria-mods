#include <cstdint>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "StopTimeIndoors";
static const char* const VERSION  = "1.1.0";

// ----- Config -----

static const char* const CONFIG_KEY_STOP_TIME_IN_DUNGEON = "stop_time_in_dungeon";
static const char* const CONFIG_KEY_STOP_TIME_ON_FARM    = "stop_time_on_farm";

static const bool DEFAULT_STOP_TIME_IN_DUNGEON = false;
static const bool DEFAULT_STOP_TIME_ON_FARM    = false;

static bool stop_time_in_dungeon = DEFAULT_STOP_TIME_IN_DUNGEON;
static bool stop_time_on_farm    = DEFAULT_STOP_TIME_ON_FARM;

// ----- State -----

static bool startup_loaded = false;
static bool game_is_active = false;

// ----- Config loading -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	stop_time_in_dungeon = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_STOP_TIME_IN_DUNGEON, DEFAULT_STOP_TIME_IN_DUNGEON);
	stop_time_on_farm    = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_STOP_TIME_ON_FARM,    DEFAULT_STOP_TIME_ON_FARM);

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_STOP_TIME_IN_DUNGEON] = stop_time_in_dungeon;
	roundtrip[CONFIG_KEY_STOP_TIME_ON_FARM]    = stop_time_on_farm;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config: %s=%s, %s=%s",
		CONFIG_KEY_STOP_TIME_IN_DUNGEON, stop_time_in_dungeon ? "true" : "false",
		CONFIG_KEY_STOP_TIME_ON_FARM,    stop_time_on_farm    ? "true" : "false");
}

// ----- Indoor predicate -----

// Treats Ari as indoors (i.e. time should stop) when she's in an indoor location, with two
// config-driven overrides:
//   - Farm: outdoor by default, but counts as indoors when stop_time_on_farm is true
//   - Dungeons: indoor location-wise, but counts as NOT indoors unless stop_time_in_dungeon is true
//     (preserves the original mod's "dungeons are an opt-in special case" semantic)
static bool AriIsIndoors()
{
	if (!game_is_active) return false;

	MMAPI::Location::Ids loc;
	if (!MMAPI::Location::TryGetCurrentLocation(loc)) return false;

	if (loc == MMAPI::Location::Ids::Farm)
		return stop_time_on_farm;

	if (MMAPI::Location::IsOutdoors(loc)) return false;

	if (MMAPI::Dungeon::IsDungeonRoom() && !stop_time_in_dungeon)
		return false;

	return true;
}

// ----- Hooks -----

void OnAfterClockUpdate(MMAPI::Calendar::ClockUpdateContext& ctx)
{
	if (!AriIsIndoors()) return;

	int64_t old_time = ctx.GetOldTime();
	int64_t new_time = static_cast<int64_t>(MMAPI::Game::GetCurrentTimeInSeconds());
	if (new_time == old_time) return;

	// Revert the clock advance by writing the pre-update value back to __clock.time. Raw ref
	// write (rather than the GML struct_set builtin) matches the original mod and avoids
	// triggering any setter-side effects the game might attach to the time member.
	*MMAPI::Internal::global_instance
		->GetRefMember("__clock")
		->GetRefMember("time") = old_time;
}

void OnBeforeSetupMainScreen()
{
	game_is_active = false;

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
	MMAPI::Location::Enable();
	MMAPI::Dungeon::Enable();
	MMAPI::Calendar::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Calendar::Hooks::AfterClockUpdate(OnAfterClockUpdate);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
