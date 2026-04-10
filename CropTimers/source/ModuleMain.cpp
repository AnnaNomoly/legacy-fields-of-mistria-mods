#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "CropTimers";
static const char* const VERSION = "1.0.0";
static const char* const GML_SCRIPT_NODE_OBJECT_SET_SPRITE = "gml_Script_set_sprite@gml_Object_obj_node_renderer_Create_0";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static std::map<std::string, int> object_category_to_id_map = {};

int RValueAsInt(RValue value)
{
	if (value.m_Kind == VALUE_REAL)
		return static_cast<int>(value.m_Real);
	if (value.m_Kind == VALUE_INT64)
		return static_cast<int>(value.m_i64);
	if (value.m_Kind == VALUE_INT32)
		return static_cast<int>(value.m_i32);
}

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_REAL && value.m_Real == 1)
		return true;
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, std::string variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, RValue(variable_name) }
	);

	return RValueAsBool(struct_exists);
}

RValue StructVariableGet(RValue the_struct, std::string variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, RValue(variable_name) }
	);
}

void LoadObjectCategories()
{
	RValue object_categories = global_instance->GetMember("__object_category__");
	RValue object_categories_length = g_ModuleInterface->CallBuiltin("array_length", { object_categories });

	for (size_t i = 0; i < object_categories_length.ToInt64(); ++i)
	{
		RValue object_category = g_ModuleInterface->CallBuiltin("array_get", { object_categories, i });
		object_category_to_id_map[object_category.ToString()] = i;
	}
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

			if (sprite_name_str.contains("stage") && (sprite_name_str.contains("crop") || sprite_name_str.contains("forage")))
			{
				RValue node = Self->GetMember("node");
				if (StructVariableExists(node, "prototype"))
				{
					RValue prototype = node.GetMember("prototype");
					if (StructVariableExists(prototype, "category_id") && IsNumeric(prototype.GetMember("category_id")))
					{
						int category_id = prototype.GetMember("category_id").ToInt64();
						if (category_id == 3) // don't hardcode this
						{
							int day_count = node.GetMember("day_count").ToInt64();
							bool stage_zero_is_seed = prototype.GetMember("stage_zero_is_seed").ToBoolean();
							bool regrow_cycle = node.GetMember("regrow_cycle").ToBoolean();

							RValue day_to_stage = prototype.GetMember("day_to_stage");
							RValue buffer = day_to_stage.GetMember("__buffer");

							int growth_days = g_ModuleInterface->CallBuiltin("array_length", { buffer }).ToInt64() - 1;
							growth_days = stage_zero_is_seed ? growth_days : growth_days - 1; // Adjust if stage 0 isn't a seed

							if (!regrow_cycle)
							{
								int days_remaining = growth_days - day_count;

								std::string replacement_sprite_name = sprite_name_str + "_timer_" + std::to_string(days_remaining);
								RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
								if (replacement_sprite.m_Kind == VALUE_REF)
									*Arguments[0] = replacement_sprite;
							}
							else
							{
								RValue post_harvest_day_to_stage = prototype.GetMember("post_harvest_day_to_stage");
								RValue post_harvest_day_to_stage_buffer = post_harvest_day_to_stage.GetMember("__buffer");

								int regrowth_days = g_ModuleInterface->CallBuiltin("array_length", { post_harvest_day_to_stage_buffer }).ToInt64() - 1;
								int days_remaining = regrowth_days - day_count;

								std::string replacement_sprite_name = sprite_name_str + "_timer_" + std::to_string(days_remaining);
								RValue replacement_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
								if (replacement_sprite.m_Kind == VALUE_REF)
									*Arguments[0] = replacement_sprite;
							}
						}
					}
				}
			}
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
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		LoadObjectCategories();

		load_on_start = false;
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