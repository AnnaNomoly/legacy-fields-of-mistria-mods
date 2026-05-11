#include <algorithm>
#include <filesystem>
#include <set>
#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "BugRadar";
static const char* const VERSION = "1.0.2";
static const char* const BUG_LIST_KEY = "bug_list";
static const char* const MODIFY_BUG_SPAWN_LOCATION_KEY = "modify_bug_spawn_location";
static const char* const DISPLAY_NOTIFICATIONS_KEY = "display_notifications";
static const std::string BUG_NAME_PLACEHOLDER_TEXT = "<BUG>";
static const char* const BUG_DETECTED_NOTIFICATION_KEY = "Notifications/Mods/Bug Radar/bug_detected";
static const std::vector<std::string> DEFAULT_BUG_LIST = {"Fairy Bee", "Flower Crown Beetle", "Snowball Beetle", "Speedy Snail", "Strobe Firefly"};
static const bool DEFAULT_MODIFY_BUG_SPAWN_LOCATION = true;
static const bool DEFAULT_DISPLAY_NOTIFICATIONS = true;

struct BugRadarConfig
{
	std::vector<std::string> bug_list = DEFAULT_BUG_LIST;
	bool modify_bug_spawn_location = DEFAULT_MODIFY_BUG_SPAWN_LOCATION;
	bool display_notifications = DEFAULT_DISPLAY_NOTIFICATIONS;
};

void to_json(json& json_object, const BugRadarConfig& config)
{
	json_object = json{
		{ BUG_LIST_KEY, config.bug_list },
		{ MODIFY_BUG_SPAWN_LOCATION_KEY, config.modify_bug_spawn_location },
		{ DISPLAY_NOTIFICATIONS_KEY, config.display_notifications }
	};
}

void from_json(const json& json_object, BugRadarConfig& config)
{
	config.bug_list = MMAPI::Config::GetValue<std::vector<std::string>>(json_object, BUG_LIST_KEY, DEFAULT_BUG_LIST);
	config.modify_bug_spawn_location = MMAPI::Config::GetValue<bool>(json_object, MODIFY_BUG_SPAWN_LOCATION_KEY, DEFAULT_MODIFY_BUG_SPAWN_LOCATION);
	config.display_notifications = MMAPI::Config::GetValue<bool>(json_object, DISPLAY_NOTIFICATIONS_KEY, DEFAULT_DISPLAY_NOTIFICATIONS);
}

