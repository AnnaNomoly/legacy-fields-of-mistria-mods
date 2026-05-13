#include <algorithm>
#include <cctype>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "Wardrobe";
static const char* const VERSION  = "1.2.0";

// ----- Config -----

static const char* const CONFIG_KEY_ACTIVATION_BUTTON = "activation_button";
static const char* const CONFIG_KEY_UNLOCK_EVERYTHING = "unlock_everything";
static const char* const CONFIG_KEY_EXAMPLE_COSMETIC  = "example_cosmetic";

static const std::string DEFAULT_ACTIVATION_BUTTON = "F9";
static const std::string DEFAULT_EXAMPLE_COSMETIC  = "Flower Hat";
static const bool        DEFAULT_UNLOCK_EVERYTHING = false;

static std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
static bool        unlock_everything = DEFAULT_UNLOCK_EVERYTHING;
static std::string example_cosmetic  = DEFAULT_EXAMPLE_COSMETIC;
static std::optional<MMAPI::Input::Keybind> keybind;

// ----- Localization keys -----

static const std::string COSMETIC_ACQUIRED_KEY      = "mods/Wardrobe/cosmetic_acquired";
static const std::string UNRECOGNIZED_COSMETIC_KEY  = "mods/Wardrobe/unrecognized_cosmetic";
static const std::string COSMETIC_NOT_ACQUIRED_KEY  = "mods/Wardrobe/cosmetic_not_acquired";

// ----- State -----

static bool startup_loaded         = false;
static bool game_is_active         = false;
static bool localize_pending       = false;
static bool unlock_everything_pending = false;

// Cosmetic data, keyed by internal name (the key in __pad.player_assets.inner):
//   cosmetic -> localized display name (seeded with the raw localization key from .name,
//               replaced with the resolved string in LocalizeCosmeticNames)
// lowercase_display -> cosmetic_name — reverse lookup so user input can match display names
//                                       case-insensitively
static std::map<std::string, std::string> cosmetic_to_localized_name;
static std::map<std::string, std::string> lowercase_localized_to_cosmetic;

// ----- Helpers -----

static std::string ToLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}

static std::string Trim(std::string s)
{
	s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
		[](unsigned char ch) { return std::isspace(ch); }));
	s.erase(std::find_if_not(s.rbegin(), s.rend(),
		[](unsigned char ch) { return std::isspace(ch); }).base(), s.end());
	return s;
}

// ----- Config loading -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	activation_button = MMAPI::Config::GetValue<std::string>(config, CONFIG_KEY_ACTIVATION_BUTTON, DEFAULT_ACTIVATION_BUTTON);
	unlock_everything = MMAPI::Config::GetValue<bool>       (config, CONFIG_KEY_UNLOCK_EVERYTHING, DEFAULT_UNLOCK_EVERYTHING);
	example_cosmetic  = MMAPI::Config::GetValue<std::string>(config, CONFIG_KEY_EXAMPLE_COSMETIC,  DEFAULT_EXAMPLE_COSMETIC);

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
	roundtrip[CONFIG_KEY_UNLOCK_EVERYTHING] = unlock_everything;
	roundtrip[CONFIG_KEY_EXAMPLE_COSMETIC]  = example_cosmetic;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config: %s=%s, %s=%s, %s=%s",
		CONFIG_KEY_ACTIVATION_BUTTON, activation_button.c_str(),
		CONFIG_KEY_UNLOCK_EVERYTHING, unlock_everything ? "true" : "false",
		CONFIG_KEY_EXAMPLE_COSMETIC,  example_cosmetic.c_str());
}

// Iterates `globalInstance.__pad.player_assets.inner` to collect every cosmetic's internal name
// and its display-name localization key. The value is seeded with the raw key and replaced with
// the resolved string in LocalizeCosmeticNames once the Localizer is ready.
static void LoadCosmeticData()
{
	MMAPI::Cosmetic::ForEachCosmetic([](const std::string& name, YYTK::RValue data)
	{
		if (!MMAPI::Engine::StructVariableExists(data, "name")) return;
		YYTK::RValue loc_key = data.GetMember("name");
		if (loc_key.m_Kind != YYTK::VALUE_STRING) return;
		cosmetic_to_localized_name[name] = loc_key.ToString();
	});

	if (cosmetic_to_localized_name.empty())
		MMAPI::Log::Warn("Failed to load cosmetic data.");
	else
		MMAPI::Log::Info("Loaded data for %zu cosmetics.", cosmetic_to_localized_name.size());
}

