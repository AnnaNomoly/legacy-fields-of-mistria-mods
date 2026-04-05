#include <set>
#include <map>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const VERSION = "1.1.2";
static const char* const MOD_NAME = "Curator";
static const char* const GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT = "gml_Script_choose_random_artifact@Archaeology@Archaeology";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const ALLOW_ARTIFACTS_FROM_ANY_SET_KEY = "allow_artifacts_from_any_set";
static const char* const DISABLE_COMMON_FINDS_KEY = "disable_common_finds";
static const char* const YYTK_KEY = "__YYTK";
static const char* const VERSION_KEY = "version";
static const char* const IGNORE_NEXT_DIG_SPOT_KEY = "ignore_next_dig_spot";
static const char* const UNPEATABLE = "unpeatable";
static const std::vector<std::string> DISABLED_ITEMS = { }; // TODO: Update as needed in future patches
static const std::vector<std::string> COMMON_FINDS_SET_ITEMS = { "clay", "peat", "shard_mass", "shards", "sod" };
static const bool PERK_MOD_VARIANT = true; // Indicates if the mod uses the custom perk.
static const bool DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET = false;
static const bool DEFAULT_DISABLE_COMMON_FINDS = false;

static YYTKInterface* g_ModuleInterface = nullptr;
static std::map<std::string, std::vector<int>> archaeology_set_to_items = {};
static std::map<int, std::string> item_id_to_archaeology_set = {};
static std::vector<std::string> struct_field_names = {};
static std::set<int> all_archaeology_items = {};
static std::vector<int> disabled_item_ids = {};
static std::vector<int> common_finds_item_ids = {};
static bool load_on_start = true;
static RValue __YYTK;
static std::mt19937 generator(std::random_device{}());
static std::map<std::string, bool> active_perk_map = {}; // Tracks which mod specific perks are active.
static std::map<std::string, int64_t> perk_name_to_id_map = {};
static bool allow_artifacts_from_any_set = DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET;
static bool disable_common_finds = DEFAULT_DISABLE_COMMON_FINDS;

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

std::string RValueKindAsString(RValue value)
{
	if (value.m_Kind == VALUE_REAL)
		return "VALUE_REAL";
	if (value.m_Kind == VALUE_STRING)
		return "VALUE_STRING";
	if (value.m_Kind == VALUE_ARRAY)
		return "VALUE_ARRAY";
	if (value.m_Kind == VALUE_PTR)
		return "VALUE_PTR";
	if (value.m_Kind == VALUE_VEC3)
		return "VALUE_VEC3";
	if (value.m_Kind == VALUE_UNDEFINED)
		return "VALUE_UNDEFINED";
	if (value.m_Kind == VALUE_OBJECT)
		return "VALUE_OBJECT";
	if (value.m_Kind == VALUE_INT32)
		return "VALUE_INT32";
	if (value.m_Kind == VALUE_VEC4)
		return "VALUE_VEC4";
	if (value.m_Kind == VALUE_VEC44)
		return "VALUE_VEC44";
	if (value.m_Kind == VALUE_INT64)
		return "VALUE_INT64";
	if (value.m_Kind == VALUE_ACCESSOR)
		return "VALUE_ACCESSOR";
	if (value.m_Kind == VALUE_NULL)
		return "VALUE_NULL";
	if (value.m_Kind == VALUE_BOOL)
		return "VALUE_BOOL";
	if (value.m_Kind == VALUE_ITERATOR)
		return "VALUE_ITERATOR";
	if (value.m_Kind == VALUE_REF)
		return "VALUE_REF";
	if (value.m_Kind == VALUE_UNSET)
		return "VALUE_UNSET";
}

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return RValueAsBool(global_variable_exists);
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return RValueAsBool(struct_exists);
}

RValue StructVariableGet(RValue the_struct, const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, variable_name }
	);
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists("__YYTK"))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet("__YYTK", __YYTK);
	}
	else
		__YYTK = GlobalVariableGet("__YYTK");
}

void CreateModInfoInGlobalYYTKVariable()
{
	if (!StructVariableExists(__YYTK, MOD_NAME))
	{
		RValue mod;
		RValue version = VERSION;
		RValue ignore_next_dig_spot = false;
		g_ModuleInterface->GetRunnerInterface().StructCreate(&mod);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&mod, VERSION_KEY, &version);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&mod, IGNORE_NEXT_DIG_SPOT_KEY, &ignore_next_dig_spot);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&__YYTK, MOD_NAME, &mod);
	}
}

void ResetStaticFields()
{
	active_perk_map = {};
}

bool GetStructFieldNames(
	IN const char* MemberName,
	IN OUT RValue* Value
)
{
	struct_field_names.push_back(MemberName);
	return false;
}

