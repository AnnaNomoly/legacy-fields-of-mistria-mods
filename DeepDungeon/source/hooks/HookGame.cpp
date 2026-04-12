#include "../utils/Utils.h"

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (load_on_start)
	{
		load_on_start = false;
		localize_mod_text = true;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		window_width = GetWindowWidth();
		window_height = GetWindowHeight();

		CreateOrLoadConfigFile();
		CreateOrGetGlobalYYTKVariable();
		CreateModInfoInGlobalYYTKVariable();

		LoadPerks();
		LoadSpells();
		LoadSpellIds();
		LoadStatusEffects();
		LoadLocations();
		LoadInfusions();
		LoadObjectIds();
		LoadItems();
		LoadMonsters();
		ModifyMonsterPrototypes();
		LoadDungeonBiomeCandidateMonsters();
		LoadPlayerStates();
		LoadMonsterStates();
		LoadBarkData();
		LoadTutorials();
		LoadStalagmiteAttackData();
		ModifyItems();
		// TODO: Load other stuff
	}
	else
		ResetStaticFields(true);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

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
		if (configuration.restrict_armor && (infusion_id == infusion_name_to_id_map["fortified"] || infusion_id == infusion_name_to_id_map["hasty"] || infusion_id == infusion_name_to_id_map["tireless"]))
			Result = 0;
		if (configuration.restrict_tools && infusion_id == infusion_name_to_id_map["lightweight"])
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


RValue& GmlScriptCreateItemPrototypesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CREATE_ITEM_PROTOTYPES));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	size_t array_length;
	g_ModuleInterface->GetArraySize(Result, array_length);

	// Load all items.
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(Result, i, array_element);

		item_id_to_prototype_map[i] = *array_element;
	}

	return Result;
}

RValue& GmlScriptSceneAudioPlayerPlayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY))
		script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY] = { Self, Other };

	if (game_is_active && configuration.randomize_dungeon_music && AriCurrentGmRoomIsDungeonFloor() && floor_number != 91)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> game_music_distribution(0, MUSIC_INTERNAL_NAMES.size() - 1);
		*Arguments[0] = RValue(MUSIC_INTERNAL_NAMES.at(game_music_distribution(random_generator)));
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptFindNpcBlipNoiseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_FIND_NPC_BLIP_NOISE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (AriCurrentGmRoomIsDungeonFloor())
	{
		std::string audio_asset_name = Result.ToString();
		if (audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipSeridiaHuman" || audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipPriestess")
			Result = "SoundEffects/NPCs/Vocal/TextBlipHeadPriestess";
	}
	
	return Result;
}

RValue& GmlScriptSaveGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (floor_number != 0)
	{
		CreateNotification(true, SAVING_DISABLED_NOTIFICATION_KEY, Self, Other);
		return Result;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SAVE_GAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptAriFaceDirCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ari_facing_dir = Arguments[0]->ToDouble();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FACE_DIR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

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

RValue& GmlScriptRecipeGenerateInfusionsCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_RECIPE_GENERATE_INFUSIONS));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (StructVariableExists(Self, "item_id"))
	{
		int item_id = Self->GetMember("item_id").ToInt64();
		if (item_id_to_sigil_map.contains(item_id) || lift_key_items.contains(item_id) || orb_items.contains(item_id))
		{
			RValue empty_array = g_ModuleInterface->CallBuiltin("array_create", { 0 });
			*Result.GetRefMember("__count") = 0;
			*Result.GetRefMember("__internal_size") = 0;
			*Result.GetRefMember("__buffer") = empty_array;
		}
	}

	return Result;
}

RValue& GmlScriptBarkEmitterCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (/*!custom_bark_playing && */(StructVariableExists(Other, "god_mode") || StructVariableExists(Other, "wimp_mode")))
		script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_BARK_EMITTER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptT2ReadCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_T2_READ))
		script_name_to_reference_map[GML_SCRIPT_T2_READ] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_T2_READ));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

