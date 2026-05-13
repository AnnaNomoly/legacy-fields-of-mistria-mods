#include <algorithm>
#include <cctype>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;
namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "RemindMe";
static const char* const VERSION  = "1.3.0";

// ----- Notification placeholder (intercepted via Text::AfterLocalizedString to inject reminder text) -----

static const char* const REMINDER_NOTIFICATION_PLACEHOLDER_KEY =
	"Notifications/Mods/Remind Me/reminder_notification_placeholder";

// ----- Config keys -----

static const char* const KEY_REMINDERS         = "reminders";
static const char* const KEY_CONDITIONS        = "conditions";
static const char* const KEY_NOTIFICATION      = "notification";
static const char* const KEY_TIME              = "26h_time";
static const char* const KEY_DAY_OF_WEEK       = "day_of_week";
static const char* const KEY_DAY_OF_MONTH      = "day_of_month";
static const char* const KEY_SEASON            = "season";
static const char* const KEY_YEAR              = "year";
static const char* const KEY_WEATHER           = "weather";
static const char* const KEY_LOCATION          = "location";
static const char* const KEY_NPC_NEARBY        = "npc_nearby";
static const char* const KEY_NPC_GIFTABLE      = "npc_giftable";
static const char* const KEY_FARM_ANIMAL       = "farm_animal_outside";
static const char* const KEY_RESET_AFTER       = "reset_after";
static const char* const KEY_CAN_TRIGGER_AFTER = "can_trigger_after";

// ----- Domain types -----

enum class ResetAfter { Time, Location, Day };

struct Reminder
{
	std::optional<int>                       time_seconds;
	std::optional<MMAPI::Calendar::Weekdays> week_day;
	std::optional<int>                       month_day;
	std::optional<MMAPI::Calendar::Seasons>  season;
	std::optional<int>                       year;
	std::optional<MMAPI::Weather::Ids>       weather;
	std::optional<MMAPI::Location::Ids>      location;
	std::optional<MMAPI::NPC::Ids>           npc;
	bool                                     npc_giftable        = false;
	bool                                     farm_animal_outside = false;
	ResetAfter                               reset_after         = ResetAfter::Day;
	int                                      reset_after_seconds = -1;
	bool                                     can_trigger_after   = false;
	bool                                     has_triggered       = false;
	int                                      last_triggered_time = -1;
	std::string                              notification;
};

struct NpcTracker
{
	int  time_last_checked = -1;
	bool is_nearby         = false;
};

// ----- State -----

static bool                                 startup_loaded = false;
static std::vector<Reminder>                reminders;
static std::string                          pending_notification_text;
static std::map<MMAPI::NPC::Ids, NpcTracker> npc_trackers;

// ----- Config name → enum mapping (user-facing names preserved from the original mod) -----

static std::string ToUpper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
	return s;
}

static std::string ToLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}

static std::optional<MMAPI::Calendar::Weekdays> WeekdayFromConfig(const std::string& s)
{
	std::string u = ToUpper(s);
	if (u == "MONDAY")    return MMAPI::Calendar::Weekdays::Monday;
	if (u == "TUESDAY")   return MMAPI::Calendar::Weekdays::Tuesday;
	if (u == "WEDNESDAY") return MMAPI::Calendar::Weekdays::Wednesday;
	if (u == "THURSDAY")  return MMAPI::Calendar::Weekdays::Thursday;
	if (u == "FRIDAY")    return MMAPI::Calendar::Weekdays::Friday;
	if (u == "SATURDAY")  return MMAPI::Calendar::Weekdays::Saturday;
	if (u == "SUNDAY")    return MMAPI::Calendar::Weekdays::Sunday;
	return std::nullopt;
}

static std::optional<MMAPI::Calendar::Seasons> SeasonFromConfig(const std::string& s)
{
	std::string u = ToUpper(s);
	if (u == "SPRING") return MMAPI::Calendar::Seasons::Spring;
	if (u == "SUMMER") return MMAPI::Calendar::Seasons::Summer;
	if (u == "FALL")   return MMAPI::Calendar::Seasons::Fall;
	if (u == "WINTER") return MMAPI::Calendar::Seasons::Winter;
	return std::nullopt;
}

