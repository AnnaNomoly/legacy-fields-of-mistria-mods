#include "../../utils/Utils.h"

using namespace State::Floor;

RValue& GmlScriptPlayHealVfxCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (time_stopped)
		return Result;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_HEAL_VFX));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
