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

static const char* const MOD_NAME = "DynamicNpcPortraits";
static const char* const VERSION  = "1.1.0";

// ----- Config schema -----

static const char* const MOD_NAME_KEY          = "mod_name";
static const char* const DYNAMIC_PORTRAITS_KEY = "dynamic_portraits";
static const char* const CONDITIONS_KEY        = "conditions";
static const char* const SPRITE_NAME_KEY       = "sprite_name";
static const char* const NPC_KEY               = "npc";
static const char* const TIME_OF_DAY_KEY       = "time_of_day";
static const char* const DAY_OF_WEEK_KEY       = "day_of_week";
static const char* const DAY_OF_MONTH_KEY      = "day_of_month";
static const char* const SEASON_KEY            = "season";
static const char* const YEAR_KEY              = "year";
static const char* const WEATHER_KEY           = "weather";
static const char* const LOCATION_KEY          = "location";

static const std::string TIME_OF_DAY_DAY   = "day";
static const std::string TIME_OF_DAY_NIGHT = "night";

// Mod-defined pseudo-locations; matched against MMAPI::Location::IsOutdoors + Dungeon override
// rather than a real Location::Ids value.
static const std::string LOCATION_INDOORS  = "indoors";
static const std::string LOCATION_OUTDOORS = "outdoors";

// ----- Portrait config -----

enum class TimeOfDayCondition { None, Day, Night };
enum class LocationKind       { None, Specific, Indoors, Outdoors };

struct DynamicPortrait
{
	MMAPI::NPC::Ids                          npc;
	std::string                              sprite_name;
	std::string_view                         mod_name;

	TimeOfDayCondition                       time_of_day = TimeOfDayCondition::None;
	std::optional<MMAPI::Calendar::Weekdays> week_day;
	std::optional<int>                       month_day;
	std::optional<MMAPI::Calendar::Seasons>  season;
	std::optional<int>                       year;
	std::optional<MMAPI::Weather::Ids>       weather;
	LocationKind                             location_kind = LocationKind::None;
	MMAPI::Location::Ids                     location_id   = MMAPI::Location::Ids::Town;

	bool HasAnyCondition() const
	{
		return time_of_day != TimeOfDayCondition::None
		    || week_day.has_value()
		    || month_day.has_value()
		    || season.has_value()
		    || year.has_value()
		    || weather.has_value()
		    || location_kind != LocationKind::None;
	}
};

// ----- State -----

static bool startup_loaded = false;
static bool game_is_active = false;
// String pool: dynamic_portrait.mod_name is a string_view referencing an entry here, so the
// backing string must outlive the portrait. unordered_set never invalidates references on insert.
static std::unordered_set<std::string> mod_names_pool;
static std::vector<DynamicPortrait>    dynamic_portraits;

// ----- Helpers -----

static std::string ToLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}

