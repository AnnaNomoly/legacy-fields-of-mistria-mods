#include "../../utils/Utils.h"

using namespace State::Maps;

// MMAPI Recipe::Hooks::AfterGenerateInfusions callback.
// Clears infusions for sigils, lift keys, orbs, and dread contracts — items that shouldn't roll infusions.
void AfterGenerateInfusions(MMAPI::Recipe::GenerateInfusionsContext& ctx)
{
	int item_id = ctx.GetItemId();
	if (item_id < 0)
		return;

	if (item_id_to_sigil_map.contains(item_id)
		|| lift_key_items.contains(item_id)
		|| orb_items.contains(item_id)
		|| dread_contract_items.contains(item_id))
	{
		ctx.Clear();
	}
}
