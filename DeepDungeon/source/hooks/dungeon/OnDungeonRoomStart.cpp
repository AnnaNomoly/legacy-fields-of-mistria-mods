#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::UI;
using namespace State::Maps;

// MMAPI Dungeon::Hooks::AfterDungeonRoomStart callback.
void AfterDungeonRoomStart(MMAPI::Dungeon::DungeonRoomStartContext& /*ctx*/)
{
	UnlockLiftKeyRecipe(nullptr, nullptr);
	ResetCustomDrawFields();

	if (is_challenge_mode)
		DropItemsForChallengeMode(nullptr, nullptr);

	salves_used.clear();
	active_sigils.clear();
	active_greater_sigils.clear();
	active_dread_contracts.clear();
	active_floor_enchantments.clear();
	spirit_link_combined_health_pool = 0;
	active_offerings = queued_offerings;
	queued_offerings.clear();
	current_floor_monsters.clear();

	active_traps.clear();
	active_traps_to_value_map.clear();
	floor_trap_positions.clear();
	revealed_floor_traps.clear();

	dread_beast_configured = false;
	dread_beast_monster_id = -1;
	dread_beasts_configured = 0;
	boss_monsters_configured = 0;
	if (!is_challenge_mode && active_offerings.empty() && ari_current_gm_room != "rm_mines_entry" && ari_current_gm_room != "rm_priestess_quarters" && ari_current_gm_room != "rm_seridias_chamber" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure") && !ari_current_gm_room.contains("milestone"))
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
		SelectDreadBeast(nullptr, nullptr);
	initial_floor_monsters.clear();

	if (Config::config.restrict_perks)
		DisableAllPerks();
	ModifySpellCosts(true, true);
	ScaleMistpoolArmor(true);
	ScaleMistpoolWeapon(true);
	ScaleMistpoolPickaxe(true);
	ScaleClassArmor(true);
	MMAPI::StatusEffect::CancelAll();
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
	sigil_of_alteration_monster_id = -1;

	unmodified_base_health = MMAPI::Player::GetMaxHealth().ToInt64();

	if (Config::config.disable_dungeon_lift && (ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_seridias_chamber") && ari_current_gm_room != "rm_void_seal" && !biome_reward_disabled)
		drop_biome_reward = true;
	biome_reward_disabled = false;

	if (ari_current_gm_room != "rm_mines_entry" && ari_current_gm_room != "rm_priestess_quarters" && ari_current_gm_room != "rm_seridias_chamber" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure") && !ari_current_gm_room.contains("milestone"))
	{
		auto armor_set_bonuses = GetArmorSetBonuses();

		if (armor_set_bonuses.rogue.Hide())
			active_sigils.insert(Sigils::CONCEALMENT);

		if (armor_set_bonuses.oracle.FullSet() && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] == 1)
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
				MMAPI::Game::CreateNotification(false, PROPHECY_FORTIFICATION_NOTIFICATION_KEY);
			}
			else if (random < 60)
			{
				active_sigils.insert(Sigils::STRENGTH);
				MMAPI::Game::CreateNotification(false, PROPHECY_STRENGTH_NOTIFICATION_KEY);
			}
			else if (random < 75)
			{
				active_sigils.insert(Sigils::PROTECTION);
				MMAPI::Game::CreateNotification(false, PROPHECY_PROTECTION_NOTIFICATION_KEY);

				MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::GuardiansShield), RValue(), 1, MMAPI::StatusEffect::InfiniteDuration);
				SetInvulnerabilityHits(2);
			}
			else if (random < 90)
			{
				active_sigils.insert(Sigils::CONCEALMENT);
				MMAPI::Game::CreateNotification(false, PROPHECY_CONCEALMENT_NOTIFICATION_KEY);
			}
			else
			{
				active_sigils.insert(Sigils::SAFETY);
				MMAPI::Game::CreateNotification(false, PROPHECY_SAFETY_NOTIFICATION_KEY);
			}
		}

		if (!active_sigils.contains(Sigils::SAFETY))
			GenerateFloorTraps();

		if (treasure_spot.state == TreasureSpot::WAITING_TO_SPAWN)
		{
			active_sigils.insert(Sigils::INTUITION);
			treasure_spot.floors_to_descend--;

			if (treasure_spot.floors_to_descend > 0)
				MMAPI::Game::CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY);
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
			MMAPI::Text::PlayConversation(FLOOR_ENCHANTMENT_AND_DREAD_BEAST_WARNING_CONVERSATION_KEY);
		else if (!active_floor_enchantments.empty())
			MMAPI::Text::PlayConversation(FLOOR_ENCHANTMENT_CONVERSATION_KEY);
		else if (active_offerings.contains(Offerings::DREAD))
			MMAPI::Text::PlayConversation(DREAD_BEAST_WARNING_CONVERSATION_KEY);

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
		if (armor_set_bonuses.cleric.AutoRegen())
			class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = current_time_in_seconds;

		MMAPI::ToolbarMenu::ForceUpdate();

		if (armor_set_bonuses.oracle.FullSet())
		{
			int bonus = 0;
			if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1)
				bonus += 10 * active_floor_enchantments.size();
			if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] == 1)
				bonus += 10 * active_offerings.size();

			if (bonus > 0)
			{
				int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
				int current_health = MMAPI::Player::GetHealth().ToInt64();
				int adjusted_max_health = max_health + bonus;
				int adjusted_current_health = current_health + bonus;

				SetMaxHealth(adjusted_max_health);
				MMAPI::Player::SetHealth(adjusted_current_health);

				MMAPI::VitalsMenu::SetMaxHealth(adjusted_max_health);
				MMAPI::VitalsMenu::SetHealth(current_health, adjusted_max_health);

				class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = bonus;
			}
		}

		if (active_floor_enchantments.contains(FloorEnchantments::HP_PENALTY))
		{
			int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
			int penalty = max_health / 4;
			int adjusted_max_health = max_health - penalty;
			hp_penalty_amount = penalty;

			SetMaxHealth(adjusted_max_health);
			int current_health = MMAPI::Player::GetHealth().ToInt64();

			MMAPI::VitalsMenu::SetMaxHealth(adjusted_max_health);
			MMAPI::VitalsMenu::SetHealth(current_health, adjusted_max_health);
		}
	}
	else if (boss_battle != BossBattle::NONE)
	{
		MMAPI::ToolbarMenu::ForceUpdate();

		if (boss_battle == BossBattle::TIDE_CAVERNS_ORB)
		{
			MMAPI::Monster::SpawnMonster(144 + 8, 208 + 8, MMAPI::Monster::Ids::RockclodBlue);
			MMAPI::Monster::SpawnMonster(240 + 8, 208 + 8, MMAPI::Monster::Ids::RockclodBlue);
			MMAPI::Monster::SpawnMonster(192 + 8, 240 + 8, MMAPI::Monster::Ids::RockclodBlue);
			MMAPI::Text::PlayConversation(BOSS_BATTLE_TIDE_CAVERNS_ORB_CONVERSATION_KEY);
		}
		else if (boss_battle == BossBattle::DEEP_EARTH_ORB)
		{
			MMAPI::Monster::SpawnMonster(144 + 8, 208 + 8, MMAPI::Monster::Ids::EnchanternBlue);
			MMAPI::Monster::SpawnMonster(240 + 8, 208 + 8, MMAPI::Monster::Ids::EnchanternBlue);
			MMAPI::Monster::SpawnMonster(192 + 8, 240 + 8, MMAPI::Monster::Ids::Stalagmite);
			MMAPI::Text::PlayConversation(BOSS_BATTLE_DEEP_EARTH_ORB_CONVERSATION_KEY);
		}
		else if (boss_battle == BossBattle::LAVA_CAVES_ORB)
		{
			MMAPI::Monster::SpawnMonster(128 + 8, 240 + 8, MMAPI::Monster::Ids::BatBlue);
			MMAPI::Monster::SpawnMonster(256 + 8, 240 + 8, MMAPI::Monster::Ids::BatBlue);
			MMAPI::Monster::SpawnMonster(192 + 8, 176 + 8, MMAPI::Monster::Ids::BatBlue);
			MMAPI::Monster::SpawnMonster(192 + 8, 224 + 8, MMAPI::Monster::Ids::Cat);
			MMAPI::Text::PlayConversation(BOSS_BATTLE_LAVA_CAVES_ORB_CONVERSATION_KEY);
		}
		else if (boss_battle == BossBattle::RUINS_ORB)
		{
			MMAPI::Monster::SpawnMonster(192 + 8, 240 + 8, MMAPI::Monster::Ids::RockStack);
			MMAPI::Monster::SpawnMonster(112 + 8, 208 + 8, MMAPI::Monster::Ids::GriffinStatue);
			MMAPI::Monster::SpawnMonster(272 + 8, 208 + 8, MMAPI::Monster::Ids::GriffinStatue);
			MMAPI::Text::PlayConversation(BOSS_BATTLE_RUINS_ORB_CONVERSATION_KEY);
		}
	}

	class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 0;
	class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 0;

	floor_start_time = current_time_in_seconds;
}
