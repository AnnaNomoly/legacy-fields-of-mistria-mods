#include <map>
#include <fstream>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "RemindMe";
static const char* const VERSION = "1.2.0";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const char* const GML_SCRIPT_GET_LOCALIZER = "gml_Script_get@Localizer@Localizer";
static const char* const GML_SCRIPT_GO_TO_ROOM = "gml_Script_goto_gm_room";
static const char* const GML_SCRIPT_GET_MINUTES = "gml_Script_get_minutes";
static const char* const GML_SCRIPT_CALENDAR_DAY = "gml_Script_day@Calendar@Calendar";
static const char* const GML_SCRIPT_CALENDAR_SEASON = "gml_Script_season@Calendar@Calendar";
static const char* const GML_SCRIPT_CALENDAR_YEAR = "gml_Script_year@Calendar@Calendar";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_NEW_DAY = "gml_Script_new_day";
static const char* const GML_SCRIPT_CUTSCENE_IS_RUNNING = "gml_Script_is_running@Mist@Mist";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const std::string REMINDER_NOTIFICATION_PLACEHOLDER_KEY = "Notifications/Mods/Remind Me/reminder_notification_placeholder";
static const std::string REMINDERS_KEY = "reminders";
static const std::string CONDITIONS_KEY = "conditions";
static const std::string NOTIFICATION_KEY = "notification";
static const std::string TWENTY_SIX_HOUR_TIME_KEY = "26h_time";
static const std::string DAY_OF_WEEK_KEY = "day_of_week";
static const std::string DAY_OF_MONTH_KEY = "day_of_month";
static const std::string SEASON_KEY = "season";
static const std::string YEAR_KEY = "year";
static const std::string WEATHER_KEY = "weather";
static const std::string LOCATION_KEY = "location";
static const std::string NPC_NEARBY_KEY = "npc_nearby";
static const std::string NPC_GIFTABLE_KEY = "npc_giftable";
static const std::string FARM_ANIMAL_OUTSIDE_KEY = "farm_animal_outside";
static const std::string RESET_AFTER_KEY = "reset_after";
static const std::string CAN_TRIGGER_AFTER_KEY = "can_trigger_after";

static enum class ResetAfter {
	TIME,
	LOCATION,
	DAY // TODO: Hook a script for new day, iterate over ALL reminders, and set reset_after_time_incremement = -1 and has_triggered = false.
};
static const std::string LOCATION = "LOCATION";
static const std::string DAY = "DAY";

static const std::string MONDAY = "MONDAY";
static const std::string TUESDAY = "TUESDAY";
static const std::string WEDNESDAY = "WEDNESDAY";
static const std::string THURSDAY = "THURSDAY";
static const std::string FRIDAY = "FRIDAY";
static const std::string SATURDAY = "SATURDAY";
static const std::string SUNDAY = "SUNDAY";
static const std::unordered_set<std::string> DAYS_OF_THE_WEEK = {
	MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY
};

static const std::string SPRING = "SPRING";
static const std::string SUMMER = "SUMMER";
static const std::string FALL = "FALL";
static const std::string WINTER = "WINTER";
static const std::unordered_set<std::string> SEASONS = {
	SPRING, SUMMER, FALL, WINTER
};

static const std::string CALM_WEATHER = "CALM"; // Sunny
static const std::string INCLEMENT_WEATHER = "INCLEMENT"; // Rain or Snow
static const std::string SEVERE_WEATHER = "SEVERE"; // Storm or Blizzard
static const std::string SPECIAL_WEATHER = "SPECIAL"; // Breezy
static const std::unordered_set<std::string> TYPES_OF_WEATHER = {
	CALM_WEATHER, INCLEMENT_WEATHER, SEVERE_WEATHER, SPECIAL_WEATHER
};

static const std::string WESTERN_RUINS = "WESTERN RUINS";
static const std::string NARROWS = "NARROWS";
static const std::string SWEETWATER_FARM = "SWEETWATER FARM";
static const std::string BEACH = "BEACH";
static const std::string TOWN = "TOWN";
static const std::string EASTERN_ROAD = "EASTERN ROAD";
static const std::string DEEP_WOODS = "DEEP WOODS";
static const std::string FARM = "FARM";
static const std::string INN = "INN";
static const std::string DUNGEON = "DUNGEON";
static const std::unordered_set<std::string> LOCATIONS = {
	WESTERN_RUINS, NARROWS, SWEETWATER_FARM, BEACH, TOWN, EASTERN_ROAD, DEEP_WOODS, FARM, INN, DUNGEON
};

static const std::string ADELINE = "adeline";
static const std::string BALOR = "balor";
static const std::string CALDARUS = "caldarus";
static const std::string CELINE = "celine";
static const std::string DARCY = "darcy";
static const std::string DELL = "dell";
static const std::string DOZY = "dozy";
static const std::string EILAND = "eiland";
static const std::string ELSIE = "elsie";
static const std::string ERROL = "errol";
static const std::string HAYDEN = "hayden";
static const std::string HEMLOCK = "hemlock";
static const std::string HENRIETTA = "henrietta";
static const std::string HOLT = "holt";
static const std::string JOSEPHINE = "josephine";
static const std::string JUNIPER = "juniper";
static const std::string LANDEN = "landen";
static const std::string LOUIS = "louis";
static const std::string LUC = "luc";
static const std::string MAPLE = "maple";
static const std::string MARCH = "march";
static const std::string MERRI = "merri";
static const std::string NORA = "nora";
static const std::string OLRIC = "olric";
static const std::string REINA = "reina";
static const std::string RYIS = "ryis";
static const std::string SERIDIA = "seridia";
//static const std::string STILLWELL = "stillwell";
static const std::string TALIFERRO = "taliferro";
static const std::string TERITHIA = "terithia";
static const std::string VALEN = "valen";
static const std::string VERA = "vera";
static const std::string WHEEDLE = "wheedle";
//static const std::string ZOREL = "zorel";
static const std::unordered_set<std::string> NPC_NAMES = { // As of 0.15
	ADELINE, BALOR, CALDARUS, CELINE, DARCY, DELL, DOZY, EILAND,
	ELSIE, ERROL, HAYDEN, HEMLOCK, HENRIETTA, HOLT, JOSEPHINE, JUNIPER,
	LANDEN, LOUIS, LUC, MAPLE, MARCH, MERRI, NORA, OLRIC,
	REINA, RYIS, SERIDIA, TALIFERRO, TERITHIA, VALEN, VERA, WHEEDLE
};