static std::optional<MMAPI::Weather::Ids> WeatherFromConfig(const std::string& s)
{
	std::string u = ToUpper(s);
	if (u == "CALM")      return MMAPI::Weather::Ids::Calm;
	if (u == "INCLEMENT") return MMAPI::Weather::Ids::Inclement;
	if (u == "SEVERE")    return MMAPI::Weather::Ids::HeavyInclement;
	if (u == "SPECIAL")   return MMAPI::Weather::Ids::Special;
	return std::nullopt;
}

static std::optional<MMAPI::Location::Ids> LocationFromConfig(const std::string& s)
{
	std::string u = ToUpper(s);
	if (u == "WESTERN RUINS")   return MMAPI::Location::Ids::WesternRuins;
	if (u == "NARROWS")         return MMAPI::Location::Ids::Narrows;
	if (u == "SWEETWATER FARM") return MMAPI::Location::Ids::HaydensFarm;
	if (u == "BEACH")           return MMAPI::Location::Ids::Beach;
	if (u == "TOWN")            return MMAPI::Location::Ids::Town;
	if (u == "EASTERN ROAD")    return MMAPI::Location::Ids::EasternRoad;
	if (u == "DEEP WOODS")      return MMAPI::Location::Ids::DeepWoods;
	if (u == "FARM")            return MMAPI::Location::Ids::Farm;
	if (u == "INN")             return MMAPI::Location::Ids::Inn;
	if (u == "DUNGEON")         return MMAPI::Location::Ids::Dungeon;
	return std::nullopt;
}

static std::optional<MMAPI::NPC::Ids> NpcFromConfig(const std::string& s)
{
	return MMAPI::NPC::TryFromInternalName(ToLower(s));
}

// Treat the configured DUNGEON location as matching any of the seal locations as well, to preserve
// the original mod's behavior of bundling seal rooms under the dungeon umbrella for reminders.
static bool LocationMatches(MMAPI::Location::Ids configured, MMAPI::Location::Ids current)
{
	if (configured == current) return true;
	if (configured != MMAPI::Location::Ids::Dungeon) return false;
	switch (current)
	{
		case MMAPI::Location::Ids::EarthSeal:
		case MMAPI::Location::Ids::FireSeal:
		case MMAPI::Location::Ids::RuinsSeal:
		case MMAPI::Location::Ids::VoidSeal:
		case MMAPI::Location::Ids::WaterSeal:
			return true;
		default:
			return false;
	}
}

// ----- HH:MM parsing -----

// Parses a 26-hour clock time string (HH:MM) into seconds-of-day. Mistria's day clock spans
// 06:00 through 26:00 (next-day 02:00). Returns std::nullopt for malformed or out-of-range input.
static std::optional<int> Parse26hClockTime(const std::string& s)
{
	if (s.size() != 5) return std::nullopt;
	if (!std::isdigit(static_cast<unsigned char>(s[0])) || !std::isdigit(static_cast<unsigned char>(s[1]))
		|| s[2] != ':'
		|| !std::isdigit(static_cast<unsigned char>(s[3])) || !std::isdigit(static_cast<unsigned char>(s[4])))
		return std::nullopt;
	int hour   = (s[0] - '0') * 10 + (s[1] - '0');
	int minute = (s[3] - '0') * 10 + (s[4] - '0');
	if (hour < 6 || hour > 26)        return std::nullopt;
	if (minute < 0 || minute > 59)    return std::nullopt;
	if (hour == 26 && minute > 0)     return std::nullopt;
	return hour * 3600 + minute * 60;
}

// Parses a reset-after duration string (HH:MM) into seconds. Valid range is 00:10 through 20:00 to
// avoid reminders that immediately re-trigger or never reset within a day.
static std::optional<int> ParseResetAfterDuration(const std::string& s)
{
	if (s.size() != 5) return std::nullopt;
	if (!std::isdigit(static_cast<unsigned char>(s[0])) || !std::isdigit(static_cast<unsigned char>(s[1]))
		|| s[2] != ':'
		|| !std::isdigit(static_cast<unsigned char>(s[3])) || !std::isdigit(static_cast<unsigned char>(s[4])))
		return std::nullopt;
	int hour   = (s[0] - '0') * 10 + (s[1] - '0');
	int minute = (s[3] - '0') * 10 + (s[4] - '0');
	if (hour < 0 || hour > 20)        return std::nullopt;
	if (minute < 0 || minute > 59)    return std::nullopt;
	if (hour == 0 && minute < 10)     return std::nullopt;
	if (hour == 20 && minute > 0)     return std::nullopt;
	return hour * 3600 + minute * 60;
}

