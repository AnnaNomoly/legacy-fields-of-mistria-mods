#include "../../utils/Utils.h"

using namespace State::UI;

// MMAPI Display::Hooks::AfterDisplayResize callback.
void AfterDisplayResize(MMAPI::Display::DisplayResizeContext& ctx)
{
	window_width = ctx.GetWindowWidth();
	window_height = ctx.GetWindowHeight();
}