static const std::map<std::string, std::vector<std::vector<std::pair<int, int>>>> ROOM_BUG_SPAWN_BOUNDING_BOXES_MAP = {
	{ "western_ruins", {
		{   // BBox 1
			{1360, 648},
			{1472, 648},
			{1472, 832},
			{1360, 832}
		},
		{	// BBox 2
			{1240, 792},
			{1592, 792},
			{1592, 848},
			{1240, 848}
		},
		{	// BBox 3
			{1128, 912},
			{1312, 912},
			{1312, 1112},
			{1128, 1112}
		},
		{	// BBox 4
			{1368, 936},
			{1464, 936},
			{1464, 1112},
			{1368, 1112}
		},
		{	// BBox 5
			{1520, 912},
			{1688, 912},
			{1688, 1096},
			{1520, 1096}
		},
		{	// BBox 6
			{1288, 1192},
			{1512, 1192},
			{1512, 1248},
			{1288, 1248}
		},
		{	// BBox 7
			{1160, 1224},
			{1304, 1224},
			{1304, 1256},
			{1160, 1256}
		},
		{	// BBox 8
			{1160, 1336},
			{1224, 1336},
			{1224, 1384},
			{1160, 1384}
		},
		{	// BBox 9
			{1240, 1224},
			{1304, 1224},
			{1304, 1312},
			{1240, 1312}
		},
		{	// BBox 10
			{1232, 1352},
			{1304, 1352},
			{1304, 1384},
			{1232, 1384}
		},
		{	// BBox 11
			{1544, 1184},
			{1632, 1184},
			{1632, 1384},
			{1544, 1384}
		},
	}},
	{ "narrows", {
		{
			// BBox 1
			{64, 448},
			{400, 448},
			{400, 544},
			{64, 544}
		},
		{
			// BBox 2
			{208, 688},
			{400, 688},
			{400, 800},
			{208, 800}
		},
		{
			// BBox 3
			{240, 832},
			{400, 832},
			{400, 896},
			{240, 896}
		},
		{
			// BBox 4
			{560, 416},
			{656, 416},
			{656, 528},
			{560, 528}
		},
		{
			// BBox 5
			{720, 432},
			{832, 432},
			{832, 528},
			{720, 528}
		},
		{
			// BBox 6
			{880, 416},
			{976, 416},
			{976, 496},
			{880, 496}
		},
		{
			// BBox 7
			{1056, 368},
			{1136, 368},
			{1136, 448},
			{1056, 448}
		},
		{
			// BBox 8
			{1008, 544},
			{1200, 544},
			{1200, 608},
			{1008, 608}
		},
		{
			// BBox 9
			{560, 640},
			{656, 640},
			{656, 896},
			{560, 896}
		},
		{
			// BBox 10
			{672, 624},
			{912, 624},
			{912, 688},
			{672, 688}
		},
		{
			// BBox 11
			{672, 784},
			{976, 784},
			{976, 880},
			{672, 880}
		},
		{
			// BBox 12
			{944, 608},
			{1040, 608},
			{1040, 800},
			{944, 800}
		},
		{
			// BBox 13
			{1056, 704},
			{1392, 704},
			{1392, 784},
			{1056, 784}
		},
		{
			// BBox 14
			{944, 864},
			{1216, 864},
			{1216, 976},
			{944, 976}
		},
		{
			// BBox 15
			{1312, 848},
			{1664, 848},
			{1664, 928},
			{1312, 928}
		},
		{
			// BBox 16
			{1376, 944},
			{1456, 944},
			{1456, 992},
			{1376, 992}
		},
		{
			// BBox 17
			{1232, 896},
			{1312, 896},
			{1312, 1072},
			{1232, 1072}
		},
		{
			// BBox 18
			{976, 1168},
			{1120, 1168},
			{1120, 1232},
			{976, 1232}
		},
		{
			// BBox 19
			{1136, 1088},
			{1456, 1088},
			{1456, 1232},
			{1136, 1232}
		},
		{
			// BBox 20
			{688, 1600},
			{800, 1600},
			{800, 1760},
			{688, 1760}
		},
		{
			// BBox 21
			{848, 1456},
			{1408, 1456},
			{1408, 1648},
			{848, 1648}
		},
		{
			// BBox 22
			{1296, 1632},
			{1488, 1632},
			{1488, 1712},
			{1296, 1712}
		},
		{
			// BBox 23
			{1504, 1680},
			{1680, 1680},
			{1680, 1760},
			{1504, 1760}
		}
	}},
	{ "haydens_farm", {
		{   // BBox 1
			{560, 304},
			{848, 304},
			{848, 448},
			{560, 448}
		},
		{	// BBox 2
			{352, 528},
			{864, 528},
			{864, 896},
			{352, 896}
		},
		{	// BBox 3
			{880, 640},
			{1040, 640},
			{1040, 896},
			{880, 896}
		},
		{	// BBox 4
			{992, 272},
			{1072, 272},
			{1072, 400},
			{992, 400}
		},
		{	// BBox 5
			{1088, 352},
			{1248, 352},
			{1248, 544},
			{1088, 544}
		},
		{	// BBox 6
			{1264, 304},
			{1392, 304},
			{1392, 544},
			{1264, 544}
		},
		{	// BBox 7
			{1056, 624},
			{1328, 624},
			{1328, 768},
			{1056, 768}
		},
		{	// BBox 8
			{1344, 624},
			{1696, 624},
			{1696, 752},
			{1344, 752}
		},
		{	// BBox 9
			{1504, 560},
			{1712, 560},
			{1712, 624},
			{1504, 624}
		},
		{	// BBox 10
			{1632, 480},
			{1728, 480},
			{1728, 560},
			{1632, 560}
		},
		{	// BBox 11
			{1600, 752},
			{1680, 752},
			{1680, 832},
			{1600, 832}
		},
		{	// BBox 12
			{1312, 848},
			{1664, 848},
			{1664, 944},
			{1312, 944}
		},
	}},
	{ "beach", {
		{
			// BBox 1
			{1216, 320},
			{1360, 320},
			{1360, 400},
			{1216, 400},
		},
		{
			// BBox 2
			{1440, 304},
			{1520, 304},
			{1520, 400},
			{1440, 400},
		},
		{
			// BBox 3
			{1536, 256},
			{1648, 256},
			{1648, 368},
			{1536, 368} // TODO: test spawning a bug directly on this spot
		},
		{
			// BBox 4
			{1696, 152},
			{1776, 152},
			{1776, 384},
			{1696, 384},
		},
		{
			// BBox 5
			{1824, 248},
			{2352, 248},
			{2352, 352},
			{1824, 352},
		},
		{
			// BBox 6
			{2480, 232},
			{2560, 232},
			{2560, 480},
			{2480, 480},
		},
		{
			// BBox 7
			{2280, 368},
			{2360, 368},
			{2360, 472},
			{2280, 472},
		},
		{
			// BBox 8
			{2280, 488},
			{2552, 488},
			{2552, 576},
			{2280, 576},
		},
		{
			// BBox 9
			{2360, 584},
			{2496, 592},
			{2496, 688},
			{2360, 688},
		},
		{
			// BBox 10
			{1328, 528},
			{1544, 528},
			{1544, 688},
			{1328, 688},
		},
		{
			// BBox 11
			{1560, 472},
			{2056, 472},
			{2056, 576},
			{1560, 576},
		},
		{
			// BBox 12
			{1984, 552},
			{2224, 552},
			{2224, 648},
			{1984, 648},
		}
	}},
	{ "town", {
		{
			// BBox 1
			{184, 808},
			{408, 808},
			{408, 944},
			{184, 936}
		},
		{
			// BBox 2
			{200, 1032},
			{424, 1032},
			{424, 1344},
			{200, 1344}
		},
		{
			// BBox 3
			{176, 1600},
			{432, 1600},
			{432, 1774},
			{176, 1774}
		},
		{
			// BBox 4
			{184, 2024},
			{480, 2024},
			{480, 2208},
			{184, 2208}
		},
		{
			// BBox 5
			{488, 2168},
			{664, 2168},
			{664, 2288},
			{488, 2288}
		},
		{
			// BBox 6
			{488, 1992},
			{728, 1992},
			{728, 2080},
			{488, 2080}
		},
		{
			// BBox 7
			{752, 2032},
			{1048, 2032},
			{1048, 2160},
			{752, 2160}
		},
		{
			// BBox 8
			{552, 2408},
			{680, 2408},
			{680, 2488},
			{552, 2488}
		},
		{
			// BBox 9
			{696, 2288},
			{1040, 2288},
			{1040, 2424},
			{696, 2424}
		},
		{
			// BBox 10
			{648, 2608},
			{960, 2608},
			{960, 2704},
			{648, 2704}
		},
		{
			// BBox 11
			{1208, 2168},
			{1416, 2168},
			{1416, 2272},
			{1208, 2272}
		},
		{
			// BBox 12
			{1224, 2376},
			{1424, 2376},
			{1424, 2448},
			{1224, 2448}
		},
		{
			// BBox 13
			{976, 2568},
			{1248, 2568},
			{1248, 2744},
			{976, 2744}
		},
		{
			// BBox 14
			{1256, 2608},
			{1504, 2608},
			{1504, 2752},
			{1256, 2752}
		},
		{
			// BBox 15
			{1496, 1992},
			{1640, 1992},
			{1648, 2128},
			{1496, 2128}
		},
		{
			// BBox 16
			{1440, 2216},
			{1640, 2216},
			{1640, 2416},
			{1440, 2416}
		},
		{
			// BBox 17
			{1760, 2280},
			{1976, 2280},
			{1984, 2448},
			{1760, 2448}
		},
		{
			// BBox 18
			{1776, 1040},
			{2016, 1040},
			{2016, 1144},
			{1776, 1144}
		}
	}},
	{ "eastern_road", {
		{
			// BBox 1
			{88, 688},
			{256, 688},
			{256, 784},
			{88, 784}
		},
		{
			// BBox 2
			{432, 520},
			{560, 520},
			{560, 584},
			{432, 584}
		},
		{
			// BBox 3
			{264, 664},
			{528, 664},
			{528, 864},
			{264, 864}
		},
		{
			// BBox 4
			{200, 952},
			{328, 952},
			{328, 1080},
			{200, 1080}
		},
		{
			// BBox 5
			{336, 840},
			{896, 840},
			{896, 1024},
			{336, 1024}
		},
		{
			// BBox 6
			{632, 600},
			{944, 600},
			{944, 752},
			{632, 752}
		},
		{
			// BBox 7
			{824, 440},
			{968, 440},
			{968, 576},
			{824, 576}
		},
		{
			// BBox 8
			{976, 488},
			{1192, 488},
			{1192, 560},
			{976, 568}
		},
		{
			// BBox 9
			{936, 696},
			{1096, 696},
			{1096, 832},
			{936, 832}
		},
		{
			// BBox 10
			{1368, 704},
			{1504, 704},
			{1504, 808},
			{1368, 808}
		},
		{
			// BBox 11
			{896, 912},
			{1152, 912},
			{1152, 1008},
			{896, 1008}
		},
		{
			// BBox 12
			{1032, 1096},
			{1184, 1096},
			{1184, 1216},
			{1032, 1216}
		},
		{
			// BBox 13
			{832, 1264},
			{1144, 1264},
			{1144, 1384},
			{832, 1384}
		},
		{
			// BBox 14
			{648, 1384},
			{944, 1384},
			{936, 1584},
			{648, 1584}
		},
		{
			// BBox 15
			{264, 1448},
			{600, 1448},
			{592, 1552},
			{264, 1552}
		},
		{
			// BBox 16
			{728, 1656},
			{888, 1656},
			{888, 1808},
			{728, 1808}
		},
		{
			// BBox 17
			{624, 1864},
			{904, 1864},
			{904, 2000},
			{624, 2000}
		},
		{
			// BBox 18
			{128, 1944},
			{352, 1944},
			{352, 2080},
			{128, 2080}
		}
	}},
	{ "deep_woods", {
		{
			// BBox 1
			{1232, 1248},
			{1440, 1248},
			{1440, 1392},
			{1232, 1392}
		},
		{
			// BBox 2
			{616, 1056},
			{1176, 1056},
			{1176, 1328},
			{616, 1328}
		},
		{
			// BBox 3
			{632, 920},
			{992, 920},
			{992, 1040},
			{632, 1040}
		},
		{
			// BBox 4
			{840, 554},
			{936, 554},
			{936, 912},
			{840, 912}
		},
		{
			// BBox 5
			{1808, 1224},
			{1912, 1224},
			{1912, 1392},
			{1808, 1392}
		},
		{
			// BBox 6
			{1928, 1288},
			{2184, 1288},
			{2184, 1424},
			{1928, 1424}
		},
		{
			// BBox 7
			{2200, 1208},
			{2296, 1208},
			{2296, 1368},
			{2200, 1368}
		},
		{
			// BBox 8
			{2000, 1176},
			{2112, 1176},
			{2112, 1280},
			{2000, 1280}
		},
		{
			// BBox 9
			{1832, 1024},
			{1952, 1024},
			{1952, 1128},
			{1832, 1128}
		},
		{
			// BBox 10
			{2000, 920},
			{2112, 920},
			{2112, 1120},
			{2000, 1120}
		},
		{
			// BBox 11
			{2160, 1024},
			{2264, 1024},
			{2264, 1120},
			{2160, 1120}
		}
	}}
};