// ----- Config -----

static json BuildDefaultConfig()
{
	json r0_conditions;
	r0_conditions[KEY_WEATHER]           = "SPECIAL";
	r0_conditions[KEY_SEASON]            = "SPRING";
	r0_conditions[KEY_CAN_TRIGGER_AFTER] = true;
	json r0;
	r0[KEY_NOTIFICATION] = "The legendary fish is in season! (Perk Required)";
	r0[KEY_CONDITIONS]   = r0_conditions;

	json r1_conditions;
	r1_conditions[KEY_SEASON]            = "WINTER";
	r1_conditions[KEY_DAY_OF_MONTH]      = 18;
	r1_conditions[KEY_CAN_TRIGGER_AFTER] = true;
	json r1;
	r1[KEY_NOTIFICATION] = "Today is Adeline's birthday!";
	r1[KEY_CONDITIONS]   = r1_conditions;

	json config;
	config[KEY_REMINDERS] = json::array({ r0, r1 });
	return config;
}

static bool LoadReminder(const json& source, Reminder& out)
{
	if (!source.contains(KEY_CONDITIONS) || !source[KEY_CONDITIONS].is_object())
	{
		MMAPI::Log::Warn("Reminder missing \"%s\"; ignored.", KEY_CONDITIONS);
		return false;
	}
	if (!source.contains(KEY_NOTIFICATION) || !source[KEY_NOTIFICATION].is_string())
	{
		MMAPI::Log::Warn("Reminder missing \"%s\"; ignored.", KEY_NOTIFICATION);
		return false;
	}
	out.notification = source[KEY_NOTIFICATION].get<std::string>();
	if (out.notification.empty())
	{
		MMAPI::Log::Warn("Reminder has empty \"%s\"; ignored.", KEY_NOTIFICATION);
		return false;
	}

	const json& c = source[KEY_CONDITIONS];
	if (c.empty())
	{
		MMAPI::Log::Warn("Reminder has no conditions; ignored.");
		return false;
	}

	if (c.contains(KEY_TIME) && c[KEY_TIME].is_string())
	{
		auto t = Parse26hClockTime(c[KEY_TIME].get<std::string>());
		if (t) out.time_seconds = *t;
		else   MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_TIME);
	}

	if (c.contains(KEY_DAY_OF_WEEK) && c[KEY_DAY_OF_WEEK].is_string())
	{
		auto w = WeekdayFromConfig(c[KEY_DAY_OF_WEEK].get<std::string>());
		if (w) out.week_day = *w;
		else   MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_DAY_OF_WEEK);
	}

	if (c.contains(KEY_DAY_OF_MONTH) && c[KEY_DAY_OF_MONTH].is_number_integer())
	{
		int d = c[KEY_DAY_OF_MONTH].get<int>();
		if (d > 0 && d < 29) out.month_day = d;
		else                 MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_DAY_OF_MONTH);
	}

	if (c.contains(KEY_SEASON) && c[KEY_SEASON].is_string())
	{
		auto s = SeasonFromConfig(c[KEY_SEASON].get<std::string>());
		if (s) out.season = *s;
		else   MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_SEASON);
	}

	if (c.contains(KEY_YEAR) && c[KEY_YEAR].is_number_integer())
	{
		int y = c[KEY_YEAR].get<int>();
		if (y > 0) out.year = y;
		else       MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_YEAR);
	}

	if (c.contains(KEY_WEATHER) && c[KEY_WEATHER].is_string())
	{
		auto w = WeatherFromConfig(c[KEY_WEATHER].get<std::string>());
		if (w) out.weather = *w;
		else   MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_WEATHER);
	}

	if (c.contains(KEY_LOCATION) && c[KEY_LOCATION].is_string())
	{
		auto l = LocationFromConfig(c[KEY_LOCATION].get<std::string>());
		if (l) out.location = *l;
		else   MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_LOCATION);
	}

	if (c.contains(KEY_NPC_NEARBY) && c[KEY_NPC_NEARBY].is_string())
	{
		auto n = NpcFromConfig(c[KEY_NPC_NEARBY].get<std::string>());
		if (n) out.npc = *n;
		else   MMAPI::Log::Warn("Invalid \"%s\" in reminder; condition ignored.", KEY_NPC_NEARBY);
	}

	if (c.contains(KEY_NPC_GIFTABLE) && c[KEY_NPC_GIFTABLE].is_boolean())
		out.npc_giftable = c[KEY_NPC_GIFTABLE].get<bool>();

	if (c.contains(KEY_FARM_ANIMAL) && c[KEY_FARM_ANIMAL].is_boolean())
		out.farm_animal_outside = c[KEY_FARM_ANIMAL].get<bool>();

	if (c.contains(KEY_CAN_TRIGGER_AFTER) && c[KEY_CAN_TRIGGER_AFTER].is_boolean())
		out.can_trigger_after = c[KEY_CAN_TRIGGER_AFTER].get<bool>();

	if (c.contains(KEY_RESET_AFTER) && c[KEY_RESET_AFTER].is_string())
	{
		std::string s = c[KEY_RESET_AFTER].get<std::string>();
		if      (s == "LOCATION") out.reset_after = ResetAfter::Location;
		else if (s == "DAY")      out.reset_after = ResetAfter::Day;
		else
		{
			auto duration = ParseResetAfterDuration(s);
			if (duration)
			{
				out.reset_after         = ResetAfter::Time;
				out.reset_after_seconds = *duration;
			}
			else
			{
				MMAPI::Log::Warn("Invalid \"%s\" in reminder; using default.", KEY_RESET_AFTER);
			}
		}
	}

	bool has_any = out.time_seconds || out.week_day || out.month_day
		|| out.season || out.year || out.weather
		|| out.location || out.npc || out.farm_animal_outside;
	if (!has_any)
	{
		MMAPI::Log::Warn("Reminder has no valid conditions; ignored.");
		return false;
	}
	return true;
}

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	if (!fs::exists(config_path))
	{
		MMAPI::Config::Save(config_path, BuildDefaultConfig());
		MMAPI::Log::Info("Wrote default config: %s", config_path.string().c_str());
	}

	json config = MMAPI::Config::Load(config_path);
	if (!config.contains(KEY_REMINDERS) || !config[KEY_REMINDERS].is_array())
	{
		MMAPI::Log::Warn("Config missing \"%s\" array; using defaults.", KEY_REMINDERS);
		config = BuildDefaultConfig();
		MMAPI::Config::Save(config_path, config);
	}

	reminders.clear();
	for (const json& source : config[KEY_REMINDERS])
	{
		Reminder reminder;
		if (LoadReminder(source, reminder))
			reminders.push_back(std::move(reminder));
	}
	MMAPI::Log::Info("Loaded %zu reminders.", reminders.size());
}

