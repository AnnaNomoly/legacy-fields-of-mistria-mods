#include "../../utils/Utils.h"

using namespace State::Floor;
using namespace State::Maps;

// MMAPI StatusEffect::Hooks::BeforeCancelStatusEffect callback.
void BeforeCancelStatusEffect(MMAPI::StatusEffect::CancelStatusEffectContext& ctx)
{
	bool toolbar_needs_update = false;

	// Protection
	if (active_sigils.contains(Sigils::PROTECTION) && ctx.GetStatusEffect() == MMAPI::StatusEffect::Ids::GuardiansShield)
	{
		active_sigils.erase(Sigils::PROTECTION);
		toolbar_needs_update = true;
	}

	// Redemption
	if (active_sigils.contains(Sigils::REDEMPTION) && ctx.GetStatusEffect() == MMAPI::StatusEffect::Ids::Fairy)
	{
		active_sigils.erase(Sigils::REDEMPTION);
		toolbar_needs_update = true;
	}

	if (toolbar_needs_update)
		MMAPI::ToolbarMenu::ForceUpdate();
}
