#include <map>
#include <fstream>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "DynamicObjectSprites";
static const char* const VERSION = "1.1.0";
static const char* const GML_SCRIPT_GET_MINUTES = "gml_Script_get_minutes";
static const char* const GML_SCRIPT_CALENDAR_DAY = "gml_Script_day@Calendar@Calendar";
static const char* const GML_SCRIPT_CALENDAR_SEASON = "gml_Script_season@Calendar@Calendar";
static const char* const GML_SCRIPT_CALENDAR_YEAR = "gml_Script_year@Calendar@Calendar";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_NODE_OBJECT_SET_SPRITE = "gml_Script_set_sprite@gml_Object_obj_node_renderer_Create_0";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const int EIGHT_PM_IN_SECONDS = 72000;
static const std::string MOD_NAME_KEY = "mod_name";
static const std::string DYNAMIC_OBJECT_SPRITES_KEY = "dynamic_object_sprites";
static const std::string CONDITIONS_KEY = "conditions";
static const std::string ORIGINAL_SPRITE_NAME_KEY = "original_sprite_name";
static const std::string REPLACEMENT_SPRITE_NAME_KEY = "replacement_sprite_name";
static const std::string BASE_SPRITE_NAME_KEY = "base_sprite_name";
static const std::string TIME_OF_DAY_KEY = "time_of_day"; // DAY or NIGHT
static const std::string DAY_OF_WEEK_KEY = "day_of_week";
static const std::string DAY_OF_MONTH_KEY = "day_of_month"; // 1 - 28
static const std::string SEASON_KEY = "season";
static const std::string YEAR_KEY = "year";
static const std::string WEATHER_KEY = "weather";

static struct DynamicObjectSprite {
	int time_of_day = -1;
	int week_day = -1;
	int month_day = -1;
	int season = -1;
	int year = -1;
	int weather = -1;
	std::string original_sprite_name;
	std::string replacement_sprite_name;
	std::string base_sprite;
	std::string_view mod_name;

	bool IsFloorSprite() { return !base_sprite.empty(); }
	bool HasTimeOfDayCondition() { return time_of_day != -1; }
	bool HasWeekDayCondition() { return week_day != -1; }
	bool HasMonthDayCondition() { return month_day != -1; }
	bool HasSeasonCondition() { return season != -1; }
	bool HasYearCondition() { return year != -1; }
	bool HasWeatherCondition() { return weather != -1; }
};

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static bool game_is_active = false;
static int current_time_in_seconds = -1;
static int current_day = -1;
static int current_year = -1;
static int current_season = -1;
static int current_weather = -1;
static std::unordered_set<std::string> mod_names = {};
static std::vector<DynamicObjectSprite> dynamic_object_sprites = {};
static std::map<std::string, int> daytime_to_id_map = {}; // __daytime__
static std::map<int, std::string> id_to_daytime_map = {}; // __daytime__
static std::map<std::string, int> day_name_to_id_map = {}; // __day__
static std::map<int, std::string> day_id_to_name_map = {}; // __day__
static std::map<std::string, int> season_name_to_id_map = {}; // __season__
static std::map<int, std::string> season_id_to_name_map = {}; // __season__
static std::map<std::string, int> weather_name_to_id_map = {}; // __weather__
static std::map<int, std::string> weather_id_to_name_map = {}; // __weather__

void ResetStaticFields(bool title_screen)
{
	if (title_screen)
	{
		game_is_active = false;
	}

	current_time_in_seconds = -1;
	current_day = -1;
	current_season = -1;
	current_year = -1;
	current_weather = -1;
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
	size_t array_length = 0;
	RValue daytime = global_instance->GetMember("__daytime__");
	g_ModuleInterface->GetArraySize(daytime, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(daytime, i, array_element);
		daytime_to_id_map[array_element->ToString()] = i;
		id_to_daytime_map[i] = array_element->ToString();
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
		day_id_to_name_map[i] = array_element->ToString();
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
		season_id_to_name_map[i] = array_element->ToString();
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
		weather_id_to_name_map[i] = array_element->ToString();
	}
}

bool IsNight()
{
	return current_time_in_seconds >= EIGHT_PM_IN_SECONDS;
}

