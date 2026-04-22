#include "Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::UI;
using namespace State::Maps;

bool FairyBuffIsActive()
{
	RValue ari = global_instance->GetMember("__ari");
	RValue status_effects = ari.GetMember("status_effects");
	RValue effects = status_effects.GetMember("effects");
	RValue inner = effects.GetMember("inner");

	if (StructVariableExists(inner, std::to_string(status_effect_name_to_id_map["fairy"]).c_str()))
	{
		RValue fairy_status = inner.GetMember(std::to_string(status_effect_name_to_id_map["fairy"]));
		return fairy_status.m_Kind == VALUE_OBJECT;
	}

	return false;
}

double GetInvulnerabilityHits()
{
	RValue ari = global_instance->GetMember("__ari");
	return ari.GetMember("invulnerable_hits").ToDouble();
}

void SetInvulnerabilityHits(double amount)
{
	RValue ari = *global_instance->GetRefMember("__ari");
	double invulnerability_hits = ari.GetMember("invulnerable_hits").ToDouble();

	if (amount == 0)
		*ari.GetRefMember("invulnerable_hits") = amount;
	else
		*ari.GetRefMember("invulnerable_hits") = invulnerability_hits + amount;
}

void SetFireBreathTime(double value)
{
	// Stop the fire breath spell
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("fire_breath_time") = value;
}

void DisableAllPerks()
{
	std::unordered_set<int> perks_to_disable = {};

	std::vector<std::string> struct_field_names = {};
	auto GetStructFieldNames = [&](IN const char* MemberName, IN OUT RValue* Value) {
		struct_field_names.push_back(MemberName);
		return false;
	};

	RValue dragon_shrine_data = global_instance->GetMember("__dragon_shrine_data");
	RValue inner = dragon_shrine_data.GetMember("inner");

	// Combat Perks
	RValue combat = inner.GetMember("combat");
	g_ModuleInterface->EnumInstanceMembers(combat, GetStructFieldNames);
	for (std::string field_name : struct_field_names)
	{
		if (field_name.contains("tier"))
		{
			size_t array_length;
			RValue tier = combat.GetMember(field_name);
			g_ModuleInterface->GetArraySize(tier, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* entry;
				g_ModuleInterface->GetArrayEntry(tier, i, entry);

				perks_to_disable.insert(entry->GetMember("perk").ToInt64());
			}
		}
	}

	// Mining Perks
	struct_field_names = {};
	RValue mining = inner.GetMember("mining");
	g_ModuleInterface->EnumInstanceMembers(mining, GetStructFieldNames);
	for (std::string field_name : struct_field_names)
	{
		if (field_name.contains("tier"))
		{
			size_t array_length;
			RValue tier = mining.GetMember(field_name);
			g_ModuleInterface->GetArraySize(tier, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* entry;
				g_ModuleInterface->GetArrayEntry(tier, i, entry);

				perks_to_disable.insert(entry->GetMember("perk").ToInt64());
			}
		}
	}

	// Cooking Perks
	perks_to_disable.insert(perk_name_to_id_map["snacktime"]);

	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue __ari_perks_active = *__ari.GetRefMember("perks_active");

	for (int perk : perks_to_disable)
		__ari_perks_active[perk] = false;
}

bool ItemHasBeenAcquired(int item_id)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue items_acquired = *__ari.GetRefMember("items_acquired");
	RValue item_acquired = g_ModuleInterface->CallBuiltin("array_get", { items_acquired, item_id });
	return item_acquired.ToBoolean();
}

void MarkDungeonTutorialUnseen()
{
	RValue ari = *global_instance->GetRefMember("__ari");
	RValue tutorials_seen = *ari.GetRefMember("tutorials_seen");

	RValue* mines_tutorial;
	g_ModuleInterface->GetArrayEntry(tutorials_seen, tutorial_name_to_id_map["mines"], mines_tutorial);

	*mines_tutorial = false; // TODO: This works, but should only be called once per save file.
}

void TrackAriResources(CInstance* Self, CInstance* Other)
{
	ari_resource_to_value_map[AriResources::HEALTH] = GetHealth(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::MAX_HEALTH] = GetMaxHealth(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::STAMINA] = GetStamina(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::MANA] = GetMana(Self, Other).ToInt64();
}

