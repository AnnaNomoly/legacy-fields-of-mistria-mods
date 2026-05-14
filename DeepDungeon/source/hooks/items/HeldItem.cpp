#include "../../utils/Utils.h"

using namespace State::Player;

// MMAPI Player::Hooks::AfterHeldItem callback.
void AfterHeldItem(MMAPI::Player::HeldItemContext& ctx)
{
	int item_id = ctx.GetItemId();
	if (item_id >= 0 && held_item_id != item_id)
		held_item_id = item_id;
}