static struct Reminder {
	int time = -1;
	int week_day = -1;
	int month_day = -1;
	int season = -1;
	int year = -1;
	int weather = -1;
	int location = -1;
	int npc = -1;
	int last_triggered_time = -1;
	int reset_after_time_incremement = -1;
	bool npc_giftable = false; // Ignored unless explicitly set to true
	bool farm_animal_outside = false; // Ignored unless explicitly set to true
	bool has_triggered = false;
	bool can_trigger_after = false;
	ResetAfter reset_after = ResetAfter::DAY;
	std::string notification;
	
	bool HasTimeCondition(){ return time != -1; }
	bool HasWeekDayCondition() { return week_day != -1; }
	bool HasMonthDayCondition() { return month_day != -1; }
	bool HasSeasonCondition() { return season != -1; }
	bool HasYearCondition() { return year != -1; }
	bool HasWeatherCondition() { return weather != -1; }
	bool HasLocationCondition() { return location != -1; }
	bool HasNpcCondition() { return npc != -1; }
};

static struct NpcTracker {
	int time_last_checked = -1;
	bool is_nearby = false;
};

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static bool game_is_active = false;
static bool cutscene_is_running = false;
static int current_time_in_seconds = -1;
static int current_day = -1;
static int current_season = -1;
static int current_year = -1;
static int current_weather = -1;
static std::string ari_current_gm_room = "";
static std::vector<Reminder> reminders = {};
static std::string notification_text = "";
static std::map<std::string, NpcTracker> npc_name_to_tracker_map = {};
static std::map<std::string, int> npc_name_to_id_map = {}; // __npc_id__
static std::map<int, std::string> npc_id_to_name_map = {}; // __npc_id__
static std::map<std::string, int> weather_name_to_id_map = {}; // __weather__
static std::map<std::string, int> location_name_to_id_map = {}; // __location_id__
static std::map<std::string, int> gm_room_name_to_location_id_map = {};

void ResetStaticFields(bool title_screen)
{
	if (title_screen)
	{
		for (Reminder& reminder : reminders)
			reminder.has_triggered = false;
	}

	game_is_active = false;
	cutscene_is_running = false;
	current_time_in_seconds = -1;
	current_day = -1;
	current_season = -1;
	current_year = -1;
	current_weather = -1;
	ari_current_gm_room = "";
	notification_text = "";
	npc_name_to_tracker_map = {};
}

uint64_t GetCurrentSystemTime() {
	return duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

bool GameIsPaused()
{
	RValue paused = global_instance->GetMember("__pause_status");
	return paused.ToInt64() > 0;
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return struct_exists.ToBoolean();
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

std::string to_upper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::toupper(c); });
	return s;
}

std::string to_lower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return s;
}

void LoadNpcs()
{
	size_t array_length = 0;
	RValue npc_data = global_instance->GetMember("__npc_id__");
	g_ModuleInterface->GetArraySize(npc_data, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(npc_data, i, array_element);
		npc_name_to_id_map[array_element->ToString()] = i;
		npc_id_to_name_map[i] = array_element->ToString();
	}
}

void LoadWeather()
{
	size_t array_length = 0;
	RValue weather = global_instance->GetMember("__weather__");
	g_ModuleInterface->GetArraySize(weather, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(weather, i, array_element);
		weather_name_to_id_map[array_element->ToString()] = i;
	}
}

void LoadLocations()
{
	size_t array_length = 0;
	RValue locations = global_instance->GetMember("__location_id__");
	g_ModuleInterface->GetArraySize(locations, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(locations, i, array_element);

		std::string location_name = array_element->ToString();
		location_name_to_id_map[location_name] = i;

		if (location_name == "western_ruins")
			gm_room_name_to_location_id_map["rm_western_ruins"] = i;
		else if (location_name == "narrows")
			gm_room_name_to_location_id_map["rm_narrows"] = i;
		else if (location_name == "haydens_farm")
			gm_room_name_to_location_id_map["rm_haydens_farm"] = i;
		else if (location_name == "beach")
			gm_room_name_to_location_id_map["rm_beach"] = i;
		else if (location_name == "town")
			gm_room_name_to_location_id_map["rm_town"] = i;
		else if (location_name == "eastern_road")
			gm_room_name_to_location_id_map["rm_eastern_road"] = i;
		else if (location_name == "deep_woods")
			gm_room_name_to_location_id_map["rm_deep_woods"] = i;
		else if (location_name == "farm")
			gm_room_name_to_location_id_map["rm_farm"] = i;
		else if (location_name == "inn")
			gm_room_name_to_location_id_map["rm_inn"] = i;
		// TODO: We can't hardcode a single ID for the dungeon. Instead, we'll have to just map all GM rooms prefixed with "rm_mines" (except entrance) => location_name_to_id_map["dungeon"]
		/*else if (location_name == "dungeon")
			gm_room_name_to_location_id_map["rm_"] = i;*/
	}
}

