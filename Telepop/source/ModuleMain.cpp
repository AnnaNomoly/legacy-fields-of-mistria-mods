#include <map>
#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "Telepop";
static const char* const VERSION = "2.3.0";

// Config keys
static const char* const SUGAR_QUANTITY_KEY = "telepop_recipe_sugar_ingredient_amount";
static const char* const COOKING_TIME_KEY   = "telepop_recipe_cooking_time_in_minutes";
static const char* const SHIPPING_BIN_KEY   = "telepop_shipping_bin_price";
static const char* const STORE_PRICE_KEY    = "telepop_store_price";

// Per-save mod data keys
static const char* const MYSTERY_LOCATION_KEY = "telepop_mystery_location";
static const char* const MYSTERY_X_KEY        = "telepop_mystery_x_coordinate";
static const char* const MYSTERY_Y_KEY        = "telepop_mystery_y_coordinate";

// Defaults
static const int DEFAULT_SUGAR_QUANTITY = 1;
static const int DEFAULT_COOKING_TIME   = 10;
static const int DEFAULT_SHIPPING_BIN   = 115;
static const int DEFAULT_STORE_PRICE    = 115;

static const std::string PLACEHOLDER_TEXT = "<PLACEHOLDER>";

// Notification localization keys
static const std::string ECHO_MINT_REMINDER_KEY  = "Notifications/Mods/Telepop/echo_mint/location_reminder";
static const std::string ECHO_MINT_SAVED_KEY     = "Notifications/Mods/Telepop/echo_mint/location_saved";
static const std::string ECHO_MINT_INVALID_KEY   = "Notifications/Mods/Telepop/echo_mint/location_invalid";
static const std::string MYSTERY_NO_LOCATION_KEY = "Notifications/Mods/Telepop/telepop_mystery/no_location_set";

// Custom item internal names
static const std::string TELEPOP_PURPLE  = "telepop_purple";   // Mines Entry
static const std::string TELEPOP_BLUE    = "telepop_blue";     // Beach
static const std::string TELEPOP_ORANGE  = "telepop_orange";   // Town
static const std::string TELEPOP_PINK    = "telepop_pink";     // Farm
static const std::string TELEPOP_GREEN   = "telepop_green";    // Eastern Road
static const std::string TELEPOP_YELLOW  = "telepop_yellow";   // Western Ruins
static const std::string TELEPOP_MYSTERY = "telepop_mystery";  // Player-chosen location
static const std::string ECHO_MINT       = "echo_mint";        // Records the player-chosen location

static const std::vector<std::string> CUSTOM_ITEM_NAMES = {
	TELEPOP_PURPLE, TELEPOP_BLUE, TELEPOP_ORANGE, TELEPOP_PINK,
	TELEPOP_GREEN, TELEPOP_YELLOW, TELEPOP_MYSTERY, ECHO_MINT,
};

struct TelepopTarget
{
	MMAPI::Location::Ids location;
	int x;
	int y;
};

// Directional Telepop item → fixed teleport destination.
static const std::map<std::string, TelepopTarget> TELEPOP_TARGETS = {
	{ TELEPOP_PURPLE, { MMAPI::Location::Ids::MinesEntry,   216,  198  } },
	{ TELEPOP_BLUE,   { MMAPI::Location::Ids::Beach,        1722, 505  } },
	{ TELEPOP_ORANGE, { MMAPI::Location::Ids::Town,         1097, 1323 } },
	{ TELEPOP_PINK,   { MMAPI::Location::Ids::Farm,         809,  306  } },
	{ TELEPOP_GREEN,  { MMAPI::Location::Ids::EasternRoad,  1128, 936  } },
	{ TELEPOP_YELLOW, { MMAPI::Location::Ids::WesternRuins, 1416, 808  } },
};

struct TelepopConfig
{
	int sugar_quantity = DEFAULT_SUGAR_QUANTITY;
	int cooking_time   = DEFAULT_COOKING_TIME;
	int shipping_bin   = DEFAULT_SHIPPING_BIN;
	int store_price    = DEFAULT_STORE_PRICE;
};

void to_json(json& j, const TelepopConfig& c)
{
	j = json{
		{ SUGAR_QUANTITY_KEY, c.sugar_quantity },
		{ COOKING_TIME_KEY,   c.cooking_time   },
		{ SHIPPING_BIN_KEY,   c.shipping_bin   },
		{ STORE_PRICE_KEY,    c.store_price    },
	};
}

