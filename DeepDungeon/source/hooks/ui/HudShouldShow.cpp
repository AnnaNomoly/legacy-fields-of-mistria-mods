#include "../../utils/Utils.h"

using namespace State::Floor;

// MMAPI Display::Hooks::AfterHudShouldShow callback.
void AfterHudShouldShow(MMAPI::Display::HudShouldShowContext& ctx)
{
	if (active_traps.contains(Traps::DISORIENTING))
		ctx.SetResult(false);
}
