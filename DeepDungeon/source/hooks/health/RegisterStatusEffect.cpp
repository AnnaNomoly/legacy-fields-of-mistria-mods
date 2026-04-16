#include "../../utils/Utils.h"

RValue& GmlScriptRegisterStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (AriCurrentGmRoomIsDungeonFloor() && held_item_id == item_name_to_id_map[SUSTAINING_POTION_NAME])
	{
		if (Arguments[0]->ToInt64() == status_effect_name_to_id_map["restorative"])
		{
			int finish = Arguments[3]->ToInt64();
			*Arguments[3] = finish - static_cast<int>(7200 * (1 - Config::config.sustaining_potion_duration_modifier)); // Modify the duration of Restoration
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_REGISTER_STATUS_EFFECT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
