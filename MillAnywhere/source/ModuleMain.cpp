#include <optional>
#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "MillAnywhere";
static const char* const VERSION  = "1.2.0";

// ----- Config -----

static const char* const CONFIG_KEY_ACTIVATION_BUTTON = "activation_button";
static const std::string DEFAULT_ACTIVATION_BUTTON    = "F12";

static std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
static std::optional<MMAPI::Input::Keybind> keybind;

// ----- GML script names -----

// Anonymous function extracted from obj_mill_menu's Create event during GameMaker compilation —
// calling this opens the mill menu UI.
static const char* const GML_SCRIPT_OPEN_MILL_MENU = "gml_Script_anon@382@gml_Object_obj_mill_menu_Create_0";

// ----- State -----

static bool startup_loaded = false;
static bool game_is_active = false;

// ----- Config loading -----

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

// ----- Mill menu open -----

// The anonymous Create-event script opens the mill menu via its own internal spawn logic — it
// doesn't read `self`, so we can invoke it with a null calling context.
static void OpenMillMenu()
{
	YYTK::CScript* gml_script = nullptr;
	MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_OPEN_MILL_MENU, reinterpret_cast<PVOID*>(&gml_script));
	if (!gml_script) return;

	YYTK::RValue result;
	gml_script->m_Functions->m_ScriptFunction(nullptr, nullptr, result, 0, nullptr);
}

// ----- Hooks -----

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

void OnAfterDrawGui()
{
	if (!game_is_active) return;
	if (!MMAPI::Engine::WindowHasFocus()) return;
	if (MMAPI::Game::IsPaused()) return;
	if (!keybind || !MMAPI::Input::IsKeybindPressed(*keybind)) return;

	OpenMillMenu();
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

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Display::Hooks::AfterDrawGui(OnAfterDrawGui);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