static YYTKInterface* g_ModuleInterface = nullptr;
static BugRadarConfig config = {};
static bool localize_pending = false;
static std::string bug_name = "";
static std::set<int> tracked_bug_ids = {};
static std::map<std::string, std::vector<std::pair<int, int>>> ROOM_BUG_SPAWN_BOUNDING_BOX_CENTER_MAP = {};

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

void LogDefaultConfigValues()
{
	config = BugRadarConfig{};
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value!", MOD_NAME, VERSION, BUG_LIST_KEY);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, MODIFY_BUG_SPAWN_LOCATION_KEY, config.modify_bug_spawn_location ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISPLAY_NOTIFICATIONS_KEY, config.display_notifications ? "true" : "false");
}

void LoadOrCreateConfigFile()
{
	std::exception_ptr eptr;
	try
	{
		std::filesystem::path config_file = MMAPI::Config::GetConfigPath(MOD_NAME);
		bool config_file_exists = std::filesystem::exists(config_file);
		json json_object = MMAPI::Config::Load(config_file);

		if (!config_file_exists)
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Configuration file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.string().c_str());

		if (json_object.empty())
		{
			if (config_file_exists)
			{
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No readable values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.string().c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Defaults will be used and written back to the configuration file.", MOD_NAME, VERSION);
			}

			LogDefaultConfigValues();
			MMAPI::Config::Save(config_file, config);
			return;
		}

		config = json_object.get<BugRadarConfig>();
		MMAPI::Config::Save(config_file, config);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Loaded configuration file: %s", MOD_NAME, VERSION, config_file.string().c_str());
	}
	catch (...)
	{
		eptr = std::current_exception();
	}

	PrintError(eptr);
}

