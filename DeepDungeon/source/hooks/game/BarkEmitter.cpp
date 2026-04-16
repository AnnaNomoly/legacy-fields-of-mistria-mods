#include "../../utils/Utils.h"

RValue& GmlScriptBarkEmitterCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (/*!custom_bark_playing && */(StructVariableExists(Other, "god_mode") || StructVariableExists(Other, "wimp_mode")))
		script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_BARK_EMITTER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