bool IgnoreNextDigSpot()
{
	if (GlobalVariableExists(YYTK_KEY))
	{
		RValue __YYTK = GlobalVariableGet(YYTK_KEY);
		if (StructVariableExists(__YYTK, MOD_NAME))
		{
			RValue Curator = StructVariableGet(__YYTK, MOD_NAME);
			if (StructVariableExists(Curator, IGNORE_NEXT_DIG_SPOT_KEY))
			{
				RValue ignore_next_dig_spot = StructVariableGet(Curator, IGNORE_NEXT_DIG_SPOT_KEY);
				return RValueAsBool(ignore_next_dig_spot);
			}
		}
	}

	return false;
}

void ParseDisabledItemAndCommonFindItemIds(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_try_string_to_item_id = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_try_string_to_item_id",
		(PVOID*)&gml_script_try_string_to_item_id
	);

	for (int i = 0; i < DISABLED_ITEMS.size(); i++)
	{
		RValue item_id;
		RValue item_name = RValue(DISABLED_ITEMS[i]);
		RValue* item_name_ptr = &item_name;
		
		gml_script_try_string_to_item_id->m_Functions->m_ScriptFunction(
			Self,
			Other,
			item_id,
			1,
			{ &item_name_ptr }
		);

		if (item_id.m_Kind == VALUE_INT32)
			disabled_item_ids.push_back(item_id.m_i32);
		if (item_id.m_Kind == VALUE_INT64)
			disabled_item_ids.push_back(item_id.m_i64);
		if (item_id.m_Kind == VALUE_REAL)
			disabled_item_ids.push_back(item_id.m_Real);
	}

	for (int i = 0; i < COMMON_FINDS_SET_ITEMS.size(); i++)
	{
		RValue item_id;
		RValue item_name = RValue(COMMON_FINDS_SET_ITEMS[i]);
		RValue* item_name_ptr = &item_name;

		gml_script_try_string_to_item_id->m_Functions->m_ScriptFunction(
			Self,
			Other,
			item_id,
			1,
			{ &item_name_ptr }
		);

		if (item_id.m_Kind == VALUE_INT32)
			common_finds_item_ids.push_back(item_id.m_i32);
		if (item_id.m_Kind == VALUE_INT64)
			common_finds_item_ids.push_back(item_id.m_i64);
		if (item_id.m_Kind == VALUE_REAL)
			common_finds_item_ids.push_back(item_id.m_Real);
	}
}

void ParsePerks()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	size_t array_length;
	RValue perks = *global_instance->GetRefMember("__perk__");
	g_ModuleInterface->GetArraySize(perks, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(perks, i, array_element);

		perk_name_to_id_map[array_element->ToString()] = i;
	}
}

void ParseMuseumData()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	// Find out the index for the archaeology museum wing.
	int archaeology_index = 0;
	RValue __museum_wing__ = *global_instance->GetRefMember("__museum_wing__");
	RValue __museum_wing__length = g_ModuleInterface->CallBuiltin("array_length", { __museum_wing__ });
	for (int i = 0; i < __museum_wing__length.m_Real; i++)
	{
		RValue array_entry = g_ModuleInterface->CallBuiltin("array_get", { __museum_wing__, i });
		if (array_entry.ToString() == "archaeology")
		{
			archaeology_index = i;
			break;
		}
	}

	// Get the archaeology sets.
	RValue __museum_data = *global_instance->GetRefMember("__museum_data");
	RValue data = *__museum_data.GetRefMember("data");
	RValue archaeology_wing = g_ModuleInterface->CallBuiltin("array_get", { data, archaeology_index });
	RValue archaeology_sets = *archaeology_wing.GetRefMember("sets");
	RValue archaeology_sets_inner = *archaeology_sets.GetRefMember("inner");

	struct_field_names = {};
	g_ModuleInterface->EnumInstanceMembers(archaeology_sets_inner, GetStructFieldNames);
	for (int i = 0; i < struct_field_names.size(); i++)
	{
		archaeology_set_to_items[struct_field_names[i]] = {};
		RValue archaeology_set = *archaeology_sets_inner.GetRefMember(struct_field_names[i]);
		RValue items = *archaeology_set.GetRefMember("items");
		RValue items_length = g_ModuleInterface->CallBuiltin("array_length", { items });
		for (int j = 0; j < items_length.m_Real; j++)
		{
			RValue item = g_ModuleInterface->CallBuiltin("array_get", { items, j });
			if (item.m_Kind == VALUE_REAL)
			{
				archaeology_set_to_items[struct_field_names[i]].push_back(item.m_Real);
				item_id_to_archaeology_set[item.m_Real] = struct_field_names[i];
				all_archaeology_items.insert(item.m_Real);
			}
			if (item.m_Kind == VALUE_INT64)
			{
				archaeology_set_to_items[struct_field_names[i]].push_back(item.m_i64);
				item_id_to_archaeology_set[item.m_i64] = struct_field_names[i];
				all_archaeology_items.insert(item.m_i64);
			}
		}
	}

	// Remove any disabled items from the all_archaeology_items list.
	for (int disabled_item : disabled_item_ids)
	{
		all_archaeology_items.erase(disabled_item);
	}

	// Remove any common find set items from the all_archaeology_items list.
	if (disable_common_finds)
	{
		for (int common_find_item : common_finds_item_ids)
		{
			all_archaeology_items.erase(common_find_item);
		}
	}
}

