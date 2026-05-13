#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;
namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "BulkBuy";
static const char* const VERSION  = "1.1.0";

// ----- Config keys -----

static const char* const CONFIG_KEY_ACTIVATION_BUTTON = "activation_button";
static const char* const CONFIG_KEY_BULK_QUANTITY     = "bulk_quantity";

// ----- Defaults & limits -----

static const char* const DEFAULT_ACTIVATION_BUTTON = "SHIFT";
static constexpr int     DEFAULT_BULK_QUANTITY     = 10;
static constexpr int     MIN_BULK_QUANTITY         = 2;
static constexpr int     MAX_BULK_QUANTITY         = 999;

// ----- State -----

static bool                                  startup_loaded    = false;
static std::optional<MMAPI::Input::Keybind>  activation_keybind = std::nullopt;
static int                                   bulk_quantity     = DEFAULT_BULK_QUANTITY;

// ----- Config -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	std::string button_name = MMAPI::Config::GetValue<std::string>(
		config, CONFIG_KEY_ACTIVATION_BUTTON, DEFAULT_ACTIVATION_BUTTON);

	activation_keybind = MMAPI::Input::TryParseKeybind(button_name);
	if (!activation_keybind)
	{
		MMAPI::Log::Warn("Invalid \"%s\" value (%s); falling back to %s.",
			CONFIG_KEY_ACTIVATION_BUTTON, button_name.c_str(), DEFAULT_ACTIVATION_BUTTON);
		button_name        = DEFAULT_ACTIVATION_BUTTON;
		activation_keybind = MMAPI::Input::TryParseKeybind(DEFAULT_ACTIVATION_BUTTON);
	}

	bulk_quantity = MMAPI::Config::GetValue<int>(
		config, CONFIG_KEY_BULK_QUANTITY, DEFAULT_BULK_QUANTITY,
		MIN_BULK_QUANTITY, MAX_BULK_QUANTITY);

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_ACTIVATION_BUTTON] = button_name;
	roundtrip[CONFIG_KEY_BULK_QUANTITY]     = bulk_quantity;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Activation button: %s, bulk quantity: %d", button_name.c_str(), bulk_quantity);
}

// ----- Hooks -----

void OnAfterStoreMenuAddItem(MMAPI::Game::AfterStoreMenuAddItemContext& ctx)
{
	if (!activation_keybind)             return;
	if (!MMAPI::Engine::WindowHasFocus()) return;
	if (!MMAPI::Input::IsKeybindDown(*activation_keybind)) return;

	ctx.RepeatOriginal(bulk_quantity - 1);
}

void OnBeforeSetupMainScreen()
{
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

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterStoreMenuAddItem(OnAfterStoreMenuAddItem);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
