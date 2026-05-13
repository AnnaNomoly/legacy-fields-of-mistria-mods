#include <algorithm>
#include <cctype>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

// ----- Mod metadata -----

static const char* const MOD_NAME = "DIY";
static const char* const VERSION  = "1.2.0";

// ----- Config -----

static const char* const CONFIG_KEY_ACTIVATION_BUTTON = "activation_button";
static const char* const CONFIG_KEY_UNLOCK_EVERYTHING = "unlock_everything";
static const char* const CONFIG_KEY_EXAMPLE_FURNITURE = "example_furniture";

static const std::string DEFAULT_ACTIVATION_BUTTON = "F8";
static const std::string DEFAULT_EXAMPLE_FURNITURE = "Picnic Display Donuts";
static const bool        DEFAULT_UNLOCK_EVERYTHING = false;

static std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
static bool        unlock_everything = DEFAULT_UNLOCK_EVERYTHING;
static std::string example_furniture = DEFAULT_EXAMPLE_FURNITURE;
static std::optional<MMAPI::Input::Keybind> keybind;

// ----- Localization keys -----

static const std::string UNRECOGNIZED_FURNITURE_KEY = "mods/DIY/unrecognized_furniture";
static const std::string FURNITURE_NOT_ACQUIRED_KEY = "mods/DIY/furniture_not_acquired";

// ----- State -----

static bool startup_loaded         = false;
static bool game_is_active         = false;
static bool localize_pending       = false;
static bool unlock_recipes_pending = false;

// Furniture data, keyed by recipe_key (the item's internal name, e.g. "picnic_donut_plate"):
//   recipe_key -> item_ids   — multiple item_ids may share a single recipe_key
//   recipe_key -> localized display name (e.g. "Picnic Display Donuts"); seeded with the
//                              raw name_key and replaced with the resolved string in LocalizeFurnitureNames
// lowercase_localized -> recipe_key — reverse lookup so user input can match display names case-insensitively
static std::map<std::string, std::vector<int>> recipe_to_ids;
static std::map<std::string, std::string>      recipe_to_localized_name;
static std::map<std::string, std::string>      lowercase_localized_to_recipe;

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

static void LoadConfig()
{
	std::filesystem::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	activation_button = MMAPI::Config::GetValue<std::string>(config, CONFIG_KEY_ACTIVATION_BUTTON, DEFAULT_ACTIVATION_BUTTON);
	unlock_everything = MMAPI::Config::GetValue<bool>       (config, CONFIG_KEY_UNLOCK_EVERYTHING, DEFAULT_UNLOCK_EVERYTHING);
	example_furniture = MMAPI::Config::GetValue<std::string>(config, CONFIG_KEY_EXAMPLE_FURNITURE, DEFAULT_EXAMPLE_FURNITURE);

	keybind = MMAPI::Input::TryParseKeybind(activation_button);
	if (!keybind)
	{
		MMAPI::Log::Warn("Invalid %s value (%s) — falling back to default %s",
			CONFIG_KEY_ACTIVATION_BUTTON, activation_button.c_str(), DEFAULT_ACTIVATION_BUTTON.c_str());
		activation_button = DEFAULT_ACTIVATION_BUTTON;
		keybind = MMAPI::Input::TryParseKeybind(activation_button);
	}

	// Persist back so the config file always reflects values in use (including filled-in missing
	// keys and the default-fallback when an invalid activation_button was supplied).
	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_ACTIVATION_BUTTON] = activation_button;
	roundtrip[CONFIG_KEY_UNLOCK_EVERYTHING] = unlock_everything;
	roundtrip[CONFIG_KEY_EXAMPLE_FURNITURE] = example_furniture;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config: %s=%s, %s=%s, %s=%s",
		CONFIG_KEY_ACTIVATION_BUTTON, activation_button.c_str(),
		CONFIG_KEY_UNLOCK_EVERYTHING, unlock_everything ? "true" : "false",
		CONFIG_KEY_EXAMPLE_FURNITURE, example_furniture.c_str());
}

// Scans __item_data for every item whose name_key contains "furniture" and which has a
// recipe.item_id member, indexing them by recipe_key. Multiple item_ids can share one recipe_key
// (e.g. the picnic plate has separate items for donuts vs. cake but a single recipe), so the
// id map is a vector.
static void LoadFurnitureData()
{
	MMAPI::Item::ForEachItem([](int item_id)
	{
		YYTK::RValue item = MMAPI::Item::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED) return;

		YYTK::RValue name_key_rv = MMAPI::Item::GetLocalizationKey(item_id);
		if (name_key_rv.m_Kind != YYTK::VALUE_STRING) return;

		std::string name_key = name_key_rv.ToString();
		if (name_key.find("furniture") == std::string::npos) return;

		if (!MMAPI::Engine::StructVariableExists(item, "recipe")) return;
		YYTK::RValue recipe = item.GetMember("recipe");
		if (recipe.m_Kind == YYTK::VALUE_UNDEFINED || recipe.m_Kind == YYTK::VALUE_NULL || recipe.m_Kind == YYTK::VALUE_UNSET)
			return;
		if (!MMAPI::Engine::StructVariableExists(recipe, "item_id")) return;

		YYTK::RValue recipe_key_rv = MMAPI::Item::GetInternalName(item_id);
		if (recipe_key_rv.m_Kind != YYTK::VALUE_STRING) return;

		std::string recipe_key = recipe_key_rv.ToString();
		recipe_to_ids[recipe_key].push_back(item_id);
		// Seed with the name_key; replaced with the resolved localized string in LocalizeFurnitureNames.
		recipe_to_localized_name[recipe_key] = name_key;
	});

	if (recipe_to_ids.empty())
		MMAPI::Log::Warn("Failed to load furniture recipes.");
	else
		MMAPI::Log::Info("Loaded data for %zu furniture recipes.", recipe_to_ids.size());
}

