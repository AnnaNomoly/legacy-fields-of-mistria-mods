#include <cmath>
#include <map>
#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;
namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "EnemyHealthBars";
static const char* const VERSION  = "1.2.0";

// ----- Config keys -----

static const char* const KEY_SMALLER_HEALTH_BAR = "smaller_health_bar";
static const char* const KEY_RED_HEALTH_BAR     = "red_health_bar";
static const char* const KEY_VERTICAL_OFFSETS   = "monster_health_bar_vertical_offsets";

// ----- Per-monster cached state field names -----

static const char* const FIELD_CAPTURED_MAX_HEALTH      = "__enemy_health_bars_max_health";
static const char* const FIELD_DEEP_DUNGEON_MAX_HEALTH  = "__deep_dungeon__default_hit_points";

// ----- Bar geometry & colors -----

static constexpr int HEALTH_BAR_WIDTH        = 40;
static constexpr int HEALTH_BAR_HEIGHT       = 9;
static constexpr int SMALL_HEALTH_BAR_WIDTH  = 30;
static constexpr int SMALL_HEALTH_BAR_HEIGHT = 9;

// GM color format is 0xBBGGRR — so 0x00FF00 is green, 0x0000FF is red, 0 is black.
static constexpr int COLOR_BLACK = 0;
static constexpr int COLOR_GREEN = 65280;
static constexpr int COLOR_RED   = 255;

// ----- Default per-monster vertical offsets -----
// Computed from each monster's "idle south" sprite height multiplied by ~0.5-0.7, placing the bar
// above the monster's head. Keyed by internal monster name from globalInstance.__monster_id__.
static const std::map<std::string, int> DEFAULT_VERTICAL_OFFSETS = {
	{ "bat",                 26 }, // 40 * 0.65, spr_monster_essence_bat_main_idle_south
	{ "bat_blue",            26 }, // 40 * 0.65
	{ "cat",                 24 }, // 48 * 0.50
	{ "cat_void",            24 }, // 48 * 0.50
	{ "copperclod",          22 }, // 40 * 0.55
	{ "enchantern",          35 }, // 64 * 0.55
	{ "enchantern_blue",     35 }, // 64 * 0.55
	{ "goldclod",            20 }, // 40 * 0.50
	{ "griffin_statue",      52 }, // 80 * 0.65
	{ "ironclod",            22 }, // 40 * 0.55
	{ "mimic",               20 }, // 40 * 0.50
	{ "mistrilclod",         22 }, // 40 * 0.55
	{ "mushroom",            22 }, // 40 * 0.55
	{ "mushroom_blue",       22 },
	{ "mushroom_green",      22 },
	{ "mushroom_purple",     22 },
	{ "rock_stack",          22 },
	{ "rockclod",            22 },
	{ "rockclod_blue",       22 },
	{ "rockclod_green",      22 },
	{ "rockclod_purple",     22 },
	{ "rockclod_red",        22 },
	{ "sapling",             22 },
	{ "sapling_blue",        22 },
	{ "sapling_cool",        22 },
	{ "sapling_orange",      22 },
	{ "sapling_orange_mini", 20 }, // 40 * 0.50
	{ "sapling_pink",        22 },
	{ "sapling_purple",      22 },
	{ "silverclod",          22 },
	{ "spirit",              28 }, // 40 * 0.70
	{ "spirit_purple",       28 },
	{ "stalagmite",          20 }, // 40 * 0.50
	{ "stalagmite_green",    20 },
	{ "stalagmite_purple",   20 },
	{ "tome",                38 }, // 64 * 0.60
};

// ----- Config -----

struct Config
{
	bool smaller_health_bar = false;
	bool red_health_bar     = false;
};

static bool                                   startup_loaded = false;
static Config                                 config;
static std::map<MMAPI::Monster::Ids, int>     vertical_offsets_by_id;

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	json doc = MMAPI::Config::Load(config_path);

	config.smaller_health_bar = MMAPI::Config::GetValue<bool>(doc, KEY_SMALLER_HEALTH_BAR, false);
	config.red_health_bar     = MMAPI::Config::GetValue<bool>(doc, KEY_RED_HEALTH_BAR,     false);

	std::map<std::string, int> offsets_by_name = DEFAULT_VERTICAL_OFFSETS;
	if (doc.contains(KEY_VERTICAL_OFFSETS) && doc[KEY_VERTICAL_OFFSETS].is_object())
		offsets_by_name = doc[KEY_VERTICAL_OFFSETS].get<std::map<std::string, int>>();

	vertical_offsets_by_id.clear();
	for (const auto& [internal_name, offset] : offsets_by_name)
	{
		auto monster = MMAPI::Monster::TryFromInternalName(internal_name);
		if (!monster)
		{
			MMAPI::Log::Warn("Unknown monster \"%s\" in %s; skipped.", internal_name.c_str(), KEY_VERTICAL_OFFSETS);
			continue;
		}
		vertical_offsets_by_id[*monster] = offset;
	}

	json roundtrip;
	roundtrip[KEY_SMALLER_HEALTH_BAR] = config.smaller_health_bar;
	roundtrip[KEY_RED_HEALTH_BAR]     = config.red_health_bar;
	roundtrip[KEY_VERTICAL_OFFSETS]   = offsets_by_name;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Tracking %zu monster types.", vertical_offsets_by_id.size());
}

