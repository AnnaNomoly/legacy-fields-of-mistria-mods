#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----
//
// The internal name is "SpawnAnyItem" even though the project directory is "DigUpAnything". The mod
// was renamed; the directory was left as-is to avoid breaking installed-mod paths. The renamed config
// file is at mod_data/SpawnAnyItem/SpawnAnyItem.json; OnSetupMainScreen migrates the old paths.

static const char* const MOD_NAME = "SpawnAnyItem";
static const char* const VERSION  = "2.1.0";

// ----- Config keys + defaults -----

static const char* const ACTIVATION_BUTTON_KEY = "activation_button";
static const char* const EXAMPLE_ITEM_KEY      = "example_item";
static const char* const DIG_SPOT_MODE_KEY     = "dig_spot_mode";

static const std::string DEFAULT_ACTIVATION_BUTTON = "F11";
static const std::string DEFAULT_EXAMPLE_ITEM      = "Tea with Lemon";
static const bool        DEFAULT_DIG_SPOT_MODE     = false;

// ----- Notification localization keys -----

static const std::string VALID_ITEM_LOCALIZATION_KEY                       = "mods/DigUpAnything/valid_item";
static const std::string DISABLED_ITEM_LOCALIZATION_KEY                    = "mods/DigUpAnything/disabled_item";
static const std::string UNRECOGNIZED_ITEM_LOCALIZATION_KEY                = "mods/DigUpAnything/unrecognized_item";
static const std::string ITEM_NOT_ACQUIRED_LOCALIZATION_KEY                = "mods/DigUpAnything/item_not_acquired";
static const std::string UNACQUIRED_ITEM_ALREADY_SPAWNED_LOCALIZATION_KEY  = "mods/DigUpAnything/unacquired_item_already_spawned";
static const std::string ITEM_MAX_STACK_IS_ONE_LOCALIZATION_KEY            = "mods/DigUpAnything/item_max_stack_is_one";

// Items the mod refuses to spawn — typically because they'd break game state (cosmetics, scrolls
// that grant permanent unlocks, the player's wallet, etc.).
static const std::unordered_set<std::string> DISABLED_ITEMS = {
	"animal_cosmetic",
	"pet_cosmetic",
	"cosmetic",
	"crafting_scroll",
	"purse",
	"recipe_scroll",
	"unidentified_artifact",
};

// ----- Cross-mod IPC keys (for SecretSanta / Curator dig-spot suppression) -----

static const char* const YYTK_KEY                 = "__YYTK";
static const char* const IGNORE_NEXT_DIG_SPOT_KEY = "ignore_next_dig_spot";
static const char* const SECRET_SANTA_MOD_NAME    = "SecretSanta";
static const char* const CURATOR_MOD_NAME         = "Curator";

// ----- Config -----

struct SpawnAnyItemConfig
{
	std::string activation_button = DEFAULT_ACTIVATION_BUTTON;
	std::string example_item      = DEFAULT_EXAMPLE_ITEM;
	bool        dig_spot_mode     = DEFAULT_DIG_SPOT_MODE;
};

void to_json(json& j, const SpawnAnyItemConfig& c)
{
	j = json{
		{ ACTIVATION_BUTTON_KEY, c.activation_button },
		{ EXAMPLE_ITEM_KEY,      c.example_item      },
		{ DIG_SPOT_MODE_KEY,     c.dig_spot_mode     },
	};
}

void from_json(const json& j, SpawnAnyItemConfig& c)
{
	c.activation_button = MMAPI::Config::GetValue<std::string>(j, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON);
	c.example_item      = MMAPI::Config::GetValue<std::string>(j, EXAMPLE_ITEM_KEY,      DEFAULT_EXAMPLE_ITEM);
	c.dig_spot_mode     = MMAPI::Config::GetValue<bool>       (j, DIG_SPOT_MODE_KEY,     DEFAULT_DIG_SPOT_MODE);

	if (!MMAPI::Input::TryParseKeybind(c.activation_button))
	{
		MMAPI::Log::Error("Invalid \"%s\" value (%s) — not one of the supported keys", ACTIVATION_BUTTON_KEY, c.activation_button.c_str());
		c.activation_button = DEFAULT_ACTIVATION_BUTTON;
	}
}

