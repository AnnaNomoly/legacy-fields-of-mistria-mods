#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

// MMAPI Text::Hooks::BeforePlayConversation callback.
void BeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx)
{
	if (!game_is_active)
		return;

	const std::string key{ ctx.GetKey() };

	// Suppress the floor-10 entrance conversation when already in a dungeon room.
	if (AriCurrentGmRoomIsDungeonFloor() && FLOOR_TEN_CONVERSATION_KEY == key)
	{
		ctx.Cancel();
		return;
	}

	if (ari_current_gm_room == "rm_mines_entry")
	{
		if (key == "Conversations/gameplay_triggered/inspect_seridia_statue")
		{
			ctx.SetKey("Conversations/Mods/Deep Dungeon/sigil_parchment_exchange");
		}
		else if (key == "Conversations/flavor_text/mines_blocked_door")
		{
			ctx.SetKey("Conversations/Mods/Deep Dungeon/challenge_mode");
		}
	}
}