// ----- Reminder evaluation -----

static void EmitReminderNotification(const std::string& text)
{
	// Set the substitution text before kicking off the notification: the game's create_notification
	// resolves the placeholder key through the Localizer synchronously, and our AfterLocalizedString
	// hook consumes pending_notification_text from inside that same call stack.
	pending_notification_text = text;
	MMAPI::Game::CreateNotification(/*ignore_cooldown=*/false, REMINDER_NOTIFICATION_PLACEHOLDER_KEY);
}

static void UpdateNpcTrackers(int current_time_seconds)
{
	for (auto& [_, tracker] : npc_trackers)
		if (current_time_seconds > tracker.time_last_checked + 300) // 5 minutes
			tracker.is_nearby = false;
}

static bool IsAnyFarmAnimalOutside()
{
	bool found = false;
	MMAPI::Animal::ForEachAnimal([&](YYTK::RValue animal)
	{
		if (found) return;
		if (animal.m_Kind != YYTK::VALUE_OBJECT) return;
		if (!MMAPI::Engine::StructVariableExists(animal, "location_position")) return;
		YYTK::RValue lp = animal.GetMember("location_position");
		if (lp.m_Kind != YYTK::VALUE_OBJECT) return;
		if (!MMAPI::Engine::StructVariableExists(lp, "location_id")) return;
		YYTK::RValue loc_id = lp.GetMember("location_id");
		if (!MMAPI::Engine::IsNumeric(loc_id)) return;
		if (loc_id.ToInt64() == static_cast<int64_t>(MMAPI::Location::Ids::Farm))
			found = true;
	});
	return found;
}