bool IsValidTime(const std::string& s)
{
	// Must be exactly "HH:MM"
	if (s.size() != 5)
		return false;

	// Check fixed format
	if (!std::isdigit(s[0]) || !std::isdigit(s[1]) || s[2] != ':' || !std::isdigit(s[3]) || !std::isdigit(s[4]))
		return false;

	// Convert to numbers
	int hour = (s[0] - '0') * 10 + (s[1] - '0');
	int minute = (s[3] - '0') * 10 + (s[4] - '0');

	// Validate ranges
	if (hour < 6 || hour > 26)
		return false;
	if (minute < 0 || minute > 59)
		return false;
	if (hour == 26 && minute > 0)
		return false;

	return true;
}

bool IsValidDayOfWeek(const std::string& s)
{
	if (s.size() == 0)
		return false;

	std::string s_upper = to_upper(s);
	return DAYS_OF_THE_WEEK.contains(s_upper);
}

bool IsValidSeason(const std::string& s)
{
	if (s.size() == 0)
		return false;

	std::string s_upper = to_upper(s);
	return SEASONS.contains(s_upper);
}

bool IsValidWeather(const std::string& s)
{
	if (s.size() == 0)
		return false;

	std::string s_upper = to_upper(s);
	return TYPES_OF_WEATHER.contains(s_upper);
}

bool IsValidLocation(const std::string& s)
{
	if (s.size() == 0)
		return false;

	std::string s_upper = to_upper(s);
	return LOCATIONS.contains(s_upper);
}

bool IsValidNPC(const std::string& s)
{
	if (s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return NPC_NAMES.contains(s_lower);
}

// Valid values are "HH:MM", "NEW_LOCATION", "NEW_DAY"
std::optional<ResetAfter> IsValidResetAfter(const std::string& s)
{
	// "HH:MM"
	if (s.size() == 5)
	{
		// Check fixed format
		if (!std::isdigit(s[0]) || !std::isdigit(s[1]) || s[2] != ':' || !std::isdigit(s[3]) || !std::isdigit(s[4]))
			return std::nullopt;

		// Convert to numbers
		int hour = (s[0] - '0') * 10 + (s[1] - '0');
		int minute = (s[3] - '0') * 10 + (s[4] - '0');

		// Validate ranges
		if (hour < 0 || hour > 20)
			return std::nullopt;
		if (minute < 0 || minute > 59)
			return std::nullopt;
		if (hour == 0 && minute < 10)
			return std::nullopt;
		if (hour == 20 && minute > 0)
			return std::nullopt;

		return ResetAfter::TIME;
	}

	if (s == LOCATION)
		return ResetAfter::LOCATION;
	if (s == DAY)
		return ResetAfter::DAY;

	return std::nullopt;
}

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}

json CreateDefaultConfigJson()
{
	// Build the JSON
	json json_reminder_0 = {};
	json json_reminder_0_conditions = {};
	json_reminder_0_conditions[WEATHER_KEY] = SPECIAL_WEATHER;
	json_reminder_0_conditions[SEASON_KEY] = SPRING;
	json_reminder_0_conditions[CAN_TRIGGER_AFTER_KEY] = true;
	json_reminder_0[NOTIFICATION_KEY] = "The legendary fish is in season! (Perk Required)";
	json_reminder_0[CONDITIONS_KEY] = json_reminder_0_conditions;

	json json_reminder_1 = {};
	json json_reminder_1_conditions = {};
	json_reminder_1_conditions[SEASON_KEY] = WINTER;
	json_reminder_1_conditions[DAY_OF_MONTH_KEY] = 18;
	json_reminder_1_conditions[CAN_TRIGGER_AFTER_KEY] = true;
	json_reminder_1[NOTIFICATION_KEY] = "Today is Adeline's birthday!";
	json_reminder_1[CONDITIONS_KEY] = json_reminder_1_conditions;

	json config_json = {};
	std::vector<json> reminders = { json_reminder_0, json_reminder_1 };
	config_json[REMINDERS_KEY] = reminders;

	// Build the actual Reminder objects
	Reminder reminder_0 = Reminder();
	reminder_0.weather = weather_name_to_id_map["special"];
	reminder_0.season = 1;

	return config_json;
}

