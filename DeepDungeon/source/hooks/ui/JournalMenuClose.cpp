#include "../../utils/Utils.h"

RValue& GmlScriptJournalMenuCloseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_JOURNAL_MENU_CLOSE));
	original(Self, Other, Result, ArgumentCount, Arguments);

	journal_menu_open = false;
	return Result;
}
