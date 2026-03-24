#include <map>
#include <fstream>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "DynamicNpcPortraits";
static const char* const VERSION = "1.1.0";
static const char* const GML_SCRIPT_T2_READ = "gml_Script_read@T2r@T2r";
static const char* const GML_SCRIPT_IS_DUNGEON_ROOM = "gml_Script_is_dungeon_room";
static const char* const GML_SCRIPT_GO_TO_ROOM = "gml_Script_goto_gm_room";
static const char* const GML_SCRIPT_TRY_LOCATION_ID_TO_STRING = "gml_Script_try_location_id_to_string";
static const char* const GML_SCRIPT_GET_MINUTES = "gml_Script_get_minutes";
static const char* const GML_SCRIPT_CALENDAR_DAY = "gml_Script_day@Calendar@Calendar";
static const char* const GML_SCRIPT_CALENDAR_SEASON = "gml_Script_season@Calendar@Calendar";
static const char* const GML_SCRIPT_CALENDAR_YEAR = "gml_Script_year@Calendar@Calendar";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_CUTSCENE_IS_RUNNING = "gml_Script_is_running@Mist@Mist";
static const char* const GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR = "gml_Script_vertigo_draw_with_color";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const int SIX_AM_IN_SECONDS = 60 * 60 * 6;
static const int EIGHT_AM_IN_SECONDS = 60 * 60 * 8;
static const int TWELVE_PM_IN_SECONDS = 60 * 60 * 12;
static const int FIVE_PM_IN_SECONDS = 60 * 60 * 17;
static const int NINE_PM_IN_SECONDS = 60 * 60 * 21;
static const int TWELVE_AM_IN_SECONDS = 60 * 60 * 24;
static const std::string INDOORS = "indoors";
static const std::string OUTDOORS = "outdoors";
static const std::string MOD_NAME_KEY = "mod_name";
static const std::string DYNAMIC_PORTRAITS_KEY = "dynamic_portraits";
static const std::string CONDITIONS_KEY = "conditions";
static const std::string SPRITE_NAME_KEY = "sprite_name";
static const std::string TIME_OF_DAY_KEY = "time_of_day"; // DAY or NIGHT
static const std::string DAY_OF_WEEK_KEY = "day_of_week";
static const std::string DAY_OF_MONTH_KEY = "day_of_month"; // 1 - 28
static const std::string SEASON_KEY = "season";
static const std::string YEAR_KEY = "year";
static const std::string WEATHER_KEY = "weather";
static const std::string LOCATION_KEY = "location"; // INDOORS, OUTDOORS, <LOCATION_NAME>
static const std::string NPC_KEY = "npc";
static const std::string HEART_LEVEL_KEY = "heart_level"; // 1 - 10, convert 
static const std::string RELATIONSHIP_STATUS_KEY = "relationship_status";
static const std::string SHOOTING_STAR_FESTIVAL_STATUS_KEY = "shooting_star_festival_status";
static const std::string SPRING_FESTIVAL_NOT_FIRST_PLACE_KEY = "spring_festival_not_first_place";
static const std::string CUSTOM_CONDITION_KEY = "custom_condition";
static const std::string CUSTOM_CONDITION_T2_NAME_KEY = "t2_name";
static const std::string CUSTOM_CONDITION_TYPE_KEY = "t2_type";
static const std::string CUSTOM_CONDITION_OPERATOR_KEY = "comparison_operator";
static const std::string CUSTOM_CONDITION_T2_VALUE_KEY = "t2_value";
static const std::string ROUTINE_KEY = "routine";
static const std::vector<std::string> TIME_OF_DAY_CONSTANTS = { "day", "night", "dawn", "morning ", "afternoon", "evening", "twilight" };

static enum class RelationshipStatus {
	UNDEFINED,
	BEST_FRIEND,
	ROMANTIC
};

static enum class ShootingStartFestivalStatus {
	UNDEFINED,
	INVITED,
	WENT
};

static struct CustomCondition {
	enum T2Type {
		BOOLEAN,
		NUMERIC,
		STRING
	};

	enum Operator {
		EQUALS,
		NOT_EQUALS,
		GREATER_THAN,
		LESS_THAN,
		GREATER_THAN_OR_EQUALS,
		LESS_THAN_OR_EQUALS
	};

	struct T2Value {
		bool bool_value;
		int int_value;
		std::string string_value;
	};

	std::string t2_name;
	T2Type t2_type;
	Operator comparison_operator;
	T2Value t2_value;
};

static struct CurrentNpcRoutine {
	int time_last_updated = -1;
	int current_routine = -1;
};

static struct DynamicPortrait {
	int time_of_day = -1;
	int week_day = -1;
	int month_day = -1;
	int season = -1;
	int year = -1;
	int weather = -1;
	int location = -1;
	int npc = -1;
	int heart_level = -1;
	int current_routine = -1;
	RelationshipStatus relationship_status = RelationshipStatus::UNDEFINED;
	ShootingStartFestivalStatus shooting_star_festival_status = ShootingStartFestivalStatus::UNDEFINED;
	bool sprint_festival_not_first_place = false;
	std::optional<CustomCondition> custom_condition;
	std::string sprite_name;
	std::string_view mod_name;

