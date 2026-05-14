#include "../../utils/Utils.h"

using namespace State::Player;

// MMAPI Game::Hooks::AfterLoadGame callback.
void AfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	save_prefix = ctx.GetSavePrefix();
	ReadChallengeModeFile();
}
