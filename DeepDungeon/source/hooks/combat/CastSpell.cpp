#include "../../utils/Utils.h"

RValue& GmlScriptCastSpellCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const bool in_dungeon = AriCurrentGmRoomIsDungeonFloor();
	const int spell_id = Arguments[0]->ToInt64();
	auto armor_set_bonuses = in_dungeon ? GetArmorSetBonuses() : ArmorSetBonuses{};

	if (in_dungeon)
	{
		// Mana Font (Mage, 5 pieces) — counts every spell cast except Flood recasts.
		if (armor_set_bonuses.mage.ManaFont()
			&& (spell_id != spell_name_to_id_map["summon_rain"]
				|| class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] == -1))
		{
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT]++;
		}

		// Set bonuses that fully replace the original spell — skip calling original.
		if (spell_id == spell_name_to_id_map["full_restore"])
		{
			// Dark Seal (Dark Knight, 3+ pieces) — Siphon Life: drain 15% of each monster's base HP.
			if (armor_set_bonuses.dark_knight.DarkSeal())
			{
				for (CInstance* monster : current_floor_monsters)
				{
					if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
					{
						RValue monster_id = monster->GetMember("monster_id");
						double hit_points = monster->GetMember("hit_points").ToDouble();
						if (IsNumeric(monster_id) && monster_id.ToInt64() != monster_name_to_id_map["mimic"] && IsNumeric(hit_points) && std::isfinite(hit_points) && hit_points > 0)
						{
							double default_hit_points = monster->GetMember("__deep_dungeon__default_hit_points").ToDouble();
							double siphon_life_amount = std::trunc(default_hit_points * 0.15);
							hit_points -= siphon_life_amount;

							*monster->GetRefMember("hit_points") = hit_points;
							int max_health = ModifyMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], siphon_life_amount);
							ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], siphon_life_amount);

							int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
							VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], max_health);
							VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, max_health);
						}
					}
				}
				class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] = 1;
				return Result;
			}
			// Elemental Seal (Mage, 3+ pieces) — applies weapon enchantment based on current seal element.
			else if (armor_set_bonuses.mage.ElementalSeal())
			{
				ElementalSealEffects effect = *magic_enum::enum_cast<ElementalSealEffects>(
					class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);

				switch (effect)
				{
				case ElementalSealEffects::FIRE:
					class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] = 1;
					RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["fire_sword"], 1.25, 1, 2147483647.0);
					break;
				case ElementalSealEffects::ICE:
					class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] = 1;
					RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["ice_sword"], 1.0, 1, 2147483647.0);
					break;
				case ElementalSealEffects::VENOM:
					class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] = 1;
					RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["venom_sword"], 1.0, 1, 2147483647.0);
					break;
				}
				return Result;
			}
			// Predict (Oracle, 5 pieces) — cost set to zero by ModifySpellCosts.
			else if (armor_set_bonuses.oracle.FullSet())
			{
				CreateNotification(false, PREDICT_SPELL_CAST_NOTIFICATION_KEY, Self, Other);
				class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 1;
				return Result;
			}
		}
		else if (spell_id == spell_name_to_id_map["growth"])
		{
			// Quake (Mage, 4+ pieces) — deals 90% of max HP to Ari and all monsters.
			if (armor_set_bonuses.mage.Quake())
			{
				int ari_max_health = ari_resource_to_value_map[AriResources::MAX_HEALTH];
				int ari_quake_damage = std::trunc(ari_max_health * 0.9);
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], ari_quake_damage * -1);

				for (CInstance* monster : current_floor_monsters)
				{
					if (StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
					{
						double hit_points = monster->GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points) && hit_points > 0)
						{
							int monster_quake_damage = std::trunc(hit_points * 0.9);
							*monster->GetRefMember("hit_points") = max(0, hit_points - monster_quake_damage);
						}
					}
				}

				class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] = 1;
				return Result;
			}
			// Condemn (Oracle, 5 pieces) — queues a random offering for the next floor.
			else if (armor_set_bonuses.oracle.FullSet())
			{
				static thread_local pcg32 random_generator([] {
					std::random_device rd;
					return pcg32(
						(static_cast<uint64_t>(rd()) << 32) | rd(),
						(static_cast<uint64_t>(rd()) << 32) | rd()
					);
				}());
				std::uniform_int_distribution<size_t> random_offering_distribution(0, magic_enum::enum_count<Offerings>() - 1);

				queued_offerings.insert(magic_enum::enum_value<Offerings>(random_offering_distribution(random_generator)));
				offering_chance_occurred = true;
				class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 1;
				PlayConversation("Conversations/Mods/Deep Dungeon/condemn", Self, Other);
				return Result;
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CAST_SPELL));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (in_dungeon)
	{
		// Divine Seal (Cleric, 3+ pieces) — full_restore clears all floor enchantments.
		if (spell_id == spell_name_to_id_map["full_restore"] && armor_set_bonuses.cleric.DivineSeal())
		{
			if (active_floor_enchantments.contains(FloorEnchantments::FEY))
			{
				std::vector<CInstance*>& refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];
				ModifySpellCosts(true, true);
				CancelStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"]);
			}

			active_floor_enchantments.clear();
			active_sigils.insert(Sigils::SERENITY);
			RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["fairy"], RValue(), 1, 2147483647.0);

			// Undo Blessed (Oracle Set Bonus) HP inflation from this floor.
			if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] > 0)
			{
				int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				int adjusted_max_health = max_health - class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED];

				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);
				class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = 0;
			}

			// Undo HP Penalty floor enchantment.
			if (hp_penalty_amount > -1)
			{
				int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				int adjusted_max_health = max_health + hp_penalty_amount;
				hp_penalty_amount = -1;

				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);
			}
		}

		// Flood (Mage, 2+ pieces) — records cast time to enforce recast cooldown.
		if (spell_id == spell_name_to_id_map["summon_rain"] && armor_set_bonuses.mage.Flood()
			&& class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] <= 0)
		{
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = current_time_in_seconds;
		}
	}

	return Result;
}