// ----- State -----

static SpawnAnyItemConfig config{};
static bool startup_loaded         = false;
static bool processing_user_input  = false;
static bool override_next_dig_spot = false;  // Armed by dig_spot_mode spawn; consumed by AfterChooseRandomArtifact.
static bool duplicate_item_pending = false;  // Armed by drop-mode spawn; consumed by BeforeGiveItem to prompt for quantity.
static int  pending_item_id        = -1;     // The item the user requested (subject of both flags above).
static std::optional<MMAPI::Input::Keybind> activation_keybind;

static double ari_x = -1.0;
static double ari_y = -1.0;

// Internal name → item_id (built once at startup from Item::ForEachItem).
static std::map<std::string, int> item_name_to_id;
// Lowercase localized display name → internal name (built once after AfterGameActive).
static std::map<std::string, std::string> lowercase_localized_to_internal;

// Items that have been spawned but not "acquired" by the player yet. Tracked by internal name so
// repeated spawns of an unacquired item can be rejected (otherwise it'd be trivial to farm an
// unacquired item by spawning + picking up to acquire it, then spawning N more).
static std::vector<std::string> unacquired_items_spawned;

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
			config = SpawnAnyItemConfig{};
		}
		else
		{
			config = j.get<SpawnAnyItemConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = SpawnAnyItemConfig{};
	}
}

// Migrates pre-2.0 config (mod_data/DigUpAnything/DigUpAnything.json) to the post-rename path
// (mod_data/SpawnAnyItem/SpawnAnyItem.json). One-shot, idempotent — does nothing if the old path doesn't exist.
void MigrateLegacyConfigPath()
{
	try
	{
		auto mod_data = std::filesystem::current_path() / "mod_data";
		auto old_dir  = mod_data / "DigUpAnything";
		auto new_dir  = mod_data / "SpawnAnyItem";

		if (std::filesystem::exists(old_dir) && std::filesystem::is_directory(old_dir) && !std::filesystem::exists(new_dir))
			std::filesystem::rename(old_dir, new_dir);

		auto old_file = new_dir / "DigUpAnything.json";
		auto new_file = new_dir / "SpawnAnyItem.json";
		if (std::filesystem::exists(old_file) && std::filesystem::is_regular_file(old_file) && !std::filesystem::exists(new_file))
			std::filesystem::rename(old_file, new_file);
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Warn("Legacy config migration encountered an error (continuing): %s", e.what());
	}
}

// Discovers every item in __item_data and builds the internal-name ↔ item_id map. Runs once at
// title-screen setup. The item data is part of the game's static asset table, so it's stable for
// the rest of the session.
void BuildItemNameMap()
{
	item_name_to_id.clear();

	MMAPI::Item::ForEachItem([](int id) {
		YYTK::RValue name = MMAPI::Item::GetInternalName(id);
		if (name.m_Kind == YYTK::VALUE_STRING)
			item_name_to_id[name.ToString()] = id;
	});

	if (item_name_to_id.empty())
		MMAPI::Log::Error("Failed to load item data!");
	else
		MMAPI::Log::Info("Loaded data for %zu items!", item_name_to_id.size());
}

