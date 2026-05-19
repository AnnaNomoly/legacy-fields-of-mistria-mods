#include "../../utils/Utils.h"

using namespace State::Floor;
using namespace State::Maps;

RValue& GmlScriptGetEquipmentBonusFromCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Argument[0]: INT64 — infusion ID (e.g. 3 == Hasty)
	// Argument[1]: String — "amount" (unused)
	// Result:      REAL  — bonus value from all equipped gear (e.g. 0.20 for 5 Hasty pieces)

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (!AriCurrentGmRoomIsDungeonFloor() || GameIsPaused())
		return Result;

	const int infusion_id = Arguments[0]->ToInt64();

	// Leech — force leeching infusion to always contribute.
	if (active_offerings.contains(Offerings::LEECH) && infusion_id == infusion_name_to_id_map["leeching"])
		Result = 1;

	// Dungeon's Curse — suppress infusion bonuses from restricted equipment slots.
	if (Config::config.restrict_armor && (infusion_id == infusion_name_to_id_map["fortified"] || infusion_id == infusion_name_to_id_map["hasty"] || infusion_id == infusion_name_to_id_map["tireless"]))
		Result = 0;
	if (Config::config.restrict_tools && infusion_id == infusion_name_to_id_map["lightweight"])
		Result = 0;

	// Class armor set bonuses — override bonus value based on number of class armor pieces with this infusion.
	const auto class_armor_infusions = GetClassArmorInfusions();
	if (class_armor_infusions.contains(infusion_id))
	{
		const int count = class_armor_infusions.at(infusion_id);
		if (infusion_id == infusion_name_to_id_map["fortified"])
			Result = count * 4;
		else if (infusion_id == infusion_name_to_id_map["hasty"])
			Result = count * 0.04;
		else if (infusion_id == infusion_name_to_id_map["tireless"])
			Result = count * 4;
	}

	return Result;
}
