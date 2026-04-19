#include "../../utils/Utils.h"

using namespace State::Maps;

RValue& GmlScriptRecipeGenerateInfusionsCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_RECIPE_GENERATE_INFUSIONS));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (StructVariableExists(Self, "item_id"))
	{
		int item_id = Self->GetMember("item_id").ToInt64();
		if (item_id_to_sigil_map.contains(item_id) || lift_key_items.contains(item_id) || orb_items.contains(item_id))
		{
			RValue empty_array = g_ModuleInterface->CallBuiltin("array_create", { 0 });
			*Result.GetRefMember("__count") = 0;
			*Result.GetRefMember("__internal_size") = 0;
			*Result.GetRefMember("__buffer") = empty_array;
		}
	}

	return Result;
}