// Resolves each item's localized display name into the lookup map. Runs once after the Localizer
// is available so we can match user-typed display names later.
void BuildLocalizedNameLookup()
{
	lowercase_localized_to_internal.clear();

	for (const auto& [internal_name, id] : item_name_to_id)
	{
		YYTK::RValue localized = MMAPI::Item::GetLocalizedName(id);
		if (localized.m_Kind != YYTK::VALUE_STRING) continue;

		std::string lower = localized.ToString();
		std::transform(lower.begin(), lower.end(), lower.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		lowercase_localized_to_internal[lower] = internal_name;
	}

	// "missing" is the placeholder name the game uses for items lacking a translation — strip it so
	// the user can't accidentally summon a "missing" item by typing the literal placeholder.
	lowercase_localized_to_internal.erase("missing");
}

// Initializes the __YYTK.SpawnAnyItem cross-mod sub-struct (version field only). Other mods can
// detect SpawnAnyItem's presence by checking for this sub-struct.
void EnsureCrossModStructExists()
{
	YYTK::RValue __YYTK;
	if (!MMAPI::Internal::module_interface->CallBuiltin("variable_global_exists", { YYTK_KEY }).ToBoolean())
	{
		MMAPI::Internal::module_interface->GetRunnerInterface().StructCreate(&__YYTK);
		MMAPI::Engine::GlobalVariableSet(YYTK_KEY, __YYTK);
	}
	else
	{
		__YYTK = MMAPI::Engine::GlobalVariableGet(YYTK_KEY);
	}

	if (!MMAPI::Engine::StructVariableExists(__YYTK, MOD_NAME))
	{
		YYTK::RValue mod_struct;
		YYTK::RValue version_rv = VERSION;
		MMAPI::Internal::module_interface->GetRunnerInterface().StructCreate(&mod_struct);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&mod_struct, "version", &version_rv);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&__YYTK, MOD_NAME, &mod_struct);
	}
}

// Signals to other mods (SecretSanta, Curator) that the next dig-spot artifact roll is being taken
// over by SpawnAnyItem so they should skip their substitution this fire. No-op if either mod isn't
// loaded (their sub-struct doesn't exist).
void SignalIgnoreNextDigSpot()
{
	if (!MMAPI::Internal::module_interface->CallBuiltin("variable_global_exists", { YYTK_KEY }).ToBoolean())
		return;

	YYTK::RValue __YYTK = MMAPI::Engine::GlobalVariableGet(YYTK_KEY);
	for (const char* mod_name : { SECRET_SANTA_MOD_NAME, CURATOR_MOD_NAME })
	{
		if (!MMAPI::Engine::StructVariableExists(__YYTK, mod_name)) continue;
		YYTK::RValue mod_struct = MMAPI::Engine::StructVariableGet(__YYTK, mod_name);
		if (!MMAPI::Engine::StructVariableExists(mod_struct, IGNORE_NEXT_DIG_SPOT_KEY)) continue;
		MMAPI::Engine::StructVariableSet(mod_struct, IGNORE_NEXT_DIG_SPOT_KEY, true);
	}
}

std::string TrimWhitespace(std::string s)
{
	s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
		[](unsigned char c) { return std::isspace(c); }));
	s.erase(std::find_if_not(s.rbegin(), s.rend(),
		[](unsigned char c) { return std::isspace(c); }).base(), s.end());
	return s;
}

