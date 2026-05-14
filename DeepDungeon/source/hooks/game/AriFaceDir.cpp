#include "../../utils/Utils.h"

using namespace State::Player;

// MMAPI Player::Hooks::BeforeFaceDir callback.
void BeforeFaceDir(MMAPI::Player::FaceDirContext& ctx)
{
	ari_facing_dir = ctx.GetDirectionDegrees();
}
