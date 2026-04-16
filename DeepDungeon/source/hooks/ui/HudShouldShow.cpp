#include "../../utils/Utils.h"

RValue& GmlScriptHudShouldShowCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_HUD_SHOULD_SHOW));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (active_traps.contains(Traps::DISORIENTING))
		Result = false;

	return Result;
}