void CalculateBoundingBoxCenters()
{
	for (const auto& room : ROOM_BUG_SPAWN_BOUNDING_BOXES_MAP) {
		for (const auto& bounding_box : room.second) {
			int total_x = 0;
			int total_y = 0;

			for (const auto& point : bounding_box) {
				total_x += point.first;
				total_y += point.second;
			}

			int center_x = total_x / bounding_box.size();
			int center_y = total_y / bounding_box.size();
			std::pair<int, int> center = { center_x, center_y };
			ROOM_BUG_SPAWN_BOUNDING_BOX_CENTER_MAP[room.first].push_back(center);
		}
	}
}

double CalculateDistance(int x1, int y1, int x2, int y2) {
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

std::tuple<int, int, int> GenerateRandomPointInClosestBoundingBox(int X, int Y, const std::string& room_name) {
	double min_distance = DBL_MAX;
	size_t closest_index = 0;

	for (size_t i = 0; i < ROOM_BUG_SPAWN_BOUNDING_BOX_CENTER_MAP[room_name].size(); ++i) {
		const auto& center = ROOM_BUG_SPAWN_BOUNDING_BOX_CENTER_MAP[room_name][i];
		double distance = CalculateDistance(X, Y, center.first, center.second);

		if (distance < min_distance) {
			min_distance = distance;
			closest_index = i;
		}
	}

	const auto& bounding_box = ROOM_BUG_SPAWN_BOUNDING_BOXES_MAP.at(room_name)[closest_index];
	int min_x = bounding_box[0].first;
	int max_x = bounding_box[0].first;
	int min_y = bounding_box[0].second;
	int max_y = bounding_box[0].second;

	for (const auto& point : bounding_box) {
		min_x = min(min_x, point.first);
		max_x = max(max_x, point.first);
		min_y = min(min_y, point.second);
		max_y = max(max_y, point.second);
	}

	srand(static_cast<unsigned int>(time(0)));
	int random_x = min_x + rand() % (max_x - min_x + 1);
	int random_y = min_y + rand() % (max_y - min_y + 1);

	return { random_x, random_y, closest_index + 1 };
}

static std::string ToLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	return s;
}

