#include "../../utils/Utils.h"

RValue& GmlScriptAttemptInteractCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && Self->m_Object != nullptr)
	{
		std::string self_name = Self->m_Object->m_Name;

		if (self_name == "obj_dungeon_ladder_down")
		{
			if (floor_number != 0)
				obj_dungeon_ladder_down_focused = true;
			else
				obj_dungeon_ladder_down_focused = false;

			obj_dungeon_elevator_focused = false;
		}
		else if (self_name == "obj_dungeon_elevator")
		{
			if (Config::config.disable_dungeon_lift)
				obj_dungeon_elevator_focused = true;
			else
				obj_dungeon_elevator_focused = false;

			obj_dungeon_ladder_down_focused = false;
		}
		else if (self_name == "obj_dragonshrine" && ari_current_gm_room == "rm_farm")
		{
			obj_dragonshrine_focused = true;
		}
		else
		{
			obj_dragonshrine_focused = false;
			obj_dungeon_elevator_focused = false;
			obj_dungeon_ladder_down_focused = false;
		}
	}
	else
	{
		obj_dragonshrine_focused = false;
		obj_dungeon_elevator_focused = false;
		obj_dungeon_ladder_down_focused = false;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ATTEMPT_INTERACT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