	bool HasTimeOfDayCondition() { return time_of_day != -1; }
	bool HasWeekDayCondition() { return week_day != -1; }
	bool HasMonthDayCondition() { return month_day != -1; }
	bool HasSeasonCondition() { return season != -1; }
	bool HasYearCondition() { return year != -1; }
	bool HasWeatherCondition() { return weather != -1; }
	bool HasLocationCondition() { return location != -1; }
	bool HasNpcCondition() { return npc != -1; }
	bool HasHeartLevelCondition() { return heart_level != -1; }
	bool HasCurrentRoutineCondition() { return current_routine != -1; }
	bool HasRelationshipStatusCondition() { return relationship_status != RelationshipStatus::UNDEFINED; }
	bool HasShootingStarDateCondition() { return shooting_star_festival_status != ShootingStartFestivalStatus::UNDEFINED; }
	bool HasSpringFestivalCondition() { return sprint_festival_not_first_place; }
	bool HasCustomCondition() { return custom_condition.has_value(); }
};

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static bool game_is_active = false;
static bool ari_is_in_dungeon = false;
static bool cutscene_is_running = false;
static int current_time_in_seconds = -1;
static int current_day = -1;
static int current_year = -1;
static int current_season = -1;
static int current_weather = -1;
static int current_location = -1;
static std::string ari_current_gm_room = "";
static std::unordered_set<std::string> mod_names = {};
static std::vector<DynamicPortrait> dynamic_portraits = {};
static std::map<int, bool> location_id_to_outdoor_map = {};
static std::map<std::string, int> time_of_day_to_id_map = {};
static std::map<std::string, int> day_name_to_id_map = {}; // __day__
static std::map<std::string, int> season_name_to_id_map = {}; // __season__
static std::map<std::string, int> weather_name_to_id_map = {}; // __weather__
static std::map<std::string, int> location_name_to_id_map = {}; // __location_id__
static std::map<int, std::string> location_id_to_name_map = {}; // __location_id__
static std::map<std::string, int> npc_name_to_id_map = {}; // __npc_id__
static std::map<int, std::string> npc_id_to_name_map = {}; // __npc_id__
static std::map<std::string, int> routine_name_to_id_map = {}; // __routine__
static std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map = {}; // Vector<CInstance*> holds references to Self and Other for each script.
static std::map<std::string, CurrentNpcRoutine> npc_name_to_current_routine_map = {};

void ResetStaticFields(bool title_screen)
{
	if (title_screen)
	{
		game_is_active = false;
		ari_current_gm_room = "";
		script_name_to_reference_map = {};
	}
	
	ari_is_in_dungeon = false;
	cutscene_is_running = false;
	current_time_in_seconds = -1;
	current_day = -1;
	current_season = -1;
	current_year = -1;
	current_weather = -1;
	current_location = -1;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::size_t start = 0;
	std::size_t end;

	while ((end = s.find(delimiter, start)) != std::string::npos) {
		tokens.push_back(s.substr(start, end - start));
		start = end + 1;
	}

	tokens.push_back(s.substr(start));
	return tokens;
}

bool GameIsPaused()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
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

void LoadDaytime()
{
	for (int i = 0; i < TIME_OF_DAY_CONSTANTS.size(); i++)
	{
		time_of_day_to_id_map[TIME_OF_DAY_CONSTANTS[i]] = i;
	}
}

void LoadDays()
{
	size_t array_length = 0;
	RValue days = global_instance->GetMember("__day__");
	g_ModuleInterface->GetArraySize(days, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(days, i, array_element);
		day_name_to_id_map[array_element->ToString()] = i;
	}
}

