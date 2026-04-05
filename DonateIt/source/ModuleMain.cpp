#include <set>
#include <map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const VERSION = "1.1.1";
static const char* const MOD_NAME = "DonateIt";
static const char* const GML_SCRIPT_GET_ITEM_UI_ICON = "gml_Script_get_ui_icon@anon@4244@LiveItem@LiveItem";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static std::set<int> donatable_items = {};
static std::set<std::string> donatable_item_names = {}; // TODO: Remove this after debugging. It shouldn't be needed by the mod.
static std::map<std::string, int> item_name_to_id_map = {}; // TODO: Remove this after debugging. It shouldn't be needed by the mod.
static std::map<int, std::string> item_id_to_name_map = {}; // TODO: Remove this after debugging. It shouldn't be needed by the mod.

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
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

// TODO: Remove this after debugging. It shouldn't be needed by the mod.
void LoadItemData()
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	size_t array_length;
	g_ModuleInterface->GetArraySize(__item_data, array_length);

	// Load all items.
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(__item_data, i, array_element);

		RValue name_key = *array_element->GetRefMember("name_key"); // The item's localization key
		if (name_key.m_Kind != VALUE_NULL && name_key.m_Kind != VALUE_UNDEFINED && name_key.m_Kind != VALUE_UNSET)
		{
			RValue item_id = *array_element->GetRefMember("item_id");
			RValue recipe_key = *array_element->GetRefMember("recipe_key"); // The internal item original_sprite_name
			RValue max_stack = *array_element->GetRefMember("max_stack"); // The max_stack amount
			item_name_to_id_map[recipe_key.ToString()] = item_id.ToInt64();
			item_id_to_name_map[item_id.ToInt64()] = recipe_key.ToString();
		}
	}
}

void LoadDonatableItems()
{
	RValue museum_data = global_instance->GetMember("__museum_data");

	RValue data = museum_data.GetMember("data"); // array
	RValue data_length = g_ModuleInterface->CallBuiltin("array_length", { data });

	for (size_t i = 0; i < data_length.ToInt64(); ++i)
	{
		RValue data_entry = g_ModuleInterface->CallBuiltin("array_get", { data, i });
		RValue sets = data_entry.GetMember("sets");
		RValue inner = sets.GetMember("inner");

		std::vector<std::string> struct_field_names = {};
		auto GetStructFieldNames = [&](IN const char* MemberName, IN OUT RValue* Value) {
			struct_field_names.push_back(MemberName);
			return false;
		};

		g_ModuleInterface->EnumInstanceMembers(inner, GetStructFieldNames);
		for (size_t j = 0; j < struct_field_names.size(); ++j)
		{
			RValue set = inner.GetMember(struct_field_names[j]);

			RValue items = set.GetMember("items");
			RValue items_length = g_ModuleInterface->CallBuiltin("array_length", { items });

			for (size_t k = 0; k < items_length.ToInt64(); ++k)
			{
				RValue item = g_ModuleInterface->CallBuiltin("array_get", { items, k });
				donatable_items.insert(item.ToInt64());
			}
		}
	}
}

bool ItemHasBeenDonated(int item_id)
{
	RValue museum_progress_data = global_instance->GetMember("__museum_progress_data");
	RValue item_donated = g_ModuleInterface->CallBuiltin("array_get", { museum_progress_data, item_id });
	return item_donated.ToBoolean();
}

RValue& GmlScriptGetUiIconCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_ITEM_UI_ICON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Self != nullptr)
	{
		RValue self = Self->ToRValue();
		if (StructVariableExists(self, "item_id"))
		{
			int item_id = self.GetMember("item_id").ToInt64();
			if (donatable_items.contains(item_id) && !ItemHasBeenDonated(item_id))
			{
				RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { Result });
				if (type.ToInt64() == 1) // asset_sprite
				{
					RValue original_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { Result });
					std::string replacement_sprite_name = original_sprite_name.ToString() + "_donatable";

					RValue sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
					if (sprite.m_Kind == VALUE_REF)
						Result = sprite;
				}
			}
		}
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
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		LoadItemData();
		LoadDonatableItems();

		for (int item_id : donatable_items)
		{
			if (item_id_to_name_map.contains(item_id))
				donatable_item_names.insert(item_id_to_name_map[item_id]);
			else
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing item name for item id: %d", MOD_NAME, VERSION, item_id);
		}

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

void CreateHookGmlScriptGetUiIcon(AurieStatus& status)
{
	CScript* gml_script_get_ui_icon = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_ITEM_UI_ICON,
		(PVOID*)&gml_script_get_ui_icon
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_ITEM_UI_ICON);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_ITEM_UI_ICON,
		gml_script_get_ui_icon->m_Functions->m_ScriptFunction,
		GmlScriptGetUiIconCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_ITEM_UI_ICON);
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

	CreateHookGmlScriptGetUiIcon(status);
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