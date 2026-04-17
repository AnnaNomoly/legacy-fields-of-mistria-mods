#include "../../utils/Utils.h"

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
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