static bool ConditionsMatch(const Reminder& r)
{
	int current_time = MMAPI::Game::GetCurrentTimeInSeconds();

	if (r.time_seconds)
	{
		if (r.can_trigger_after)
		{
			if (current_time < *r.time_seconds) return false;
		}
		else
		{
			// Window the trigger so a missed exact-second tick (e.g. paused) doesn't permanently
			// suppress the reminder — but cap it at 5 minutes to avoid late-fires the user wouldn't expect.
			if (current_time < *r.time_seconds)         return false;
			if (current_time >= *r.time_seconds + 300)  return false;
		}
	}

	if (r.week_day)
	{
		MMAPI::Calendar::Weekdays w;
		if (!MMAPI::Calendar::TryGetWeekday(w)) return false;
		if (w != *r.week_day) return false;
	}

	if (r.month_day)
	{
		YYTK::RValue day_rv = MMAPI::Calendar::GetDay();
		if (day_rv.m_Kind == YYTK::VALUE_UNDEFINED) return false;
		if (static_cast<int>(day_rv.ToInt64()) != *r.month_day) return false;
	}

	if (r.season)
	{
		MMAPI::Calendar::Seasons s;
		if (!MMAPI::Calendar::TryGetSeason(s)) return false;
		if (s != *r.season) return false;
	}

	if (r.year)
	{
		YYTK::RValue year_rv = MMAPI::Calendar::GetYear();
		if (year_rv.m_Kind == YYTK::VALUE_UNDEFINED) return false;
		if (static_cast<int>(year_rv.ToInt64()) != *r.year) return false;
	}

	if (r.weather)
	{
		MMAPI::Weather::Ids w;
		if (!MMAPI::Weather::TryGetWeather(w)) return false;
		if (w != *r.weather) return false;
	}

	if (r.location)
	{
		MMAPI::Location::Ids l;
		if (!MMAPI::Location::TryGetCurrentLocation(l)) return false;
		if (!LocationMatches(*r.location, l)) return false;
	}

	if (r.npc)
	{
		auto it = npc_trackers.find(*r.npc);
		if (it == npc_trackers.end() || !it->second.is_nearby) return false;
		if (r.npc_giftable && !MMAPI::NPC::IsGiftable(*r.npc)) return false;
	}

	if (r.farm_animal_outside && !IsAnyFarmAnimalOutside())
		return false;

	return true;
}

static void TickReminders()
{
	if (MMAPI::Game::IsPaused()) return;
	if (MMAPI::Cutscene::IsRunning()) return;

	int current_time = MMAPI::Game::GetCurrentTimeInSeconds();
	UpdateNpcTrackers(current_time);

	for (Reminder& r : reminders)
	{
		if (r.reset_after == ResetAfter::Time
			&& r.last_triggered_time != -1
			&& current_time > r.last_triggered_time + r.reset_after_seconds)
		{
			r.has_triggered = false;
		}

		if (r.has_triggered)        continue;
		if (!ConditionsMatch(r))    continue;

		r.has_triggered       = true;
		r.last_triggered_time = current_time;
		EmitReminderNotification(r.notification);
	}
}

// ----- NPC proximity tracker -----
// One callback registered per obj_<npc_name>; the match name tells us which NPC fired.