void CreateOrLoadConfigFile()
{
	// Load config file.
	std::exception_ptr eptr;
	try
	{
		// Try to find the mod_data directory.
		std::string current_dir = std::filesystem::current_path().string();
		std::string mod_data_folder = current_dir + "\\mod_data";
		if (!std::filesystem::exists(mod_data_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
			std::filesystem::create_directory(mod_data_folder);
		}

		// Try to find the mod_data/RemindMe directory.
		std::string remind_me_folder = mod_data_folder + "\\RemindMe";
		if (!std::filesystem::exists(remind_me_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"RemindMe\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, remind_me_folder.c_str());
			std::filesystem::create_directory(remind_me_folder);
		}

		// Try to find the mod_data/RemindMe/RemindMe.json config file.
		bool create_config_file = false;
		std::string config_file = remind_me_folder + "\\" + "RemindMe.json";
		std::ifstream in_stream(config_file);
		if (in_stream.good())
		{
			try
			{
				json json_object = json::parse(in_stream);

				// Check if the json_object is empty.
				if (json_object.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.c_str());
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Creating a default configuration file.", MOD_NAME, VERSION);
					create_config_file = true;
				}
				else
				{
					// Try loading the reminders value.
					if (json_object.contains(REMINDERS_KEY))
					{
						std::vector<json> json_reminders = json_object[REMINDERS_KEY];
						if (!json_reminders.empty())
						{
							for (json json_reminder : json_reminders)
							{
								if (!json_reminder.contains(CONDITIONS_KEY))
								{
									g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing \"%s\" value in a reminder!", MOD_NAME, VERSION, CONDITIONS_KEY.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The reminder will be ignored.", MOD_NAME, VERSION);
									continue;
								}

								if (!json_reminder.contains(NOTIFICATION_KEY))
								{
									g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing \"%s\" value in a reminder!", MOD_NAME, VERSION, NOTIFICATION_KEY.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The reminder will be ignored.", MOD_NAME, VERSION);
									continue;
								}

								json reminder_conditions = json_reminder[CONDITIONS_KEY];
								if (reminder_conditions.empty())
								{
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - No conditions were set in a reminder!", MOD_NAME, VERSION, REMINDERS_KEY.c_str(), config_file.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The reminder will be ignored.", MOD_NAME, VERSION);
									continue;
								}

								std::string reminder_notification = json_reminder[NOTIFICATION_KEY];
								if (reminder_notification.size() == 0)
								{
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - No notification was set in a reminder!", MOD_NAME, VERSION, REMINDERS_KEY.c_str(), config_file.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The reminder will be ignored.", MOD_NAME, VERSION);
									continue;
								}

								Reminder reminder = Reminder();
								reminder.notification = reminder_notification;

								// 26h_time condition.
								if (reminder_conditions.contains(TWENTY_SIX_HOUR_TIME_KEY))
								{
									std::string time_str = reminder_conditions[TWENTY_SIX_HOUR_TIME_KEY];
									if (IsValidTime(time_str))
									{
										int hour = (time_str[0] - '0') * 10 + (time_str[1] - '0');
										int minute = (time_str[3] - '0') * 10 + (time_str[4] - '0');
										reminder.time = (hour * 60 * 60) + (minute * 60);
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, TWENTY_SIX_HOUR_TIME_KEY.c_str(), time_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// day_of_week condition.
								if (reminder_conditions.contains(DAY_OF_WEEK_KEY))
								{
									std::string day_of_week_str = reminder_conditions[DAY_OF_WEEK_KEY];
									if (IsValidDayOfWeek(day_of_week_str))
									{
										day_of_week_str = to_upper(day_of_week_str);
										if (day_of_week_str == MONDAY) // NOTE: The game starts on a Monday
											reminder.week_day = 0; // This will correspond to current_day - 1 % 7 == 0
										if (day_of_week_str == TUESDAY)
											reminder.week_day = 1; // This will correspond to current_day - 1 % 7 == 1
										if (day_of_week_str == WEDNESDAY)
											reminder.week_day = 2; // This will correspond to current_day - 1 % 7 == 2
										if (day_of_week_str == THURSDAY)
											reminder.week_day = 3; // This will correspond to current_day - 1 % 7 == 3
										if (day_of_week_str == FRIDAY)
											reminder.week_day = 4; // This will correspond to current_day - 1 % 7 == 4
										if (day_of_week_str == SATURDAY)
											reminder.week_day = 5; // This will correspond to current_day - 1 % 7 == 5
										if (day_of_week_str == SUNDAY)
											reminder.week_day = 6; // This will correspond to current_day - 1 % 7 == 6
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, DAY_OF_WEEK_KEY.c_str(), day_of_week_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// day_of_month condition.
								if (reminder_conditions.contains(DAY_OF_MONTH_KEY))
								{
									int day_of_month = reminder_conditions[DAY_OF_MONTH_KEY];
									if (day_of_month > 0 && day_of_month < 29)
									{
										reminder.month_day = day_of_month; // This will correspond to current_day == <value>
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %d!", MOD_NAME, VERSION, DAY_OF_MONTH_KEY.c_str(), day_of_month);
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// season condition.
								if (reminder_conditions.contains(SEASON_KEY))
								{
									std::string season_str = reminder_conditions[SEASON_KEY];
									if (IsValidSeason(season_str))
									{
										season_str = to_upper(season_str);
										if (season_str == SPRING)
											reminder.season = 0; // This will correspond to current_season == 0 (SPRING)
										if (season_str == SUMMER)
											reminder.season = 1; // This will correspond to current_season == 1 (SUMMER)
										if (season_str == FALL)
											reminder.season = 2; // This will correspond to current_season == 2 (FALL)
										if (season_str == WINTER)
											reminder.season = 3; // This will correspond to current_season == 3 (WINTER)
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, SEASON_KEY.c_str(), season_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// year condition.
								if (reminder_conditions.contains(YEAR_KEY))
								{
									int year = reminder_conditions[YEAR_KEY];
									if (year > 0)
									{
										reminder.year = year; // This will correspond to current_year == <value>
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %d!", MOD_NAME, VERSION, YEAR_KEY.c_str(), year);
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// weather condition.
								if (reminder_conditions.contains(WEATHER_KEY))
								{
									std::string weather_str = reminder_conditions[WEATHER_KEY];
									if (IsValidWeather(weather_str))
									{
										weather_str = to_upper(weather_str);
										if (weather_str == CALM_WEATHER)
											reminder.weather = weather_name_to_id_map["calm"]; // This will correspond to current_weather == CALM
										if (weather_str == INCLEMENT_WEATHER)
											reminder.weather = weather_name_to_id_map["inclement"]; // This will correspond to current_weather == INCLEMENT
										if (weather_str == SEVERE_WEATHER)
											reminder.weather = weather_name_to_id_map["heavy_inclement"]; // This will correspond to current_weather == HEAVY_INCLEMENT
										if (weather_str == SPECIAL_WEATHER)
											reminder.weather = weather_name_to_id_map["special"]; // This will correspond to current_weather == SPECIAL
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, WEATHER_KEY.c_str(), weather_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// location condition.
								if (reminder_conditions.contains(LOCATION_KEY))
								{
									std::string current_location_str = reminder_conditions[LOCATION_KEY];
									if (IsValidLocation(current_location_str))
									{
										current_location_str = to_upper(current_location_str);
										if (current_location_str == WESTERN_RUINS)
											reminder.location = location_name_to_id_map["western_ruins"]; // This will correspond to gm_room_name_to_location_id_map[ari_current_gm_room] == <value>
										if (current_location_str == NARROWS)
											reminder.location = location_name_to_id_map["narrows"];
										if (current_location_str == SWEETWATER_FARM)
											reminder.location = location_name_to_id_map["haydens_farm"];
										if (current_location_str == BEACH)
											reminder.location = location_name_to_id_map["beach"];
										if (current_location_str == TOWN)
											reminder.location = location_name_to_id_map["town"];
										if (current_location_str == EASTERN_ROAD)
											reminder.location = location_name_to_id_map["eastern_road"];
										if (current_location_str == DEEP_WOODS)
											reminder.location = location_name_to_id_map["deep_woods"];
										if (current_location_str == FARM)
											reminder.location = location_name_to_id_map["farm"];
										if (current_location_str == INN)
											reminder.location = location_name_to_id_map["inn"];
										if (current_location_str == DUNGEON)
											reminder.location = location_name_to_id_map["dungeon"];
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, LOCATION_KEY.c_str(), current_location_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// npc condition.
								if (reminder_conditions.contains(NPC_NEARBY_KEY))
								{
									std::string npc_str = reminder_conditions[NPC_NEARBY_KEY];
									if (IsValidNPC(npc_str))
									{
										npc_str = to_lower(npc_str);
										reminder.npc = npc_name_to_id_map[npc_str];
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, LOCATION_KEY.c_str(), npc_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// npc_giftable condition.
								if (reminder_conditions.contains(NPC_GIFTABLE_KEY))
								{
									if (reminder_conditions.at(NPC_GIFTABLE_KEY).is_boolean())
									{
										reminder.npc_giftable = reminder_conditions[NPC_GIFTABLE_KEY];
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition!", MOD_NAME, VERSION, NPC_GIFTABLE_KEY.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// farm_animal_outside condition.
								if (reminder_conditions.contains(FARM_ANIMAL_OUTSIDE_KEY))
								{
									if (reminder_conditions.at(FARM_ANIMAL_OUTSIDE_KEY).is_boolean())
									{
										reminder.farm_animal_outside = reminder_conditions[FARM_ANIMAL_OUTSIDE_KEY];
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition!", MOD_NAME, VERSION, FARM_ANIMAL_OUTSIDE_KEY.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will be ignored.", MOD_NAME, VERSION);
									}
								}

								// can_trigger_after instruction.
								if (reminder_conditions.contains(CAN_TRIGGER_AFTER_KEY))
								{
									bool can_trigger_after = reminder_conditions[CAN_TRIGGER_AFTER_KEY];
									reminder.can_trigger_after = can_trigger_after;
								}
								else
								{
									g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing \"%s\" value in a condition!", MOD_NAME, VERSION, CAN_TRIGGER_AFTER_KEY.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will use the default \"%s\" value.", MOD_NAME, VERSION, CAN_TRIGGER_AFTER_KEY.c_str());
								}

								// reset_after instruction.
								if (reminder_conditions.contains(RESET_AFTER_KEY))
								{
									std::string reset_after_str = reminder_conditions[RESET_AFTER_KEY];
									auto opt_reset_after = IsValidResetAfter(reset_after_str);
									if (opt_reset_after.has_value())
									{
										ResetAfter reset_after = opt_reset_after.value();
										reminder.reset_after = reset_after;

										if (reset_after == ResetAfter::TIME)
										{
											int hour = (reset_after_str[0] - '0') * 10 + (reset_after_str[1] - '0');
											int minute = (reset_after_str[3] - '0') * 10 + (reset_after_str[4] - '0');
											reminder.reset_after_time_incremement = (hour * 60 * 60) + (minute * 60);
										}
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid \"%s\" value in a condition: %s!", MOD_NAME, VERSION, RESET_AFTER_KEY.c_str(), reset_after_str.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will use the default \"%s\" value.", MOD_NAME, VERSION, RESET_AFTER_KEY.c_str());
									}
								}
								else
								{
									g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing \"%s\" value in a condition!", MOD_NAME, VERSION, RESET_AFTER_KEY.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The condition will use the default \"%s\" value.", MOD_NAME, VERSION, RESET_AFTER_KEY.c_str());
								}
								
								// Final validation. Confirm the reminder has at least one valid condition.
								if (reminder.HasTimeCondition() || reminder.HasWeekDayCondition() || reminder.HasMonthDayCondition() || reminder.HasSeasonCondition() || reminder.HasYearCondition() || reminder.HasWeatherCondition() || reminder.HasLocationCondition() || reminder.HasNpcCondition() || reminder.farm_animal_outside)
									reminders.push_back(reminder);
								else
								{
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - No conditions were set in a reminder!", MOD_NAME, VERSION, REMINDERS_KEY.c_str(), config_file.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The reminder will be ignored.", MOD_NAME, VERSION);
								}
							}
						}
						else
						{
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - No reminders were set in mod configuration file: %s!", MOD_NAME, VERSION, REMINDERS_KEY.c_str(), config_file.c_str());
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Creating a default configuration file.", MOD_NAME, VERSION);
							create_config_file = true;
						}
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, REMINDERS_KEY.c_str(), config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Creating a default configuration file.", MOD_NAME, VERSION);
						create_config_file = true;
					}

				}
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();
			create_config_file = true;
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"RemindMe.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());
		}

		if (create_config_file)
		{
			json config_json = CreateDefaultConfigJson();
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << config_json << std::endl;
			out_stream.close();
		}
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);
	}
}

void CreateNotification(std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_NOTIFICATION,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = RValue(notification_localization_str);
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);
}

RValue GetAllAnimals(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_all_animals = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_get_all_animals",
		(PVOID*)&gml_script_get_all_animals
	);

	RValue all_animals;
	gml_script_get_all_animals->m_Functions->m_ScriptFunction(
		Self,
		Other,
		all_animals,
		0,
		nullptr
	);

	return all_animals;
}

bool AnyFarmAnimalOutside(CInstance* Self, CInstance* Other)
{
	RValue all_animals = GetAllAnimals(Self, Other);
	if (all_animals.m_Kind == VALUE_OBJECT)
	{
		RValue __buffer = *all_animals.GetRefMember("__buffer");
		if (__buffer.m_Kind == VALUE_ARRAY)
		{
			size_t size = 0;
			g_ModuleInterface->GetArraySize(__buffer, size);

			for (size_t i = 0; i < size; i++)
			{
				RValue entry = __buffer[i];
				if (entry.m_Kind == VALUE_OBJECT && StructVariableExists(entry, "location_position"))
				{
					RValue location_position = entry.GetMember("location_position");
					if (location_position.m_Kind == VALUE_OBJECT && StructVariableExists(location_position, "location_id"))
					{
						RValue location_id = location_position.GetMember("location_id");
						if (IsNumeric(location_id) && location_id.ToInt64() == location_name_to_id_map["farm"])
							return true;
					}
				}
			}
		}
	}

	return false;
}

void TrackNPC(CInstance* npc, std::string npc_name)
{
	RValue me_exists = g_ModuleInterface->CallBuiltin("struct_exists", { npc, "me" });
	if (me_exists.m_Kind == VALUE_BOOL && me_exists.m_Real == 1)
		npc_name_to_tracker_map[npc_name] = { current_time_in_seconds, true };
}

void UpdateTrackedNPCs()
{
	for (auto& entry : npc_name_to_tracker_map)
		if (current_time_in_seconds > entry.second.time_last_checked + 300) // 5m
			entry.second.is_nearby = false;
}

bool NpcGiftable(int npc_id)
{
	RValue npc_data = global_instance->GetMember("__npc_database");
	RValue npc = npc_data[npc_id];

	if (npc.m_Kind == VALUE_OBJECT && StructVariableExists(npc, "gift_flag"))
		return npc.GetMember("gift_flag").ToBoolean();

	return false;
}

void ObjectCallback(
	IN FWCodeEvent& CodeEvent
)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self)
		return;

	if (!self->m_Object)
		return;

	if (game_is_active && !GameIsPaused() && !cutscene_is_running)
	{
		UpdateTrackedNPCs();

		if (strstr(self->m_Object->m_Name, "obj_ari"))
		{
			// TODO
		}
		else if (strstr(self->m_Object->m_Name, "obj_adeline"))
			TrackNPC(self, ADELINE);
		else if (strstr(self->m_Object->m_Name, "obj_balor"))
			TrackNPC(self, BALOR);
		else if (strstr(self->m_Object->m_Name, "obj_caldarus"))
			TrackNPC(self, CALDARUS);
		else if (strstr(self->m_Object->m_Name, "obj_celine"))
			TrackNPC(self, CELINE);
		else if (strstr(self->m_Object->m_Name, "obj_darcy"))
			TrackNPC(self, DARCY);
		else if (strstr(self->m_Object->m_Name, "obj_dell"))
			TrackNPC(self, DELL);
		else if (strstr(self->m_Object->m_Name, "obj_dozy"))
			TrackNPC(self, DOZY);
		else if (strstr(self->m_Object->m_Name, "obj_eiland"))
			TrackNPC(self, EILAND);
		else if (strstr(self->m_Object->m_Name, "obj_elsie"))
			TrackNPC(self, ELSIE);
		else if (strstr(self->m_Object->m_Name, "obj_errol"))
			TrackNPC(self, ERROL);
		else if (strstr(self->m_Object->m_Name, "obj_hayden"))
			TrackNPC(self, HAYDEN);
		else if (strstr(self->m_Object->m_Name, "obj_hemlock"))
			TrackNPC(self, HEMLOCK);
		else if (strstr(self->m_Object->m_Name, "obj_henrietta"))
			TrackNPC(self, HENRIETTA);
		else if (strstr(self->m_Object->m_Name, "obj_holt"))
			TrackNPC(self, HOLT);
		else if (strstr(self->m_Object->m_Name, "obj_josephine"))
			TrackNPC(self, JOSEPHINE);
		else if (strstr(self->m_Object->m_Name, "obj_juniper"))
			TrackNPC(self, JUNIPER);
		else if (strstr(self->m_Object->m_Name, "obj_landen"))
			TrackNPC(self, LANDEN);
		else if (strstr(self->m_Object->m_Name, "obj_louis"))
			TrackNPC(self, LOUIS);
		else if (strstr(self->m_Object->m_Name, "obj_luc"))
			TrackNPC(self, LUC);
		else if (strstr(self->m_Object->m_Name, "obj_maple"))
			TrackNPC(self, MAPLE);
		else if (strstr(self->m_Object->m_Name, "obj_march"))
			TrackNPC(self, MARCH);
		else if (strstr(self->m_Object->m_Name, "obj_merri"))
			TrackNPC(self, MERRI);
		else if (strstr(self->m_Object->m_Name, "obj_nora"))
			TrackNPC(self, NORA);
		else if (strstr(self->m_Object->m_Name, "obj_olric"))
			TrackNPC(self, OLRIC);
		else if (strstr(self->m_Object->m_Name, "obj_reina"))
			TrackNPC(self, REINA);
		else if ( strstr(self->m_Object->m_Name, "obj_ryis"))
			TrackNPC(self, RYIS);
		else if (strstr(self->m_Object->m_Name, "obj_seridia"))
			TrackNPC(self, SERIDIA);
		//else if (strstr(self->m_Object->m_Name, "obj_stillwell"))
		//	TrackNPC(self, STILLWELL);
		else if (strstr(self->m_Object->m_Name, "obj_taliferro"))
			TrackNPC(self, TALIFERRO);
		else if (strstr(self->m_Object->m_Name, "obj_terithia"))
			TrackNPC(self, TERITHIA);
		else if (strstr(self->m_Object->m_Name, "obj_valen"))
			TrackNPC(self, VALEN);
		else if (strstr(self->m_Object->m_Name, "obj_vera"))
			TrackNPC(self, VERA);
		else if (strstr(self->m_Object->m_Name, "obj_wheedle"))
			TrackNPC(self, WHEEDLE);
		//else if (strstr(self->m_Object->m_Name, "obj_zorel"))
		//	TrackNPC(self, ZOREL);
	}

}

RValue& GmlScriptGetLocalizerCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_LOCALIZER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active)
	{
		if (Arguments[0]->ToString() == REMINDER_NOTIFICATION_PLACEHOLDER_KEY)
		{
			Result = RValue(notification_text);
			return Result;
		}

	}

	return Result;
}

RValue& GmlScriptGoToRoomCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GO_TO_ROOM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	RValue gm_room = Result.GetMember("gm_room");
	RValue room_name = g_ModuleInterface->CallBuiltin("room_get_name", { gm_room });
	ari_current_gm_room = room_name.ToString();

	if (ari_current_gm_room != "rm_mines_entry" && (ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal")) && !gm_room_name_to_location_id_map.contains(ari_current_gm_room))
		gm_room_name_to_location_id_map[ari_current_gm_room] = location_name_to_id_map["dungeon"];

	npc_name_to_tracker_map.clear();

	for (Reminder& reminder : reminders)
		if (reminder.reset_after == ResetAfter::LOCATION)
			reminder.has_triggered = false;
	
	return Result;
}

RValue& GmlScriptGetMinutesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MINUTES));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && !GameIsPaused() && !cutscene_is_running)
	{
		RValue time = global_instance->GetMember("__clock").GetMember("time");
		current_time_in_seconds = time.ToInt64();

		// TODO
		for (Reminder& reminder : reminders)
		{
			if (reminder.reset_after == ResetAfter::TIME && reminder.last_triggered_time != -1 && current_time_in_seconds > (reminder.last_triggered_time + reminder.reset_after_time_incremement))
				reminder.has_triggered = false;
			
			if (reminder.has_triggered)
				continue;

			bool trigger = true;
			if (reminder.HasTimeCondition())
			{
				if (reminder.can_trigger_after)
					trigger &= current_time_in_seconds >= reminder.time;
				else
					trigger &= (current_time_in_seconds >= reminder.time && current_time_in_seconds < reminder.time + 300); // TODO: This may need more tuning
			}
			if (reminder.HasWeekDayCondition())
				trigger &= reminder.week_day == (current_day - 1) % 7;
			if (reminder.HasMonthDayCondition())
				trigger &= reminder.month_day == current_day;
			if (reminder.HasSeasonCondition())
				trigger &= reminder.season == current_season;
			if (reminder.HasYearCondition())
				trigger &= reminder.year == current_year;
			if (reminder.HasWeatherCondition())
				trigger &= reminder.weather == current_weather;
			if (reminder.HasLocationCondition())
				trigger &= reminder.location == gm_room_name_to_location_id_map[ari_current_gm_room];
			if (reminder.HasNpcCondition())
				trigger &= npc_name_to_tracker_map[npc_id_to_name_map[reminder.npc]].is_nearby;
			if (reminder.npc_giftable && reminder.HasNpcCondition())
				trigger &= NpcGiftable(reminder.npc);
			if (reminder.farm_animal_outside)
				trigger &= AnyFarmAnimalOutside(Self, Other);

			if (trigger)
			{
				reminder.has_triggered = true;
				reminder.last_triggered_time = current_time_in_seconds;
				notification_text = reminder.notification;
				CreateNotification(REMINDER_NOTIFICATION_PLACEHOLDER_KEY, Self, Other);
			}
		}
	}

	return Result;
}

RValue& GmlScriptCalendarDayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CALENDAR_DAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active)
		current_day = Result.ToInt64() + 1; // Result is a VALUE_REAL that is the 0-indexed calendar day

	return Result;
}

RValue& GmlScriptCalendarSeasonCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CALENDAR_SEASON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active)
		current_season = Result.ToInt64(); // Result is a VALUE_REAL that is the 0-indexed calendar season

	return Result;
}

RValue& GmlScriptCalendarYearCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CALENDAR_YEAR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active)
		current_year = Result.ToInt64() + 1; // Result is a VALUE_REAL that is the 0-indexed calendar year

	return Result;
}

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_WEATHER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (IsNumeric(Result))
		current_weather = Result.ToInt64();

	game_is_active = true;
	return Result;
}

RValue& GmlScriptNewDayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_NEW_DAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	for (Reminder& reminder : reminders)
		reminder.has_triggered = false;

	return Result;
}

RValue& GmlScriptCutsceneIsRunningCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CUTSCENE_IS_RUNNING));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && Result.m_Kind == VALUE_BOOL)
		cutscene_is_running = Result.ToBoolean();

	return Result;
}

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (load_on_start)
	{
		load_on_start = false;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		
		LoadNpcs();
		LoadWeather();
		LoadLocations();
		CreateOrLoadConfigFile();
	}
	else
		ResetStaticFields(true);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

void CreateObjectCallback(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook (EVENT_OBJECT_CALL)!", MOD_NAME, VERSION);
	}
}

void CreateHookGmlScriptGetLocalizer(AurieStatus& status)
{
	CScript* gml_script_get_localizer = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_LOCALIZER,
		gml_script_get_localizer->m_Functions->m_ScriptFunction,
		GmlScriptGetLocalizerCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}
}

