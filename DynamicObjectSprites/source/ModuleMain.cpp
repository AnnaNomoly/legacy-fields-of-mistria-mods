#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;
using nlohmann::json;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "DynamicObjectSprites";
static const char* const VERSION  = "1.2.0";

// ----- Config schema -----

static const char* const MOD_NAME_KEY                = "mod_name";
static const char* const DYNAMIC_OBJECT_SPRITES_KEY  = "dynamic_object_sprites";
static const char* const CONDITIONS_KEY              = "conditions";
static const char* const ORIGINAL_SPRITE_NAME_KEY    = "original_sprite_name";
static const char* const REPLACEMENT_SPRITE_NAME_KEY = "replacement_sprite_name";
static const char* const BASE_SPRITE_NAME_KEY        = "base_sprite_name";
static const char* const TIME_OF_DAY_KEY             = "time_of_day";
static const char* const DAY_OF_WEEK_KEY             = "day_of_week";
static const char* const DAY_OF_MONTH_KEY            = "day_of_month";
static const char* const SEASON_KEY                  = "season";
static const char* const YEAR_KEY                    = "year";
static const char* const WEATHER_KEY                 = "weather";

static const std::string TIME_OF_DAY_DAY   = "day";
static const std::string TIME_OF_DAY_NIGHT = "night";

// ----- Substitution config -----

enum class TimeOfDayCondition { None, Day, Night };

// A configured sprite substitution. Two modes:
//   - Direct: when `base_sprite_name` is empty, the renderer's Arguments[0] is rewritten from
//     `original_sprite_name` to `replacement_sprite_name` whenever the set_sprite hook fires.
//   - Floor: when `base_sprite_name` is set, the hook fires for that base sprite and we call
//     `sprite_assign(original, replacement)` — a permanent in-memory swap of the original
//     sprite's frames, used for floor tiles whose asset reference can't be swapped at draw time.
struct DynamicObjectSprite
{
	std::string                              original_sprite_name;
	std::string                              replacement_sprite_name;
	std::string                              base_sprite_name;    // empty = direct mode; non-empty = floor mode
	std::string_view                         mod_name;

	TimeOfDayCondition                       time_of_day = TimeOfDayCondition::None;
	std::optional<MMAPI::Calendar::Weekdays> week_day;
	std::optional<int>                       month_day;
	std::optional<MMAPI::Calendar::Seasons>  season;
	std::optional<int>                       year;
	std::optional<MMAPI::Weather::Ids>       weather;

	bool IsFloorSprite() const { return !base_sprite_name.empty(); }

	bool HasAnyCondition() const
	{
		return time_of_day != TimeOfDayCondition::None
		    || week_day.has_value()
		    || month_day.has_value()
		    || season.has_value()
		    || year.has_value()
		    || weather.has_value();
	}
};

// ----- State -----

static bool startup_loaded = false;
static bool game_is_active = false;
// String pool: dynamic_object_sprite.mod_name is a string_view referencing an entry here, so
// the backing string must outlive the substitution config. unordered_set never invalidates
// references on insert.
static std::unordered_set<std::string>   mod_names_pool;
static std::vector<DynamicObjectSprite>  dynamic_object_sprites;

// ----- Helpers -----

static std::string ToLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}

// ----- Config loading -----

static fs::path ConfigDir()
{
	return fs::current_path() / "mod_data" / MOD_NAME;
}

// Parses the JSON conditions block onto `dp`, dropping invalid fields silently (matching the
// original mod's "skip-unknown-values" semantics). Returns true if at least one valid condition
// was set — the caller skips substitutions with no conditions.
static bool ParseConditions(const json& conditions, DynamicObjectSprite& dp)
{
	if (conditions.contains(TIME_OF_DAY_KEY) && conditions[TIME_OF_DAY_KEY].is_string())
	{
		std::string v = ToLower(conditions[TIME_OF_DAY_KEY].get<std::string>());
		if      (v == TIME_OF_DAY_DAY)   dp.time_of_day = TimeOfDayCondition::Day;
		else if (v == TIME_OF_DAY_NIGHT) dp.time_of_day = TimeOfDayCondition::Night;
	}

	if (conditions.contains(DAY_OF_WEEK_KEY) && conditions[DAY_OF_WEEK_KEY].is_string())
	{
		std::string v = ToLower(conditions[DAY_OF_WEEK_KEY].get<std::string>());
		if (auto wd = MMAPI::Calendar::TryWeekdayFromInternalName(v))
			dp.week_day = wd;
	}

	if (conditions.contains(DAY_OF_MONTH_KEY) && conditions[DAY_OF_MONTH_KEY].is_number_integer())
	{
		int day = conditions[DAY_OF_MONTH_KEY].get<int>();
		if (day >= 1 && day <= 28)
			dp.month_day = day;
	}

	if (conditions.contains(SEASON_KEY) && conditions[SEASON_KEY].is_string())
	{
		std::string v = ToLower(conditions[SEASON_KEY].get<std::string>());
		if (auto s = MMAPI::Calendar::TrySeasonFromInternalName(v))
			dp.season = s;
	}

	if (conditions.contains(YEAR_KEY) && conditions[YEAR_KEY].is_number_integer())
	{
		int y = conditions[YEAR_KEY].get<int>();
		if (y > 0) dp.year = y;
	}

	if (conditions.contains(WEATHER_KEY) && conditions[WEATHER_KEY].is_string())
	{
		std::string v = ToLower(conditions[WEATHER_KEY].get<std::string>());
		if (auto w = MMAPI::Weather::TryFromInternalName(v))
			dp.weather = w;
	}

	return dp.HasAnyCondition();
}

