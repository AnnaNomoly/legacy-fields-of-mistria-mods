#include "../../utils/Utils.h"

RValue& GmlScriptModifyStaminaCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Exhaustion
	if (active_floor_enchantments.contains(FloorEnchantments::EXHAUSTION))
	{
		if (Arguments[0]->ToDouble() < 0)
		{
			double modified_stamina_cost = Arguments[0]->ToDouble() * 2;
			*Arguments[0] = modified_stamina_cost;
		}
	}

	// Spirit Surge
	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE) && Arguments[0]->ToDouble() < 0)
		*Arguments[0] = 0;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_STAMINA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
