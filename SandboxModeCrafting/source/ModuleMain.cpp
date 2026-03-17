#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "SandboxModeCrafting";
static const char* const VERSION = "1.1.0";
static const char* const BLACKSMITHING_JSON_KEY = "blacksmithing";
static const char* const COOKING_JSON_KEY = "cooking";
static const char* const MILLING_JSON_KEY = "milling";
static const char* const REFINING_JSON_KEY = "refining";
static const char* const WOODCRAFTING_JSON_KEY = "woodcrafting";
static const char* const DISABLE_TIME_JSON_KEY = "disable_time";
static const char* const DISABLE_MATERIALS_JSON_KEY = "disable_materials";
static const char* const GML_SCRIPT_CRAFTING_MENU_INITIALIZE = "gml_Script_initialize@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_CRAFTING_MENU_CLOSE = "gml_Script_on_close@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_PAY_COMPONENT_COSTS = "gml_Script_pay_component_costs";
static const char* const GML_SCRIPT_MAXIMUM_CRAFTS = "gml_Script_maximum_crafts@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_CHECK_ITEM_CRAFTABLE = "gml_Script_check_item_craftable@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool run_once_on_start = true;
static std::map<std::string, int> crafting_context_to_id_map = {};
static int current_crafting_context = -1;

static struct Configuration {
	struct CraftingOptions {
		bool disable_time = true;
		bool disable_materials = true;
	};

	CraftingOptions blacksmithing = CraftingOptions();
	CraftingOptions cooking = CraftingOptions();
	CraftingOptions milling = CraftingOptions();
	CraftingOptions refining = CraftingOptions();
	CraftingOptions woodcrafting = CraftingOptions();
};
static Configuration configuration = Configuration();

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

