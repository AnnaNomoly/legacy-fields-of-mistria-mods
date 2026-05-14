#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

static void StopChallengeMode(bool cleared)
{
	time_stopped = false;
	is_challenge_mode = false;
	Config::RestoreAfterChallengeMode();
	RefreshPrototypes();

	if (cleared)
	{
		UpdateChallengeModeProgress();

		if (floor_number != 100)
			MMAPI::Game::CreateNotification(false, CHALLENGE_MODE_BIOME_CLEARED_NOTIFICATION_KEY);
		else
			MMAPI::Game::CreateNotification(false, CHALLENGE_MODE_COMPLETED_NOTIFICATION_KEY);
	}
	else
	{
		challenge_mode_progress = {};
		MMAPI::Game::CreateNotification(false, CHALLENGE_MODE_FAILED_NOTIFICATION_KEY);
	}

	challenge_mode_progress.run_in_progress = false;
	WriteChallengeModeFile();
	RemoveItemsFromInventoryForChallengeMode();
}

// MMAPI Location::Hooks::BeforeGoToRoom callback.
void BeforeGoToRoom(MMAPI::Location::BeforeGoToRoomContext& ctx)
{
	ResetCustomDrawFields();
	revealed_floor_traps.clear();
	meteor_aoes.clear();
	gaze_aoes.clear();
	void_aoes.clear();

	// Teleport Ari to the ritual chamber for boss battles.
	if (boss_battle == BossBattle::TIDE_CAVERNS_ORB && !ari_current_gm_room.contains("ritual_chamber"))
		ctx.SetTargetRoom("rm_mines_tide_ritual_chamber");
	else if (boss_battle == BossBattle::DEEP_EARTH_ORB && !ari_current_gm_room.contains("ritual"))
		ctx.SetTargetRoom("rm_mines_deep_ritual_chamber");
	else if (boss_battle == BossBattle::LAVA_CAVES_ORB && !ari_current_gm_room.contains("ritual"))
		ctx.SetTargetRoom("rm_mines_lava_ritual_chamber");
	else if (boss_battle == BossBattle::RUINS_ORB && !ari_current_gm_room.contains("ritual"))
		ctx.SetTargetRoom("rm_mines_ruins_ritual_chamber");
	else if (boss_battle != BossBattle::NONE && ari_current_gm_room.contains("ritual"))
		boss_battle = BossBattle::NONE;

	if (Config::config.disable_dungeon_lift && ari_current_gm_room == "rm_void_seal")
		biome_reward_disabled = true;
}

// MMAPI Location::Hooks::AfterGoToRoom callback.
void AfterGoToRoom(MMAPI::Location::AfterGoToRoomContext& ctx)
{
	// Capture "was I on a dungeon floor before this transition?" before updating ari_current_gm_room.
	const bool was_in_dungeon_floor = AriCurrentGmRoomIsDungeonFloor();

	ari_current_gm_room = std::string{ ctx.GetRoomName() };

	if ((ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_priestess_quarters" || ari_current_gm_room == "rm_seridias_chamber") && ari_current_gm_room != "rm_mines_entry")
		SetFloorNumber();
	else
		floor_number = 0;

	if (is_challenge_mode)
	{
		if ((ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_seridias_chamber") && floor_number == challenge_mode_progress.highest_floor_reached + 20)
			StopChallengeMode(true);
		else if (!AriCurrentGmRoomIsDungeonFloor())
			StopChallengeMode(false);
	}

	time_stopped = AriCurrentGmRoomIsDungeonFloor() && (is_challenge_mode || Config::config.experimental_stop_time_in_dungeon);

	*__YYTK.GetRefMember(MOD_NAME)->GetRefMember("floor") = floor_number;

	ModifyMistpoolWeaponSprites();
	ModifyMistpoolPickaxeSprites();
	ModifyBarkSprites();

	MMAPI::ToolbarMenu::ForceUpdate();

	if (Config::config.randomize_dungeon_music && floor_number > 0)
		MMAPI::Game::StopSceneAudio();

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

	hp_penalty_amount = -1;
	if (unmodified_base_health != -1)
	{
		SetMaxHealth(unmodified_base_health);
		int current_health = MMAPI::Player::GetHealth().ToInt64();

		MMAPI::VitalsMenu::SetMaxHealth(unmodified_base_health);
		MMAPI::VitalsMenu::SetHealth(current_health, unmodified_base_health);
		unmodified_base_health = -1;
	}

	if (Config::config.experimental_max_health_bug_fix)
	{
		int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
		if (floor_number <= 100 && max_health > 200)
		{
			SetMaxHealth(200);
			int current_health = MMAPI::Player::GetHealth().ToInt64();

			MMAPI::VitalsMenu::SetMaxHealth(200);
			MMAPI::VitalsMenu::SetHealth(current_health, 200);
		}

		if (floor_number == 100)
		{
			if (max_health < 200)
			{
				SetMaxHealth(200);
				int current_health = MMAPI::Player::GetHealth().ToInt64();

				MMAPI::VitalsMenu::SetMaxHealth(200);
				MMAPI::VitalsMenu::SetHealth(current_health, 200);
			}
		}
		else if (floor_number > 80)
		{
			if (max_health < 180)
			{
				SetMaxHealth(180);
				int current_health = MMAPI::Player::GetHealth().ToInt64();

				MMAPI::VitalsMenu::SetMaxHealth(180);
				MMAPI::VitalsMenu::SetHealth(current_health, 180);
			}
		}
		else if (floor_number > 60)
		{
			if (max_health < 160)
			{
				SetMaxHealth(160);
				int current_health = MMAPI::Player::GetHealth().ToInt64();

				MMAPI::VitalsMenu::SetMaxHealth(160);
				MMAPI::VitalsMenu::SetHealth(current_health, 160);
			}
		}
		else if (floor_number > 40)
		{
			if (max_health < 140)
			{
				SetMaxHealth(140);
				int current_health = MMAPI::Player::GetHealth().ToInt64();

				MMAPI::VitalsMenu::SetMaxHealth(140);
				MMAPI::VitalsMenu::SetHealth(current_health, 140);
			}
		}
		else if (floor_number > 20)
		{
			if (max_health < 120)
			{
				SetMaxHealth(120);
				int current_health = MMAPI::Player::GetHealth().ToInt64();

				MMAPI::VitalsMenu::SetMaxHealth(120);
				MMAPI::VitalsMenu::SetHealth(current_health, 120);
			}
		}
	}

	if (floor_number != 0 && (treasure_spot.state == TreasureSpot::SPAWNED || treasure_spot.state == TreasureSpot::FOUND))
		treasure_spot = TreasureSpot();

	if (was_in_dungeon_floor && (!ari_current_gm_room.contains("rm_mines") || ari_current_gm_room == "rm_mines_entry"))
	{
		ResetCustomDrawFields();
		salves_used.clear();
		active_sigils.clear();
		active_greater_sigils.clear();
		active_dread_contracts.clear();
		active_floor_enchantments.clear();
		active_offerings.clear();
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
		MMAPI::StatusEffect::CancelAll();
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
		sigil_of_alteration_monster_id = -1;
		dread_beast_monster_id = -1;
		dread_beasts_configured = 0;
		boss_monsters_configured = 0;
		class_name_to_set_bonus_effect_value_map.clear();
		initial_floor_monsters.clear();
		treasure_spot = TreasureSpot();

		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 0;
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 0;
	}
	else
		active_offerings.clear();
}
