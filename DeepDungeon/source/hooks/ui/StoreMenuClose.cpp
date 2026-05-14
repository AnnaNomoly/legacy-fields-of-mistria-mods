#include "../../utils/Utils.h"

using namespace State::UI;

// MMAPI Game::Hooks::AfterStoreMenuClose callback.
void AfterStoreMenuClose()
{
	store_menu_open = false;
}
