#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>
#include <string>
#include <unordered_set>

#include <Windows.h>  // VK_* constants

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----

static const char* const MOD_NAME = "Cookbook";
static const char* const VERSION  = "1.2.0";

// ----- Config keys + defaults -----

static const char* const ACTIVATION_BUTTON_KEY = "activation_button";
static const char* const UNLOCK_EVERYTHING_KEY = "unlock_everything";
static const char* const EXAMPLE_RECIPE_KEY    = "example_recipe";

static const std::string DEFAULT_ACTIVATION_BUTTON = "F10";
static const std::string DEFAULT_EXAMPLE_RECIPE    = "Hot Chocolate";
static const bool        DEFAULT_UNLOCK_EVERYTHING = false;

// ----- Localization keys -----

static const std::string UNRECOGNIZED_RECIPE_LOCALIZATION_KEY = "mods/Cookbook/unrecognized_recipe";
static const std::string RECIPE_NOT_ACQUIRED_LOCALIZATION_KEY = "mods/Cookbook/recipe_not_acquired";

// Activation-button name → input code. Keyboard names map to Win32 VK_* constants; gamepad names map
// to GameMaker gamepad button constants (0x80xx).
static const std::map<std::string, int> KEY_NAME_TO_VK = {
	{ "F1", VK_F1 }, { "F2", VK_F2 }, { "F3", VK_F3 }, { "F4", VK_F4 },
	{ "F5", VK_F5 }, { "F6", VK_F6 }, { "F7", VK_F7 }, { "F8", VK_F8 },
	{ "F9", VK_F9 }, { "F10", VK_F10 }, { "F11", VK_F11 }, { "F12", VK_F12 },
	{ "NUMPAD_0", VK_NUMPAD0 }, { "NUMPAD_1", VK_NUMPAD1 }, { "NUMPAD_2", VK_NUMPAD2 },
	{ "NUMPAD_3", VK_NUMPAD3 }, { "NUMPAD_4", VK_NUMPAD4 }, { "NUMPAD_5", VK_NUMPAD5 },
	{ "NUMPAD_6", VK_NUMPAD6 }, { "NUMPAD_7", VK_NUMPAD7 }, { "NUMPAD_8", VK_NUMPAD8 },
	{ "NUMPAD_9", VK_NUMPAD9 },
	{ "0", '0' }, { "1", '1' }, { "2", '2' }, { "3", '3' }, { "4", '4' },
	{ "5", '5' }, { "6", '6' }, { "7", '7' }, { "8", '8' }, { "9", '9' },
	{ "A", 'A' }, { "B", 'B' }, { "C", 'C' }, { "D", 'D' }, { "E", 'E' },
	{ "F", 'F' }, { "G", 'G' }, { "H", 'H' }, { "I", 'I' }, { "J", 'J' },
	{ "K", 'K' }, { "L", 'L' }, { "M", 'M' }, { "N", 'N' }, { "O", 'O' },
	{ "P", 'P' }, { "Q", 'Q' }, { "R", 'R' }, { "S", 'S' }, { "T", 'T' },
	{ "U", 'U' }, { "V", 'V' }, { "W", 'W' }, { "X", 'X' }, { "Y", 'Y' },
	{ "Z", 'Z' },
	{ "INSERT", VK_INSERT }, { "DELETE", VK_DELETE }, { "HOME", VK_HOME },
	{ "PAGE_UP", VK_PRIOR }, { "PAGE_DOWN", VK_NEXT }, { "NUM_LOCK", VK_NUMLOCK },
	{ "SCROLL_LOCK", VK_SCROLL }, { "CAPS_LOCK", VK_CAPITAL }, { "PAUSE_BREAK", VK_PAUSE },
};

