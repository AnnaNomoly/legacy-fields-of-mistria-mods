#include "../../utils/Utils.h"

using namespace State::Maps;

RValue& GmlScriptDeserializeInventoryCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DESERIALIZE_INVENTORY))
		script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DESERIALIZE_INVENTORY));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
