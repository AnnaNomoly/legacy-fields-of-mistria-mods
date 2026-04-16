#include "../../utils/Utils.h"
#include "../../patterns/MonsterPatterns.h"

RValue& GmlScriptOnDungeonRoomStartCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	UnlockLiftKeyRecipe(Self, Other);
	ResetCustomDrawFields();

	salves_used.clear();
	active_sigils.clear();
	active_greater_sigils.clear();
	active_floor_enchantments.clear();
	spirit_link_combined_health_pool = 0;
	active_offerings = queued_offerings;
	queued_offerings.clear();
	current_floor_monsters.clear();

	// Floor Trap controls
	active_traps.clear();
	active_traps_to_value_map.clear();
	floor_trap_positions.clear();
	revealed_floor_traps.clear();

	// Dread Beast & Boss controls
	dread_beast_configured = false;
	dread_beast_monster_id = -1;
	dread_beasts_configured = 0;
	boss_monsters_configured = 0;
	if (active_offerings.empty() && ari_current_gm_room != "rm_mines_entry" && ari_current_gm_room != "rm_priestess_quarters" && ari_current_gm_room != "rm_seridias_chamber" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure") && !ari_current_gm_room.contains("milestone"))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

		int random = zero_to_ninety_nine_distribution(random_generator);
		if (random < Config::config.random_dread_beast_spawn_chance && floor_number > 1 && !ari_current_gm_room.contains("seal"))
			active_offerings.insert(Offerings::DREAD);
	}
	show_dashes = active_offerings.contains(Offerings::DREAD) || boss_battle != BossBattle::NONE;
	show_danger_banner = active_offerings.contains(Offerings::DREAD) || boss_battle != BossBattle::NONE;
	if (active_offerings.contains(Offerings::DREAD))
		SelectDreadBeast(Self, Other);
	initial_floor_monsters.clear();

	if (Config::config.restrict_perks)
		DisableAllPerks();
	ModifySpellCosts(true, true);
	ScaleMistpoolArmor(true);
	ScaleMistpoolWeapon(true);
	ScaleMistpoolPickaxe(true);
	ScaleClassArmor(true);
	CancelAllStatusEffects();
	SetInvulnerabilityHits(0);
	SetFireBreathTime(0);
	drop_biome_reward = false;
	inner_fire_cast = false;
	reckoning_applied = false;
	fairy_buff_applied = false;
	stoneskin_applied = false;
	offering_chance_occurred = false;
	frailty_hit_counter = 0;
	grudge_counter = 0;
	deep_wounds_damage_pool = 0;
	stoneskin_shield_amount = 0;
	sigil_of_silence_count = 0;
	sigil_of_alteration_monster_id = 0;

	// Track Unmodified Max HP
	unmodified_base_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

	// Toggle reward on seal rooms when dungeon lift is disabled
	if (Config::config.disable_dungeon_lift && (ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_seridias_chamber") && ari_current_gm_room != "rm_void_seal" && !biome_reward_disabled)
		drop_biome_reward = true;
	biome_reward_disabled = false;

	if (ari_current_gm_room != "rm_mines_entry" && ari_current_gm_room != "rm_priestess_quarters" && ari_current_gm_room != "rm_seridias_chamber" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure") && !ari_current_gm_room.contains("milestone"))
	{
		// Hide (Rogue Set Bonus)
		if (CountEquippedClassArmor()[Classes::ROGUE] > 0)
			active_sigils.insert(Sigils::CONCEALMENT);

		// Prophecy (Oracle Set Bonus)
		if (CountEquippedClassArmor()[Classes::ORACLE] >= 5 && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] == 1)
		{
			static thread_local pcg32 random_generator([] {
				std::random_device rd;
				return pcg32(
					(static_cast<uint64_t>(rd()) << 32) | rd(),
					(static_cast<uint64_t>(rd()) << 32) | rd()
					);
				}());
			std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

			int random = zero_to_ninety_nine_distribution(random_generator);
			if (random < 30)
			{
				active_sigils.insert(Sigils::FORTIFICATION);
				CreateNotification(false, PROPHECY_FORTIFICATION_NOTIFICATION_KEY, Self, Other);
			}
			else if (random < 60)
			{
				active_sigils.insert(Sigils::STRENGTH);
				CreateNotification(false, PROPHECY_STRENGTH_NOTIFICATION_KEY, Self, Other);
			}
			else if (random < 75)
			{
				active_sigils.insert(Sigils::PROTECTION);
				CreateNotification(false, PROPHECY_PROTECTION_NOTIFICATION_KEY, Self, Other);

				RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["guardians_shield"], RValue(), 1, 2147483647.0);
				SetInvulnerabilityHits(2);
			}
			else if (random < 90)
			{
				active_sigils.insert(Sigils::CONCEALMENT);
				CreateNotification(false, PROPHECY_CONCEALMENT_NOTIFICATION_KEY, Self, Other);
			}
			else
			{
				active_sigils.insert(Sigils::SAFETY);
				CreateNotification(false, PROPHECY_SAFETY_NOTIFICATION_KEY, Self, Other);
			}
		}

		if (!active_sigils.contains(Sigils::SAFETY)) // This should only happen after Prophecy (Oracle Set Bonus)
			GenerateFloorTraps();

		if (treasure_spot.state == TreasureSpot::WAITING_TO_SPAWN)
		{
			active_sigils.insert(Sigils::INTUITION);
			treasure_spot.floors_to_descend--;

			if(treasure_spot.floors_to_descend > 0)
				CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
		}

		if (ari_current_gm_room == "rm_mines_upper_floor1")
			active_floor_enchantments = RandomFloorEnchantments(true, DungeonBiomes::UPPER);
		else if (ari_current_gm_room.find("rm_mines_upper") != std::string::npos)
			active_floor_enchantments = RandomFloorEnchantments(false, DungeonBiomes::UPPER);
		else if (ari_current_gm_room.find("rm_mines_tide") != std::string::npos)
			active_floor_enchantments = RandomFloorEnchantments(false, DungeonBiomes::TIDE_CAVERNS);
		else if (ari_current_gm_room.find("rm_mines_deep") != std::string::npos)
			active_floor_enchantments = RandomFloorEnchantments(false, DungeonBiomes::DEEP_EARTH);
		else if (ari_current_gm_room.find("rm_mines_lava") != std::string::npos)
			active_floor_enchantments = RandomFloorEnchantments(false, DungeonBiomes::LAVA_CAVES);
		else if (ari_current_gm_room.find("rm_mines_ruins") != std::string::npos)
			active_floor_enchantments = RandomFloorEnchantments(false, DungeonBiomes::RUINS);

		if (!active_floor_enchantments.empty() && active_offerings.contains(Offerings::DREAD))
			PlayConversation(FLOOR_ENCHANTMENT_AND_DREAD_BEAST_WARNING_CONVERSATION_KEY, Self, Other);
		else if (!active_floor_enchantments.empty())
			PlayConversation(FLOOR_ENCHANTMENT_CONVERSATION_KEY, Self, Other);
		else if (active_offerings.contains(Offerings::DREAD))
			PlayConversation(DREAD_BEAST_WARNING_CONVERSATION_KEY, Self, Other);

		if (active_floor_enchantments.contains(FloorEnchantments::RESTORATION))
			time_of_last_restoration_tick = current_time_in_seconds;
		if (active_floor_enchantments.contains(FloorEnchantments::SECOND_WIND))
			time_of_last_second_wind_tick = current_time_in_seconds;
		if (active_floor_enchantments.contains(FloorEnchantments::FUMIGATE))
			time_of_last_fumigate_tick = current_time_in_seconds;
		if (active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS))
			time_of_last_deep_wounds_tick = current_time_in_seconds;
		if (active_offerings.contains(Offerings::OUTBREAK))
			time_of_last_outbreak_tick = current_time_in_seconds - TWENTY_FIVE_MINUTES_IN_SECONDS;
		if (CountEquippedClassArmor()[Classes::CLERIC] > 0)
			class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = current_time_in_seconds;

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

		// Blessed (Oracle Set Bonus)
		if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			int bonus = 0;
			if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1)
				bonus += 10 * active_floor_enchantments.size();
			if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] == 1)
				bonus += 10 * active_offerings.size();

			if (bonus > 0)
			{
				int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				int adjusted_max_health = max_health + bonus;
				int adjusted_current_health = current_health + bonus;

				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
				SetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_current_health);

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);

				class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = bonus;
			}
		}

		// HP Penalty
		if (active_floor_enchantments.contains(FloorEnchantments::HP_PENALTY))
		{
			int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
			int penalty = max_health / 4;
			int adjusted_max_health = max_health - penalty;
			hp_penalty_amount = penalty;

			SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
			int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

			VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
			VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);
		}
	}
	else if (boss_battle != BossBattle::NONE)
	{
		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

		if (boss_battle == BossBattle::TIDE_CAVERNS_ORB)
		{
			SpawnMonster(Self, Other, 144 + 8, 208 + 8, monster_name_to_id_map["rockclod_blue"]); // Left
			SpawnMonster(Self, Other, 240 + 8, 208 + 8, monster_name_to_id_map["rockclod_blue"]); // Right
			SpawnMonster(Self, Other, 192 + 8, 240 + 8, monster_name_to_id_map["rockclod_blue"]); // Middle
			PlayConversation(BOSS_BATTLE_TIDE_CAVERNS_ORB_CONVERSATION_KEY, Self, Other);
		}
		else if (boss_battle == BossBattle::DEEP_EARTH_ORB)
		{
			SpawnMonster(Self, Other, 144 + 8, 208 + 8, monster_name_to_id_map["enchantern_blue"]); // Left
			SpawnMonster(Self, Other, 240 + 8, 208 + 8, monster_name_to_id_map["enchantern_blue"]); // Right
			SpawnMonster(Self, Other, 192 + 8, 240 + 8, monster_name_to_id_map["stalagmite"]); // Middle
			PlayConversation(BOSS_BATTLE_DEEP_EARTH_ORB_CONVERSATION_KEY, Self, Other);
		}
		else if (boss_battle == BossBattle::LAVA_CAVES_ORB)
		{
			SpawnMonster(Self, Other, 128 + 8, 240 + 8, monster_name_to_id_map["bat_blue"]); // West
			SpawnMonster(Self, Other, 256 + 8, 240 + 8, monster_name_to_id_map["bat_blue"]); // East
			SpawnMonster(Self, Other, 192 + 8, 176 + 8, monster_name_to_id_map["bat_blue"]); // North
			SpawnMonster(Self, Other, 192 + 8, 224 + 8, monster_name_to_id_map["cat"]); // Center
			PlayConversation(BOSS_BATTLE_LAVA_CAVES_ORB_CONVERSATION_KEY, Self, Other);
		}
		else if (boss_battle == BossBattle::RUINS_ORB)
		{
			SpawnMonster(Self, Other, 192 + 8, 240 + 8, monster_name_to_id_map["rock_stack"]); // Center
			SpawnMonster(Self, Other, 112 + 8, 208 + 8, monster_name_to_id_map["griffin_statue"]);
			SpawnMonster(Self, Other, 272 + 8, 208 + 8, monster_name_to_id_map["griffin_statue"]);
			PlayConversation(BOSS_BATTLE_RUINS_ORB_CONVERSATION_KEY, Self, Other);
		}
	}

	class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 0;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_DUNGEON_ROOM_START));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	floor_start_time = current_time_in_seconds;
	return Result;
}
