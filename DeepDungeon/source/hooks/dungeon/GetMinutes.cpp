#include "../../utils/Utils.h"
#include "../../patterns/MonsterPatterns.h"

RValue& GmlScriptGetMinutesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MINUTES));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (game_is_active)
	{
		RValue time = global_instance->GetMember("__clock").GetMember("time");
		current_time_in_seconds = time.ToInt64();

		RevealFloorTraps();
		ApplyFloorTraps(Self, Other);
		ProcessCustomAOEs();
		ProcessTreasureSpot(Self, Other);
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
				const std::unordered_set<int> restricted_monsters = { // TODO: Update as needed with new monsters
					monster_name_to_id_map["barrel"],
					monster_name_to_id_map["copperclod"],
					monster_name_to_id_map["goldclod"],
					monster_name_to_id_map["ironclod"],
					monster_name_to_id_map["mimic"],
					monster_name_to_id_map["mistrilclod"],
					monster_name_to_id_map["rock_stack_lava"],
					monster_name_to_id_map["sapling_orange_mini"],
					monster_name_to_id_map["silverclod"],
					monster_name_to_id_map["rockclod_purple"]
				};

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
							g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Configured Outbreak: %s", MOD_NAME, VERSION, monster_id_to_name_map[monster_id].c_str());

							break;
						}
					}
				}

				time_of_last_outbreak_tick = current_time_in_seconds;
			}
		}

		// Auto Regen (Cleric Set Bonus)
		if (!GameIsPaused() && AriCurrentGmRoomIsDungeonFloor() && CountEquippedClassArmor()[Classes::CLERIC] > 0)
		{
			if (class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] == 0)
				class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = current_time_in_seconds;
			else if (current_time_in_seconds - class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] >= THREE_MINUTES_IN_SECONDS)
			{
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				if (current_health > 0)
				{
					int recovery = GetClericAutoRegenPotency();
					ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], recovery);
				}
				class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AUTO_REGEN] = current_time_in_seconds;
			}
		}
	}

	return Result;
}
