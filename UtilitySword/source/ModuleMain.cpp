#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "UtilitySword";
static const char* const VERSION  = "1.2.0";

// ----- Config -----

static const char* const CONFIG_KEY_SWORD_CAN_PICK         = "sword_can_pick";
static const char* const CONFIG_KEY_SWORD_CAN_CHOP         = "sword_can_chop";
static const char* const CONFIG_KEY_ENABLED_ON_FARM        = "enabled_on_farm";
static const char* const CONFIG_KEY_ENABLED_IN_FARM_HOUSE  = "enabled_in_farm_house";
static const char* const CONFIG_KEY_ENABLED_IN_MISTRIA     = "enabled_in_mistria";
static const char* const CONFIG_KEY_ENABLED_IN_MINES       = "enabled_in_mines";

static const bool DEFAULT_SWORD_CAN_PICK        = true;
static const bool DEFAULT_SWORD_CAN_CHOP        = true;
static const bool DEFAULT_ENABLED_ON_FARM       = false;
static const bool DEFAULT_ENABLED_IN_FARM_HOUSE = false;
static const bool DEFAULT_ENABLED_IN_MISTRIA    = true;
static const bool DEFAULT_ENABLED_IN_MINES      = true;

static bool sword_can_pick        = DEFAULT_SWORD_CAN_PICK;
static bool sword_can_chop        = DEFAULT_SWORD_CAN_CHOP;
static bool enabled_on_farm       = DEFAULT_ENABLED_ON_FARM;
static bool enabled_in_farm_house = DEFAULT_ENABLED_IN_FARM_HOUSE;
static bool enabled_in_mistria    = DEFAULT_ENABLED_IN_MISTRIA;
static bool enabled_in_mines      = DEFAULT_ENABLED_IN_MINES;

// ----- State -----

static bool startup_loaded = false;
// Most recent non-undefined held item_id observed via the held_item hook — matches the original
// mod's "last weapon stays current after a brief undefined window" behavior. Damage and artifact
// hooks check this to decide whether Ari is wielding a sword.
static int  held_item_id   = -1;

// ----- Config loading -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	sword_can_pick        = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_SWORD_CAN_PICK,        DEFAULT_SWORD_CAN_PICK);
	sword_can_chop        = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_SWORD_CAN_CHOP,        DEFAULT_SWORD_CAN_CHOP);
	enabled_on_farm       = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_ENABLED_ON_FARM,       DEFAULT_ENABLED_ON_FARM);
	enabled_in_farm_house = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_ENABLED_IN_FARM_HOUSE, DEFAULT_ENABLED_IN_FARM_HOUSE);
	enabled_in_mistria    = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_ENABLED_IN_MISTRIA,    DEFAULT_ENABLED_IN_MISTRIA);
	enabled_in_mines      = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_ENABLED_IN_MINES,      DEFAULT_ENABLED_IN_MINES);

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_SWORD_CAN_PICK]        = sword_can_pick;
	roundtrip[CONFIG_KEY_SWORD_CAN_CHOP]        = sword_can_chop;
	roundtrip[CONFIG_KEY_ENABLED_ON_FARM]       = enabled_on_farm;
	roundtrip[CONFIG_KEY_ENABLED_IN_FARM_HOUSE] = enabled_in_farm_house;
	roundtrip[CONFIG_KEY_ENABLED_IN_MISTRIA]    = enabled_in_mistria;
	roundtrip[CONFIG_KEY_ENABLED_IN_MINES]      = enabled_in_mines;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config loaded: pick=%s, chop=%s, farm=%s, farmhouse=%s, mistria=%s, mines=%s",
		sword_can_pick ? "true" : "false",
		sword_can_chop ? "true" : "false",
		enabled_on_farm ? "true" : "false",
		enabled_in_farm_house ? "true" : "false",
		enabled_in_mistria ? "true" : "false",
		enabled_in_mines ? "true" : "false");
}

// ----- Location predicates -----

// Room-name match preserved from the original — mixing Location::Ids with substring matches
// would obscure the "rm_farmhouse" / "rm_mines" patterns that drive the original's behavior.

static bool AriIsOnFarm()
{
	return MMAPI::Game::IsCurrentRoom("rm_farm");
}

static bool AriIsInFarmHouse()
{
	return MMAPI::Game::CurrentRoomNameContains("rm_farmhouse");
}

static bool AriIsInMistria()
{
	std::string room = MMAPI::Game::GetCurrentRoomName();
	return room == "rm_narrows"
	    || room == "rm_beach"
	    || room == "rm_deep_woods"
	    || room == "rm_western_ruins"
	    || room == "rm_summit"
	    || room == "rm_eastern_road"
	    || room == "rm_haydens_farm"
	    || room == "rm_town";
}

static bool AriIsInMines()
{
	return MMAPI::Dungeon::IsCurrentRoomDungeonFloor();
}

static bool LocationIsEnabled()
{
	if (AriIsOnFarm()      && enabled_on_farm)       return true;
	if (AriIsInFarmHouse() && enabled_in_farm_house) return true;
	if (AriIsInMistria()   && enabled_in_mistria)    return true;
	if (AriIsInMines()     && enabled_in_mines)      return true;
	return false;
}

// ----- Hooks -----

void OnAfterHeldItem(MMAPI::Player::HeldItemContext& ctx)
{
	// Only update on defined held items — preserves the original behavior where briefly
	// undefined results (e.g. mid-consume) don't clobber the last known weapon.
	int id = ctx.GetItemId();
	if (id >= 0)
		held_item_id = id;
}

void OnBeforeDamage(MMAPI::Damage::BeforeDamageContext& ctx)
{
	if (MMAPI::Game::IsPaused()) return;
	if (held_item_id < 0) return;
	if (!MMAPI::Item::HasTag(held_item_id, "weapon")) return;
	if (ctx.IsTargetAri()) return;  // only enable utility behavior when hitting world objects
	if (!LocationIsEnabled()) return;

	ctx.SetCanPickGridObjects(sword_can_pick);
	ctx.SetCanChopGridObjects(sword_can_chop);
}

void OnAfterChooseRandomArtifact(MMAPI::Archaeology::AfterChooseRandomArtifactContext& /*ctx*/)
{
	if (held_item_id < 0) return;
	if (!MMAPI::Item::HasTag(held_item_id, "weapon")) return;

	// Match the game's default break-dig-spot XP, but pulled live each fire so any mod or game
	// patch that adjusts __xp_values.break_dig_spot is automatically reflected.
	YYTK::RValue xp_rv = MMAPI::Game::GetXpValue(MMAPI::Game::XpValues::BreakDigSpot);
	double xp = MMAPI::Engine::IsNumeric(xp_rv) ? xp_rv.ToDouble() : 3.0;

	MMAPI::Skill::GainExperience(MMAPI::Skill::Ids::Archaeology, xp);
}

void OnBeforeSetupMainScreen()
{
	if (!startup_loaded)
	{
		LoadConfig();
		startup_loaded = true;
	}
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
	MMAPI::Damage::Enable();
	MMAPI::Archaeology::Enable();
	MMAPI::Skill::Enable();
	MMAPI::Item::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Player::Hooks::AfterHeldItem(OnAfterHeldItem);
	MMAPI::Damage::Hooks::BeforeDamage(OnBeforeDamage);
	MMAPI::Archaeology::Hooks::AfterChooseRandomArtifact(OnAfterChooseRandomArtifact);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
