#include "../../utils/Utils.h"

RValue& GmlScriptOnBeginStepCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Revealed Traps
	for (int i = 0; i < revealed_floor_traps.size(); i++)
	{
		if (revealed_floor_traps[i].is_active)
		{
			RValue spr_revealed_floor_trap = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_revealed_floor_trap" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { revealed_floor_traps[i].instance, "sprite_index", spr_revealed_floor_trap });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { revealed_floor_traps[i].instance, "image_speed", 0.10 }); // 0.15
			g_ModuleInterface->CallBuiltin("variable_instance_set", { revealed_floor_traps[i].instance, "depth", 350 });
		}
	}

	// Treasure Spot
	if (treasure_spot.is_active)
	{
		RValue spr_treasure_spot = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_treasure_spot" });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "sprite_index", spr_treasure_spot });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "image_speed", 0.3 }); // 0.6
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "depth", 350 });
	}


	// Meteor Sprites
	for (int i = 0; i < meteor_aoes.size(); i++)
	{
		if (meteor_aoes[i].is_active)
		{
			RValue spr_trap_meteor = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_meteor" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { meteor_aoes[i].instance, "sprite_index", spr_trap_meteor });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { meteor_aoes[i].instance, "image_speed", 0.6 });
		}
	}

	// Gaze Traps
	for (int i = 0; i < gaze_aoes.size(); i++)
	{
		if (gaze_aoes[i].is_active)
		{
			// if (gaze.is_active && current_time_in_seconds >= gaze.spawned_time + gaze.duration)
			if (current_time_in_seconds < gaze_aoes[i].spawned_time + gaze_aoes[i].duration - 120)
			{
				RValue spr_trap_gaze = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_gaze" });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "sprite_index", spr_trap_gaze });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "image_speed", 0.6 });
			}
			else
			{
				RValue spr_trap_gaze_vanish = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_gaze_vanish" });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "sprite_index", spr_trap_gaze_vanish });
				g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "image_speed", 0.6 });

				if (!StructVariableExists(gaze_aoes[i].instance, "__deep_dungeon__reset_image_index"))
				{
					StructVariableSet(gaze_aoes[i].instance, "__deep_dungeon__reset_image_index", true);
					g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "image_index", 0 });   // Reset frame
				}
			}
		}
	}

	// Void Traps
	for (int i = 0; i < void_aoes.size(); i++)
	{
		if (void_aoes[i].is_active)
		{
			RValue spr_trap_void = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_void" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { void_aoes[i].instance, "sprite_index", spr_trap_void });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { void_aoes[i].instance, "image_speed", 0.25 }); // 0.1
			g_ModuleInterface->CallBuiltin("variable_instance_set", { void_aoes[i].instance, "depth", -1000 });
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_BEGIN_STEP));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
