#include "../../utils/Utils.h"

using namespace State::UI;

// MMAPI Game::Hooks::AfterStoreMenuOpen callback.
void AfterStoreMenuOpen()
{
	store_menu_open = true;
}