void CreateHookGmlScriptGoToRoom(AurieStatus& status)
{
	CScript* gml_script_go_to_room = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GO_TO_ROOM,
		(PVOID*)&gml_script_go_to_room
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GO_TO_ROOM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GO_TO_ROOM,
		gml_script_go_to_room->m_Functions->m_ScriptFunction,
		GmlScriptGoToRoomCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GO_TO_ROOM);
	}
}

void CreateHookGmlScriptGetMinutes(AurieStatus& status)
{
	CScript* gml_script_get_minutes = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MINUTES,
		(PVOID*)&gml_script_get_minutes
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MINUTES);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_MINUTES,
		gml_script_get_minutes->m_Functions->m_ScriptFunction,
		GmlScriptGetMinutesCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MINUTES);
	}
}

void CreateHookGmlScriptCalendarDay(AurieStatus& status)
{
	CScript* gml_script_calendar_day = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CALENDAR_DAY,
		(PVOID*)&gml_script_calendar_day
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CALENDAR_DAY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CALENDAR_DAY,
		gml_script_calendar_day->m_Functions->m_ScriptFunction,
		GmlScriptCalendarDayCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CALENDAR_DAY);
	}
}

void CreateHookGmlScriptCalendarSeason(AurieStatus& status)
{
	CScript* gml_script_calendar_season = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CALENDAR_SEASON,
		(PVOID*)&gml_script_calendar_season
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CALENDAR_SEASON);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CALENDAR_SEASON,
		gml_script_calendar_season->m_Functions->m_ScriptFunction,
		GmlScriptCalendarSeasonCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CALENDAR_SEASON);
	}
}