void NormalizeBugList()
{
	if (!MMAPI::Internal::global_instance)
		return;

	std::vector<std::string> normalized_config_entries;
	normalized_config_entries.reserve(config.bug_list.size());
	for (const auto& entry : config.bug_list)
		normalized_config_entries.push_back(ToLower(entry));

	YYTK::RValue item_data = MMAPI::Internal::global_instance->GetMember("__item_data");
	size_t item_count = 0;
	g_ModuleInterface->GetArraySize(item_data, item_count);

	for (size_t i = 0; i < item_count; i++)
	{
		YYTK::RValue internal_name_rv = MMAPI::Item::GetInternalName(static_cast<int>(i));
		if (internal_name_rv.m_Kind != VALUE_STRING)
			continue;
		std::string internal_name_lc = ToLower(internal_name_rv.ToString());

		YYTK::RValue localized_name_rv = MMAPI::Item::GetLocalizedName(static_cast<int>(i));
		std::string localized_name_lc;
		if (localized_name_rv.m_Kind == VALUE_STRING)
			localized_name_lc = ToLower(localized_name_rv.ToString());

		for (const auto& entry : normalized_config_entries)
		{
			if (entry == internal_name_lc || (!localized_name_lc.empty() && entry == localized_name_lc))
			{
				tracked_bug_ids.insert(static_cast<int>(i));
				break;
			}
		}
	}
}

