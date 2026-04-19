#include "../../utils/Utils.h"

using namespace State::Floor;

RValue& GmlScriptOnBeginStepCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Revealed Traps
	RValue spr_revealed_floor_trap = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_revealed_floor_trap" });
	for (const RevealedFloorTrap& trap : revealed_floor_traps)
	{
		if (trap.is_active)
		{
			g_ModuleInterface->CallBuiltin("variable_instance_set", { trap.instance, "sprite_index", spr_revealed_floor_trap });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { trap.instance, "image_speed", 0.10 });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { trap.instance, "depth", 350 });
		}
	}

	// Treasure Spot
	if (treasure_spot.is_active)
	{
		RValue spr_treasure_spot = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_treasure_spot" });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "sprite_index", spr_treasure_spot });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "image_speed", 0.3 });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "depth", 350 });
	}

	// Meteor AOEs
	RValue spr_trap_meteor = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_meteor" });
	for (const CustomAOE& aoe : meteor_aoes)
	{
		if (aoe.is_active)
		{
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "sprite_index", spr_trap_meteor });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "image_speed", 0.6 });
		}
	}

	// Gaze AOEs — switches to a vanish sprite when the AOE is near expiry.
	RValue spr_trap_gaze       = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_gaze" });
	RValue spr_trap_gaze_vanish = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_gaze_vanish" });
	for (CustomAOE& aoe : gaze_aoes)
	{
		if (!aoe.is_active)
			continue;

		if (current_time_in_seconds < aoe.spawned_time + aoe.duration - 120)
		{
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "sprite_index", spr_trap_gaze });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "image_speed", 0.6 });
		}
		else
		{
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "sprite_index", spr_trap_gaze_vanish });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "image_speed", 0.6 });

			if (!StructVariableExists(aoe.instance, "__deep_dungeon__reset_image_index"))
			{
				StructVariableSet(aoe.instance, "__deep_dungeon__reset_image_index", true);
				g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "image_index", 0 });
			}
		}
	}

	// Void AOEs
	RValue spr_trap_void = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_void" });
	for (const CustomAOE& aoe : void_aoes)
	{
		if (aoe.is_active)
		{
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "sprite_index", spr_trap_void });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "image_speed", 0.25 });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { aoe.instance, "depth", -1000 });
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_BEGIN_STEP));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
