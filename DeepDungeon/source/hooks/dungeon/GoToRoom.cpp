#include "../../utils/Utils.h"
#include "../../patterns/MonsterPatterns.h"

RValue& GmlScriptGoToRoomCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ResetCustomDrawFields();
	revealed_floor_traps.clear();
	meteor_aoes.clear();
	gaze_aoes.clear();
	void_aoes.clear();

	// Teleport Ari to the ritual chamber for boss battles.
	if (boss_battle == BossBattle::TIDE_CAVERNS_ORB && !ari_current_gm_room.contains("ritual_chamber"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_tide_ritual_chamber" });
	else if (boss_battle == BossBattle::DEEP_EARTH_ORB && !ari_current_gm_room.contains("ritual"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_deep_ritual_chamber" });
	else if (boss_battle == BossBattle::LAVA_CAVES_ORB && !ari_current_gm_room.contains("ritual"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_lava_ritual_chamber" });
	else if (boss_battle == BossBattle::RUINS_ORB && !ari_current_gm_room.contains("ritual"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_ruins_ritual_chamber" });
	// End Boss Battles when leaving the ritual floor.
	else if (boss_battle != BossBattle::NONE && ari_current_gm_room.contains("ritual"))
		boss_battle = BossBattle::NONE;

	// If leaving the void seal, prohibit the key from spawning in progression mode.
	if (Config::config.disable_dungeon_lift && ari_current_gm_room == "rm_void_seal")
		biome_reward_disabled = true;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GO_TO_ROOM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	RValue gm_room = Result.GetMember("gm_room");
	RValue room_name = g_ModuleInterface->CallBuiltin("room_get_name", { gm_room });
	ari_current_gm_room = room_name.ToString();

	if ((ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_priestess_quarters" || ari_current_gm_room == "rm_seridias_chamber") && ari_current_gm_room != "rm_mines_entry")
		SetFloorNumber();
	else
		floor_number = 0;

	// Store the floor number in the global instance for other mods.
	*__YYTK.GetRefMember(MOD_NAME)->GetRefMember("floor") = floor_number;

	ModifyMistpoolWeaponSprites();
	ModifyMistpoolPickaxeSprites();
	ModifyBarkSprites();

	if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
		UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

	// Stop music for dungeon floors to force a new song to play.
	if (Config::config.randomize_dungeon_music && floor_number > 0 && script_name_to_reference_map.contains(GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY))
		SceneAudioPlayerStop(script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY][0], script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY][1]);

	// Reset any floor specific set bonus effects.
	class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = -1;
	class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] = magic_enum::enum_integer(GetRandomElementalSealEffect());
	class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = 0;

	// Reset Max HP Adjustments
	hp_penalty_amount = -1;
	if (unmodified_base_health != -1)
	{
		SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], unmodified_base_health);
		int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

		VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], unmodified_base_health);
		VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, unmodified_base_health);
		unmodified_base_health = -1;
	}

	// Max HP bug fix (experimental)
	if (Config::config.experimental_max_health_bug_fix)
	{
		int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
		if (floor_number == 100)
		{
			if (max_health < 200)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 200);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 200);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 200);
			}
		}
		else if (floor_number > 80)
		{
			if (max_health < 180)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 180);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 180);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 180);
			}
		}
		else if (floor_number > 60)
		{
			if (max_health < 160)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 160);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 160);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 160);
			}
		}
		else if (floor_number > 40)
		{
			if (max_health < 140)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 140);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 140);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 140);
			}
		}
		else if (floor_number > 20)
		{
			if (max_health < 120)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 120);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 120);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 120);
			}
		}
	}

	if (floor_number != 0 && (treasure_spot.state == TreasureSpot::SPAWNED || treasure_spot.state == TreasureSpot::FOUND))
		treasure_spot = TreasureSpot();

	if (ari_current_location == "dungeon" && (!ari_current_gm_room.contains("rm_mines") || ari_current_gm_room == "rm_mines_entry")) // TODO: Don't use ari_current_location
	{
		// TODO: Run logic to actually undo all active floor enchantments.
		// TOOD: Remove all buffs.
		ResetCustomDrawFields();
		salves_used.clear();
		active_sigils.clear();
		active_greater_sigils.clear();
		active_floor_enchantments.clear();
		active_offerings.clear(); // Different than OnDungeonRoomStart
		queued_offerings.clear();
		active_traps.clear();
		active_traps_to_value_map.clear();
		floor_trap_positions.clear();
		current_floor_monsters.clear();
		ModifySpellCosts(true, false);
		ScaleMistpoolArmor(false);
		ScaleMistpoolWeapon(false);
		ScaleMistpoolPickaxe(false);
		ScaleClassArmor(false);
		CancelAllStatusEffects();
		SetInvulnerabilityHits(0);
		SetFireBreathTime(0);
		drop_biome_reward = false;
		biome_reward_disabled = false;
		dread_beast_configured = false;
		inner_fire_cast = false;
		reckoning_applied = false;
		fairy_buff_applied = false;
		stoneskin_applied = false;
		offering_chance_occurred = false;
		floor_start_time = 0;
		frailty_hit_counter = 0;
		grudge_counter = 0;
		deep_wounds_damage_pool = 0;
		stoneskin_shield_amount = 0;
		spirit_link_combined_health_pool = 0;
		sigil_of_silence_count = 0;
		sigil_of_alteration_monster_id = 0;
		dread_beast_monster_id = -1;
		dread_beasts_configured = 0;
		boss_monsters_configured = 0;
		class_name_to_set_bonus_effect_value_map.clear();
		initial_floor_monsters.clear();
		treasure_spot = TreasureSpot();

		// Reset Oracle set bonus effects.
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 0;
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 0;

	}
	else
		active_offerings.clear();

	return Result;
}
