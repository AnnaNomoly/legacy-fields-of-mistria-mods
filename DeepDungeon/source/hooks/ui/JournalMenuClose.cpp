#include "../../utils/Utils.h"

using namespace State::UI;

// MMAPI Game::Hooks::AfterJournalMenuClose callback.
void AfterJournalMenuClose()
{
	journal_menu_open = false;
}