json CreateConfigJson(bool use_defaults)
{
	json blacksmithing_options_json = {
		{ DISABLE_TIME_JSON_KEY, use_defaults ? true : configuration.blacksmithing.disable_time },
		{ DISABLE_MATERIALS_JSON_KEY, use_defaults ? true : configuration.blacksmithing.disable_materials },
	};
	json cooking_options_json = {
		{ DISABLE_TIME_JSON_KEY, use_defaults ? true : configuration.cooking.disable_time },
		{ DISABLE_MATERIALS_JSON_KEY, use_defaults ? true : configuration.cooking.disable_materials },
	};
	json milling_options_json = {
		{ DISABLE_TIME_JSON_KEY, use_defaults ? true : configuration.milling.disable_time },
		{ DISABLE_MATERIALS_JSON_KEY, use_defaults ? true : configuration.milling.disable_materials },
	};
	json refining_options_json = {
		{ DISABLE_TIME_JSON_KEY, use_defaults ? true : configuration.refining.disable_time },
		{ DISABLE_MATERIALS_JSON_KEY, use_defaults ? true : configuration.refining.disable_materials },
	};
	json woodcrafting_options_json = {
		{ DISABLE_TIME_JSON_KEY, use_defaults ? true : configuration.woodcrafting.disable_time },
		{ DISABLE_MATERIALS_JSON_KEY, use_defaults ? true : configuration.woodcrafting.disable_materials },
	};

	json config_json = {
		{ BLACKSMITHING_JSON_KEY, blacksmithing_options_json },
		{ COOKING_JSON_KEY, cooking_options_json },
		{ MILLING_JSON_KEY, milling_options_json },
		{ REFINING_JSON_KEY, refining_options_json },
		{ WOODCRAFTING_JSON_KEY, woodcrafting_options_json }
	};
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

		// Try to find the mod_data/SandboxModeCrafting directory.
		std::string sandbox_mode_crafting_folder = mod_data_folder + "\\SandboxModeCrafting";
		if (!std::filesystem::exists(sandbox_mode_crafting_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"SandboxModeCrafting\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, sandbox_mode_crafting_folder.c_str());
			std::filesystem::create_directory(sandbox_mode_crafting_folder);
		}

		// Try to find the mod_data/SandboxModeCrafting/SandboxModeCrafting.json config file.
		bool update_config_file = false;
		std::string config_file = sandbox_mode_crafting_folder + "\\" + "SandboxModeCrafting.json";
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
				}
				else
				{
					// Try loading the blacksmithing options.
					if (json_object.contains(BLACKSMITHING_JSON_KEY) && json_object.at(BLACKSMITHING_JSON_KEY).is_object())
					{
						json blacksmithing_json = json_object[BLACKSMITHING_JSON_KEY];
						if (blacksmithing_json.contains(DISABLE_TIME_JSON_KEY) && blacksmithing_json.at(DISABLE_TIME_JSON_KEY).is_boolean())
							configuration.blacksmithing.disable_time = blacksmithing_json[DISABLE_TIME_JSON_KEY];
						if (blacksmithing_json.contains(DISABLE_MATERIALS_JSON_KEY) && blacksmithing_json.at(DISABLE_MATERIALS_JSON_KEY).is_boolean())
							configuration.blacksmithing.disable_materials = blacksmithing_json[DISABLE_MATERIALS_JSON_KEY];
					}

					// Try loading the cooking options.
					if (json_object.contains(COOKING_JSON_KEY) && json_object.at(COOKING_JSON_KEY).is_object())
					{
						json cooking_json = json_object[COOKING_JSON_KEY];
						if (cooking_json.contains(DISABLE_TIME_JSON_KEY) && cooking_json.at(DISABLE_TIME_JSON_KEY).is_boolean())
							configuration.cooking.disable_time = cooking_json[DISABLE_TIME_JSON_KEY];
						if (cooking_json.contains(DISABLE_MATERIALS_JSON_KEY) && cooking_json.at(DISABLE_MATERIALS_JSON_KEY).is_boolean())
							configuration.cooking.disable_materials = cooking_json[DISABLE_MATERIALS_JSON_KEY];
					}

					// Try loading the milling options.
					if (json_object.contains(MILLING_JSON_KEY) && json_object.at(MILLING_JSON_KEY).is_object())
					{
						json milling_json = json_object[MILLING_JSON_KEY];
						if (milling_json.contains(DISABLE_TIME_JSON_KEY) && milling_json.at(DISABLE_TIME_JSON_KEY).is_boolean())
							configuration.milling.disable_time = milling_json[DISABLE_TIME_JSON_KEY];
						if (milling_json.contains(DISABLE_MATERIALS_JSON_KEY) && milling_json.at(DISABLE_MATERIALS_JSON_KEY).is_boolean())
							configuration.milling.disable_materials = milling_json[DISABLE_MATERIALS_JSON_KEY];
					}

					// Try loading the refining options.
					if (json_object.contains(REFINING_JSON_KEY) && json_object.at(REFINING_JSON_KEY).is_object())
					{
						json refining_json = json_object[REFINING_JSON_KEY];
						if (refining_json.contains(DISABLE_TIME_JSON_KEY) && refining_json.at(DISABLE_TIME_JSON_KEY).is_boolean())
							configuration.refining.disable_time = refining_json[DISABLE_TIME_JSON_KEY];
						if (refining_json.contains(DISABLE_MATERIALS_JSON_KEY) && refining_json.at(DISABLE_MATERIALS_JSON_KEY).is_boolean())
							configuration.refining.disable_materials = refining_json[DISABLE_MATERIALS_JSON_KEY];
					}

					// Try loading the woodcrafting options.
					if (json_object.contains(WOODCRAFTING_JSON_KEY) && json_object.at(WOODCRAFTING_JSON_KEY).is_object())
					{
						json woodcrafting_json = json_object[WOODCRAFTING_JSON_KEY];
						if (woodcrafting_json.contains(DISABLE_TIME_JSON_KEY) && woodcrafting_json.at(DISABLE_TIME_JSON_KEY).is_boolean())
							configuration.woodcrafting.disable_time = woodcrafting_json[DISABLE_TIME_JSON_KEY];
						if (woodcrafting_json.contains(DISABLE_MATERIALS_JSON_KEY) && woodcrafting_json.at(DISABLE_MATERIALS_JSON_KEY).is_boolean())
							configuration.woodcrafting.disable_materials = woodcrafting_json[DISABLE_MATERIALS_JSON_KEY];
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
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"SandboxModeCrafting.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();
		}

		if (update_config_file)
		{
			json config_json = CreateConfigJson(false);
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

RValue StructVariableGet(RValue the_struct, std::string variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, RValue(variable_name) }
	);
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void ModifyItemCraftTime(RValue item)
{
	if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "recipe"))
	{
		RValue recipe = StructVariableGet(item, "recipe");
		if (recipe.m_Kind == VALUE_OBJECT && StructVariableExists(recipe, "components"))
		{
			RValue components = StructVariableGet(recipe, "components");
			if (components.m_Kind == VALUE_OBJECT && StructVariableExists(components, "__buffer"))
			{
				RValue buffer = StructVariableGet(components, "__buffer");
				if (buffer.m_Kind == VALUE_ARRAY)
				{
					size_t buffer_array_length;
					g_ModuleInterface->GetArraySize(buffer, buffer_array_length);

					for (size_t j = 0; j < buffer_array_length; j++)
					{
						RValue buffer_element = g_ModuleInterface->CallBuiltin("array_get", { buffer, j });
						if (buffer_element.m_Kind == VALUE_OBJECT && StructVariableExists(buffer_element, "duration"))
							StructVariableSet(buffer_element, "duration", 0);
					}
				}
			}
		}
	}
}

