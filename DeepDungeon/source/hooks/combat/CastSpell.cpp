#include "../../utils/Utils.h"

RValue& GmlScriptCastSpellCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Mana Font (Mage Set Bonus)
	if (CountEquippedClassArmor()[Classes::MAGE] == 5 && AriCurrentGmRoomIsDungeonFloor() && (Arguments[0]->ToInt64() != spell_name_to_id_map["summon_rain"] || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] == -1))
		class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT]++;

	// Dark Seal (Dark Knight Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3 && AriCurrentGmRoomIsDungeonFloor())
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

	// Elemental Seal (Mage Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::MAGE] >= 3 && AriCurrentGmRoomIsDungeonFloor())
	{
		ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);

		if (elemental_seal_effect == ElementalSealEffects::FIRE)
		{
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] = 1;
			RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["fire_sword"], 1.25, 1, 2147483647.0);
		}
		else if (elemental_seal_effect == ElementalSealEffects::ICE)
		{
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] = 1;
			RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["ice_sword"], 1.0, 1, 2147483647.0);
		}
		else if (elemental_seal_effect == ElementalSealEffects::VENOM)
		{
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] = 1;
			RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["venom_sword"], 1.0, 1, 2147483647.0);
		}

		return Result;
	}

	// Quake (Mage Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::MAGE] >= 4 && AriCurrentGmRoomIsDungeonFloor())
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
					double default_hit_points = monster->GetMember("__deep_dungeon__default_hit_points").ToDouble();
					int monster_quake_damage = std::trunc(hit_points * 0.9);
					*monster->GetRefMember("hit_points") = max(0, hit_points - monster_quake_damage);
				}
			}
		}

		class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] = 1;
		return Result;
	}

	// Predict (Oracle Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5 && AriCurrentGmRoomIsDungeonFloor())
	{
		CreateNotification(false, PREDICT_SPELL_CAST_NOTIFICATION_KEY, Self, Other);
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 1;
		return Result;
	}

	// Condemn (Oracle Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5 && AriCurrentGmRoomIsDungeonFloor())
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> random_offering_distribution(0, magic_enum::enum_count<Offerings>() - 1);

		Offerings offering = magic_enum::enum_value<Offerings>(random_offering_distribution(random_generator));
		queued_offerings.insert(offering);
		offering_chance_occurred = true;

		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 1;
		PlayConversation("Conversations/Mods/Deep Dungeon/condemn", Self, Other);

		return Result;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CAST_SPELL));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Divine Seal (Cleric Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::CLERIC] >= 3 && AriCurrentGmRoomIsDungeonFloor())
	{
		// Undo Fey
		if (active_floor_enchantments.contains(FloorEnchantments::FEY))
		{
			std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

			ModifySpellCosts(true, true);
			CancelStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"]);
		}

		active_floor_enchantments.clear();
		active_sigils.insert(Sigils::SERENITY); // Prevent Serenity on the floor so it isn't wasted.
		RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["fairy"], RValue(), 1, 2147483647.0);

		// Undo Blessed (Oracle Set Bonus)
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

		// Undo HP Penalty
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

	// Flood (Mage Set Bonus)
	if (Arguments[0]->ToInt64() == spell_name_to_id_map["summon_rain"] && CountEquippedClassArmor()[Classes::MAGE] >= 2 && AriCurrentGmRoomIsDungeonFloor() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] <= 0)
		class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = current_time_in_seconds;

	return Result;
}