// Returns the internal name of an item given either an internal name or localized display name (any
// case). Empty if the input doesn't resolve to any known item.
std::string ResolveItemInternalName(std::string user_input)
{
	user_input = TrimWhitespace(user_input);
	std::transform(user_input.begin(), user_input.end(), user_input.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	if (auto it = lowercase_localized_to_internal.find(user_input); it != lowercase_localized_to_internal.end())
		return it->second;

	if (item_name_to_id.contains(user_input))
		return user_input;

	return {};
}

// Drops `item_id` at Ari's current position, or arms dig_spot_mode for the next artifact roll —
// whichever is configured. Handles all the user-facing notifications and the unacquired-spawn dedupe.
void SpawnItem(int item_id, const std::string& internal_name)
{
	pending_item_id = item_id;

	if (config.dig_spot_mode)
	{
		override_next_dig_spot = true;
		SignalIgnoreNextDigSpot();
		MMAPI::Game::CreateNotification(false, VALID_ITEM_LOCALIZATION_KEY);
		MMAPI::Log::Info("Next artifact spot will be: %s", internal_name.c_str());
	}
	else
	{
		duplicate_item_pending = true;
		MMAPI::Item::Drop(item_id, ari_x, ari_y);
		MMAPI::Log::Info("Spawning item: %s", internal_name.c_str());
	}
}

// Re-checks the player's `__ari.items_acquired` array — if the item has been acquired since being
// spawned, remove it from the unacquired-spawned list so the player can spawn more of it.
void RefreshUnacquiredSpawnedList(int item_id)
{
	if (!MMAPI::Item::HasBeenAcquired(item_id)) return;

	YYTK::RValue name_rv = MMAPI::Item::GetInternalName(item_id);
	if (name_rv.m_Kind != YYTK::VALUE_STRING) return;

	std::string name = name_rv.ToString();
	unacquired_items_spawned.erase(
		std::remove(unacquired_items_spawned.begin(), unacquired_items_spawned.end(), name),
		unacquired_items_spawned.end());
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	// Per-session resets (run on every return-to-title).
	processing_user_input  = false;
	override_next_dig_spot = false;
	duplicate_item_pending = false;
	pending_item_id        = -1;
	ari_x                  = -1.0;
	ari_y                  = -1.0;
	unacquired_items_spawned.clear();

	if (startup_loaded) return;

	MigrateLegacyConfigPath();
	LoadOrCreateConfigFile();
	activation_keybind = MMAPI::Input::TryParseKeybind(config.activation_button);
	EnsureCrossModStructExists();
	BuildItemNameMap();

	startup_loaded = true;
}

void OnAfterGameActive()
{
	// First-fire-per-session: build localized-name lookup now that the Localizer is reliably available.
	if (lowercase_localized_to_internal.empty())
		BuildLocalizedNameLookup();
}

void OnAriTick(CInstance* self)
{
	if (!self) return;
	ari_x = MMAPI::Engine::InstanceVariableGet(self, "x").ToDouble();
	ari_y = MMAPI::Engine::InstanceVariableGet(self, "y").ToDouble();
}

void OnBeforeGiveItem(MMAPI::Item::GiveItemContext& ctx)
{
	if (!duplicate_item_pending) return;
	if (pending_item_id < 0) return;

	// Only intercept the give_item that's being driven by *our* SpawnItem call. The mod arms the
	// flag, drops the item, the live drop yields a give_item with our item_id — that's the one we
	// want to prompt for quantity on. Other give_item calls (gifts, harvest, etc.) shouldn't trigger
	// the prompt.
	int unidentified_artifact_id = -1;
	if (auto it = item_name_to_id.find("unidentified_artifact"); it != item_name_to_id.end())
		unidentified_artifact_id = it->second;
	if (!ctx.IsItem(pending_item_id) && !(unidentified_artifact_id >= 0 && ctx.IsItem(unidentified_artifact_id)))
		return;

	duplicate_item_pending = false;

	if (!MMAPI::Item::HasBeenAcquired(pending_item_id))
	{
		// First time spawning this item — track it so the player can't spawn N more before acquiring it.
		YYTK::RValue name_rv = MMAPI::Item::GetInternalName(pending_item_id);
		if (name_rv.m_Kind == YYTK::VALUE_STRING)
			unacquired_items_spawned.push_back(name_rv.ToString());

		MMAPI::Game::CreateNotification(false, ITEM_NOT_ACQUIRED_LOCALIZATION_KEY);
		MMAPI::Log::Warn("The requested item hasn't been acquired before. Only 1 may be spawned.");
		return;
	}

	YYTK::RValue max_stack_rv = MMAPI::Item::GetMaxStack(pending_item_id);
	int max_stack = MMAPI::Engine::IsNumeric(max_stack_rv) ? static_cast<int>(max_stack_rv.ToInt64()) : 0;
	if (max_stack == 1)
	{
		MMAPI::Game::CreateNotification(false, ITEM_MAX_STACK_IS_ONE_LOCALIZATION_KEY);
		MMAPI::Log::Warn("The requested item has a max stack limit of one. Only 1 will be spawned.");
		return;
	}

	std::string modal_text =
		"SpawnAnyItem v" + std::string(VERSION) + "\r\n"
		"------------------------------\r\n"
		"How many of the item would you like?\r\n"
		"Input a number between 1 and 999.\r\n";

	YYTK::RValue user_input = MMAPI::Internal::module_interface->CallBuiltin(
		"get_integer", { YYTK::RValue(modal_text), 1 }
	);

	int value = static_cast<int>(user_input.ToDouble());
	if (value < 1)   value = 1;
	if (value > 999) value = 999;
	ctx.SetQuantity(value);
	MMAPI::Log::Info("Spawning %d of the requested item.", value);
}

void OnAfterChooseRandomArtifact(MMAPI::Archaeology::AfterChooseRandomArtifactContext& ctx)
{
	if (!override_next_dig_spot) return;
	if (pending_item_id < 0) return;

	override_next_dig_spot = false;
	duplicate_item_pending = true;  // The dig-spot give_item will still flow through BeforeGiveItem.
	ctx.SetItemId(pending_item_id);
	MMAPI::Log::Info("Modified the artifact spot!");
}

void OnAfterDrawGui()
{
	if (!MMAPI::Game::WindowHasFocus() || processing_user_input)
		return;

	if (!activation_keybind || !MMAPI::Input::IsKeybindPressed(*activation_keybind))
		return;

	processing_user_input = true;

	std::string modal_text =
		"SpawnAnyItem v" + std::string(VERSION) + "\r\n"
		"------------------------------\r\n"
		"Input the desired item's Display Name or Internal Name.\r\n"
		"Case (capitalization) does not matter.\r\n"
		"Example 1 (Display Name): Tea with Lemon\r\n"
		"Example 2 (Display Name): tea with lemon\r\n"
		"Example 3 (Internal Name): cup_of_tea\r\n"
		"More Info: https://github.com/AnnaNomoly/YYToolkit/tree/stable/DigUpAnything";

	YYTK::RValue user_input = MMAPI::Internal::module_interface->CallBuiltin(
		"get_string",
		{ YYTK::RValue(modal_text), YYTK::RValue(config.example_item) }
	);

	std::string internal_name = ResolveItemInternalName(user_input.ToString());

	if (internal_name.empty())
	{
		MMAPI::Game::CreateNotification(false, UNRECOGNIZED_ITEM_LOCALIZATION_KEY);
		MMAPI::Log::Warn("Ignoring unrecognized item: %s", TrimWhitespace(user_input.ToString()).c_str());
		processing_user_input = false;
		return;
	}

	int item_id = item_name_to_id.at(internal_name);
	RefreshUnacquiredSpawnedList(item_id);

	bool already_spawned_unacquired = std::find(
		unacquired_items_spawned.begin(), unacquired_items_spawned.end(), internal_name)
		!= unacquired_items_spawned.end();

	if (already_spawned_unacquired)
	{
		MMAPI::Game::CreateNotification(false, UNACQUIRED_ITEM_ALREADY_SPAWNED_LOCALIZATION_KEY);
		MMAPI::Log::Warn("Ignoring duplicate unacquired item: %s", internal_name.c_str());
	}
	else if (DISABLED_ITEMS.contains(internal_name))
	{
		MMAPI::Game::CreateNotification(false, DISABLED_ITEM_LOCALIZATION_KEY);
		MMAPI::Log::Warn("Ignoring disabled item: %s", internal_name.c_str());
	}
	else
	{
		SpawnItem(item_id, internal_name);
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

	MMAPI::Archaeology::Enable();
	MMAPI::Game::Enable();
	MMAPI::Input::Enable();
	MMAPI::Item::Enable();
	MMAPI::Text::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnAfterGameActive);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Item::Hooks::BeforeGiveItem(OnBeforeGiveItem);
	MMAPI::Archaeology::Hooks::AfterChooseRandomArtifact(OnAfterChooseRandomArtifact);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, OnAriTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