void AfterBugSpawn(MMAPI::Bug::BugSpawnContext& ctx)
{
	int item_id = ctx.GetItemId();
	if (!tracked_bug_ids.contains(item_id))
		return;

	YYTK::CInstance* bug = ctx.GetBug();
	if (!bug)
		return;

	YYTK::RValue localized_name_rv = MMAPI::Item::GetLocalizedName(item_id);
	bug_name = localized_name_rv.m_Kind == VALUE_STRING ? localized_name_rv.ToString() : "";

	if (config.display_notifications)
		MMAPI::Game::CreateNotification(false, BUG_DETECTED_NOTIFICATION_KEY);

	if (config.modify_bug_spawn_location)
	{
		MMAPI::Location::Ids current_location;
		if (!MMAPI::Location::TryGetCurrentLocation(current_location))
			return;

		YYTK::RValue location_name_rv = MMAPI::Location::GetInternalName(current_location);
		if (location_name_rv.m_Kind != VALUE_STRING)
			return;

		std::string location_name = location_name_rv.ToString();
		if (!ROOM_BUG_SPAWN_BOUNDING_BOXES_MAP.contains(location_name))
			return;

		YYTK::RValue x = MMAPI::Engine::InstanceVariableGet(bug, "x");
		YYTK::RValue y = MMAPI::Engine::InstanceVariableGet(bug, "y");

		auto [new_x, new_y, bbox_num] = GenerateRandomPointInClosestBoundingBox(
			static_cast<int>(x.ToDouble()),
			static_cast<int>(y.ToDouble()),
			location_name
		);

		MMAPI::Engine::InstanceVariableSet(bug, "x", new_x);
		MMAPI::Engine::InstanceVariableSet(bug, "y", new_y);

		g_ModuleInterface->Print(CM_GREEN, "[%s %s] - Modified bug (%s) to spawn at position (%d, %d) in Bounding Box (%d).",
			MOD_NAME, VERSION, bug_name.c_str(), new_x, new_y, bbox_num);
	}
}

void BeforeGetWeather()
{
	if (!localize_pending && tracked_bug_ids.empty())
		localize_pending = true;
}

void AfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& ctx)
{
	if (localize_pending)
	{
		localize_pending = false;
		NormalizeBugList();
	}

	if (ctx.GetKey() == BUG_DETECTED_NOTIFICATION_KEY && !bug_name.empty())
	{
		std::string resolved(ctx.GetResolved());
		size_t placeholder_pos = resolved.find(BUG_NAME_PLACEHOLDER_TEXT);
		if (placeholder_pos != std::string::npos)
		{
			resolved.replace(placeholder_pos, BUG_NAME_PLACEHOLDER_TEXT.length(), bug_name);
			ctx.SetResolved(std::move(resolved));
		}
	}
}

void BeforeSetupMainScreen()
{
	localize_pending = false;
	bug_name = "";
	tracked_bug_ids.clear();
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(g_ModuleInterface, global_instance, g_ArSelfModule);

	MMAPI::Bug::Enable();
	MMAPI::Text::Enable();
	MMAPI::Location::Enable();

	MMAPI::Bug::Hooks::AfterBugSpawn(AfterBugSpawn);
	MMAPI::Weather::Hooks::BeforeGetWeather(BeforeGetWeather);
	MMAPI::Text::Hooks::AfterLocalizedString(AfterLocalizedString);
	MMAPI::Game::Hooks::BeforeSetupMainScreen(BeforeSetupMainScreen);

	LoadOrCreateConfigFile();
	CalculateBoundingBoxCenters();

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}