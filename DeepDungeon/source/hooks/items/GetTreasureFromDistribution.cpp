#include "../../utils/Utils.h"

RValue& GmlScriptGetTreasureFromDistributionCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	CInstance* source = (Self != nullptr && StructVariableExists(Self, "object_id")) ? Self
	                  : (Other != nullptr && StructVariableExists(Other, "object_id")) ? Other
	                  : nullptr;
	if (source != nullptr)
	{
		int object_id = source->GetMember("object_id").ToInt64();
		if (object_id_to_name_map.contains(object_id))
		{
			std::string object_name = object_id_to_name_map[object_id];
			if (DUNGEON_TREASURE_CHEST_NAMES.contains(object_name) && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
				GenerateTreasureChestLoot(object_name, Self, Other);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
