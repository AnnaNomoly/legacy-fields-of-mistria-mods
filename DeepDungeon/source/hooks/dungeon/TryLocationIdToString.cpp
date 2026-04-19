#include "../../utils/Utils.h"

using namespace State::Player;

RValue& GmlScriptTryLocationIdToStringCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (game_is_active && Result.m_Kind == VALUE_STRING)
		ari_current_location = Result.ToString();

	return Result;
}