RValue GetMuseumProgress()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	return *global_instance->GetRefMember("__museum_progress_data");
}

bool PerkActive(CInstance* Self, CInstance* Other, std::string perk_name)
{
	int64_t perk_id = perk_name_to_id_map[perk_name];

	CScript* gml_script_perk_active = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_perk_active@Ari@Ari",
		(PVOID*)&gml_script_perk_active
	);

	RValue result;
	RValue input = perk_id;
	RValue* input_ptr = &input;
	gml_script_perk_active->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	if (result.m_Kind == VALUE_BOOL && result.m_Real == 1.0)
		return true;
	return false;
}

json CreateModConfigJson(bool use_defaults)
{
	json config_json = {
		{ ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, use_defaults ? DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET : allow_artifacts_from_any_set },
		{ DISABLE_COMMON_FINDS_KEY, use_defaults ? DEFAULT_DISABLE_COMMON_FINDS : disable_common_finds }
	};
	return config_json;
}

void LogDefaultModConfigValues()
{
	allow_artifacts_from_any_set = DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET;
	disable_common_finds = DEFAULT_DISABLE_COMMON_FINDS;

	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, allow_artifacts_from_any_set ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_COMMON_FINDS_KEY, disable_common_finds ? "true" : "false");
}

bool CreateOrLoadModConfigFile()
{
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

			// Verify the directory now exists.
			if (!std::filesystem::exists(mod_data_folder))
			{
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to create the \"mod_data\" directory: ", MOD_NAME, VERSION, mod_data_folder.c_str());
				return false;
			}
		}

		// Try to find the mod_data/Curator directory.
		std::string curator_folder = mod_data_folder + "\\Curator";
		if (!std::filesystem::exists(curator_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"Curator\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, curator_folder.c_str());
			std::filesystem::create_directory(curator_folder);

			// Verify the directory now exists.
			if (!std::filesystem::exists(curator_folder))
			{
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to create the \"Curator\" directory: ", MOD_NAME, VERSION, curator_folder.c_str());
				return false;
			}
		}

		// Try to find the mod_data/Curator/Curator.json config file.
		bool update_config_file = false;
		std::string config_file = curator_folder + "\\" + "Curator.json";
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
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Add your desired values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
					LogDefaultModConfigValues();
				}
				else
				{
					// Try loading the allow_artifacts_from_any_set value.
					if (json_object.contains(ALLOW_ARTIFACTS_FROM_ANY_SET_KEY))
					{
						allow_artifacts_from_any_set = json_object[ALLOW_ARTIFACTS_FROM_ANY_SET_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, allow_artifacts_from_any_set ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET ? "true" : "false");
					}

					// Try loading the disable_common_finds value.
					if (json_object.contains(DISABLE_COMMON_FINDS_KEY))
					{
						disable_common_finds = json_object[DISABLE_COMMON_FINDS_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_COMMON_FINDS_KEY, disable_common_finds ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, DISABLE_COMMON_FINDS_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_COMMON_FINDS_KEY, DEFAULT_DISABLE_COMMON_FINDS ? "true" : "false");
					}
				}

				update_config_file = true;
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
				LogDefaultModConfigValues();
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"Curator.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateModConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();

			// Verify the file now exists.
			if (!std::filesystem::exists(config_file))
			{
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to create the \"Curator.json\" file: ", MOD_NAME, VERSION, config_file.c_str());
				return false;
			}

			LogDefaultModConfigValues();
		}

		if (update_config_file)
		{
			json config_json = CreateModConfigJson(false);
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
		return false;
	}

	return true;
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

	if (strstr(self->m_Object->m_Name, "obj_ari"))
	{
		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);

		// Update active perks.
		if (PerkActive(global_instance->GetRefMember("__ari")->ToInstance(), self, UNPEATABLE))
			active_perk_map[UNPEATABLE] = true;
		else
			active_perk_map[UNPEATABLE] = false;
	}
}

RValue& GmlScriptChooseRandomArtifactCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	bool modify_dig_spot = false;
	if (!IgnoreNextDigSpot())
	{
		if (PERK_MOD_VARIANT)
		{
			if (active_perk_map[UNPEATABLE])
			{
				modify_dig_spot = true;
			}
		}
		else
		{
			modify_dig_spot = true;
		}
	}
	else
	{
		RValue Curator = StructVariableGet(__YYTK, MOD_NAME);
		RValue ignore_next_dig_spot = false;
		StructVariableSet(Curator, "ignore_next_dig_spot", ignore_next_dig_spot);
	}

	if (modify_dig_spot)
	{
		if (Result.m_Kind == VALUE_INT64)
		{
			// Check if the artifact is a museum item.
			if (item_id_to_archaeology_set.count(Result.m_i64) > 0)
			{
				// Check if the artifact has already been donated.
				RValue museum_progress = GetMuseumProgress();
				RValue item_donated = g_ModuleInterface->CallBuiltin("array_get", { museum_progress, Result.m_i64 });
				if (RValueAsBool(item_donated))
				{
					// Make a list of candidate replacements from the set.
					std::string archaeology_set = item_id_to_archaeology_set[Result.m_i64];
					std::vector<int> archaeology_set_items = archaeology_set_to_items[archaeology_set];
					std::vector<int> candidate_replacement_items = {};
					for (int i = 0; i < archaeology_set_items.size(); i++)
					{
						RValue set_item_donated = g_ModuleInterface->CallBuiltin("array_get", { museum_progress, archaeology_set_items[i] });
						if (!RValueAsBool(set_item_donated) && std::find(disabled_item_ids.begin(), disabled_item_ids.end(), archaeology_set_items[i]) == disabled_item_ids.end())
						{
							candidate_replacement_items.push_back(archaeology_set_items[i]);
						}
					}

					if (candidate_replacement_items.size() > 0)
					{
						// Pick a random artifact from the candidates.
						std::uniform_int_distribution<int> distribution(0, candidate_replacement_items.size() - 1);
						int random = distribution(generator);
						Result.m_i64 = candidate_replacement_items[random];

						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Changed the donated artifact to a missing one from the matching set: %s!", MOD_NAME, VERSION, archaeology_set.c_str());
					}
					else if (allow_artifacts_from_any_set)
					{
						// Pick a new artifact randomly from any set.
						RValue museum_progress = GetMuseumProgress();
						std::vector<int64_t> possible_artifacts = {};

						for (int64_t item_id : all_archaeology_items)
						{
							RValue item_donated = g_ModuleInterface->CallBuiltin("array_get", { museum_progress, item_id });
							if (!RValueAsBool(item_donated))
							{
								possible_artifacts.push_back(item_id);
							}
						}

						if (possible_artifacts.size() > 0)
						{
							std::uniform_int_distribution<int> distribution(0, possible_artifacts.size() - 1);
							int random = distribution(generator);
							Result.m_i64 = possible_artifacts[random];

							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Changed the donated artifact to a missing one from any set!", MOD_NAME, VERSION, archaeology_set.c_str());
						}
						else if (disable_common_finds)
						{
							std::uniform_int_distribution<int> distribution(0, all_archaeology_items.size() - 1);
							int random = distribution(generator);
							Result.m_i64 = *std::next(all_archaeology_items.begin(), random);

							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - All artifacts have been donated! The artifact was a Common Finds item and was randomly replaced.", MOD_NAME, VERSION, archaeology_set.c_str());
						}
						else
						{
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - All artifacts have been donated! The dig spot was unmodified.", MOD_NAME, VERSION, archaeology_set.c_str());
						}
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - All artifacts have been already donated for the matching set: %s!", MOD_NAME, VERSION, archaeology_set.c_str());
					}
				}
			}
		}
		else
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Expected gml_Script_choose_random_artifact Result to be type INT64, but it was %s!", MOD_NAME, VERSION, RValueKindAsString(Result).c_str());
		}
	}
	else
	{
		RValue Curator = StructVariableGet(__YYTK, MOD_NAME);
		RValue ignore_next_dig_spot = false;
		StructVariableSet(Curator, IGNORE_NEXT_DIG_SPOT_KEY, ignore_next_dig_spot);
	}

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
		CreateOrGetGlobalYYTKVariable();
		CreateModInfoInGlobalYYTKVariable();
		ParseDisabledItemAndCommonFindItemIds(Self, Other);
		ParseMuseumData();
		ParsePerks();
		CreateOrLoadModConfigFile();
		load_on_start = false;
	}
	else
	{
		ResetStaticFields();
	}

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

void CreateHookGmlScriptChooseRandomArtifact(AurieStatus& status)
{
	CScript* gml_script_choose_random_artifact = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT,
		(PVOID*)&gml_script_choose_random_artifact
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT,
		gml_script_choose_random_artifact->m_Functions->m_ScriptFunction,
		GmlScriptChooseRandomArtifactCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT);
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

EXPORTED AurieStatus ModuleInitialize(
	IN AurieModule* Module,
	IN const fs::path& ModulePath
)
{
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

	CreateHookGmlScriptChooseRandomArtifact(status);
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