void LoadSeasons()
{
	size_t array_length = 0;
	RValue seasons = global_instance->GetMember("__season__");
	g_ModuleInterface->GetArraySize(seasons, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(seasons, i, array_element);
		season_name_to_id_map[array_element->ToString()] = i;
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
	// Location IDs
	RValue location_ids = global_instance->GetMember("__location_id__");
	RValue location_ids_size = g_ModuleInterface->CallBuiltin("array_length", { location_ids });
	for (int i = 0; i < location_ids_size.m_Real; i++)
	{
		RValue location = g_ModuleInterface->CallBuiltin("array_get", { location_ids, i });
		location_name_to_id_map[location.ToString()] = i;
		location_id_to_name_map[i] = location.ToString();
	}

	// Append "indoors" and "outdoors" to location maps.
	location_name_to_id_map[INDOORS] = location_name_to_id_map.size();
	location_name_to_id_map[OUTDOORS] = location_name_to_id_map.size();
	location_id_to_name_map[location_name_to_id_map[INDOORS]] = INDOORS;
	location_id_to_name_map[location_name_to_id_map[OUTDOORS]] = OUTDOORS;

	// Default location data
	RValue locations = global_instance->GetMember("__locations");
	RValue locations_size = g_ModuleInterface->CallBuiltin("array_length", { locations });

	for (int i = 0; i < locations_size.m_Real; i++)
	{
		RValue location = g_ModuleInterface->CallBuiltin("array_get", { locations, i });
		RValue outdoor = g_ModuleInterface->CallBuiltin("struct_get", { location, "outdoor" });
		location_id_to_outdoor_map[i] = outdoor.ToBoolean();
	}
}

void LoadNpcs()
{
	size_t array_length = 0;
	RValue npcs = global_instance->GetMember("__npc_id__");
	g_ModuleInterface->GetArraySize(npcs, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(npcs, i, array_element);
		npc_name_to_id_map[array_element->ToString()] = i;
		npc_id_to_name_map[i] = array_element->ToString();
	}
}

void LoadRoutines()
{
	size_t array_length = 0;
	RValue routines = global_instance->GetMember("__routine__");
	g_ModuleInterface->GetArraySize(routines, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(routines, i, array_element);
		routine_name_to_id_map[array_element->ToString()] = i;
	}
}

RValue GetNpcFromDatabase(int npc_id)
{
	RValue __npc_database = global_instance->GetMember("__npc_database");
	return g_ModuleInterface->CallBuiltin("array_get", { __npc_database, npc_id });
}

int GetNpcHeartPoints(RValue npc)
{
	return npc.GetMember("heart_points").ToInt64();
}

int HeartPointsToLevel(int heart_points)
{
	if (heart_points >= 1755)
		return 10;
	if (heart_points >= 1400)
		return 9;
	if (heart_points >= 1125)
		return 8;
	if (heart_points >= 900)
		return 7;
	if (heart_points >= 705)
		return 6;
	if (heart_points >= 530)
		return 5;
	if (heart_points >= 390)
		return 4;
	if (heart_points >= 280)
		return 3;
	if (heart_points >= 180)
		return 2;
	if (heart_points >= 80)
		return 1;
	return 0;
}

bool GetNpcGiftFlag(RValue npc)
{
	return npc.GetMember("gift_flag").ToBoolean();
}

RValue T2Read(std::string key, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_T2_READ,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

bool AriIsIndoors()
{
	if (!game_is_active || current_location == -1)
		return false;

	if (location_id_to_outdoor_map[current_location])
		return false;

	if (ari_is_in_dungeon)
		return false;

	return true;
}

bool IsDay() // 6AM to 9PM
{
	return current_time_in_seconds < NINE_PM_IN_SECONDS;
}

bool IsNight() // 9PM to 2AM
{
	return current_time_in_seconds >= NINE_PM_IN_SECONDS;
}

bool IsDawn() // 6AM to 8AM
{
	return current_time_in_seconds < EIGHT_AM_IN_SECONDS;
}

bool IsMorning() // 6AM to 12PM
{
	return current_time_in_seconds < TWELVE_PM_IN_SECONDS;
}

bool IsAfternoon() // 12PM to 5PM
{
	return current_time_in_seconds >= TWELVE_PM_IN_SECONDS && current_time_in_seconds < FIVE_PM_IN_SECONDS;
}

bool IsEvening() // 5PM to 9PM
{
	return current_time_in_seconds >= FIVE_PM_IN_SECONDS && current_time_in_seconds < NINE_PM_IN_SECONDS;
}

bool IsTwilight() // 12AM to 2AM
{
	return current_time_in_seconds >= TWELVE_AM_IN_SECONDS;
}

bool IsValidTimeOfDay(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return time_of_day_to_id_map.contains(s_lower);
}

bool IsValidDayOfWeek(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return day_name_to_id_map.contains(s_lower);
}

bool IsValidSeason(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return season_name_to_id_map.contains(s_lower);
}

bool IsValidWeather(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return weather_name_to_id_map.contains(s_lower);
}

bool IsValidLocation(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return location_name_to_id_map.contains(s_lower);
}

bool IsValidRoutine(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return routine_name_to_id_map.contains(s_lower);
}

bool IsValidNpc(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return npc_name_to_id_map.contains(s_lower);
}

bool IsValidRelationshipStatus(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;
	if (s == "best_friend" || s == "romantic")
		return true;

	return false;
}

RelationshipStatus ParseRelationshipStatus(const std::string& s)
{
	if (s == "best_friend")
		return RelationshipStatus::BEST_FRIEND;
	if (s == "romantic")
		return RelationshipStatus::ROMANTIC;
	return RelationshipStatus::UNDEFINED;
}

bool IsValidShootingStarFestivalStatus(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;
	if (s.contains("invited") || s.contains("went"))
		return true;

	return false;
}

ShootingStartFestivalStatus ParseShootingStarFestivalStatus(const std::string& s)
{
	if (s.contains("invited"))
		return ShootingStartFestivalStatus::INVITED;
	if (s.contains("went"))
		return ShootingStartFestivalStatus::WENT;
	return ShootingStartFestivalStatus::UNDEFINED;
}

CustomCondition::T2Type ParseCustomConditionType(const std::string& s)
{
	if (s == "BOOLEAN")
		return CustomCondition::T2Type::BOOLEAN;
	if (s == "NUMERIC")
		return CustomCondition::T2Type::NUMERIC;
	if (s == "STRING")
		return CustomCondition::T2Type::STRING;
}

CustomCondition::Operator ParseCustomConditionOperator(const std::string& s)
{
	if (s == "EQUALS")
		return CustomCondition::Operator::EQUALS;
	if (s == "NOT_EQUALS")
		return CustomCondition::Operator::NOT_EQUALS;
	if (s == "GREATER_THAN")
		return CustomCondition::Operator::GREATER_THAN;
	if (s == "LESS_THAN")
		return CustomCondition::Operator::LESS_THAN;
	if (s == "GREATER_THAN_OR_EQUALS")
		return CustomCondition::Operator::GREATER_THAN_OR_EQUALS;
	if (s == "LESS_THAN_OR_EQUALS")
		return CustomCondition::Operator::LESS_THAN_OR_EQUALS;
}

CustomCondition::T2Value ParseCustomConditionValue(CustomCondition::T2Type t2_type, const std::string& s)
{
	CustomCondition::T2Value t2_value = CustomCondition::T2Value();

	if (t2_type == CustomCondition::T2Type::NUMERIC)
	{
		double d = std::stod(s);
		t2_value.int_value = static_cast<int>(d);
	}
	if (t2_type == CustomCondition::T2Type::BOOLEAN)
	{
		if (s == "true")
			t2_value.bool_value = true;
		if (s == "false")
			t2_value.bool_value = false;
	}
	if (t2_type == CustomCondition::T2Type::STRING)
		t2_value.string_value = s;

	return t2_value;
}

bool IsValidCustomCondition(const json& json)
{
	if (!json.contains(CUSTOM_CONDITION_T2_NAME_KEY) || !json.at(CUSTOM_CONDITION_T2_NAME_KEY).is_string())
		return false;
	if (!json.contains(CUSTOM_CONDITION_TYPE_KEY) || !json.at(CUSTOM_CONDITION_TYPE_KEY).is_string())
		return false;
	if (!json.contains(CUSTOM_CONDITION_OPERATOR_KEY) || !json.at(CUSTOM_CONDITION_OPERATOR_KEY).is_string())
		return false;
	if (!json.contains(CUSTOM_CONDITION_T2_VALUE_KEY) || !json.at(CUSTOM_CONDITION_T2_VALUE_KEY).is_string())
		return false;

	std::string t2_name = json[CUSTOM_CONDITION_T2_NAME_KEY];
	if (t2_name.empty() || t2_name.size() == 0)
		return false;

	std::string t2_type_str = json[CUSTOM_CONDITION_TYPE_KEY];
	if (t2_type_str.empty() || t2_type_str.size() == 0)
		return false;
	if (t2_type_str != "BOOLEAN" && t2_type_str != "NUMERIC" && t2_type_str != "STRING")
		return false;

	std::string t2_operator_str = json[CUSTOM_CONDITION_OPERATOR_KEY];
	if (t2_operator_str.empty() || t2_operator_str.size() == 0)
		return false;
	if (t2_operator_str != "EQUALS" && t2_operator_str != "NOT_EQUALS" && t2_operator_str != "GREATER_THAN" && t2_operator_str != "LESS_THAN" && t2_operator_str != "GREATER_THAN_OR_EQUALS" && t2_operator_str != "LESS_THAN_OR_EQUALS")
		return false;

	std::string t2_value_str = json[CUSTOM_CONDITION_T2_VALUE_KEY];
	if (t2_value_str.empty() || t2_value_str.size() == 0)
		return false;

	CustomCondition::T2Type t2_type = ParseCustomConditionType(t2_type_str);
	CustomCondition::Operator t2_operator = ParseCustomConditionOperator(t2_operator_str);

	if (t2_type == CustomCondition::T2Type::BOOLEAN || t2_type == CustomCondition::T2Type::STRING)
	{
		if (t2_operator != CustomCondition::Operator::EQUALS && CustomCondition::Operator::NOT_EQUALS)
			return false;
	}

	if (t2_type == CustomCondition::T2Type::NUMERIC)
	{
		try {
			size_t pos;
			double d = std::stod(t2_value_str, &pos);

			if (pos != t2_value_str.size()) {
				return false; // not fully numeric
			}

			int x = static_cast<int>(d);  // truncates toward 0
		}
		catch (const std::invalid_argument&) { return false; }
		catch (const std::out_of_range&) { return false; }
	}
	if (t2_type == CustomCondition::T2Type::BOOLEAN)
	{
		if (t2_value_str != "true" && t2_value_str != "false")
			return false;
	}
	if (t2_type == CustomCondition::T2Type::STRING)
	{
		if (t2_value_str.empty() || t2_value_str.size() == 0)
			return false;
	}

	return true;
}

CustomCondition ParseCustomCondition(const json& json)
{
	std::string t2_name = json[CUSTOM_CONDITION_T2_NAME_KEY];
	std::string t2_type_str = json[CUSTOM_CONDITION_TYPE_KEY];
	std::string t2_operator_str = json[CUSTOM_CONDITION_OPERATOR_KEY];
	std::string t2_value_str = json[CUSTOM_CONDITION_T2_VALUE_KEY];

	CustomCondition::T2Type t2_type = ParseCustomConditionType(t2_type_str);
	CustomCondition::Operator t2_operator = ParseCustomConditionOperator(t2_operator_str);
	CustomCondition::T2Value t2_value = ParseCustomConditionValue(t2_type, t2_value_str);

	return CustomCondition(t2_name, t2_type, t2_operator, t2_value);
}

void GetCurrentRoutine(CInstance* npc, std::string npc_name)
{
	if (StructVariableExists(npc, "me"))
	{
		RValue me = npc->GetMember("me");
		if (IsObject(me) && StructVariableExists(me, "activity_handler"))
		{
			RValue activity_handler = me.GetMember("activity_handler");
			if (IsObject(activity_handler) && StructVariableExists(activity_handler, "routine"))
			{
				RValue routine = activity_handler.GetMember("routine");
				if (IsObject(routine) && StructVariableExists(routine, "id"))
				{
					RValue id = routine.GetMember("id");
					if (IsNumeric(id))
						npc_name_to_current_routine_map[npc_name] = CurrentNpcRoutine(current_time_in_seconds, id.ToInt64());
				}
			}
		}
	}		
}

void PruneOutdatedNpcRoutines()
{
	for (auto& entry : npc_name_to_current_routine_map)
		if (current_time_in_seconds > entry.second.time_last_updated + 300) // 5m
			entry.second.current_routine = -1;
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

		// Try to find the mod_data/DynamicNpcPortraits directory.
		std::string dynamic_npc_portraits_folder = mod_data_folder + "\\DynamicNpcPortraits";
		if (!std::filesystem::exists(dynamic_npc_portraits_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DynamicNpcPortraits\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, dynamic_npc_portraits_folder.c_str());
			std::filesystem::create_directory(dynamic_npc_portraits_folder);
		}

		// Iterate over all files in the mod_data/DynamicNpcPortraits directory.
		try
		{
			for (const auto& file : fs::directory_iterator(dynamic_npc_portraits_folder))
			{
				if (file.is_regular_file() && file.path().extension() == ".json")
				{
					std::string filename = file.path().filename().string();
					
					std::ifstream in_stream(file.path());
					if (in_stream.good())
					{
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Reading JSON file: %s", MOD_NAME, VERSION, filename.c_str());

						try
						{
							json json_object = json::parse(in_stream);

							// Check if the json_object is empty.
							if (json_object.empty())
							{
								g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in file: %s", MOD_NAME, VERSION, filename.c_str());
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Ignoring file: %s", MOD_NAME, VERSION, filename.c_str());
							}
							else
							{
								// Try loading the mod_name value.
								std::string_view mod_name_view;
								if (json_object.contains(MOD_NAME_KEY) && json_object.at(MOD_NAME_KEY).is_string())
								{
									std::string mod_name = json_object[MOD_NAME_KEY];
									if (!mod_name.empty() && mod_name.size() > 0)
									{
										if(!mod_names.contains(mod_name))
											mod_names.insert(mod_name);
										mod_name_view = *mod_names.find(mod_name);
									}
								}

								// Try loading the conditions value.
								if (json_object.contains(DYNAMIC_PORTRAITS_KEY) && json_object.at(DYNAMIC_PORTRAITS_KEY).is_array())
								{
									std::vector<json> json_dynamic_portraits = json_object[DYNAMIC_PORTRAITS_KEY];
									if (!json_dynamic_portraits.empty())
									{
										for (json json_dynamic_portrait : json_dynamic_portraits)
										{
											if (!json_dynamic_portrait.contains(CONDITIONS_KEY) || !json_dynamic_portrait.at(CONDITIONS_KEY).is_object() || !json_dynamic_portrait.contains(SPRITE_NAME_KEY) || !json_dynamic_portrait.at(SPRITE_NAME_KEY).is_string() || !json_dynamic_portrait.contains(NPC_KEY) || !json_dynamic_portrait.at(NPC_KEY).is_string())
												continue;

											json json_dynamic_portrait_conditions = json_dynamic_portrait[CONDITIONS_KEY];
											if (json_dynamic_portrait_conditions.empty())
												continue;
											
											std::string json_dynamic_portrait_sprite_name = json_dynamic_portrait[SPRITE_NAME_KEY];
											if (json_dynamic_portrait_sprite_name.empty() || json_dynamic_portrait_sprite_name.size() == 0)
												continue;

											std::string json_dynamic_portrait_npc = json_dynamic_portrait[NPC_KEY];
											if (json_dynamic_portrait_npc.empty() || json_dynamic_portrait_npc.size() == 0 || !IsValidNpc(json_dynamic_portrait_npc))
												continue;

											// Test the base_sprite_name using the neutral expression.
											std::string test_base_sprite_neutral_expression = json_dynamic_portrait_sprite_name + "_neutral";
											RValue asset_index = g_ModuleInterface->CallBuiltin("asset_get_index", { test_base_sprite_neutral_expression.c_str() });
											if (asset_index.m_Kind != VALUE_REF)
											{
												g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - The provided base sprite doesn't exist: %s", MOD_NAME, VERSION, json_dynamic_portrait_sprite_name.c_str());
												continue;
											}

											DynamicPortrait dynamic_portrait = DynamicPortrait();
											dynamic_portrait.mod_name = mod_name_view;
											dynamic_portrait.sprite_name = json_dynamic_portrait_sprite_name;
											dynamic_portrait.npc = npc_name_to_id_map[json_dynamic_portrait_npc];

											// time_of_day condition.
											if (json_dynamic_portrait_conditions.contains(TIME_OF_DAY_KEY) && json_dynamic_portrait_conditions.at(TIME_OF_DAY_KEY).is_string())
											{
												std::string time_of_day_str = json_dynamic_portrait_conditions[TIME_OF_DAY_KEY];
												if (IsValidTimeOfDay(time_of_day_str))
													dynamic_portrait.time_of_day = time_of_day_to_id_map[time_of_day_str];
											}

											// day_of_week condition.
											if (json_dynamic_portrait_conditions.contains(DAY_OF_WEEK_KEY) && json_dynamic_portrait_conditions.at(DAY_OF_WEEK_KEY).is_string())
											{
												std::string day_of_week_str = json_dynamic_portrait_conditions[DAY_OF_WEEK_KEY];
												if (IsValidDayOfWeek(day_of_week_str))
													dynamic_portrait.week_day = day_name_to_id_map[day_of_week_str];
											}

											// day_of_month condition.
											if (json_dynamic_portrait_conditions.contains(DAY_OF_MONTH_KEY) && json_dynamic_portrait_conditions.at(DAY_OF_MONTH_KEY).is_number_integer())
											{
												int day_of_month = json_dynamic_portrait_conditions[DAY_OF_MONTH_KEY];
												if (day_of_month > 0 && day_of_month < 29)
													dynamic_portrait.month_day = day_of_month;
											}

											// season condition.
											if (json_dynamic_portrait_conditions.contains(SEASON_KEY) && json_dynamic_portrait_conditions.at(SEASON_KEY).is_string())
											{
												std::string season_str = json_dynamic_portrait_conditions[SEASON_KEY];
												if (IsValidSeason(season_str))
													dynamic_portrait.season = season_name_to_id_map[season_str];
											}

											// year condition.
											if (json_dynamic_portrait_conditions.contains(YEAR_KEY) && json_dynamic_portrait_conditions.at(YEAR_KEY).is_number_integer())
											{
												int year = json_dynamic_portrait_conditions[YEAR_KEY];
												if (year > 0)
													dynamic_portrait.year = year;
											}

											// weather condition.
											if (json_dynamic_portrait_conditions.contains(WEATHER_KEY) && json_dynamic_portrait_conditions.at(WEATHER_KEY).is_string())
											{
												std::string weather_str = json_dynamic_portrait_conditions[WEATHER_KEY];
												if (IsValidWeather(weather_str))
													dynamic_portrait.weather = weather_name_to_id_map[weather_str];
											}

											// location condition.
											if (json_dynamic_portrait_conditions.contains(LOCATION_KEY) && json_dynamic_portrait_conditions.at(LOCATION_KEY).is_string())
											{
												std::string location_str = json_dynamic_portrait_conditions[LOCATION_KEY];
												if (IsValidLocation(location_str))
													dynamic_portrait.location = location_name_to_id_map[location_str];
											}

											// heart_level condition
											if (json_dynamic_portrait_conditions.contains(HEART_LEVEL_KEY) && json_dynamic_portrait_conditions.at(HEART_LEVEL_KEY).is_number_integer())
											{
												int heart_level = json_dynamic_portrait_conditions[HEART_LEVEL_KEY];
												if (heart_level > 0 && heart_level <= 10)
													dynamic_portrait.heart_level = heart_level;
											}

											// routine condition
											if (json_dynamic_portrait_conditions.contains(ROUTINE_KEY) && json_dynamic_portrait_conditions.at(ROUTINE_KEY).is_string())
											{
												std::string routine_str = json_dynamic_portrait_conditions[ROUTINE_KEY];
												if (IsValidRoutine(routine_str))
													dynamic_portrait.current_routine = routine_name_to_id_map[routine_str];
											}

											// relationship_status condition
											if (json_dynamic_portrait_conditions.contains(RELATIONSHIP_STATUS_KEY) && json_dynamic_portrait_conditions.at(RELATIONSHIP_STATUS_KEY).is_string())
											{
												std::string relationship_str = json_dynamic_portrait_conditions[RELATIONSHIP_STATUS_KEY];
												if (IsValidRelationshipStatus(relationship_str))
													dynamic_portrait.relationship_status = ParseRelationshipStatus(relationship_str);
											}

											// shooting_star_festival_status
											if (json_dynamic_portrait_conditions.contains(SHOOTING_STAR_FESTIVAL_STATUS_KEY) && json_dynamic_portrait_conditions.at(SHOOTING_STAR_FESTIVAL_STATUS_KEY).is_string())
											{
												std::string shooting_star_festival_status_str = json_dynamic_portrait_conditions[SHOOTING_STAR_FESTIVAL_STATUS_KEY];
												if (IsValidShootingStarFestivalStatus(shooting_star_festival_status_str))
													dynamic_portrait.shooting_star_festival_status = ParseShootingStarFestivalStatus(shooting_star_festival_status_str);
											}

											// spring_festival condition
											if (json_dynamic_portrait_conditions.contains(SPRING_FESTIVAL_NOT_FIRST_PLACE_KEY) && json_dynamic_portrait_conditions.at(SPRING_FESTIVAL_NOT_FIRST_PLACE_KEY).is_boolean())
											{
												bool spring_festival_not_first_place = json_dynamic_portrait_conditions[SPRING_FESTIVAL_NOT_FIRST_PLACE_KEY];
												if (spring_festival_not_first_place)
													dynamic_portrait.sprint_festival_not_first_place = true;
											}

											// custom_condition
											if (json_dynamic_portrait_conditions.contains(CUSTOM_CONDITION_KEY) && json_dynamic_portrait_conditions.at(CUSTOM_CONDITION_KEY).is_object())
											{
												json custom_condition_json = json_dynamic_portrait_conditions[CUSTOM_CONDITION_KEY];
												if (IsValidCustomCondition(custom_condition_json))
													dynamic_portrait.custom_condition = ParseCustomCondition(custom_condition_json);
											}

											// Final validation. Confirm the reminder has at least one valid condition.
											if (dynamic_portrait.HasTimeOfDayCondition() || dynamic_portrait.HasWeekDayCondition() || dynamic_portrait.HasMonthDayCondition() || dynamic_portrait.HasSeasonCondition() || dynamic_portrait.HasYearCondition() || dynamic_portrait.HasWeatherCondition() || dynamic_portrait.HasLocationCondition() || dynamic_portrait.HasHeartLevelCondition() || dynamic_portrait.HasRelationshipStatusCondition() || dynamic_portrait.HasShootingStarDateCondition() || dynamic_portrait.HasSpringFestivalCondition() || dynamic_portrait.HasCustomCondition())
											{
												dynamic_portraits.push_back(dynamic_portrait);
												g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Dynamic Portrait config loaded for sprite: %s", MOD_NAME, VERSION, json_dynamic_portrait_sprite_name.c_str());
											}
										}
									}
									else
									{
										g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No conditions were set in file: %s", MOD_NAME, VERSION, filename.c_str());
										g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Ignoring the file.", MOD_NAME, VERSION);
									}
								}
								else
								{
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s", MOD_NAME, VERSION, DYNAMIC_PORTRAITS_KEY.c_str(), filename.c_str());
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Ignoring the file.", MOD_NAME, VERSION);
								}
							}
						}
						catch (...)
						{
							eptr = std::current_exception();
							PrintError(eptr);

							g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from file: %s", MOD_NAME, VERSION, filename.c_str());
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
						}

						in_stream.close();
					}
					else
					{
						in_stream.close();
						g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to load JSON file: %s", MOD_NAME, VERSION, filename.c_str());
					}
				}
			}
		}
		catch (...)
		{
			eptr = std::current_exception();
			PrintError(eptr);

			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error reading dynamic portrait config files in folder: %s", MOD_NAME, VERSION, dynamic_npc_portraits_folder.c_str());
		}
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred reading the dynamic portrait config files.", MOD_NAME, VERSION);
	}
}

RValue GetDynamicNpcPortrait(std::string sprite_name)
{
	// Example: spr_portrait_balor_spring_sincere_special
	std::vector<std::string> sprite_name_parts = split(sprite_name, '_');
	std::string npc_name = sprite_name_parts[2];

	std::string expression = "";
	for (int i = 4; i < sprite_name_parts.size(); i++)
	{
		expression += sprite_name_parts[i];
		if (i != sprite_name_parts.size() - 1)
			expression += "_";
	}

	for (DynamicPortrait dynamic_portrait : dynamic_portraits)
	{
		if (npc_name != npc_id_to_name_map[dynamic_portrait.npc])
			continue;

		if (dynamic_portrait.HasTimeOfDayCondition())
		{
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["day"] && !IsDay())
				continue;
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["night"] && !IsNight())
				continue;
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["dawn"] && !IsDawn())
				continue;
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["morning"] && !IsMorning())
				continue;
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["afternoon"] && !IsAfternoon())
				continue;
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["evening"] && !IsEvening())
				continue;
			if (dynamic_portrait.time_of_day == time_of_day_to_id_map["twilight"] && !IsTwilight())
				continue;
		}

		if (dynamic_portrait.HasWeekDayCondition() && ((current_day - 1) % 7) != dynamic_portrait.week_day)
			continue;

		if (dynamic_portrait.HasMonthDayCondition() && current_day != dynamic_portrait.month_day)
			continue;

		if (dynamic_portrait.HasSeasonCondition() && current_season != dynamic_portrait.season)
			continue;

		if (dynamic_portrait.HasYearCondition() && current_year != dynamic_portrait.year)
			continue;

		if (dynamic_portrait.HasWeatherCondition() && current_weather != dynamic_portrait.weather)
			continue;

		if (dynamic_portrait.HasLocationCondition())
		{
			if (dynamic_portrait.location == location_name_to_id_map[OUTDOORS] || dynamic_portrait.location == location_name_to_id_map[INDOORS])
			{
				if (dynamic_portrait.location == location_name_to_id_map[OUTDOORS] && AriIsIndoors())
					continue;
				if (dynamic_portrait.location == location_name_to_id_map[INDOORS] && !AriIsIndoors())
					continue;
			}
			else if (dynamic_portrait.HasLocationCondition() && current_location != dynamic_portrait.location)
				continue;
		}

		if (dynamic_portrait.HasHeartLevelCondition())
		{
			RValue npc = GetNpcFromDatabase(dynamic_portrait.npc);
			int heart_points = GetNpcHeartPoints(npc);
			int heart_level = HeartPointsToLevel(heart_points);

			if (heart_level < dynamic_portrait.heart_level)
				continue;
		}

		if (dynamic_portrait.HasRelationshipStatusCondition())
		{
			std::string npc_name = npc_id_to_name_map[dynamic_portrait.npc];
			RValue relationship_status = T2Read(npc_name + "_status", script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1]);

			if (relationship_status.m_Kind != VALUE_STRING)
				continue;
			if (!IsValidRelationshipStatus(relationship_status.ToString()))
				continue;
			if (dynamic_portrait.relationship_status != ParseRelationshipStatus(relationship_status.ToString()))
				continue;
		}

		if (dynamic_portrait.HasShootingStarDateCondition())
		{
			std::string npc_name = npc_id_to_name_map[dynamic_portrait.npc];
			RValue shooting_star_date_status = T2Read("shooting_star_date_status", script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1]);

			if (shooting_star_date_status.m_Kind != VALUE_STRING)
				continue;
			if (!IsValidShootingStarFestivalStatus(shooting_star_date_status.ToString()))
				continue;
			if (!shooting_star_date_status.ToString().contains(npc_id_to_name_map[dynamic_portrait.npc]))
				continue;
			if (dynamic_portrait.shooting_star_festival_status != ParseShootingStarFestivalStatus(shooting_star_date_status.ToString()))
				continue;
		}

		if (dynamic_portrait.HasSpringFestivalCondition())
		{
			RValue sf_first_place = T2Read("sf_first_place", script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1]);
			RValue sf_first_place_plus = T2Read("sf_first_place_plus", script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1]);

			if (sf_first_place.m_Kind != VALUE_BOOL || sf_first_place_plus.m_Kind != VALUE_BOOL)
				continue;
			if (sf_first_place.ToBoolean() || sf_first_place_plus.ToBoolean())
				continue;
		}

		if (dynamic_portrait.HasCustomCondition())
		{
			RValue t2_value = T2Read(dynamic_portrait.custom_condition.value().t2_name, script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1]);

			if (dynamic_portrait.custom_condition.value().t2_type == CustomCondition::T2Type::BOOLEAN && t2_value.m_Kind != VALUE_BOOL)
				continue;
			if (dynamic_portrait.custom_condition.value().t2_type == CustomCondition::T2Type::NUMERIC && !IsNumeric(t2_value))
				continue;
			if (dynamic_portrait.custom_condition.value().t2_type == CustomCondition::T2Type::STRING && t2_value.m_Kind != VALUE_STRING)
				continue;

			if (t2_value.m_Kind == VALUE_BOOL && dynamic_portrait.custom_condition.value().t2_value.bool_value != t2_value.ToBoolean())
				continue;
			if (IsNumeric(t2_value) && dynamic_portrait.custom_condition.value().t2_value.int_value != t2_value.ToInt64())
				continue;
			if (t2_value.m_Kind == VALUE_STRING && dynamic_portrait.custom_condition.value().t2_value.string_value != t2_value.ToString())
				continue;
		}

		if (dynamic_portrait.HasCurrentRoutineCondition())
		{
			if (dynamic_portrait.current_routine != npc_name_to_current_routine_map[npc_id_to_name_map[dynamic_portrait.npc]].current_routine)
				continue;
		}

		std::string sprite_name = dynamic_portrait.sprite_name + "_" + expression;
		RValue sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { sprite_name.c_str() });
		if (sprite.m_Kind == VALUE_REF)
			return sprite;
	}
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
		PruneOutdatedNpcRoutines();

		if (strstr(self->m_Object->m_Name, "obj_adeline"))
			GetCurrentRoutine(self, "adeline");
		else if (strstr(self->m_Object->m_Name, "obj_balor"))
			GetCurrentRoutine(self, "balor");
		else if (strstr(self->m_Object->m_Name, "obj_caldarus"))
			GetCurrentRoutine(self, "caldarus");
		else if (strstr(self->m_Object->m_Name, "obj_celine"))
			GetCurrentRoutine(self, "celine");
		else if (strstr(self->m_Object->m_Name, "obj_darcy"))
			GetCurrentRoutine(self, "darcy");
		else if (strstr(self->m_Object->m_Name, "obj_dell"))
			GetCurrentRoutine(self, "dell");
		else if (strstr(self->m_Object->m_Name, "obj_dozy"))
			GetCurrentRoutine(self, "dozy");
		else if (strstr(self->m_Object->m_Name, "obj_eiland"))
			GetCurrentRoutine(self, "eiland");
		else if (strstr(self->m_Object->m_Name, "obj_elsie"))
			GetCurrentRoutine(self, "elsie");
		else if (strstr(self->m_Object->m_Name, "obj_errol"))
			GetCurrentRoutine(self, "errol");
		else if (strstr(self->m_Object->m_Name, "obj_hayden"))
			GetCurrentRoutine(self, "hayden");
		else if (strstr(self->m_Object->m_Name, "obj_hemlock"))
			GetCurrentRoutine(self, "hemlock");
		else if (strstr(self->m_Object->m_Name, "obj_henrietta"))
			GetCurrentRoutine(self, "henrietta");
		else if (strstr(self->m_Object->m_Name, "obj_holt"))
			GetCurrentRoutine(self, "holt");
		else if (strstr(self->m_Object->m_Name, "obj_josephine"))
			GetCurrentRoutine(self, "josephine");
		else if (strstr(self->m_Object->m_Name, "obj_juniper"))
			GetCurrentRoutine(self, "juniper");
		else if (strstr(self->m_Object->m_Name, "obj_landen"))
			GetCurrentRoutine(self, "landen");
		else if (strstr(self->m_Object->m_Name, "obj_louis"))
			GetCurrentRoutine(self, "louis");
		else if (strstr(self->m_Object->m_Name, "obj_luc"))
			GetCurrentRoutine(self, "luc");
		else if (strstr(self->m_Object->m_Name, "obj_maple"))
			GetCurrentRoutine(self, "maple");
		else if (strstr(self->m_Object->m_Name, "obj_march"))
			GetCurrentRoutine(self, "march");
		else if (strstr(self->m_Object->m_Name, "obj_merri"))
			GetCurrentRoutine(self, "merri");
		else if (strstr(self->m_Object->m_Name, "obj_nora"))
			GetCurrentRoutine(self, "nora");
		else if (strstr(self->m_Object->m_Name, "obj_olric"))
			GetCurrentRoutine(self, "olric");
		else if (strstr(self->m_Object->m_Name, "obj_reina"))
			GetCurrentRoutine(self, "reina");
		else if (strstr(self->m_Object->m_Name, "obj_ryis"))
			GetCurrentRoutine(self, "ryis");
		else if (strstr(self->m_Object->m_Name, "obj_seridia"))
			GetCurrentRoutine(self, "seridia");
		//else if (strstr(self->m_Object->m_Name, "obj_stillwell"))
		//	GetCurrentRoutine(self, "stillwell");
		else if (strstr(self->m_Object->m_Name, "obj_taliferro"))
			GetCurrentRoutine(self, "taliferro");
		else if (strstr(self->m_Object->m_Name, "obj_terithia"))
			GetCurrentRoutine(self, "terithia");
		else if (strstr(self->m_Object->m_Name, "obj_valen"))
			GetCurrentRoutine(self, "valen");
		else if (strstr(self->m_Object->m_Name, "obj_vera"))
			GetCurrentRoutine(self, "vera");
		else if (strstr(self->m_Object->m_Name, "obj_wheedle"))
			GetCurrentRoutine(self, "wheedle");
		//else if (strstr(self->m_Object->m_Name, "obj_zorel"))
		//	GetCurrentRoutine(self, "zorel");
	}
}

