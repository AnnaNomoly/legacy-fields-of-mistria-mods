#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

// MMAPI StatusEffect::Hooks::BeforeRegisterStatusEffect callback.
// Extends the duration of Restoration when a Sustaining Potion is the held item.
void BeforeRegisterStatusEffect(MMAPI::StatusEffect::RegisterStatusEffectContext& ctx)
{
	if (AriCurrentGmRoomIsDungeonFloor()
		&& held_item_id == item_name_to_id_map[SUSTAINING_POTION_NAME]
		&& ctx.GetStatusEffect() == MMAPI::StatusEffect::Ids::Restorative)
	{
		ctx.SetFinish(ctx.GetFinish() - static_cast<int>(7200 * (1 - Config::config.sustaining_potion_duration_modifier)));
	}
}
