#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::UI;
using namespace State::Maps;

// MMAPI Instance::Hooks::BeforeAttemptInteract callback.
void BeforeAttemptInteract(MMAPI::Instance::AttemptInteractContext& ctx)
{
	// Default all focus flags off; set the relevant one below if conditions match.
	obj_dragonshrine_focused = false;
	obj_dungeon_elevator_focused = false;
	obj_dungeon_ladder_down_focused = false;

	if (!game_is_active)
		return;

	const std::string_view self_name = ctx.GetObjectName();
	if (self_name.empty())
		return;

	if (self_name == "obj_dungeon_ladder_down")
		obj_dungeon_ladder_down_focused = (floor_number != 0);
	else if (self_name == "obj_dungeon_elevator")
		obj_dungeon_elevator_focused = Config::config.disable_dungeon_lift;
	else if (self_name == "obj_dragonshrine" && ari_current_gm_room == "rm_farm")
		obj_dragonshrine_focused = true;
}