void UnlockLiftKeyRecipe(CInstance* Self, CInstance* Other)
{
	if (!Config::config.disable_dungeon_lift)
		return;

	if (floor_number == 5)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F5_NAME], Self, Other);
	else if (floor_number == 10)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F10_NAME], Self, Other);
	else if (floor_number == 15)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F15_NAME], Self, Other);
	else if (floor_number == 20)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], Self, Other);
	else if (floor_number == 25)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME], Self, Other);
	else if (floor_number == 30)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME], Self, Other);
	else if (floor_number == 35)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME], Self, Other);
	else if (floor_number == 40)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], Self, Other);
	else if (floor_number == 45)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME], Self, Other);
	else if (floor_number == 50)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME], Self, Other);
	else if (floor_number == 55)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME], Self, Other);
	else if (floor_number == 60)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], Self, Other);
	else if (floor_number == 65)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME], Self, Other);
	else if (floor_number == 70)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME], Self, Other);
	else if (floor_number == 75)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME], Self, Other);
	else if (floor_number == 80)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F80_NAME], Self, Other);
	else if (floor_number == 85)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F85_NAME], Self, Other);
	else if (floor_number == 90)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F90_NAME], Self, Other);
	else if (floor_number == 95)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F95_NAME], Self, Other);
	else if (floor_number == 100)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F100_NAME], Self, Other);
}

void DropLiftKey()
{
	if(floor_number >= 95)
		DropItem(item_name_to_id_map[RUINS_KEY_F95_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 90)
		DropItem(item_name_to_id_map[RUINS_KEY_F90_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 85)
		DropItem(item_name_to_id_map[RUINS_KEY_F85_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 80)
		DropItem(item_name_to_id_map[RUINS_KEY_F80_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 75)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 70)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 65)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 60)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 55)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 50)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 45)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 40)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 35)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 30)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 25)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 20)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 15)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F15_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 10)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F10_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 5)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F5_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
}

bool AriCurrentGmRoomIsDungeonFloor()
{
	if (boss_battle != BossBattle::NONE)
		return true;
	if (is_challenge_mode && ari_current_gm_room == "rm_priestess_quarters")
		return true;
	return ari_current_gm_room.contains("rm_mines") && ari_current_gm_room != "rm_mines_entry" && !ari_current_gm_room.contains("seal");
}

void SetFloorNumber()
{
	if (boss_battle != BossBattle::NONE)
	{
		if (ari_current_gm_room == "rm_mines_tide_ritual_chamber")
			floor_number = 20;
		else if (ari_current_gm_room == "rm_mines_deep_ritual_chamber")
			floor_number = 40;
		else if (ari_current_gm_room == "rm_mines_lava_ritual_chamber")
			floor_number = 60;
		else if (ari_current_gm_room == "rm_mines_ruins_ritual_chamber")
			floor_number = 80;
	}
	else if (ari_current_gm_room.contains("treasure") || ari_current_gm_room.contains("milestone"))
		return; // Update 0.15.0 changed treasure rooms to "be considered side rooms rather than level-progressing rooms"
	else if (ari_current_gm_room.contains("ritual"))
		return; // Update 0.15.0 changed ritual rooms to "be considered side rooms rather than level-progressing rooms"
	else if (ari_current_gm_room == "rm_mines_upper_floor1")
		floor_number = 1;
	else if (ari_current_gm_room == "rm_mines_upper_elevator5")
		floor_number = 5;
	else if (ari_current_gm_room == "rm_mines_upper_elevator10")
		floor_number = 10;
	else if (ari_current_gm_room == "rm_mines_upper_elevator15")
		floor_number = 15;
	else if (ari_current_gm_room == "rm_water_seal")
		floor_number = 20;
	else if (ari_current_gm_room == "rm_mines_tide_floor21")
		floor_number = 21;
	else if (ari_current_gm_room == "rm_mines_tide_elevator25")
		floor_number = 25;
	else if (ari_current_gm_room == "rm_mines_tide_elevator30")
		floor_number = 30;
	else if (ari_current_gm_room == "rm_mines_tide_elevator35")
		floor_number = 35;
	else if (ari_current_gm_room == "rm_earth_seal")
		floor_number = 40;
	else if (ari_current_gm_room == "rm_mines_deep_41")
		floor_number = 41;
	else if (ari_current_gm_room == "rm_mines_deep_45")
		floor_number = 45;
	else if (ari_current_gm_room == "rm_mines_deep_50")
		floor_number = 50;
	else if (ari_current_gm_room == "rm_mines_deep_55")
		floor_number = 55;
	else if (ari_current_gm_room == "rm_fire_seal")
		floor_number = 60;
	else if (ari_current_gm_room == "rm_mines_lava_61")
		floor_number = 61;
	else if (ari_current_gm_room == "rm_mines_lava_65")
		floor_number = 65;
	else if (ari_current_gm_room == "rm_mines_lava_70")
		floor_number = 70;
	else if (ari_current_gm_room == "rm_mines_lava_75")
		floor_number = 75;
	else if (ari_current_gm_room == "rm_ruins_seal" || ari_current_gm_room == "rm_void_seal")
		floor_number = 80;
	else if (ari_current_gm_room == "rm_mines_ruins_85")
		floor_number = 85;
	else if (ari_current_gm_room == "rm_priestess_quarters")
		floor_number = 90;
	else if (ari_current_gm_room == "rm_mines_ruins_95")
		floor_number = 95;
	else if (ari_current_gm_room == "rm_seridias_chamber")
		floor_number = 100;
	else
		floor_number++;
}