static const std::pair<MMAPI::NPC::Ids, const char*> npc_object_names[] = {
	{ MMAPI::NPC::Ids::Adeline,   "obj_adeline"   },
	{ MMAPI::NPC::Ids::Balor,     "obj_balor"     },
	{ MMAPI::NPC::Ids::Caldarus,  "obj_caldarus"  },
	{ MMAPI::NPC::Ids::Celine,    "obj_celine"    },
	{ MMAPI::NPC::Ids::Darcy,     "obj_darcy"     },
	{ MMAPI::NPC::Ids::Dell,      "obj_dell"      },
	{ MMAPI::NPC::Ids::Dozy,      "obj_dozy"      },
	{ MMAPI::NPC::Ids::Eiland,    "obj_eiland"    },
	{ MMAPI::NPC::Ids::Elsie,     "obj_elsie"     },
	{ MMAPI::NPC::Ids::Errol,     "obj_errol"     },
	{ MMAPI::NPC::Ids::Hayden,    "obj_hayden"    },
	{ MMAPI::NPC::Ids::Hemlock,   "obj_hemlock"   },
	{ MMAPI::NPC::Ids::Henrietta, "obj_henrietta" },
	{ MMAPI::NPC::Ids::Holt,      "obj_holt"      },
	{ MMAPI::NPC::Ids::Josephine, "obj_josephine" },
	{ MMAPI::NPC::Ids::Juniper,   "obj_juniper"   },
	{ MMAPI::NPC::Ids::Landen,    "obj_landen"    },
	{ MMAPI::NPC::Ids::Louis,     "obj_louis"     },
	{ MMAPI::NPC::Ids::Luc,       "obj_luc"       },
	{ MMAPI::NPC::Ids::Maple,     "obj_maple"     },
	{ MMAPI::NPC::Ids::March,     "obj_march"     },
	{ MMAPI::NPC::Ids::Merri,     "obj_merri"     },
	{ MMAPI::NPC::Ids::Nora,      "obj_nora"      },
	{ MMAPI::NPC::Ids::Olric,     "obj_olric"     },
	{ MMAPI::NPC::Ids::Reina,     "obj_reina"     },
	{ MMAPI::NPC::Ids::Ryis,      "obj_ryis"      },
	{ MMAPI::NPC::Ids::Seridia,   "obj_seridia"   },
	{ MMAPI::NPC::Ids::Taliferro, "obj_taliferro" },
	{ MMAPI::NPC::Ids::Terithia,  "obj_terithia"  },
	{ MMAPI::NPC::Ids::Valen,     "obj_valen"     },
	{ MMAPI::NPC::Ids::Vera,      "obj_vera"      },
	{ MMAPI::NPC::Ids::Wheedle,   "obj_wheedle"   },
};

static std::unordered_map<std::string, MMAPI::NPC::Ids> obj_name_to_npc_id;

static void OnNpcTick(YYTK::CInstance* npc_instance)
{
	if (!npc_instance || !npc_instance->m_Object || !npc_instance->m_Object->m_Name) return;
	auto it = obj_name_to_npc_id.find(npc_instance->m_Object->m_Name);
	if (it == obj_name_to_npc_id.end()) return;

	// NPC instances briefly exist before their `me` struct is populated; skip until it is so the
	// reminder evaluator never sees a "nearby" flag set against a half-initialized NPC.
	YYTK::RValue npc_rv = npc_instance->ToRValue();
	if (!MMAPI::Engine::StructVariableExists(npc_rv, "me")) return;

	NpcTracker& tracker = npc_trackers[it->second];
	tracker.time_last_checked = MMAPI::Game::GetCurrentTimeInSeconds();
	tracker.is_nearby         = true;
}

static void RegisterNpcTrackers()
{
	for (const auto& [npc_id, obj_name] : npc_object_names)
	{
		obj_name_to_npc_id[obj_name] = npc_id;
		MMAPI::Instance::Hooks::OnObjectCall(obj_name, OnNpcTick);
	}
}

// ----- Hooks -----

void OnAfterClockUpdate(MMAPI::Calendar::ClockUpdateContext& /*ctx*/)
{
	TickReminders();
}

void OnBeforeNewDay()
{
	for (Reminder& r : reminders) r.has_triggered = false;
}

void OnAfterGoToRoom(MMAPI::Location::AfterGoToRoomContext& /*ctx*/)
{
	npc_trackers.clear();
	for (Reminder& r : reminders)
		if (r.reset_after == ResetAfter::Location)
			r.has_triggered = false;
}

void OnAfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& ctx)
{
	if (ctx.GetKey() == REMINDER_NOTIFICATION_PLACEHOLDER_KEY)
		ctx.SetResolved(pending_notification_text);
}

void OnBeforeSetupMainScreen()
{
	npc_trackers.clear();
	pending_notification_text.clear();

	if (!startup_loaded)
	{
		LoadConfig();
		startup_loaded = true;
		return;
	}

	// Returning to title — re-arm reminders for the next save.
	for (Reminder& r : reminders) r.has_triggered = false;
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
	MMAPI::Calendar::Enable();
	MMAPI::Cutscene::Enable();
	MMAPI::Weather::Enable();
	MMAPI::Location::Enable();
	MMAPI::Animal::Enable();
	MMAPI::Text::Enable();
	MMAPI::Instance::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::BeforeNewDay(OnBeforeNewDay);
	MMAPI::Calendar::Hooks::AfterClockUpdate(OnAfterClockUpdate);
	MMAPI::Location::Hooks::AfterGoToRoom(OnAfterGoToRoom);
	MMAPI::Text::Hooks::AfterLocalizedString(OnAfterLocalizedString);

	RegisterNpcTrackers();

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
