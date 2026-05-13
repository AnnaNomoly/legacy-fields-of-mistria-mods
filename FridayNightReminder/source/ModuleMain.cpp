#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "FridayNightReminder";
static const char* const VERSION  = "1.2.0";

// ----- Config -----

static const char* const CONFIG_KEY_NOTIFICATION_TIME = "notification_time_in_seconds";

// ----- Constants -----

static constexpr int MIN_NOTIFICATION_TIME = 21600; // 06:00
static constexpr int MAX_NOTIFICATION_TIME = 86400; // 26:00
static constexpr int DEFAULT_NOTIFICATION_TIME = MMAPI::Calendar::NightStartTimeInSeconds;

// ----- Notification -----

static const char* const VISIT_THE_INN_NOTIFICATION_KEY = "Mods/Notifications/Friday Night Reminder/visit_the_inn";

// ----- State -----

static bool startup_loaded     = false;
static bool notification_sent  = false;
static int  notification_time  = DEFAULT_NOTIFICATION_TIME;

// ----- Config -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	notification_time = MMAPI::Config::GetValue<int>(
		config, CONFIG_KEY_NOTIFICATION_TIME, DEFAULT_NOTIFICATION_TIME,
		MIN_NOTIFICATION_TIME, MAX_NOTIFICATION_TIME);

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_NOTIFICATION_TIME] = notification_time;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Notification time: %d seconds.", notification_time);
}

// ----- Hooks -----

void OnAfterClockUpdate(MMAPI::Calendar::ClockUpdateContext& /*ctx*/)
{
	if (notification_sent) return;
	if (!MMAPI::Calendar::IsWeekday(MMAPI::Calendar::Weekdays::Friday)) return;
	if (MMAPI::Game::GetCurrentTimeInSeconds() < notification_time) return;

	MMAPI::Game::CreateNotification(/*ignore_cooldown=*/false, VISIT_THE_INN_NOTIFICATION_KEY);
	notification_sent = true;
}

void OnBeforeNewDay()
{
	notification_sent = false;
}

void OnBeforeSetupMainScreen()
{
	notification_sent = false;

	if (startup_loaded) return;
	LoadConfig();
	startup_loaded = true;
}

// ----- Init -----

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
	MMAPI::Calendar::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::BeforeNewDay(OnBeforeNewDay);
	MMAPI::Calendar::Hooks::AfterClockUpdate(OnAfterClockUpdate);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
