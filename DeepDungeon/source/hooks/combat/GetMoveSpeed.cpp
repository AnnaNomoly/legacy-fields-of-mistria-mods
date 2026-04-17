#include "../../utils/Utils.h"

RValue& GmlScriptGetMoveSpeedCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MOVE_SPEED));
	original(Self, Other, Result, ArgumentCount, Arguments);

	// Default run speed is 2.0. Each override below supersedes the previous.
	if (active_floor_enchantments.contains(FloorEnchantments::GRAVITY))
		Result = 1.25;

	if (active_floor_enchantments.contains(FloorEnchantments::HASTE))
		Result = 3.0;

	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE))
		Result = 4.0;

	return Result;
}
