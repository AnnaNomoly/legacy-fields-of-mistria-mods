#include "../../utils/Utils.h"

using namespace State::Floor;

// MMAPI Player::Hooks::BeforeStaminaChange callback.
void BeforeStaminaChange(MMAPI::Player::BeforeStaminaChangeContext& ctx)
{
	const double amount = ctx.GetAmount();
	if (amount >= 0)
		return;

	// Exhaustion — doubles all stamina costs.
	if (active_floor_enchantments.contains(FloorEnchantments::EXHAUSTION))
		ctx.SetAmount(amount * 2);

	// Spirit Surge — eliminates all stamina costs. (Re-read amount in case Exhaustion ran first.)
	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE))
		ctx.SetAmount(0);
}
