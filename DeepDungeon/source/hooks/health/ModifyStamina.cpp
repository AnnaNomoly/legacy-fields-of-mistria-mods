#include "../../utils/Utils.h"

RValue& GmlScriptModifyStaminaCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Exhaustion — doubles all stamina costs.
	if (active_floor_enchantments.contains(FloorEnchantments::EXHAUSTION) && Arguments[0]->ToDouble() < 0)
		*Arguments[0] = Arguments[0]->ToDouble() * 2;

	// Spirit Surge — eliminates all stamina costs.
	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE) && Arguments[0]->ToDouble() < 0)
		*Arguments[0] = 0;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_STAMINA));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