// ----- Draw -----

void OnAfterDrawMonster(YYTK::CInstance* monster)
{
	if (!monster) return;

	YYTK::RValue monster_rv = monster->ToRValue();
	if (!MMAPI::Engine::StructVariableExists(monster_rv, "monster_id")) return;
	if (!MMAPI::Engine::StructVariableExists(monster_rv, "hit_points")) return;

	YYTK::RValue monster_id_rv = monster_rv.GetMember("monster_id");
	if (!MMAPI::Engine::IsNumeric(monster_id_rv)) return;

	auto monster_enum = static_cast<MMAPI::Monster::Ids>(monster_id_rv.ToInt64());
	auto offset_it = vertical_offsets_by_id.find(monster_enum);
	if (offset_it == vertical_offsets_by_id.end()) return;

	YYTK::RValue hit_points_rv = monster_rv.GetMember("hit_points");
	if (!MMAPI::Engine::IsNumeric(hit_points_rv)) return;
	double current_hp = hit_points_rv.ToDouble();
	if (!std::isfinite(current_hp)) return;

	// Resolve max HP. The DeepDungeon mod stamps its own field on monsters in its dungeon — prefer
	// that when present so HP bars in DeepDungeon runs scale correctly. Otherwise capture the first
	// observed HP as the max (the monster is visible on this fire, so it hasn't been engaged yet in
	// the typical flow).
	double max_hp = 0.0;
	if (MMAPI::Engine::StructVariableExists(monster_rv, FIELD_DEEP_DUNGEON_MAX_HEALTH))
	{
		max_hp = monster_rv.GetMember(FIELD_DEEP_DUNGEON_MAX_HEALTH).ToDouble();
	}
	else if (MMAPI::Engine::StructVariableExists(monster_rv, FIELD_CAPTURED_MAX_HEALTH))
	{
		max_hp = monster_rv.GetMember(FIELD_CAPTURED_MAX_HEALTH).ToDouble();
	}
	else
	{
		MMAPI::Engine::StructVariableSet(monster_rv, FIELD_CAPTURED_MAX_HEALTH, current_hp);
		max_hp = current_hp;
	}
	if (max_hp <= 0.0) return;

	double x = MMAPI::Engine::InstanceVariableGet(monster, "x").ToDouble();
	double y = MMAPI::Engine::InstanceVariableGet(monster, "y").ToDouble();

	int bar_width  = config.smaller_health_bar ? SMALL_HEALTH_BAR_WIDTH  : HEALTH_BAR_WIDTH;
	int bar_height = config.smaller_health_bar ? SMALL_HEALTH_BAR_HEIGHT : HEALTH_BAR_HEIGHT;
	int bar_x      = static_cast<int>(x);
	int bar_y      = static_cast<int>(y) - offset_it->second;

	double hp_percent = current_hp / max_hp;
	if (hp_percent < 0.0) hp_percent = 0.0;
	if (hp_percent > 1.0) hp_percent = 1.0;

	int left   = bar_x - bar_width / 2;
	int right  = bar_x + bar_width / 2;
	int top    = bar_y - bar_height / 2;
	int fill_top    = top + bar_height / 3;
	int fill_bottom = top + (bar_height * 2) / 3;
	int fill_right  = left + static_cast<int>(bar_width * hp_percent) - 1;

	MMAPI::Engine::DrawRectangle(COLOR_BLACK, left, fill_top, right - 1, fill_bottom, false);

	int fill_color = config.red_health_bar ? COLOR_RED : COLOR_GREEN;
	MMAPI::Engine::DrawRectangle(fill_color, left, fill_top, fill_right, fill_bottom, false);

	YYTK::RValue sprite_asset = MMAPI::Engine::AssetGetIndex(
		config.smaller_health_bar ? "enemy_health_bar_small" : "enemy_health_bar"
	);
	MMAPI::Engine::DrawSprite(sprite_asset, -1, left, top);
}

// ----- Hooks -----

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
	MMAPI::Monster::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Monster::Hooks::AfterDrawMonster(OnAfterDrawMonster);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