static const std::map<std::string, int> KEY_NAME_TO_GAMEPAD = {
	{ "GAMEPAD_A", 0x8001 }, { "GAMEPAD_B", 0x8002 }, { "GAMEPAD_X", 0x8003 }, { "GAMEPAD_Y", 0x8004 },
	{ "GAMEPAD_LEFT_SHOULDER",  0x8005 }, { "GAMEPAD_RIGHT_SHOULDER", 0x8006 },
	{ "GAMEPAD_LEFT_TRIGGER",   0x8007 }, { "GAMEPAD_RIGHT_TRIGGER",  0x8008 },
	{ "GAMEPAD_SELECT", 0x8009 }, { "GAMEPAD_START", 0x800A },
	{ "GAMEPAD_LEFT_STICK", 0x800B }, { "GAMEPAD_RIGHT_STICK", 0x800C },
	{ "GAMEPAD_DPAD_UP",    0x800D }, { "GAMEPAD_DPAD_DOWN",  0x800E },
	{ "GAMEPAD_DPAD_LEFT",  0x800F }, { "GAMEPAD_DPAD_RIGHT", 0x8010 },
};

// ----- Config -----

struct CookbookConfig
{
	std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
	bool        unlock_everything = DEFAULT_UNLOCK_EVERYTHING;
	std::string example_recipe    = DEFAULT_EXAMPLE_RECIPE;
};

void to_json(json& j, const CookbookConfig& c)
{
	j = json{
		{ ACTIVATION_BUTTON_KEY, c.activation_button },
		{ UNLOCK_EVERYTHING_KEY, c.unlock_everything },
		{ EXAMPLE_RECIPE_KEY,    c.example_recipe    },
	};
}

void from_json(const json& j, CookbookConfig& c)
{
	c.activation_button = MMAPI::Config::GetValue<std::string>(j, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON);
	c.unlock_everything = MMAPI::Config::GetValue<bool>       (j, UNLOCK_EVERYTHING_KEY, DEFAULT_UNLOCK_EVERYTHING);
	c.example_recipe    = MMAPI::Config::GetValue<std::string>(j, EXAMPLE_RECIPE_KEY,    DEFAULT_EXAMPLE_RECIPE);

	// Validate activation_button against the known key sets; fall back to default if unrecognized.
	if (!KEY_NAME_TO_VK.contains(c.activation_button) && !KEY_NAME_TO_GAMEPAD.contains(c.activation_button))
		c.activation_button = DEFAULT_ACTIVATION_BUTTON;
}

// ----- State -----

static CookbookConfig config{};
static bool startup_loaded         = false;
static bool recipes_unlocked_all   = false;  // One-shot guard for unlock-everything mode.
static int  activation_button_code = -1;
static bool activation_is_gamepad  = false;
static bool processing_user_input  = false;  // Prevents re-entry while the get_string modal is open.

// Cooking recipes discovered in __item_data: internal name ↔ item_id.
static std::map<std::string, int> item_name_to_id;
static std::map<int, std::string> item_id_to_name;

// Lowercase localized display name → internal name (for user-typed lookup in the modal).
static std::map<std::string, std::string> lowercase_localized_to_internal;

// ----- Helpers -----

