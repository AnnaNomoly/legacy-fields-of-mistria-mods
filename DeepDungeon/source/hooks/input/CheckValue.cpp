#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;

// MMAPI Input::Hooks::BeforeCheckValue callback — fires before the game's check_value script.
// Use ctx.SetAction() to remap the action (e.g. Confusion Trap swaps direction inputs).
void BeforeCheckValue(MMAPI::Input::CheckValueContext& ctx)
{
	// Confusion Trap — swap up/down and left/right while active.
	if (!game_is_active || GameIsPaused() || !active_traps.contains(Traps::CONFUSING))
		return;

	switch (ctx.GetAction())
	{
		case MMAPI::Input::Actions::MoveUp:    ctx.SetAction(MMAPI::Input::Actions::MoveDown);  break;
		case MMAPI::Input::Actions::MoveDown:  ctx.SetAction(MMAPI::Input::Actions::MoveUp);    break;
		case MMAPI::Input::Actions::MoveLeft:  ctx.SetAction(MMAPI::Input::Actions::MoveRight); break;
		case MMAPI::Input::Actions::MoveRight: ctx.SetAction(MMAPI::Input::Actions::MoveLeft);  break;
		default: break;
	}
}