static void LoadConfigFile(const fs::path& path)
{
	std::string filename = path.filename().string();

	std::ifstream stream(path);
	if (!stream.good())
	{
		MMAPI::Log::Error("Failed to open config file: %s", filename.c_str());
		return;
	}

	json doc;
	try
	{
		doc = json::parse(stream);
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Failed to parse %s: %s", filename.c_str(), e.what());
		return;
	}

	if (doc.empty())
	{
		MMAPI::Log::Warn("Empty config file: %s — ignoring", filename.c_str());
		return;
	}

	MMAPI::Log::Info("Reading config: %s", filename.c_str());

	std::string_view mod_name_view;
	if (doc.contains(MOD_NAME_KEY) && doc[MOD_NAME_KEY].is_string())
	{
		std::string name = doc[MOD_NAME_KEY].get<std::string>();
		if (!name.empty())
		{
			auto [it, _] = mod_names_pool.insert(name);
			mod_name_view = *it;
		}
	}

	if (!doc.contains(DYNAMIC_OBJECT_SPRITES_KEY) || !doc[DYNAMIC_OBJECT_SPRITES_KEY].is_array())
	{
		MMAPI::Log::Warn("Missing/invalid \"%s\" in %s — ignoring", DYNAMIC_OBJECT_SPRITES_KEY, filename.c_str());
		return;
	}

	for (const auto& entry : doc[DYNAMIC_OBJECT_SPRITES_KEY])
	{
		if (!entry.is_object()
		    || !entry.contains(CONDITIONS_KEY)              || !entry[CONDITIONS_KEY].is_object()
		    || !entry.contains(ORIGINAL_SPRITE_NAME_KEY)    || !entry[ORIGINAL_SPRITE_NAME_KEY].is_string()
		    || !entry.contains(REPLACEMENT_SPRITE_NAME_KEY) || !entry[REPLACEMENT_SPRITE_NAME_KEY].is_string())
			continue;

		std::string original    = entry[ORIGINAL_SPRITE_NAME_KEY].get<std::string>();
		std::string replacement = entry[REPLACEMENT_SPRITE_NAME_KEY].get<std::string>();
		if (original.empty() || replacement.empty()) continue;

		std::string base_sprite;
		if (entry.contains(BASE_SPRITE_NAME_KEY) && entry[BASE_SPRITE_NAME_KEY].is_string())
		{
			base_sprite = entry[BASE_SPRITE_NAME_KEY].get<std::string>();
			if (base_sprite.empty()) continue;
		}

		YYTK::RValue original_asset = MMAPI::Engine::AssetGetIndex(original);
		if (original_asset.m_Kind != YYTK::VALUE_REF)
		{
			MMAPI::Log::Error("Original sprite doesn't exist: %s", original.c_str());
			continue;
		}

		YYTK::RValue replacement_asset = MMAPI::Engine::AssetGetIndex(replacement);
		if (replacement_asset.m_Kind != YYTK::VALUE_REF)
		{
			MMAPI::Log::Error("Replacement sprite doesn't exist: %s", replacement.c_str());
			continue;
		}

		if (!base_sprite.empty())
		{
			YYTK::RValue base_asset = MMAPI::Engine::AssetGetIndex(base_sprite);
			if (base_asset.m_Kind != YYTK::VALUE_REF)
			{
				MMAPI::Log::Error("Base sprite doesn't exist: %s", base_sprite.c_str());
				continue;
			}
		}

		DynamicObjectSprite dp;
		dp.original_sprite_name    = std::move(original);
		dp.replacement_sprite_name = std::move(replacement);
		dp.base_sprite_name        = std::move(base_sprite);
		dp.mod_name                = mod_name_view;

		if (!ParseConditions(entry[CONDITIONS_KEY], dp))
			continue;

		MMAPI::Log::Info("Loaded substitution for sprite: %s", dp.original_sprite_name.c_str());
		dynamic_object_sprites.push_back(std::move(dp));
	}
}

