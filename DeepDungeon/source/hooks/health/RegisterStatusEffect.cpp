#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

RValue& GmlScriptRegisterStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Sustaining Potion — extends the duration of the Restoration status effect.
	if (AriCurrentGmRoomIsDungeonFloor()
		&& held_item_id == item_name_to_id_map[SUSTAINING_POTION_NAME]
		&& Arguments[0]->ToInt64() == status_effect_name_to_id_map["restorative"])
	{
		int finish = Arguments[3]->ToInt64();
		*Arguments[3] = finish - static_cast<int>(7200 * (1 - Config::config.sustaining_potion_duration_modifier));
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_REGISTER_STATUS_EFFECT));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