// Resolves every furniture name_key to its localized display name and builds the reverse lookup
// table keyed by the lowercased display name (so user input can match either the internal
// recipe_key or the localized display name, case-insensitive).
static void LocalizeFurnitureNames()
{
	for (auto& [recipe_key, name] : recipe_to_localized_name)
	{
		YYTK::RValue localized = MMAPI::Text::GetLocalizedString(name);
		if (localized.m_Kind != YYTK::VALUE_STRING) continue;

		std::string localized_str = localized.ToString();
		name = localized_str;
		lowercase_localized_to_recipe[ToLower(localized_str)] = recipe_key;
	}
}

// Unlocks every item_id sharing this recipe_key. When `silent` is false: shows the game's
// "recipe unlocked" popup if anything was newly unlocked, or a "furniture not acquired"
// notification if everything was already unlocked.
static void UnlockFurnitureRecipe(const std::string& recipe_key, bool silent)
{
	auto it = recipe_to_ids.find(recipe_key);
	if (it == recipe_to_ids.end()) return;

	std::vector<int> to_unlock;
	for (int id : it->second)
	{
		if (!MMAPI::Recipe::IsUnlocked(id))
			to_unlock.push_back(id);
	}

	if (to_unlock.empty())
	{
		if (!silent)
		{
			MMAPI::Game::CreateNotification(true, FURNITURE_NOT_ACQUIRED_KEY);
			MMAPI::Log::Info("Ignoring already known recipe: %s", recipe_key.c_str());
		}
		return;
	}

	// Show the popup only on the first newly-unlocked id; quiet for the rest so the player sees
	// at most one "recipe unlocked!" window per recipe even when several item_ids back it.
	for (size_t i = 0; i < to_unlock.size(); ++i)
		MMAPI::Recipe::Unlock(to_unlock[i], !silent && i == 0);

	if (!silent)
		MMAPI::Log::Info("Unlocked recipe: %s", recipe_key.c_str());
}

static void PromptForRecipe()
{
	std::string modal_text =
		"DIY v" + std::string(VERSION) + "\r\n"
		"------------------------------\r\n"
		"Input the desired furniture's Display Name or Internal Name.\r\n"
		"Case (capitalization) does not matter.\r\n"
		"Example 1 (Display Name): Picnic Display Donuts\r\n"
		"Example 2 (Display Name): picnic display donuts\r\n"
		"Example 3 (Internal Name): picnic_donut_plate\r\n"
		"More Info: https://github.com/AnnaNomoly/YYToolkit/tree/stable/DIY";

	YYTK::RValue user_input = MMAPI::Internal::module_interface->CallBuiltin(
		"get_string",
		{ YYTK::RValue(modal_text), YYTK::RValue(example_furniture) }
	);

	std::string lookup_key = ToLower(Trim(user_input.ToString()));

	auto display_match = lowercase_localized_to_recipe.find(lookup_key);
	if (display_match != lowercase_localized_to_recipe.end())
		lookup_key = display_match->second;

	if (recipe_to_ids.count(lookup_key) > 0)
	{
		UnlockFurnitureRecipe(lookup_key, false);
	}
	else
	{
		MMAPI::Game::CreateNotification(true, UNRECOGNIZED_FURNITURE_KEY);
		MMAPI::Log::Info("Ignoring invalid recipe: %s", lookup_key.c_str());
	}
}

// ----- Hooks -----

void OnBeforeSetupMainScreen()
{
	game_is_active = false;
	unlock_recipes_pending = true;

	if (!startup_loaded)
	{
		LoadConfig();
		LoadFurnitureData();
		localize_pending = true;
		startup_loaded = true;
	}
}

void OnGameActive()
{
	game_is_active = true;

	if (unlock_everything && unlock_recipes_pending)
	{
		unlock_recipes_pending = false;
		for (const auto& [recipe_key, _] : recipe_to_ids)
			UnlockFurnitureRecipe(recipe_key, true);
		MMAPI::Log::Info("Unlocked all furniture recipes!");
	}
}

void OnAfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& /*ctx*/)
{
	if (!localize_pending) return;
	localize_pending = false;
	LocalizeFurnitureNames();
}

void OnAfterDrawGui()
{
	if (!game_is_active) return;
	if (!MMAPI::Game::WindowHasFocus()) return;
	if (!keybind || !MMAPI::Input::IsKeybindPressed(*keybind)) return;

	PromptForRecipe();
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
	MMAPI::Item::Enable();
	MMAPI::Recipe::Enable();
	MMAPI::Text::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Text::Hooks::AfterLocalizedString(OnAfterLocalizedString);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