bool IsValidTimeOfDay(const std::string& s)
{
	if (s.empty() || s.size() == 0)
		return false;

	std::string s_lower = to_lower(s);
	return daytime_to_id_map.contains(s_lower);
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

void LoadConfigFiles()
{
	// Load config files.
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

		// Try to find the mod_data/DynamicObjectSprites directory.
		std::string dynamic_object_sprites_folder = mod_data_folder + "\\DynamicObjectSprites";
		if (!std::filesystem::exists(dynamic_object_sprites_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DynamicObjectSprites\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, dynamic_object_sprites_folder.c_str());
			std::filesystem::create_directory(dynamic_object_sprites_folder);
		}

		// Iterate over all files in the mod_data/DynamicObjectSprites directory.
		try
		{
			for (const auto& file : fs::directory_iterator(dynamic_object_sprites_folder))
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
										if (!mod_names.contains(mod_name))
											mod_names.insert(mod_name);
										mod_name_view = *mod_names.find(mod_name);
									}
								}

								// Try loading the conditions value.
								if (json_object.contains(DYNAMIC_OBJECT_SPRITES_KEY) && json_object.at(DYNAMIC_OBJECT_SPRITES_KEY).is_array())
								{
									std::vector<json> json_dynamic_object_sprites = json_object[DYNAMIC_OBJECT_SPRITES_KEY];
									if (!json_dynamic_object_sprites.empty())
									{
										for (json json_dynamic_object_sprite : json_dynamic_object_sprites)
										{
											if (!json_dynamic_object_sprite.contains(CONDITIONS_KEY) || !json_dynamic_object_sprite.at(CONDITIONS_KEY).is_object() || !json_dynamic_object_sprite.contains(ORIGINAL_SPRITE_NAME_KEY) || !json_dynamic_object_sprite.at(ORIGINAL_SPRITE_NAME_KEY).is_string() || !json_dynamic_object_sprite.contains(REPLACEMENT_SPRITE_NAME_KEY) || !json_dynamic_object_sprite.at(REPLACEMENT_SPRITE_NAME_KEY).is_string())
												continue;

											json json_dynamic_object_sprite_conditions = json_dynamic_object_sprite[CONDITIONS_KEY];
											if (json_dynamic_object_sprite_conditions.empty())
												continue;

											std::string json_dynamic_object_sprite_original_sprite_name = json_dynamic_object_sprite[ORIGINAL_SPRITE_NAME_KEY];
											if (json_dynamic_object_sprite_original_sprite_name.empty() || json_dynamic_object_sprite_original_sprite_name.size() == 0)
												continue;

											std::string json_dynamic_object_sprite_replacement_sprite_name = json_dynamic_object_sprite[REPLACEMENT_SPRITE_NAME_KEY];
											if (json_dynamic_object_sprite_replacement_sprite_name.empty() || json_dynamic_object_sprite_replacement_sprite_name.size() == 0)
												continue;

											bool is_base_sprite = json_dynamic_object_sprite.contains(BASE_SPRITE_NAME_KEY) && json_dynamic_object_sprite.at(BASE_SPRITE_NAME_KEY).is_string();
											if (is_base_sprite)
											{
												std::string json_dynamic_object_sprite_base_sprite_name = json_dynamic_object_sprite[BASE_SPRITE_NAME_KEY];
												if (json_dynamic_object_sprite_base_sprite_name.empty() || json_dynamic_object_sprite_base_sprite_name.size() == 0)
													continue;
											}

											// Test the original_sprite_name exists.
											RValue original_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { json_dynamic_object_sprite_original_sprite_name.c_str() });
											if (original_sprite.m_Kind != VALUE_REF)
											{
												g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - The provided original sprite doesn't exist: %s", MOD_NAME, VERSION, json_dynamic_object_sprite_original_sprite_name.c_str());
												continue;
											}

											// Test the replacement_sprite_name exists.
											RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { json_dynamic_object_sprite_replacement_sprite_name.c_str() });
											if (replacement_sprite.m_Kind != VALUE_REF)
											{
												g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - The provided original sprite doesn't exist: %s", MOD_NAME, VERSION, json_dynamic_object_sprite_replacement_sprite_name.c_str());
												continue;
											}

											// Test the base_sprite exists if applicable.
											if (is_base_sprite)
											{
												std::string json_dynamic_object_sprite_base_sprite_name = json_dynamic_object_sprite[BASE_SPRITE_NAME_KEY];
												RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { json_dynamic_object_sprite_base_sprite_name.c_str() });
												if (replacement_sprite.m_Kind != VALUE_REF)
												{
													g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - The provided base sprite doesn't exist: %s", MOD_NAME, VERSION, json_dynamic_object_sprite_base_sprite_name.c_str());
													continue;
												}
											}

											DynamicObjectSprite dynamic_object_sprite = DynamicObjectSprite();
											dynamic_object_sprite.mod_name = mod_name_view;
											dynamic_object_sprite.original_sprite_name = json_dynamic_object_sprite_original_sprite_name;
											dynamic_object_sprite.replacement_sprite_name = json_dynamic_object_sprite_replacement_sprite_name;
											if (is_base_sprite) dynamic_object_sprite.base_sprite = json_dynamic_object_sprite[BASE_SPRITE_NAME_KEY];

											// time_of_day condition.
											if (json_dynamic_object_sprite_conditions.contains(TIME_OF_DAY_KEY) && json_dynamic_object_sprite_conditions.at(TIME_OF_DAY_KEY).is_string())
											{
												std::string time_of_day_str = json_dynamic_object_sprite_conditions[TIME_OF_DAY_KEY];
												if (IsValidTimeOfDay(time_of_day_str))
													dynamic_object_sprite.time_of_day = daytime_to_id_map[time_of_day_str];
											}

											// day_of_week condition.
											if (json_dynamic_object_sprite_conditions.contains(DAY_OF_WEEK_KEY) && json_dynamic_object_sprite_conditions.at(DAY_OF_WEEK_KEY).is_string())
											{
												std::string day_of_week_str = json_dynamic_object_sprite_conditions[DAY_OF_WEEK_KEY];
												if (IsValidDayOfWeek(day_of_week_str))
													dynamic_object_sprite.week_day = day_name_to_id_map[day_of_week_str];
											}

											// day_of_month condition.
											if (json_dynamic_object_sprite_conditions.contains(DAY_OF_MONTH_KEY) && json_dynamic_object_sprite_conditions.at(DAY_OF_MONTH_KEY).is_number_integer())
											{
												int day_of_month = json_dynamic_object_sprite_conditions[DAY_OF_MONTH_KEY];
												if (day_of_month > 0 && day_of_month < 29)
													dynamic_object_sprite.month_day = day_of_month;
											}

											// season condition.
											if (json_dynamic_object_sprite_conditions.contains(SEASON_KEY) && json_dynamic_object_sprite_conditions.at(SEASON_KEY).is_string())
											{
												std::string season_str = json_dynamic_object_sprite_conditions[SEASON_KEY];
												if (IsValidSeason(season_str))
													dynamic_object_sprite.season = season_name_to_id_map[season_str];
											}

											// year condition.
											if (json_dynamic_object_sprite_conditions.contains(YEAR_KEY) && json_dynamic_object_sprite_conditions.at(YEAR_KEY).is_number_integer())
											{
												int year = json_dynamic_object_sprite_conditions[YEAR_KEY];
												if (year > 0)
													dynamic_object_sprite.year = year;
											}

											// weather condition.
											if (json_dynamic_object_sprite_conditions.contains(WEATHER_KEY) && json_dynamic_object_sprite_conditions.at(WEATHER_KEY).is_string())
											{
												std::string weather_str = json_dynamic_object_sprite_conditions[WEATHER_KEY];
												if (IsValidWeather(weather_str))
													dynamic_object_sprite.weather = weather_name_to_id_map[weather_str];
											}

											// Final validation. Confirm the DynamicObjectSprite has at least one valid condition.
											if (dynamic_object_sprite.HasTimeOfDayCondition() || dynamic_object_sprite.HasWeekDayCondition() || dynamic_object_sprite.HasMonthDayCondition() || dynamic_object_sprite.HasSeasonCondition() || dynamic_object_sprite.HasYearCondition() || dynamic_object_sprite.HasWeatherCondition())
											{
												dynamic_object_sprites.push_back(dynamic_object_sprite);
												g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Loaded Dynamic Object Sprite: %s", MOD_NAME, VERSION, json_dynamic_object_sprite_original_sprite_name.c_str());
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
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s", MOD_NAME, VERSION, DYNAMIC_OBJECT_SPRITES_KEY.c_str(), filename.c_str());
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

			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error reading dynamic portrait config files in folder: %s", MOD_NAME, VERSION, dynamic_object_sprites_folder.c_str());
		}
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred reading the dynamic portrait config files.", MOD_NAME, VERSION);
	}
}

