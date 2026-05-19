#include "../../utils/Utils.h"

using namespace State::UI;

RValue& GmlScriptDisplayResizeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DISPLAY_RESIZE));
	original(Self, Other, Result, ArgumentCount, Arguments);

	window_width = GetWindowWidth();
	window_height = GetWindowHeight();

	return Result;
}
