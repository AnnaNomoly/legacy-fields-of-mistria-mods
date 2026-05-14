#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

// MMAPI Item::Hooks::BeforeGiveItem callback.
void BeforeGiveItem(MMAPI::Item::GiveItemContext& ctx)
{
	if (!is_challenge_mode)
		return;

	int item_id = ctx.GetItemId();
	if (item_id < 0)
		return;

	if (!challenge_mode_bulk_given_item_ids.contains(item_id) || !challenge_mode_item_drop_quantities.contains(item_id))
		return;

	if (item_id == item_name_to_id_map[SUSTAINING_POTION_NAME])
		ctx.SetInfusion(MMAPI::Infusion::Ids::Restorative);

	ctx.SetQuantity(challenge_mode_item_drop_quantities[item_id]);
	challenge_mode_bulk_given_item_ids.insert(item_id);
}
