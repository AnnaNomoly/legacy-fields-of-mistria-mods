#include "../../utils/Utils.h"

using namespace State::UI;

// MMAPI Crafting::Hooks::AfterMenuClose callback.
void AfterCraftingMenuClose()
{
	crafting_menu_open = false;
}