RValue& GmlScriptT2ReadCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_T2_READ))
		script_name_to_reference_map[GML_SCRIPT_T2_READ] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_T2_READ));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);
	
	return Result;
}

RValue& GmlScriptIsDungeonRoomCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_IS_DUNGEON_ROOM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && Result.m_Kind == VALUE_BOOL)
		ari_is_in_dungeon = Result.ToBoolean();

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
	
	return Result;
}

RValue& GmlScriptTryLocationIdToStringCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active)
		current_location = Arguments[0]->ToInt64();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

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

	if (game_is_active && !GameIsPaused())
	{
		RValue time = global_instance->GetMember("__clock").GetMember("time");
		current_time_in_seconds = time.ToInt64();
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

RValue& GmlScriptVertigoDrawWithColorCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active)
	{
		RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { *Arguments[0] });
		if (type.ToInt64() == 1) // asset_sprite
		{
			RValue name = g_ModuleInterface->CallBuiltin("sprite_get_name", { *Arguments[0] });
			std::string name_str = name.ToString();

			if (name_str.contains("spr_portrait"))
			{
				RValue dynamic_sprite = GetDynamicNpcPortrait(name_str);
				if (dynamic_sprite.m_Kind == VALUE_REF)
					*Arguments[0] = dynamic_sprite;
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

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
		
		LoadDaytime();
		LoadDays();
		LoadSeasons();
		LoadWeather();
		LoadLocations();
		LoadNpcs();
		LoadRoutines();
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

void CreateHookGmlScriptT2Read(AurieStatus& status)
{
	CScript* gml_script_t2_read = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_T2_READ,
		(PVOID*)&gml_script_t2_read
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_T2_READ);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_T2_READ,
		gml_script_t2_read->m_Functions->m_ScriptFunction,
		GmlScriptT2ReadCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_T2_READ);
	}
}

