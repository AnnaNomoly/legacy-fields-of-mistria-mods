#include "../../utils/Utils.h"

using namespace State::Floor;

// MMAPI Display::Hooks::BeforePlayHealVfx callback.
void BeforePlayHealVfx(MMAPI::Display::PlayHealVfxContext& ctx)
{
	if (time_stopped)
		ctx.Cancel();
}
