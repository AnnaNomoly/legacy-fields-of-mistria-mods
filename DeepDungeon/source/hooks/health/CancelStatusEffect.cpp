#include "../../utils/Utils.h"

using namespace State::Floor;
using namespace State::Maps;

RValue& GmlScriptCancelStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const int64_t status_id = Arguments[0]->ToInt64();
	bool toolbar_needs_update = false;

	// Protection
	if (active_sigils.contains(Sigils::PROTECTION) && status_id == status_effect_name_to_id_map["guardians_shield"])
	{
		active_sigils.erase(Sigils::PROTECTION);
		toolbar_needs_update = true;
	}

	// Redemption
	if (active_sigils.contains(Sigils::REDEMPTION) && status_id == status_effect_name_to_id_map["fairy"])
	{
		active_sigils.erase(Sigils::REDEMPTION);
		toolbar_needs_update = true;
	}

	if (toolbar_needs_update && script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
		UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CANCEL_STATUS_EFFECT));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