void LoadOrCreateConfigFile()
{
	try
	{
		auto path = MMAPI::Config::GetConfigPath(MOD_NAME);
		bool existed = std::filesystem::exists(path);
		json j = MMAPI::Config::Load(path);

		if (!existed)
			MMAPI::Log::Warn("Configuration file was not found. Creating file: %s", path.string().c_str());

		if (j.empty())
		{
			if (existed)
				MMAPI::Log::Error("No readable values in configuration file: %s!", path.string().c_str());
			config = CookbookConfig{};
		}
		else
		{
			config = j.get<CookbookConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = CookbookConfig{};
	}
}

void ConfigureActivationButton()
{
	if (auto it = KEY_NAME_TO_GAMEPAD.find(config.activation_button); it != KEY_NAME_TO_GAMEPAD.end())
	{
		activation_is_gamepad  = true;
		activation_button_code = it->second;
	}
	else if (auto it = KEY_NAME_TO_VK.find(config.activation_button); it != KEY_NAME_TO_VK.end())
	{
		activation_is_gamepad  = false;
		activation_button_code = it->second;
	}
}

// Discovers every cooking recipe in `__item_data` and populates the internal-name ↔ item_id maps.
// Filter mirrors the original mod: items whose `name_key` contains "cooked_dishes" or whose
// `kitchen_tier_requirement` is set, and which have a `recipe.item_id` member.
void LoadRecipeData()
{
	item_name_to_id.clear();
	item_id_to_name.clear();

	MMAPI::Item::ForEachItem([](int id) {
		YYTK::RValue item = MMAPI::Item::GetItemData(id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED) return;

		// Filter: must be a cooking recipe.
		bool is_cooked_dish = false;
		if (MMAPI::Engine::StructVariableExists(item, "name_key"))
		{
			YYTK::RValue name_key = item.GetMember("name_key");
			if (name_key.m_Kind == YYTK::VALUE_STRING && std::string_view(name_key.ToString()).find("cooked_dishes") != std::string_view::npos)
				is_cooked_dish = true;
		}

		bool has_kitchen_tier = false;
		if (MMAPI::Engine::StructVariableExists(item, "kitchen_tier_requirement"))
		{
			YYTK::RValue tier = item.GetMember("kitchen_tier_requirement");
			has_kitchen_tier = tier.m_Kind != YYTK::VALUE_UNDEFINED && tier.m_Kind != YYTK::VALUE_UNSET && tier.m_Kind != YYTK::VALUE_NULL;
		}

		if (!is_cooked_dish && !has_kitchen_tier) return;

		// Must have a recipe definition.
		if (!MMAPI::Engine::StructVariableExists(item, "recipe")) return;
		YYTK::RValue recipe = item.GetMember("recipe");
		if (recipe.m_Kind != YYTK::VALUE_OBJECT) return;
		if (!MMAPI::Engine::StructVariableExists(recipe, "item_id")) return;

		YYTK::RValue recipe_key_rv = item.GetMember("recipe_key");
		if (recipe_key_rv.m_Kind != YYTK::VALUE_STRING) return;

		std::string recipe_key = recipe_key_rv.ToString();
		item_name_to_id[recipe_key] = id;
		item_id_to_name[id]         = recipe_key;
	});

	if (item_name_to_id.empty())
		MMAPI::Log::Error("Failed to load recipe data!");
	else
		MMAPI::Log::Info("Loaded data for %zu recipes!", item_name_to_id.size());
}

// Resolves each recipe's localized display name into the lookup map. Run once after the Localizer
// is available so we can match user input against localized names later.
void BuildLocalizedNameLookup()
{
	lowercase_localized_to_internal.clear();

	for (const auto& [internal_name, item_id] : item_name_to_id)
	{
		YYTK::RValue localized = MMAPI::Item::GetLocalizedName(item_id);
		if (localized.m_Kind != YYTK::VALUE_STRING) continue;

		std::string lower = localized.ToString();
		std::transform(lower.begin(), lower.end(), lower.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		lowercase_localized_to_internal[lower] = internal_name;
	}
}

std::string TrimWhitespace(std::string s)
{
	s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
		[](unsigned char ch) { return std::isspace(ch); }));
	s.erase(std::find_if_not(s.rbegin(), s.rend(),
		[](unsigned char ch) { return std::isspace(ch); }).base(), s.end());
	return s;
}

