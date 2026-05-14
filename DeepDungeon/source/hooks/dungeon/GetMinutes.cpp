#include "../../utils/Utils.h"
#include "../../patterns/MonsterPatterns.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

// MMAPI Calendar::Hooks::AfterClockUpdate callback — fires after the game's update@Clock@Clock
// script. ctx.GetOldTime() returns the clock time captured before the original advanced it.
void AfterClockUpdate(MMAPI::Calendar::ClockUpdateContext& ctx)
{
	static bool was_time_stopped = false;

	const int64_t old_time_value = ctx.GetOldTime();

	if (!game_is_active)
		return;

	RValue time = global_instance->GetMember("__clock").GetMember("time");
	current_time_in_seconds = time.ToInt64();

	if (time_stopped)
	{
		// Capture how much the game tried to advance the clock this tick.
		int64_t tick_delta = current_time_in_seconds - old_time_value;

		// Revert the clock to prevent in-game time from advancing.
		if (tick_delta != 0)
			*global_instance->GetRefMember("__clock")->GetRefMember("time") = old_time_value;
		current_time_in_seconds = old_time_value;

		// Reset the accumulator on the first call of a new time-stop window.
		if (!was_time_stopped)
			time_stopped_tick_accumulator = 0;
		was_time_stopped = true;

		TickTimeStoppedSystems(nullptr, nullptr, tick_delta);
		// Fall through — let all processing run with the decremented timestamps.
	}
	else
	{
		was_time_stopped = false;
	}

	RevealFloorTraps();
	ApplyFloorTraps(nullptr, nullptr);
	ProcessCustomAOEs();
	ProcessTreasureSpot(nullptr, nullptr);
	ProcessSpiritConcealment();

	// Restoration
	if (active_floor_enchantments.contains(FloorEnchantments::RESTORATION)
		&& !is_restoration_tracked_interval
		&& (current_time_in_seconds - time_of_last_restoration_tick) >= TWO_MINUTES_IN_SECONDS)
	{
		is_restoration_tracked_interval = true;
		time_of_last_restoration_tick = current_time_in_seconds;
	}

	// Second Wind
	if (active_floor_enchantments.contains(FloorEnchantments::SECOND_WIND)
		&& !is_second_wind_tracked_interval
		&& (current_time_in_seconds - time_of_last_second_wind_tick) >= TWO_MINUTES_IN_SECONDS)
	{
		is_second_wind_tracked_interval = true;
		time_of_last_second_wind_tick = current_time_in_seconds;
	}

	// Fumigate
	if (active_floor_enchantments.contains(FloorEnchantments::FUMIGATE)
		&& !is_fumigate_tracked_interval
		&& (current_time_in_seconds - time_of_last_fumigate_tick) >= TWO_MINUTES_AND_THIRTY_SECONDS)
	{
		is_fumigate_tracked_interval = true;
		time_of_last_fumigate_tick = current_time_in_seconds;
	}

	// Deep Wounds
	if (active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS)
		&& !is_deep_wounds_tracked_interval
		&& (current_time_in_seconds - time_of_last_deep_wounds_tick) >= TWO_MINUTES_IN_SECONDS)
	{
		is_deep_wounds_tracked_interval = true;
		time_of_last_deep_wounds_tick = current_time_in_seconds;
	}

	// Outbreak
	if (active_offerings.contains(Offerings::OUTBREAK))
	{
		if ((current_time_in_seconds - time_of_last_outbreak_tick) >= THIRTY_MINUTES_IN_SECONDS)
		{
			std::unordered_set<int> restricted_monsters = { // TODO: Update as needed with new monsters
				static_cast<int>(MMAPI::Monster::Ids::Barrel),
				static_cast<int>(MMAPI::Monster::Ids::Copperclod),
				static_cast<int>(MMAPI::Monster::Ids::Goldclod),
				static_cast<int>(MMAPI::Monster::Ids::Ironclod),
				static_cast<int>(MMAPI::Monster::Ids::Mimic),
				static_cast<int>(MMAPI::Monster::Ids::Mistrilclod),
				static_cast<int>(MMAPI::Monster::Ids::SaplingOrangeMini),
				static_cast<int>(MMAPI::Monster::Ids::Silverclod),
				static_cast<int>(MMAPI::Monster::Ids::RockclodPurple)
			};
			// rock_stack_lava isn't in MMAPI::Monster::Ids yet — resolve at runtime from __monster_id__.
			if (auto lava = MMAPI::Monster::TryFromInternalName("rock_stack_lava"))
				restricted_monsters.insert(static_cast<int>(*lava));

			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points") && !StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
				{
					int monster_id = monster->GetMember("monster_id").ToInt64();
					double hit_points = monster->GetMember("hit_points").ToDouble();

					if (!restricted_monsters.contains(monster_id) && std::isfinite(hit_points) && hit_points > 0)
					{
						ModifyDreadBeastAttackPatterns(false, true, monster);
						StructVariableSet(monster, "__deep_dungeon__dread_beast", true);
						StructVariableSet(monster, "__deep_dungeon__outbreak", true);
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Configured Outbreak: %s", MOD_NAME, MOD_VERSION, MMAPI::Monster::GetInternalName(static_cast<MMAPI::Monster::Ids>(monster_id)).c_str());

						break;
					}
				}
			}

			time_of_last_outbreak_tick = current_time_in_seconds;
		}
	}

	// Auto Regen (Cleric Set Bonus)
	if (!GameIsPaused() && AriCurrentGmRoomIsDungeonFloor() && GetArmorSetBonuses().cleric.AutoRegen())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] == 0)
			class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = current_time_in_seconds;
		else if (current_time_in_seconds - class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] >= THREE_MINUTES_IN_SECONDS)
		{
			int current_health = MMAPI::Player::GetHealth().ToInt64();
			if (current_health > 0)
			{
				int recovery = GetClericAutoRegenPotency();
				MMAPI::Player::ModifyHealth(recovery);
			}
			class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = current_time_in_seconds;
		}
	}
}