void ResetStaticFields(bool returned_to_title_screen)
{
	if (returned_to_title_screen)
	{
		game_is_active = false;
		unlock_recipes = true;
		is_restoration_tracked_interval = false;
		is_second_wind_tracked_interval = false;
		is_fumigate_tracked_interval = false;
		is_deep_wounds_tracked_interval = false;
		ari_x = -1;
		ari_y = -1;
		ari_facing_dir = -1;
		floor_number = 0;
		floor_start_time = 0;
		current_time_in_seconds = -1;
		time_of_last_restoration_tick = -1;
		time_of_last_second_wind_tick = -1;
		time_of_last_fumigate_tick = -1;
		time_of_last_deep_wounds_tick = -1;
		time_of_last_outbreak_tick = -1;
		held_item_id = -1;
		unmodified_base_health = -1;
		hp_penalty_amount = -1;
		in_whirl_pool = false;
		is_new_game = false;
		is_challenge_mode = false;
		save_prefix = "";
		challenge_mode_progress = {};
		bool was_challenge_mode = saved_config.has_value();
		Config::RestoreAfterChallengeMode();
		if (was_challenge_mode)
			RefreshPrototypes();
		ari_current_location = "";
		ari_current_gm_room = "";
		ari_resource_to_value_map.clear();
		ari_resource_to_penalty_map.clear();
		time_stopped = false;
		time_stopped_tick_accumulator = 0;
		boss_battle = BossBattle::NONE;
		treasure_spot = TreasureSpot();
		meteor_aoes.clear();
		gaze_aoes.clear();
		void_aoes.clear();
		revealed_floor_traps.clear();
		initial_floor_monsters.clear();
		class_name_to_set_bonus_effect_value_map.clear();
		script_name_to_reference_map.clear();
	}

	crafting_menu_open = false;
	journal_menu_open = false;
	store_menu_open = false;
	drop_biome_reward = false;
	biome_reward_disabled = false;
	dread_beast_configured = false;
	sigil_item_used = false;
	greater_sigil_item_used = false;
	salve_item_used = false;
	lift_key_used = false;
	orb_item_used = false;
	dread_contract_used = false;
	heart_crystal_used = false;
	inner_fire_cast = false;
	reckoning_applied = false;
	fairy_buff_applied = false;
	stoneskin_applied = false;
	offering_chance_occurred = false;
	obj_dragonshrine_focused = false;
	obj_dungeon_elevator_focused = false;
	obj_dungeon_ladder_down_focused = false;
	frailty_hit_counter = 0;
	grudge_counter = 0;
	deep_wounds_damage_pool = 0;
	stoneskin_shield_amount = 0;
	spirit_link_combined_health_pool = 0;
	sigil_of_silence_count = 0;
	sigil_of_alteration_monster_id = -1;
	dread_beast_monster_id = -1;
	dread_beasts_configured = 0;
	boss_monsters_configured = 0;
	salves_used.clear();
	active_sigils.clear();
	active_greater_sigils.clear();
	active_dread_contracts.clear();
	queued_offerings.clear();
	active_offerings.clear();
	active_floor_enchantments.clear();
	active_traps.clear();
	active_traps_to_value_map.clear();
	floor_trap_positions.clear();
	current_floor_monsters.clear();
	ResetCustomDrawFields();
	ModifySpellCosts(true, false); // TODO: Make sure this works. Go into the dungeon, Growth should be reduced to 4, exit to title, load back in, growth should be 8 (since you won't load into the dungeon).
}