// Unlocks a recipe and optionally shows the appropriate popup/notification.
//   - On a newly-unlocked recipe (silent=false): the game's recipe-unlocked popup is shown via `Recipe::Unlock(item_id, true)`.
//   - On an already-known recipe (silent=false): displays the "recipe not acquired" notification.
//   - silent=true: unlocks without any popup (used for the unlock-everything bulk path).
void UnlockRecipe(int item_id, bool silent)
{
	if (silent)
	{
		MMAPI::Recipe::Unlock(item_id, /*show_popup=*/false);
		return;
	}

	bool newly_unlocked = MMAPI::Recipe::Unlock(item_id, /*show_popup=*/true);
	if (newly_unlocked)
	{
		MMAPI::Log::Info("Unlocked recipe: %s", item_id_to_name[item_id].c_str());
	}
	else
	{
		MMAPI::Game::CreateNotification(false, RECIPE_NOT_ACQUIRED_LOCALIZATION_KEY);
		MMAPI::Log::Warn("Ignoring already-known recipe: %s", item_id_to_name[item_id].c_str());
	}
}

bool CheckActivationPressed()
{
	if (activation_button_code < 0) return false;

	if (activation_is_gamepad)
	{
		int slot = MMAPI::Input::GetFirstConnectedGamepadSlot();
		if (slot < 0) return false;
		return MMAPI::Input::GamepadButtonCheckPressed(slot, activation_button_code);
	}

	if (MMAPI::Input::KeyboardCheckPressed(activation_button_code))
	{
		processing_user_input = true;
		return true;
	}
	return false;
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	recipes_unlocked_all  = false;
	processing_user_input = false;

	if (startup_loaded) return;

	LoadOrCreateConfigFile();
	ConfigureActivationButton();
	LoadRecipeData();

	startup_loaded = true;
}

void OnAfterGameActive()
{
	// First-fire-per-session work: build the localized-name lookup (Localizer is reliably available
	// by the time the game becomes interactive). If unlock_everything is enabled, bulk-unlock here.
	if (lowercase_localized_to_internal.empty())
		BuildLocalizedNameLookup();

	if (config.unlock_everything && !recipes_unlocked_all)
	{
		recipes_unlocked_all = true;
		for (const auto& [name, item_id] : item_name_to_id)
			UnlockRecipe(item_id, /*silent=*/true);
		MMAPI::Log::Info("Unlocked all cooking recipes!");
	}
}

void OnAfterDrawGui()
{
	if (!MMAPI::Game::WindowHasFocus()) return;
	if (processing_user_input) return;

	if (!CheckActivationPressed())
		return;

	std::string modal_text =
		"Cookbook v" + std::string(VERSION) + "\r\n"
		"------------------------------\r\n"
		"Input the desired recipe's Display Name or Internal Name.\r\n"
		"Case (capitalization) does not matter.\r\n"
		"Example 1 (Display Name): Hot Chocolate\r\n"
		"Example 2 (Display Name): hot chocolate\r\n"
		"Example 3 (Internal Name): hot_cocoa\r\n"
		"More Info: https://github.com/AnnaNomoly/YYToolkit/tree/stable/Cookbook";

	YYTK::RValue user_input = MMAPI::Internal::module_interface->CallBuiltin(
		"get_string",
		{ YYTK::RValue(modal_text), YYTK::RValue(config.example_recipe) }
	);

	std::string input = TrimWhitespace(user_input.ToString());
	std::transform(input.begin(), input.end(), input.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	// User can type either an internal name (e.g. "hot_cocoa") or a localized name (e.g. "hot chocolate").
	// Resolve a localized match first, then fall back to internal-name lookup.
	if (auto it = lowercase_localized_to_internal.find(input); it != lowercase_localized_to_internal.end())
		input = it->second;

	if (auto it = item_name_to_id.find(input); it != item_name_to_id.end())
	{
		UnlockRecipe(it->second, /*silent=*/false);
	}
	else
	{
		MMAPI::Game::CreateNotification(false, UNRECOGNIZED_RECIPE_LOCALIZATION_KEY);
		MMAPI::Log::Warn("Ignoring invalid recipe: %s", input.c_str());
	}

	processing_user_input = false;
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
	MMAPI::Input::Enable();
	MMAPI::Item::Enable();
	MMAPI::Recipe::Enable();
	MMAPI::Text::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnAfterGameActive);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