static void LocalizeCosmeticNames()
{
	for (auto& [cosmetic, name] : cosmetic_to_localized_name)
	{
		YYTK::RValue localized = MMAPI::Text::GetLocalizedString(name);
		if (localized.m_Kind != YYTK::VALUE_STRING) continue;

		std::string localized_str = localized.ToString();
		name = localized_str;
		lowercase_localized_to_cosmetic[ToLower(localized_str)] = cosmetic;
	}
}

// ----- Cosmetic unlocking -----

static void UnlockCosmetic(const std::string& cosmetic_name, bool silent)
{
	if (MMAPI::Cosmetic::Unlock(cosmetic_name))
	{
		if (!silent)
		{
			MMAPI::Game::CreateNotification(true, COSMETIC_ACQUIRED_KEY);
			MMAPI::Log::Info("Unlocked cosmetic: %s", cosmetic_name.c_str());
		}
	}
	else if (!silent)
	{
		MMAPI::Game::CreateNotification(true, COSMETIC_NOT_ACQUIRED_KEY);
		MMAPI::Log::Info("Ignoring already obtained cosmetic: %s", cosmetic_name.c_str());
	}
}

static void PromptForCosmetic()
{
	std::string modal_text =
		"Wardrobe v" + std::string(VERSION) + "\r\n"
		"------------------------------\r\n"
		"Input the desired cosmetic's Display Name or Internal Name.\r\n"
		"Case (capitalization) does not matter.\r\n"
		"Example 1 (Display Name): Flower Hat\r\n"
		"Example 2 (Display Name): flower hat\r\n"
		"Example 3 (Internal Name): head_flower_hat\r\n"
		"More Info: https://github.com/AnnaNomoly/YYToolkit/tree/stable/Wardrobe";

	YYTK::RValue user_input = MMAPI::Internal::module_interface->CallBuiltin(
		"get_string",
		{ YYTK::RValue(modal_text), YYTK::RValue(example_cosmetic) }
	);

	std::string lookup_key = ToLower(Trim(user_input.ToString()));

	auto display_match = lowercase_localized_to_cosmetic.find(lookup_key);
	if (display_match != lowercase_localized_to_cosmetic.end())
		lookup_key = display_match->second;

	if (cosmetic_to_localized_name.count(lookup_key) > 0)
	{
		UnlockCosmetic(lookup_key, false);
	}
	else
	{
		MMAPI::Game::CreateNotification(true, UNRECOGNIZED_COSMETIC_KEY);
		MMAPI::Log::Info("Ignoring unrecognized cosmetic: %s", lookup_key.c_str());
	}
}

// ----- Hooks -----

void OnBeforeSetupMainScreen()
{
	game_is_active = false;
	unlock_everything_pending = true;

	if (!startup_loaded)
	{
		LoadConfig();
		LoadCosmeticData();
		localize_pending = true;
		startup_loaded = true;
	}
}

void OnGameActive()
{
	game_is_active = true;

	if (unlock_everything && unlock_everything_pending)
	{
		unlock_everything_pending = false;
		for (const auto& [cosmetic, _] : cosmetic_to_localized_name)
			UnlockCosmetic(cosmetic, true);
		MMAPI::Log::Info("Unlocked all cosmetics!");
	}
}

void OnAfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& /*ctx*/)
{
	if (!localize_pending) return;
	localize_pending = false;
	LocalizeCosmeticNames();
}

void OnAfterDrawGui()
{
	if (!game_is_active) return;
	if (!MMAPI::Game::WindowHasFocus()) return;
	if (!keybind || !MMAPI::Input::IsKeybindPressed(*keybind)) return;

	PromptForCosmetic();
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
	MMAPI::Text::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Text::Hooks::AfterLocalizedString(OnAfterLocalizedString);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
