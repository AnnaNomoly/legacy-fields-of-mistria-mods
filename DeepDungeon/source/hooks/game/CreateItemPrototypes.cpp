#include "../../utils/Utils.h"

using namespace State::Maps;

RValue& GmlScriptCreateItemPrototypesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CREATE_ITEM_PROTOTYPES));
	original(Self, Other, Result, ArgumentCount, Arguments);

	size_t array_length;
	g_ModuleInterface->GetArraySize(Result, array_length);

	// Load all items.
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(Result, i, array_element);

		item_id_to_prototype_map[i] = *array_element;
	}

	return Result;
}