void from_json(const json& j, TelepopConfig& c)
{
	c.sugar_quantity = MMAPI::Config::GetValue<int>(j, SUGAR_QUANTITY_KEY, DEFAULT_SUGAR_QUANTITY, 1, 5);
	c.cooking_time   = MMAPI::Config::GetValue<int>(j, COOKING_TIME_KEY,   DEFAULT_COOKING_TIME,   0, 60);
	c.shipping_bin   = MMAPI::Config::GetValue<int>(j, SHIPPING_BIN_KEY,   DEFAULT_SHIPPING_BIN,   1, 10000);
	c.store_price    = MMAPI::Config::GetValue<int>(j, STORE_PRICE_KEY,    DEFAULT_STORE_PRICE,    1, 10000);

	// Cooking time must be a multiple of 10 (or 0).
	if (c.cooking_time % 10 != 0)
		c.cooking_time = DEFAULT_COOKING_TIME;
}

// Per-save mod state.
static TelepopConfig config = {};
static std::string save_prefix;
static std::string mystery_location_internal_name;
static int mystery_x = -1;
static int mystery_y = -1;
static bool game_is_active = false;
static bool notify_on_load = true;

bool HasMysteryLocation()
{
	return !mystery_location_internal_name.empty() && mystery_x != -1 && mystery_y != -1;
}

int ItemIdFor(const std::string& internal_name)
{
	RValue id_rv = MMAPI::Item::GetIdFromInternalName(internal_name);
	if (id_rv.m_Kind == VALUE_UNDEFINED)
		return -1;
	return static_cast<int>(id_rv.ToInt64());
}

bool TryGetTelepopTarget(int item_id, TelepopTarget& out_target)
{
	for (const auto& [name, target] : TELEPOP_TARGETS)
	{
		if (ItemIdFor(name) == item_id)
		{
			out_target = target;
			return true;
		}
	}
	return false;
}

bool IsEchoMintProhibited(MMAPI::Location::Ids loc)
{
	switch (loc)
	{
		case MMAPI::Location::Ids::Dungeon:
		case MMAPI::Location::Ids::WaterSeal:
		case MMAPI::Location::Ids::EarthSeal:
		case MMAPI::Location::Ids::FireSeal:
		case MMAPI::Location::Ids::RuinsSeal:
		case MMAPI::Location::Ids::SmallBarn:
		case MMAPI::Location::Ids::MediumBarn:
		case MMAPI::Location::Ids::LargeBarn:
		case MMAPI::Location::Ids::SmallCoop:
		case MMAPI::Location::Ids::MediumCoop:
		case MMAPI::Location::Ids::LargeCoop:
		case MMAPI::Location::Ids::SmallGreenhouse:
		case MMAPI::Location::Ids::LargeGreenhouse:
			return true;
		default:
			return false;
	}
}

void WriteModSaveFile()
{
	MMAPI::ModSave::Write(save_prefix, json{
		{ MYSTERY_LOCATION_KEY, mystery_location_internal_name },
		{ MYSTERY_X_KEY,        mystery_x },
		{ MYSTERY_Y_KEY,        mystery_y },
	});
}

void ReadModSaveFile()
{
	json data = MMAPI::ModSave::Read(save_prefix);
	if (data.empty())
		return;
	mystery_location_internal_name = data.value(MYSTERY_LOCATION_KEY, std::string());
	mystery_x = data.value(MYSTERY_X_KEY, -1);
	mystery_y = data.value(MYSTERY_Y_KEY, -1);
}

void ModifyCustomItems()
{
	for (const std::string& name : CUSTOM_ITEM_NAMES)
	{
		int item_id = ItemIdFor(name);
		if (item_id < 0)
		{
			MMAPI::Log::Error("Failed to find custom item: %s", name.c_str());
			continue;
		}

		MMAPI::Recipe::SetComponentCount(item_id, 0, config.sugar_quantity);
		MMAPI::Recipe::SetComponentDuration(item_id, 1, config.cooking_time);
		MMAPI::Item::SetBinValue(item_id, config.shipping_bin);
		MMAPI::Item::SetStoreValue(item_id, config.store_price);
	}
}

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
				MMAPI::Log::Error("No readable values found in mod configuration file: %s!", path.string().c_str());
			config = TelepopConfig{};
		}
		else
		{
			config = j.get<TelepopConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = TelepopConfig{};
	}
}

void OnSetupMainScreen()
{
	game_is_active = false;
	notify_on_load = true;
	save_prefix.clear();
	mystery_location_internal_name.clear();
	mystery_x = -1;
	mystery_y = -1;
}

