#include "../../utils/Utils.h"

using namespace State::Maps;

RValue& GmlScriptVitalsMenuSetMaxHealthCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH))
		script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