static void LoadAllConfigs()
{
	fs::path dir = ConfigDir();
	std::error_code ec;
	fs::create_directories(dir, ec);

	if (!fs::is_directory(dir, ec))
	{
		MMAPI::Log::Warn("Config directory unavailable: %s", dir.string().c_str());
		return;
	}

	for (const auto& entry : fs::directory_iterator(dir))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".json")
			LoadConfigFile(entry.path());
	}
}

// ----- Substitution -----

static bool MatchesCurrent(const DynamicObjectSprite& dp)
{
	if (dp.time_of_day != TimeOfDayCondition::None)
	{
		bool is_night = MMAPI::Calendar::IsNight();
		if (dp.time_of_day == TimeOfDayCondition::Day   &&  is_night) return false;
		if (dp.time_of_day == TimeOfDayCondition::Night && !is_night) return false;
	}

	if (dp.week_day)
	{
		MMAPI::Calendar::Weekdays current_wd;
		if (!MMAPI::Calendar::TryGetWeekday(current_wd)) return false;
		if (current_wd != *dp.week_day) return false;
	}

	if (dp.month_day)
	{
		YYTK::RValue day_rv = MMAPI::Calendar::GetDay();
		if (day_rv.m_Kind == YYTK::VALUE_UNDEFINED) return false;
		if (static_cast<int>(day_rv.ToInt64()) != *dp.month_day) return false;
	}

	if (dp.season)
	{
		MMAPI::Calendar::Seasons current_s;
		if (!MMAPI::Calendar::TryGetSeason(current_s)) return false;
		if (current_s != *dp.season) return false;
	}

	if (dp.year)
	{
		YYTK::RValue year_rv = MMAPI::Calendar::GetYear();
		if (year_rv.m_Kind == YYTK::VALUE_UNDEFINED) return false;
		if (static_cast<int>(year_rv.ToInt64()) != *dp.year) return false;
	}

	if (dp.weather)
	{
		MMAPI::Weather::Ids current_w;
		if (!MMAPI::Weather::TryGetWeather(current_w)) return false;
		if (current_w != *dp.weather) return false;
	}

	return true;
}

// ----- Hooks -----

void OnBeforeNodeRendererSetSprite(MMAPI::Object::NodeRendererSetSpriteContext& ctx)
{
	if (!game_is_active) return;

	std::string sprite_name(ctx.GetSpriteName());
	if (sprite_name.empty()) return;

	for (const auto& dp : dynamic_object_sprites)
	{
		// Direct mode matches on original_sprite_name; floor mode matches on base_sprite_name.
		// The original sprite asset that triggers a floor swap is the base sprite, not the
		// substitution's "original" — the latter is the asset whose pixels get overwritten.
		const std::string& match_name = dp.IsFloorSprite() ? dp.base_sprite_name : dp.original_sprite_name;
		if (sprite_name != match_name) continue;
		if (!MatchesCurrent(dp))       continue;

		if (dp.IsFloorSprite())
		{
			// Floor-tile path: permanent in-memory swap. The renderer keeps its current asset
			// reference (the base sprite), but that asset's pixel data is now the replacement's.
			YYTK::RValue original    = MMAPI::Engine::AssetGetIndex(dp.original_sprite_name);
			YYTK::RValue replacement = MMAPI::Engine::AssetGetIndex(dp.replacement_sprite_name);
			MMAPI::Internal::module_interface->CallBuiltin("sprite_assign", { original, replacement });
			// No SetSpriteAsset — the trampoline runs unchanged, but the asset is now visually different.
		}
		else
		{
			YYTK::RValue replacement = MMAPI::Engine::AssetGetIndex(dp.replacement_sprite_name);
			if (replacement.m_Kind == YYTK::VALUE_REF)
				ctx.SetSpriteAsset(replacement);
		}
		return;
	}
}

void OnBeforeSetupMainScreen()
{
	game_is_active = false;

	if (!startup_loaded)
	{
		LoadAllConfigs();
		startup_loaded = true;
	}
}

void OnGameActive()
{
	game_is_active = true;
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
	MMAPI::Calendar::Enable();
	MMAPI::Weather::Enable();
	MMAPI::Object::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Object::Hooks::BeforeNodeRendererSetSprite(OnBeforeNodeRendererSetSprite);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