void OnGameActive()
{
	ModifyCustomItems();
	game_is_active = true;
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	if (save_prefix.empty())
		save_prefix = ctx.GetSavePrefix();

	if (HasMysteryLocation())
		WriteModSaveFile();
}

void OnAfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	save_prefix = ctx.GetSavePrefix();
	ReadModSaveFile();
}

void OnAfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& ctx)
{
	if (ctx.GetKey() != ECHO_MINT_REMINDER_KEY)
		return;

	std::string resolved(ctx.GetResolved());
	std::size_t pos = resolved.find(PLACEHOLDER_TEXT);
	if (pos != std::string::npos)
	{
		resolved.replace(pos, PLACEHOLDER_TEXT.length(), mystery_location_internal_name);
		ctx.SetResolved(std::move(resolved));
	}
}

void OnBeforeUseItem(MMAPI::Item::UseItemContext& ctx)
{
	if (!ctx.IsAriUse())
		return;

	int item_id = ctx.GetItemId();

	if (item_id == ItemIdFor(ECHO_MINT))
	{
		MMAPI::Location::Ids current;
		if (!MMAPI::Location::TryGetCurrentLocation(current) || IsEchoMintProhibited(current))
		{
			MMAPI::Log::Warn("Echo Mint cannot be used at this location.");
			MMAPI::Game::CreateNotification(false, ECHO_MINT_INVALID_KEY);
			ctx.Cancel();
		}
	}
	else if (item_id == ItemIdFor(TELEPOP_MYSTERY))
	{
		if (!HasMysteryLocation())
		{
			MMAPI::Log::Warn("Telepop (Mystery) has no recorded location — use an Echo Mint first.");
			MMAPI::Game::CreateNotification(false, MYSTERY_NO_LOCATION_KEY);
			ctx.Cancel();
		}
	}
}

void OnAfterUseActionComplete(MMAPI::Player::AfterUseActionContext& ctx)
{
	int item_id = ctx.GetItemId();

	if (item_id == ItemIdFor(ECHO_MINT))
	{
		MMAPI::Location::Ids current;
		if (!MMAPI::Location::TryGetCurrentLocation(current))
			return;

		auto pos = MMAPI::Player::GetPosition();
		if (!pos)
			return;

		mystery_location_internal_name = MMAPI::Location::LocationIdToString(current);
		mystery_x = static_cast<int>(pos->x);
		mystery_y = static_cast<int>(pos->y);

		MMAPI::Game::CreateNotification(false, ECHO_MINT_SAVED_KEY);
		MMAPI::Log::Info("Echo recorded at %s (%d, %d)",
			mystery_location_internal_name.c_str(), mystery_x, mystery_y);
		return;
	}

	if (item_id == ItemIdFor(TELEPOP_MYSTERY))
	{
		auto loc = MMAPI::Location::TryFromInternalName(mystery_location_internal_name);
		if (!loc)
		{
			MMAPI::Log::Warn("Failed to resolve recorded Echo location: %s",
				mystery_location_internal_name.c_str());
			return;
		}
		MMAPI::Location::TeleportAri(*loc, mystery_x, mystery_y);
		MMAPI::Log::Info("Teleported to Echo (%s)", mystery_location_internal_name.c_str());
		return;
	}

	TelepopTarget target;
	if (TryGetTelepopTarget(item_id, target))
	{
		MMAPI::Location::TeleportAri(target.location, target.x, target.y);
		MMAPI::Log::Info("Teleported via directional Telepop");
	}
}

void HandleAri(CInstance* /*self*/)
{
	if (notify_on_load && game_is_active)
	{
		notify_on_load = false;
		if (HasMysteryLocation())
			MMAPI::Game::CreateNotification(false, ECHO_MINT_REMINDER_KEY);
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)(module_interface));
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Item::Enable();
	MMAPI::Location::Enable();
	MMAPI::Player::Enable();
	MMAPI::Recipe::Enable();
	MMAPI::Text::Enable();
	MMAPI::Game::Enable();

	LoadOrCreateConfigFile();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Game::Hooks::AfterLoadGame(OnAfterLoadGame);
	MMAPI::Text::Hooks::AfterLocalizedString(OnAfterLocalizedString);
	MMAPI::Item::Hooks::BeforeUseItem(OnBeforeUseItem);
	MMAPI::Player::Hooks::AfterUseActionComplete(OnAfterUseActionComplete);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, HandleAri);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
