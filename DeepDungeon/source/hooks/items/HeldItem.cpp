#include "../../utils/Utils.h"

RValue& GmlScriptHeldItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_HELD_ITEM));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (Result.m_Kind != VALUE_UNDEFINED)
	{
		int item_id = Result.GetMember("item_id").ToInt64();
		if (held_item_id != item_id)
			held_item_id = item_id;
	}

	return Result;
}
