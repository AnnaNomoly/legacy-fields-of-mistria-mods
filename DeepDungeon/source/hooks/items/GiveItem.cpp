#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

RValue& GmlScriptGiveItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (is_challenge_mode && ArgumentCount >= 2 && IsObject(*Arguments[0]) && StructVariableExists(*Arguments[0], "item_id"))
	{
		int item_id = Arguments[0]->GetMember("item_id").ToInt64();
		if (!challenge_mode_bulk_given_item_ids.contains(item_id) && challenge_mode_item_drop_quantities.contains(item_id))
		{
			if (item_id == item_name_to_id_map[SUSTAINING_POTION_NAME] && StructVariableExists(*Arguments[0], "infusion"))
				StructVariableSet(*Arguments[0], "infusion", infusion_name_to_id_map["restorative"]);

			*Arguments[1] = challenge_mode_item_drop_quantities[item_id];
			challenge_mode_bulk_given_item_ids.insert(item_id);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GIVE_ITEM));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
