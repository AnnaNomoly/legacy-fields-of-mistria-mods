#include "../../utils/Utils.h"

RValue& GmlScriptGetEquipmentBonusFromCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Argument[0]: INT64 == 3 // ID of the infusion, see __infusion__ global, 3 == Hasty
	// Argument[1]: String == "amount" // Unused

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (AriCurrentGmRoomIsDungeonFloor() && !GameIsPaused())
	{
		int infusion_id = Arguments[0]->ToInt64();

		// Leech
		if (active_offerings.contains(Offerings::LEECH) && infusion_id == infusion_name_to_id_map["leeching"])
			Result = 1;

		// Dungeon's Curse: Prevent infusions on armor and tools from applying.
		if (Config::config.restrict_armor && (infusion_id == infusion_name_to_id_map["fortified"] || infusion_id == infusion_name_to_id_map["hasty"] || infusion_id == infusion_name_to_id_map["tireless"]))
			Result = 0;
		if (Config::config.restrict_tools && infusion_id == infusion_name_to_id_map["lightweight"])
			Result = 0;

		// Class armor bonuses
		std::map<int, int> class_armor_infusions = GetClassArmorInfusions();
		if (class_armor_infusions.contains(infusion_id))
		{
			if (infusion_id == infusion_name_to_id_map["fortified"])
				Result = class_armor_infusions[infusion_id] * 4;
			if (infusion_id == infusion_name_to_id_map["hasty"])
				Result = class_armor_infusions[infusion_id] * 0.04;
			if (infusion_id == infusion_name_to_id_map["tireless"])
				Result = class_armor_infusions[infusion_id] * 4;
		}
	}

	// Result: REAL == 0.20 // The value of the infusion bonus from all gear (0.20 for 5 Hasty armor pieces)
	return Result;
}
