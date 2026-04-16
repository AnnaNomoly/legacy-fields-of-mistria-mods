#include "../../utils/Utils.h"

RValue& GmlScriptDamageCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());

	// Frailty
	if (active_floor_enchantments.contains(FloorEnchantments::FRAILTY))
	{
		if (!StructVariableExists(*Arguments[0], "__deep_dungeon__frailty_applied")) // Prevents attacks that "persist" from repeatedly getting Frailty applied
		{
			RValue target = Arguments[0]->GetMember("target");
			if (target.ToInt64() == 1) // Ari
			{
				int modifier = frailty_hit_counter * 5;
				double additional_damage = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * modifier / 100);
				*Arguments[0]->GetRefMember("damage") = Arguments[0]->GetMember("damage").ToDouble() + additional_damage;
			}

			StructVariableSet(*Arguments[0], "__deep_dungeon__frailty_applied", true);
		}
	}

	// Grudge
	if (active_floor_enchantments.contains(FloorEnchantments::GRUDGE))
	{
		if (!StructVariableExists(*Arguments[0], "__deep_dungeon__grudge_applied")) // Prevents attacks that "persist" from repeatedly getting Grudge applied
		{
			RValue target = Arguments[0]->GetMember("target");
			if (target.ToInt64() == 1) // Ari
			{
				int modifier = grudge_counter * 10; // Ari takes 10% more damage per grudge stack
				double additional_damage = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * modifier / 100);
				*Arguments[0]->GetRefMember("damage") = Arguments[0]->GetMember("damage").ToDouble() + additional_damage;
			}
			else
			{
				int modifier = grudge_counter * 5; // Ari deals 5% less damage per grudge stack
				double penalty = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * modifier / 100);
				*Arguments[0]->GetRefMember("damage") = Arguments[0]->GetMember("damage").ToDouble() - penalty;
			}

			StructVariableSet(*Arguments[0], "__deep_dungeon__grudge_applied", true);
		}
	}

	// Distortion
	if (active_floor_enchantments.contains(FloorEnchantments::DISTORTION))
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__distortion_applied"))
			{
				std::uniform_int_distribution<size_t> zero_to_two_distribution(0, 2);
				int random = zero_to_two_distribution(random_generator);
				if (random == 0) // 33% chance to miss
				{
					*Arguments[0]->GetRefMember("damage") = 0.0;
					*Arguments[0]->GetRefMember("critical") = false;
					*Arguments[0]->GetRefMember("knockback") = false;
				}
				StructVariableSet(*Arguments[0], "__deep_dungeon__distortion_applied", true);
			}
		}
	}

	// Blink
	if (active_floor_enchantments.contains(FloorEnchantments::BLINK))
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() == 1) // Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__blink_applied"))
			{
				std::uniform_int_distribution<size_t> zero_to_four_distribution(0, 4);
				int random = zero_to_four_distribution(random_generator);
				if (random == 0) // 20% chance to miss
					*Arguments[0]->GetRefMember("damage") = 0.0;

				StructVariableSet(*Arguments[0], "__deep_dungeon__blink_applied", true);
			}
		}
	}

	// Damage Down
	if (active_floor_enchantments.contains(FloorEnchantments::DAMAGE_DOWN))
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__damage_down_applied"))
			{
				double damage = Arguments[0]->GetMember("damage").ToDouble();
				int penalty = std::trunc(damage * 0.30); // 30% reduced damage
				*Arguments[0]->GetRefMember("damage") = damage - penalty;
				StructVariableSet(*Arguments[0], "__deep_dungeon__damage_down_applied", true);
			}
		}
	}

	// Gloom
	if (active_floor_enchantments.contains(FloorEnchantments::GLOOM))
	{
		if (!StructVariableExists(*Arguments[0], "__deep_dungeon__gloom_applied")) // Prevents attacks that "persist" from repeatedly getting Gloom applied
		{
			RValue target = Arguments[0]->GetMember("target");
			if (target.ToInt64() == 1) // Ari
			{
				double damage = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * Config::config.gloom_damage_dealt_modifier); // 50% increased damage
				*Arguments[0]->GetRefMember("damage") = damage;
			}
			else
			{
				double damage = Arguments[0]->GetMember("damage").ToDouble();
				int penalty = std::trunc(damage * Config::config.gloom_damage_received_modifier); // 50% reduced damage
				*Arguments[0]->GetRefMember("damage") = damage - penalty;
			}

			StructVariableSet(*Arguments[0], "__deep_dungeon__gloom_applied", true);
		}
	}

	// Sigil of Fortification
	if (active_sigils.contains(Sigils::FORTIFICATION))
	{
		if (!StructVariableExists(*Arguments[0], "__deep_dungeon__fortification_applied")) // Prevents monster attacks that "persist" from repeatedly getting Fortification applied
		{
			RValue target = Arguments[0]->GetMember("target");
			if (target.ToInt64() == 1) // Ari
			{
				double damage = Arguments[0]->GetMember("damage").ToDouble();
				int penalty = std::trunc(damage * 0.40); // 40% reduced damage
				*Arguments[0]->GetRefMember("damage") = damage - penalty;
			}

			StructVariableSet(*Arguments[0], "__deep_dungeon__fortification_applied", true);
		}
	}

	// Sigil of Strength
	if (active_sigils.contains(Sigils::STRENGTH))
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__strength_applied"))
			{
				double damage = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * 1.3); // 30% increased damage
				*Arguments[0]->GetRefMember("damage") = damage;
				StructVariableSet(*Arguments[0], "__deep_dungeon__strength_applied", true);
			}
		}
	}

	// Afflatus Misery (Cleric Set Bonus)
	bool afflatus_misery_proc = false;
	if (AriCurrentGmRoomIsDungeonFloor() && CountEquippedClassArmor()[Classes::CLERIC] == 5 && !active_sigils.contains(Sigils::RAGE) && global_instance->GetMember("__ari").GetMember("fire_breath_time").ToInt64() == 0)
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__afflatus_misery_applied") && class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY] > 0)
			{
				std::uniform_int_distribution<size_t> zero_to_nine_distribution(0, 9);
				int random = zero_to_nine_distribution(random_generator);
				if (random == 7) // 10% chance to proc
				{
					double damage = Arguments[0]->GetMember("damage").ToDouble() + class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY];
					*Arguments[0]->GetRefMember("damage") = damage;
					*Arguments[0]->GetRefMember("critical") = true;

					afflatus_misery_proc = true;
					StructVariableSet(*Arguments[0], "__deep_dungeon__afflatus_misery_applied", true);
					g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - The Cleric Set Bonus effect \"Afflatus Misery\" triggered, increasing your damage by: %d!", MOD_NAME, VERSION, class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY]);
				}
			}
		}
	}

	// Soul Eater (Dark Knight Set Bonus)
	bool soul_eater_proc = false;
	int soul_eater_amount = 0;
	if (AriCurrentGmRoomIsDungeonFloor() && CountEquippedClassArmor()[Classes::DARK_KNIGHT] == 5 && !active_sigils.contains(Sigils::RAGE) && global_instance->GetMember("__ari").GetMember("fire_breath_time").ToInt64() == 0)
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			double max_health = ari_resource_to_value_map[AriResources::MAX_HEALTH];
			double current_health = ari_resource_to_value_map[AriResources::HEALTH];
			bool hp_half_or_higher = (current_health / max_health) >= 0.5 ? true : false;

			if (hp_half_or_higher && !StructVariableExists(*Arguments[0], "__deep_dungeon__soul_eater_applied"))
			{
				std::uniform_int_distribution<size_t> zero_to_nine_distribution(0, 9);
				int random = zero_to_nine_distribution(random_generator);
				if (random == 7) // 10% chance to proc
				{
					soul_eater_amount = std::trunc(current_health * 0.25);
					double damage = Arguments[0]->GetMember("damage").ToDouble() + soul_eater_amount;
					*Arguments[0]->GetRefMember("damage") = damage;
					*Arguments[0]->GetRefMember("critical") = true;

					soul_eater_proc = true;
					StructVariableSet(*Arguments[0], "__deep_dungeon__soul_eater_applied", true);
					g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - The Dark Knight Set Bonus effect \"Soul Eater\" triggered, increasing your damage by: %d!", MOD_NAME, VERSION, soul_eater_amount);
				}
			}
		}
	}

	// Drain (Dark Knight Set Bonus)
	bool drain_proc = false;
	if (AriCurrentGmRoomIsDungeonFloor() && !soul_eater_proc && CountEquippedClassArmor()[Classes::DARK_KNIGHT] > 0 && global_instance->GetMember("__ari").GetMember("fire_breath_time").ToInt64() == 0)
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__drain_applied"))
			{
				std::uniform_int_distribution<size_t> zero_to_one_distribution(0, 1);
				int random = zero_to_one_distribution(random_generator);
				if (random == 1) // 50% chance to proc
				{
					drain_proc = true;
					StructVariableSet(*Arguments[0], "__deep_dungeon__drain_applied", true);
				}
			}
		}
	}

	// Temperance (Paladin Set Bonus)
	if (AriCurrentGmRoomIsDungeonFloor() && CountEquippedClassArmor()[Classes::PALADIN] >= 3 && global_instance->GetMember("__ari").GetMember("fire_breath_time").ToInt64() == 0)
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			if (!StructVariableExists(*Arguments[0], "__deep_dungeon__temperance_applied"))
			{
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				double damage = Arguments[0]->GetMember("damage").ToDouble();

				damage = ScaleTemperanceDamage(current_health, max_health, damage);
				*Arguments[0]->GetRefMember("damage") = damage;
				StructVariableSet(*Arguments[0], "__deep_dungeon__temperance_applied", true);
			}
		}
	}

	// Sigil of Rage & Sneak Attack (Rogue Set Bonus)
	if (active_sigils.contains(Sigils::RAGE) || (active_sigils.contains(Sigils::CONCEALMENT) && CountEquippedClassArmor()[Classes::ROGUE] >= 3))
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			double damage = Arguments[0]->GetMember("damage").ToDouble();
			if (damage != 0) // Not a miss
			{
				*Arguments[0]->GetRefMember("critical") = true;
				*Arguments[0]->GetRefMember("damage") = 9999.0;
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DAMAGE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Spikes
	if (active_offerings.contains(Offerings::SPIKES) && Result.ToBoolean())
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() != 1) // Everything not Ari
		{
			int damage = Arguments[0]->GetMember("damage").ToInt64();
			int penalty = max(1, damage * 20 / 100);
			ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * penalty);
		}
	}

	// Reflect
	if (active_offerings.contains(Offerings::REFLECT) && Result.ToBoolean())
	{
		RValue target = Arguments[0]->GetMember("target");
		if (target.ToInt64() == 1) // Ari
		{
			std::map<int, CInstance*> distance_to_monster_map = {};
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points > 0)
					{
						RValue monster_x;
						RValue monster_y;
						g_ModuleInterface->GetBuiltin("x", monster, NULL_INDEX, monster_x);
						g_ModuleInterface->GetBuiltin("y", monster, NULL_INDEX, monster_y);

						double distance = GetDistance(ari_x, ari_y, monster_x.ToInt64(), monster_y.ToInt64());
						if (!distance_to_monster_map.contains(distance))
							distance_to_monster_map[distance] = monster;
					}
				}
			}

			if (!distance_to_monster_map.empty())
			{
				int damage = Arguments[0]->GetMember("damage").ToInt64();
				int penalty = max(1, damage * 20 / 100);

				CInstance* closest_monster = distance_to_monster_map.begin()->second;
				int hit_points = closest_monster->GetMember("hit_points").ToInt64();
				*closest_monster->GetRefMember("hit_points") = max(0, hit_points - penalty);
			}
		}
	}

	if (afflatus_misery_proc && Result.ToBoolean())
		class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY] = 0;
	if (drain_proc && Result.ToBoolean())
		class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN]++;
	if (soul_eater_proc && Result.ToBoolean())
		class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] += soul_eater_amount;

	return Result;
}
