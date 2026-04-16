#include "../../utils/Utils.h"

RValue& GmlScriptCheckValueCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && !GameIsPaused() && active_traps.contains(Traps::CONFUSING) && Arguments[0]->ToInt64() <= 4 && Arguments[0]->ToInt64() >= 1)
	{
		// Confusion Trap
		int new_value = 0;
		if (Arguments[0]->ToInt64() == 1)
			new_value = 2;
		if (Arguments[0]->ToInt64() == 2)
			new_value = 1;
		if (Arguments[0]->ToInt64() == 3)
			new_value = 4;
		if (Arguments[0]->ToInt64() == 4)
			new_value = 3;
		*Arguments[0] = new_value;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "gml_Script_check_value@Input@Input"));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