RValue GetDynamicObjectSprite(std::string sprite_name)
{
	for (DynamicObjectSprite dynamic_object_sprite : dynamic_object_sprites)
	{
		if (!dynamic_object_sprite.IsFloorSprite() && sprite_name != dynamic_object_sprite.original_sprite_name)
			continue;

		if (dynamic_object_sprite.IsFloorSprite() && sprite_name != dynamic_object_sprite.base_sprite)
			continue;

		if (dynamic_object_sprite.HasTimeOfDayCondition())
		{
			if (dynamic_object_sprite.time_of_day == daytime_to_id_map["day"] && IsNight())
				continue;
			if (dynamic_object_sprite.time_of_day == daytime_to_id_map["night"] && !IsNight())
				continue;
		}

		if (dynamic_object_sprite.HasWeekDayCondition() && ((current_day - 1) % 7) != dynamic_object_sprite.week_day)
			continue;

		if (dynamic_object_sprite.HasMonthDayCondition() && current_day != dynamic_object_sprite.month_day)
			continue;

		if (dynamic_object_sprite.HasSeasonCondition() && current_season != dynamic_object_sprite.season)
			continue;

		if (dynamic_object_sprite.HasYearCondition() && current_year != dynamic_object_sprite.year)
			continue;

		if (dynamic_object_sprite.HasWeatherCondition() && current_weather != dynamic_object_sprite.weather)
			continue;

		if (!dynamic_object_sprite.IsFloorSprite())
		{
			RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { RValue(dynamic_object_sprite.replacement_sprite_name) });
			if (replacement_sprite.m_Kind == VALUE_REF)
				return replacement_sprite;
		}
		else
		{
			RValue original = g_ModuleInterface->CallBuiltin("asset_get_index", { RValue(dynamic_object_sprite.original_sprite_name) });
			RValue replacement = g_ModuleInterface->CallBuiltin("asset_get_index", { RValue(dynamic_object_sprite.replacement_sprite_name) });
			
			g_ModuleInterface->CallBuiltin("sprite_assign", { original, replacement });
			return RValue();
		}
	}

	return RValue();
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

