#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::UI;
using namespace State::Maps;

RValue& GmlScriptAttemptInteractCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Default all focus flags off; set the relevant one below if conditions match.
	obj_dragonshrine_focused = false;
	obj_dungeon_elevator_focused = false;
	obj_dungeon_ladder_down_focused = false;

	if (game_is_active && Self->m_Object != nullptr)
	{
		const std::string self_name = Self->m_Object->m_Name;

		if (self_name == "obj_dungeon_ladder_down")
			obj_dungeon_ladder_down_focused = (floor_number != 0);
		else if (self_name == "obj_dungeon_elevator")
			obj_dungeon_elevator_focused = Config::config.disable_dungeon_lift;
		else if (self_name == "obj_dragonshrine" && ari_current_gm_room == "rm_farm")
			obj_dragonshrine_focused = true;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ATTEMPT_INTERACT));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
