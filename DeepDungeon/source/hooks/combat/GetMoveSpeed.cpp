#include "../../utils/Utils.h"

using namespace State::Floor;

// MMAPI Player::Hooks::AfterMoveSpeed callback.
// Default run speed is 2.0. Each override below supersedes the previous.
void AfterMoveSpeed(MMAPI::Player::MoveSpeedContext& ctx)
{
	if (active_floor_enchantments.contains(FloorEnchantments::GRAVITY))
		ctx.SetOverride(1.25);

	if (active_floor_enchantments.contains(FloorEnchantments::HASTE))
		ctx.SetOverride(3.0);

	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE))
		ctx.SetOverride(4.0);
}
