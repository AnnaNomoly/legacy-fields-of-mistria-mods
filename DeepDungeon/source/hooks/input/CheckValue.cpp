#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;

RValue& GmlScriptCheckValueCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const int input_value = Arguments[0]->ToInt64();

	// Confusion Trap
	if (game_is_active && !GameIsPaused() && active_traps.contains(Traps::CONFUSING) && input_value >= 1 && input_value <= 4)
	{
		int new_value = 0;
		if (input_value == 1)	    new_value = 2;
		else if (input_value == 2)	new_value = 1;
		else if (input_value == 3)	new_value = 4;
		else if (input_value == 4)	new_value = 3;
		*Arguments[0] = new_value;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "gml_Script_check_value@Input@Input"));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