static std::vector<std::string> Split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t end;
	while ((end = s.find(delimiter, start)) != std::string::npos)
	{
		tokens.push_back(s.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(s.substr(start));
	return tokens;
}

// Returns true if Ari is in an indoor non-dungeon location. The original mod treats dungeons as
// "not indoors" so that "outdoors" conditions match while in a dungeon — that semantic is
// preserved here.
static bool AriIsIndoors()
{
	MMAPI::Location::Ids current_location;
	if (!MMAPI::Location::TryGetCurrentLocation(current_location))
		return false;

	if (MMAPI::Location::IsOutdoors(current_location))
		return false;

	if (MMAPI::Dungeon::IsDungeonRoom())
		return false;

	return true;
}

// ----- Config loading -----

static fs::path ConfigDir()
{
	return fs::current_path() / "mod_data" / MOD_NAME;
}

// Parses the JSON conditions block onto `dp`, dropping invalid fields silently (matching the
// original mod's "skip-unknown-values" semantics). Returns true if at least one valid condition
// was set — the caller skips portraits with no conditions.
static bool ParseConditions(const json& conditions, DynamicPortrait& dp)
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

	if (conditions.contains(LOCATION_KEY) && conditions[LOCATION_KEY].is_string())
	{
		std::string v = ToLower(conditions[LOCATION_KEY].get<std::string>());
		if      (v == LOCATION_INDOORS)  dp.location_kind = LocationKind::Indoors;
		else if (v == LOCATION_OUTDOORS) dp.location_kind = LocationKind::Outdoors;
		else if (auto loc = MMAPI::Location::TryFromInternalName(v))
		{
			dp.location_kind = LocationKind::Specific;
			dp.location_id   = *loc;
		}
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

	if (!doc.contains(DYNAMIC_PORTRAITS_KEY) || !doc[DYNAMIC_PORTRAITS_KEY].is_array())
	{
		MMAPI::Log::Warn("Missing/invalid \"%s\" in %s — ignoring", DYNAMIC_PORTRAITS_KEY, filename.c_str());
		return;
	}

	for (const auto& entry : doc[DYNAMIC_PORTRAITS_KEY])
	{
		if (!entry.is_object()
		    || !entry.contains(CONDITIONS_KEY)   || !entry[CONDITIONS_KEY].is_object()
		    || !entry.contains(SPRITE_NAME_KEY)  || !entry[SPRITE_NAME_KEY].is_string()
		    || !entry.contains(NPC_KEY)          || !entry[NPC_KEY].is_string())
			continue;

		std::string sprite_name = entry[SPRITE_NAME_KEY].get<std::string>();
		if (sprite_name.empty()) continue;

		std::string npc_name = ToLower(entry[NPC_KEY].get<std::string>());
		auto npc_id = MMAPI::NPC::TryFromInternalName(npc_name);
		if (!npc_id) continue;

		// Probe the base sprite via its neutral-expression variant — every NPC has at least the
		// "_neutral" expression, so if `<sprite_name>_neutral` doesn't exist the config is bad.
		std::string probe = sprite_name + "_neutral";
		YYTK::RValue probe_asset = MMAPI::Engine::AssetGetIndex(probe);
		if (probe_asset.m_Kind != YYTK::VALUE_REF)
		{
			MMAPI::Log::Error("Base sprite doesn't exist: %s", sprite_name.c_str());
			continue;
		}

		DynamicPortrait dp;
		dp.npc         = *npc_id;
		dp.sprite_name = std::move(sprite_name);
		dp.mod_name    = mod_name_view;

		if (!ParseConditions(entry[CONDITIONS_KEY], dp))
			continue;

		MMAPI::Log::Info("Loaded portrait config for sprite: %s", dp.sprite_name.c_str());
		dynamic_portraits.push_back(std::move(dp));
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

// ----- Portrait substitution -----

static bool MatchesCurrent(const DynamicPortrait& dp)
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

	switch (dp.location_kind)
	{
		case LocationKind::None:
			break;
		case LocationKind::Indoors:
			if (!AriIsIndoors()) return false;
			break;
		case LocationKind::Outdoors:
			if (AriIsIndoors()) return false;
			break;
		case LocationKind::Specific:
		{
			MMAPI::Location::Ids current_loc;
			if (!MMAPI::Location::TryGetCurrentLocation(current_loc)) return false;
			if (current_loc != dp.location_id) return false;
			break;
		}
	}

	return true;
}

// Sprite name format: `spr_portrait_<npc>_<season_or_variant>_<expression_parts...>`
// (e.g. `spr_portrait_balor_spring_sincere_special`). The "season_or_variant" segment at
// parts[3] is intentionally discarded — the configured replacement sprite is the new base, and
// the trailing expression (`sincere_special`) is appended to it.
static YYTK::RValue ResolveDynamicPortrait(const std::string& sprite_name)
{
	std::vector<std::string> parts = Split(sprite_name, '_');
	if (parts.size() < 5)
		return {};

	std::string npc_name = parts[2];
	auto current_npc = MMAPI::NPC::TryFromInternalName(npc_name);
	if (!current_npc)
		return {};

	std::string expression;
	for (size_t i = 4; i < parts.size(); ++i)
	{
		expression += parts[i];
		if (i + 1 < parts.size()) expression += "_";
	}

	for (const auto& dp : dynamic_portraits)
	{
		if (dp.npc != *current_npc) continue;
		if (!MatchesCurrent(dp))    continue;

		std::string substitute = dp.sprite_name + "_" + expression;
		YYTK::RValue asset = MMAPI::Engine::AssetGetIndex(substitute);
		if (asset.m_Kind == YYTK::VALUE_REF)
			return asset;
	}

	return {};
}

// ----- Hooks -----

void OnBeforeVertigoDrawWithColor(MMAPI::Display::VertigoDrawWithColorContext& ctx)
{
	if (!game_is_active) return;
	// Skip substitution while a cutscene is running — those use scripted portraits that the
	// dynamic conditions weren't designed for, and substituting would clash with the cutscene.
	if (MMAPI::Cutscene::IsRunning()) return;

	std::string sprite_name(ctx.GetSpriteName());
	if (sprite_name.find("spr_portrait") == std::string::npos) return;

	YYTK::RValue substitute = ResolveDynamicPortrait(sprite_name);
	if (substitute.m_Kind == YYTK::VALUE_REF)
		ctx.SetSpriteAsset(substitute);
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
	MMAPI::Location::Enable();
	MMAPI::Dungeon::Enable();
	MMAPI::Display::Enable();
	MMAPI::Cutscene::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Display::Hooks::BeforeVertigoDrawWithColor(OnBeforeVertigoDrawWithColor);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