void CreateHookGmlScriptCalendarYear(AurieStatus& status)
{
	CScript* gml_script_calendar_year = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CALENDAR_YEAR,
		(PVOID*)&gml_script_calendar_year
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CALENDAR_YEAR);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CALENDAR_YEAR,
		gml_script_calendar_year->m_Functions->m_ScriptFunction,
		GmlScriptCalendarYearCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CALENDAR_YEAR);
	}
}

void CreateHookGmlScriptGetWeather(AurieStatus& status)
{
	CScript* gml_script_get_weather = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_WEATHER,
		(PVOID*)&gml_script_get_weather
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_WEATHER,
		gml_script_get_weather->m_Functions->m_ScriptFunction,
		GmlScriptGetWeatherCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}
}

void CreateHookGmlScriptNewDay(AurieStatus& status)
{
	CScript* gml_script_new_day = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_NEW_DAY,
		(PVOID*)&gml_script_new_day
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NEW_DAY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_NEW_DAY,
		gml_script_new_day->m_Functions->m_ScriptFunction,
		GmlScriptNewDayCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NEW_DAY);
	}
}

void CreateHookGmlScriptCutsceneIsRunning(AurieStatus& status)
{
	CScript* gml_script_cutscene_is_running = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CUTSCENE_IS_RUNNING,
		(PVOID*)&gml_script_cutscene_is_running
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CUTSCENE_IS_RUNNING);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CUTSCENE_IS_RUNNING,
		gml_script_cutscene_is_running->m_Functions->m_ScriptFunction,
		GmlScriptCutsceneIsRunningCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CUTSCENE_IS_RUNNING);
	}
}

void CreateHookGmlScriptSetupMainScreen(AurieStatus& status)
{
	CScript* gml_script_setup_main_screen = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		(PVOID*)&gml_script_setup_main_screen
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		gml_script_setup_main_screen->m_Functions->m_ScriptFunction,
		GmlScriptSetupMainScreenCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateObjectCallback(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetLocalizer(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGoToRoom(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetMinutes(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCalendarDay(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCalendarSeason(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCalendarYear(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetWeather(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptNewDay(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCutsceneIsRunning(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSetupMainScreen(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}