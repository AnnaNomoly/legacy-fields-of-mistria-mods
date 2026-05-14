#include "../../utils/Utils.h"

using namespace State::UI;

// MMAPI Game::Hooks::AfterJournalMenuOpen callback.
void AfterJournalMenuOpen()
{
	journal_menu_open = true;
}