void LoadItemData()
{
	size_t array_length;
	RValue item_data = *global_instance->GetRefMember("__item_data");
	g_ModuleInterface->GetArraySize(item_data, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue item = g_ModuleInterface->CallBuiltin("array_get", { item_data, i });

		if (IsObject(item) && StructVariableExists(item, "recipe") && StructVariableExists(item, "crafting_level_requirement"))
		{
			// Cooking check.
			if (StructVariableExists(item, "kitchen_tier_requirement") && configuration.cooking.disable_time)
			{
				RValue kitchen_tier_requirement = item.GetMember("kitchen_tier_requirement");
				if (IsNumeric(kitchen_tier_requirement))
				{
					ModifyItemCraftTime(item);
					continue;
				}
			}
			// All other crafting.
			if (StructVariableExists(item, "tags"))
			{
				RValue tags = item.GetMember("tags");
				RValue buffer = tags.GetMember("__buffer");

				size_t buffer_length = 0;
				g_ModuleInterface->GetArraySize(buffer, buffer_length);
				for (size_t j = 0; j < buffer_length; j++)
				{
					RValue* buffer_element;
					g_ModuleInterface->GetArrayEntry(buffer, j, buffer_element);

					// Blacksmithing
					if (buffer_element->ToString() == "blacksmithing" && configuration.blacksmithing.disable_time)
					{
						ModifyItemCraftTime(item);
						continue;
					}
					// Refining
					else if ((buffer_element->ToString().contains("refine") || buffer_element->ToString().contains("essence")) && configuration.refining.disable_time)
					{
						ModifyItemCraftTime(item);
						continue;
					}
					// Woodcrafting 
					else if (buffer_element->ToString() == "furniture" && configuration.woodcrafting.disable_time)
					{
						ModifyItemCraftTime(item);
						continue;
					}
					// Milling
					else if (buffer_element->ToString() == "milling" && configuration.milling.disable_time)
					{
						ModifyItemCraftTime(item);
						continue;
					}
				}
			}
		}
	}
}

void LoadRecipeContexts()
{
	size_t array_length;
	RValue item_data = *global_instance->GetRefMember("__recipe_context__");
	g_ModuleInterface->GetArraySize(item_data, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue context = g_ModuleInterface->CallBuiltin("array_get", { item_data, i });
		crafting_context_to_id_map[context.ToString()] = i;
	}
}

RValue& GmlScriptCraftingMenuInitializeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	/*
	 - [array] __recipe_context__
	   - [string] __recipe_context__[0] = 'blacksmithing'
	   - [string] __recipe_context__[1] = 'woodcrafting'
	   - [string] __recipe_context__[2] = 'cooking'
	   - [string] __recipe_context__[3] = 'milling'
	   - [string] __recipe_context__[4] = 'refining'
	*/

	if (ArgumentCount > 0 && IsObject(*Arguments[0]) && StructVariableExists(*Arguments[0], "inner"))
	{
		RValue inner = Arguments[0]->GetMember("inner");
		if (IsObject(inner) && StructVariableExists(inner, "context"))
		{
			RValue context = inner.GetMember("context");
			if (IsNumeric(context))
				current_crafting_context = context.ToInt64();
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CRAFTING_MENU_INITIALIZE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptCraftingMenuCloseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CRAFTING_MENU_CLOSE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	current_crafting_context = -1;
	return Result;
}

RValue& GmlScriptPayComponentCostsCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (current_crafting_context == crafting_context_to_id_map["blacksmithing"] && configuration.blacksmithing.disable_materials)
		return Result;
	else if (current_crafting_context == crafting_context_to_id_map["cooking"] && configuration.cooking.disable_materials)
		return Result;
	else if (current_crafting_context == crafting_context_to_id_map["milling"] && configuration.milling.disable_materials)
		return Result;
	else if (current_crafting_context == crafting_context_to_id_map["refining"] && configuration.refining.disable_materials)
		return Result;
	else if (current_crafting_context == crafting_context_to_id_map["woodcrafting"] && configuration.woodcrafting.disable_materials)
		return Result;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PAY_COMPONENT_COSTS));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptMaximumCraftsCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	
	if (current_crafting_context == crafting_context_to_id_map["blacksmithing"] && configuration.blacksmithing.disable_materials)
	{
		Result = 999;
		return Result;
	}
	
	else if (current_crafting_context == crafting_context_to_id_map["cooking"] && configuration.cooking.disable_materials)
	{
		Result = 999;
		return Result;
	}
	else if (current_crafting_context == crafting_context_to_id_map["milling"] && configuration.milling.disable_materials)
	{
		Result = 999;
		return Result;
	}
	else if (current_crafting_context == crafting_context_to_id_map["refining"] && configuration.refining.disable_materials)
	{
		Result = 999;
		return Result;
	}
	else if (current_crafting_context == crafting_context_to_id_map["woodcrafting"] && configuration.woodcrafting.disable_materials)
	{
		Result = 999;
		return Result;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MAXIMUM_CRAFTS));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptCheckItemCraftableCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (current_crafting_context == crafting_context_to_id_map["blacksmithing"] && configuration.blacksmithing.disable_materials)
	{
		Result = true;
		return Result;
	}

	else if (current_crafting_context == crafting_context_to_id_map["cooking"] && configuration.cooking.disable_materials)
	{
		Result = true;
		return Result;
	}
	else if (current_crafting_context == crafting_context_to_id_map["milling"] && configuration.milling.disable_materials)
	{
		Result = true;
		return Result;
	}
	else if (current_crafting_context == crafting_context_to_id_map["refining"] && configuration.refining.disable_materials)
	{
		Result = true;
		return Result;
	}
	else if (current_crafting_context == crafting_context_to_id_map["woodcrafting"] && configuration.woodcrafting.disable_materials)
	{
		Result = true;
		return Result;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CHECK_ITEM_CRAFTABLE));
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
	if (run_once_on_start)
	{
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		CreateOrLoadConfigFile();
		LoadRecipeContexts();
		LoadItemData();
		run_once_on_start = false;
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

void CreateHookGmlScriptCraftingMenuInitialize(AurieStatus& status)
{
	CScript* gml_script_get_display_name = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CRAFTING_MENU_INITIALIZE,
		(PVOID*)&gml_script_get_display_name
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_INITIALIZE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CRAFTING_MENU_INITIALIZE,
		gml_script_get_display_name->m_Functions->m_ScriptFunction,
		GmlScriptCraftingMenuInitializeCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_INITIALIZE);
	}
}

void CreateHookGmlScriptCraftingMenuClose(AurieStatus& status)
{
	CScript* gml_script_get_display_name = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CRAFTING_MENU_CLOSE,
		(PVOID*)&gml_script_get_display_name
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_CLOSE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CRAFTING_MENU_CLOSE,
		gml_script_get_display_name->m_Functions->m_ScriptFunction,
		GmlScriptCraftingMenuCloseCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_CLOSE);
	}
}

void CreateHookGmlScriptPayComponentCosts(AurieStatus& status)
{
	CScript* gml_script_pay_component_costs = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PAY_COMPONENT_COSTS,
		(PVOID*)&gml_script_pay_component_costs
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PAY_COMPONENT_COSTS);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_PAY_COMPONENT_COSTS,
		gml_script_pay_component_costs->m_Functions->m_ScriptFunction,
		GmlScriptPayComponentCostsCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PAY_COMPONENT_COSTS);
	}
}

void CreateHookGmlScriptMaximumCrafts(AurieStatus& status)
{
	CScript* gml_script_maximum_crafts = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MAXIMUM_CRAFTS,
		(PVOID*)&gml_script_maximum_crafts
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MAXIMUM_CRAFTS);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_MAXIMUM_CRAFTS,
		gml_script_maximum_crafts->m_Functions->m_ScriptFunction,
		GmlScriptMaximumCraftsCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MAXIMUM_CRAFTS);
	}
}

void CreateHookGmlScriptCheckItemCraftable(AurieStatus& status)
{
	CScript* gml_script_check_item_craftable = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CHECK_ITEM_CRAFTABLE,
		(PVOID*)&gml_script_check_item_craftable
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHECK_ITEM_CRAFTABLE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CHECK_ITEM_CRAFTABLE,
		gml_script_check_item_craftable->m_Functions->m_ScriptFunction,
		GmlScriptCheckItemCraftableCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHECK_ITEM_CRAFTABLE);
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

	CreateHookGmlScriptCraftingMenuInitialize(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCraftingMenuClose(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptPayComponentCosts(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptMaximumCrafts(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCheckItemCraftable(status);
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