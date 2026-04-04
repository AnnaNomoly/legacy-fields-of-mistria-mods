#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "BetterChargedTools";
static const char* const VERSION = "1.0.0";
static const char* const GML_SCRIPT_USE_ITEM = "gml_Script_use_item";
static const char* const GML_SCRIPT_ARI_FSM_HOE = "gml_Script_hoe@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_ARI_FSM_AXE = "gml_Script_axe@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_ARI_FSM_PICK_AXE = "gml_Script_pick_axe@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_ARI_FSM_SHOVEL = "gml_Script_shovel@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_ARI_FSM_NET = "gml_Script_net@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_ARI_FSM_WATERING_CAN = "gml_Script_watering_can@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_ARI_FSM_SOW = "gml_Script_sow@anon@84872@AriFsm@AriFsm";
static const char* const GML_SCRIPT_MODIFY_STAMINA = "gml_Script_modify_stamina@Ari@Ari";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static bool stamina_consumed = false;
static std::map<std::string, int> script_name_to_stamina_consumed_map = {};
static std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map = {};

void ModifyStamina(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	RValue result;
	RValue stamina_modifier = value;
	RValue* stamina_modifier_ptr = &stamina_modifier;

	gml_script_modify_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &stamina_modifier_ptr }
	);
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
		if (!script_name_to_reference_map.contains("obj_ari"))
			script_name_to_reference_map["obj_ari"] = { global_instance->GetRefMember("__ari")->ToInstance(), self };
	}
}

RValue& GmlScriptUseItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	script_name_to_stamina_consumed_map.clear();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_USE_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptAriFsmHoeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	stamina_consumed = false;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_HOE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (stamina_consumed)
	{
		script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_HOE] += 1;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_HOE] % 3 == 0)
			ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 1);
	}
	stamina_consumed = false;

	return Result;
}

RValue& GmlScriptAriFsmAxeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	stamina_consumed = false;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_AXE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (stamina_consumed)
	{
		script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_AXE] += 1;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_AXE] % 3 == 0)
			ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 1);
	}
	stamina_consumed = false;

	return Result;
}

RValue& GmlScriptAriFsmPickAxeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	stamina_consumed = false;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_PICK_AXE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (stamina_consumed)
	{
		script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_PICK_AXE] += 1;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_PICK_AXE] % 3 == 0)
			ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 1);
	}
	stamina_consumed = false;

	return Result;
}

RValue& GmlScriptAriFsmShovelCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	stamina_consumed = false;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_SHOVEL));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (stamina_consumed)
	{
		script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_SHOVEL] += 1;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_SHOVEL] % 3 == 0)
			ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 1);
	}
	stamina_consumed = false;

	return Result;
}

RValue& GmlScriptAriFsmNetCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	stamina_consumed = false;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_NET));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (stamina_consumed)
	{
		script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_NET] += 1;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_NET] % 3 == 0)
			ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 1);
	}
	stamina_consumed = false;

	return Result;
}

RValue& GmlScriptAriFsmWateringCanCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_WATERING_CAN] += 1;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_WATERING_CAN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptAriFsmSowCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	stamina_consumed = false;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FSM_SOW));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (stamina_consumed)
	{
		script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_SOW] += 1;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_SOW] % 3 == 0)
			ModifyStamina(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 1);
	}
	stamina_consumed = false;

	return Result;
}

RValue& GmlScriptModifyStaminaCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (Arguments[0]->ToInt64() < 0)
	{
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_WATERING_CAN] % 3 == 0)
			*Arguments[0] = 0;
		if (script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_WATERING_CAN] > 0)
			script_name_to_stamina_consumed_map[GML_SCRIPT_ARI_FSM_WATERING_CAN] -= 1;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_STAMINA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Arguments[0]->ToInt64() < 0)
		stamina_consumed = true;

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
	}

	// Reset static fields
	stamina_consumed = false;
	script_name_to_stamina_consumed_map = {};
	script_name_to_reference_map = {};

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

void CreateHookGmlScriptUseItem(AurieStatus& status)
{
	CScript* gml_script_use_item = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_USE_ITEM,
		(PVOID*)&gml_script_use_item
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_USE_ITEM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_USE_ITEM,
		gml_script_use_item->m_Functions->m_ScriptFunction,
		GmlScriptUseItemCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_USE_ITEM);
	}
}

void CreateHookGmlScriptAriFsmHoe(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_hoe = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_HOE,
		(PVOID*)&gml_script_ari_fsm_hoe
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_HOE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_HOE,
		gml_script_ari_fsm_hoe->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmHoeCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_HOE);
	}
}

void CreateHookGmlScriptAriFsmAxe(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_axe = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_AXE,
		(PVOID*)&gml_script_ari_fsm_axe
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_AXE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_AXE,
		gml_script_ari_fsm_axe->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmAxeCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_AXE);
	}
}

void CreateHookGmlScriptAriFsmPickAxe(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_pick_axe = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_PICK_AXE,
		(PVOID*)&gml_script_ari_fsm_pick_axe
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_PICK_AXE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_PICK_AXE,
		gml_script_ari_fsm_pick_axe->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmPickAxeCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_PICK_AXE);
	}
}

void CreateHookGmlScriptAriFsmShovel(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_shovel = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_SHOVEL,
		(PVOID*)&gml_script_ari_fsm_shovel
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_SHOVEL);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_SHOVEL,
		gml_script_ari_fsm_shovel->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmShovelCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_SHOVEL);
	}
}

void CreateHookGmlScriptAriFsmNet(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_net = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_NET,
		(PVOID*)&gml_script_ari_fsm_net
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_NET);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_NET,
		gml_script_ari_fsm_net->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmNetCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_NET);
	}
}

void CreateHookGmlScriptAriFsmWateringCan(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_watering_can = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_WATERING_CAN,
		(PVOID*)&gml_script_ari_fsm_watering_can
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_WATERING_CAN);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_WATERING_CAN,
		gml_script_ari_fsm_watering_can->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmWateringCanCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_WATERING_CAN);
	}
}

void CreateHookGmlScriptAriFsmSow(AurieStatus& status)
{
	CScript* gml_script_ari_fsm_sow = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FSM_SOW,
		(PVOID*)&gml_script_ari_fsm_sow
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_SOW);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FSM_SOW,
		gml_script_ari_fsm_sow->m_Functions->m_ScriptFunction,
		GmlScriptAriFsmSowCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FSM_SOW);
	}
}

void CreateHookGmlScriptModifyStamina(AurieStatus& status)
{
	CScript* gml_script_modify_stamina = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_STAMINA);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_MODIFY_STAMINA,
		gml_script_modify_stamina->m_Functions->m_ScriptFunction,
		GmlScriptModifyStaminaCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_STAMINA);
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

	CreateHookGmlScriptUseItem(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmHoe(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmAxe(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmPickAxe(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmShovel(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmNet(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmWateringCan(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFsmSow(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptModifyStamina(status);
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