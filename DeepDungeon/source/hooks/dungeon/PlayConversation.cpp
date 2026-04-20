#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

RValue& GmlScriptPlayConversationCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && AriCurrentGmRoomIsDungeonFloor() && FLOOR_TEN_CONVERSATION_KEY == Arguments[1]->ToString())
		return Result;

	if (game_is_active && ari_current_gm_room == "rm_mines_entry")
	{
		if (Arguments[1]->ToString() == "Conversations/gameplay_triggered/inspect_seridia_statue")
		{
			if (!script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
				*Arguments[1] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/unavailable";
			else
				*Arguments[1] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange";
		}
		else if (Arguments[1]->ToString() == "Conversations/flavor_text/mines_blocked_door")
		{
			*Arguments[1] = "Conversations/Mods/Deep Dungeon/challenge_mode/start";
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_CONVERSATION));
	original(Self, Other, Result, ArgumentCount, Arguments);
	return Result;
}