void CreateHookGmlScriptIsDungeonRoom(AurieStatus& status)
{
	CScript* gml_script_is_dungeon_room = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_IS_DUNGEON_ROOM,
		(PVOID*)&gml_script_is_dungeon_room
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_IS_DUNGEON_ROOM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_IS_DUNGEON_ROOM,
		gml_script_is_dungeon_room->m_Functions->m_ScriptFunction,
		GmlScriptIsDungeonRoomCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_IS_DUNGEON_ROOM);
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

void CreateHookGmlScriptTryLocationIdToString(AurieStatus& status)
{
	CScript* gml_script_try_location_id_to_string = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
		(PVOID*)&gml_script_try_location_id_to_string
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
		gml_script_try_location_id_to_string->m_Functions->m_ScriptFunction,
		GmlScriptTryLocationIdToStringCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
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

void CreateHookGmlScriptVertigoDrawWithColor(AurieStatus& status)
{
	CScript* gml_script_vertigo_draw_with_color = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR,
		(PVOID*)&gml_script_vertigo_draw_with_color
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR,
		gml_script_vertigo_draw_with_color->m_Functions->m_ScriptFunction,
		GmlScriptVertigoDrawWithColorCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR);
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

	CreateHookGmlScriptT2Read(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptIsDungeonRoom(status);
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

	CreateHookGmlScriptTryLocationIdToString(status);
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

	CreateHookGmlScriptCutsceneIsRunning(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptVertigoDrawWithColor(status);
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