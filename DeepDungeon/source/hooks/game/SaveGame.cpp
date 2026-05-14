#include "../../utils/Utils.h"

using namespace State::Floor;

// MMAPI Game::Hooks::BeforeSaveGame callback.
void BeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	if (floor_number != 0)
	{
		MMAPI::Game::CreateNotification(true, SAVING_DISABLED_NOTIFICATION_KEY);
		ctx.Cancel();
	}
}