RValue& GmlScriptNodeObjectSetSpriteCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (ArgumentCount > 0 && Arguments[0]->m_Kind == VALUE_REF)
	{
		RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { *Arguments[0] });
		if (type.ToInt64() == 1) // asset_sprite
		{
			RValue sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { *Arguments[0] });
			std::string sprite_name_str = sprite_name.ToString();

			RValue dynamic_object_sprite = GetDynamicObjectSprite(sprite_name_str);
			if(dynamic_object_sprite.m_Kind == VALUE_REF)
				*Arguments[0] = dynamic_object_sprite;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_NODE_OBJECT_SET_SPRITE));
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
		LoadConfigFiles();
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

void CreateHookGmlScriptNodeObjectSetSprite(AurieStatus& status)
{
	CScript* gml_script_node_object_set_sprite = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_NODE_OBJECT_SET_SPRITE,
		(PVOID*)&gml_script_node_object_set_sprite
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NODE_OBJECT_SET_SPRITE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_NODE_OBJECT_SET_SPRITE,
		gml_script_node_object_set_sprite->m_Functions->m_ScriptFunction,
		GmlScriptNodeObjectSetSpriteCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NODE_OBJECT_SET_SPRITE);
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

	CreateHookGmlScriptNodeObjectSetSprite(status);
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