#include "../utils/Utils.h"

RValue& GmlScriptSpawnMonsterCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!active_traps.contains(Traps::LURING))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

		// Sigil of Silence
		if (active_sigils.contains(Sigils::SILENCE))
		{
			int chance_to_activate = zero_to_ninety_nine_distribution(random_generator);
			int activation_threshold = 100;
			for (int i = 0; i < sigil_of_silence_count; i++)
			{
				if(ari_current_gm_room == "rm_mines_ruins_arena3")
					activation_threshold /= 5;
				else
					activation_threshold /= 3;
			}

			bool activate = false;
			if (activation_threshold == 100)
				activate = true;
			else if (chance_to_activate < activation_threshold)
				activate = true;

			sigil_of_silence_count++;

			if (activate)
				return Result;
		}

		// Sigil of Alteration
		if (active_sigils.contains(Sigils::ALTERATION))
		{
			if (sigil_of_alteration_monster_id != -1)
				*Arguments[2] = sigil_of_alteration_monster_id;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SPAWN_MONSTER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), Arguments[2]->ToInt64()) == initial_floor_monsters.end())
		initial_floor_monsters.push_back(Arguments[2]->ToInt64());

	return Result;
}

RValue& GmlScriptCanCastSpellCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CAN_CAST_SPELL));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Amnesia
	if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA))
		Result = 0;
	// Boss Fights
	else if (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
		Result = 0;
	// Dark Seal (Dark Knight Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0)
			Result = 0;
		else if (active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]] / 2))
			Result = 0;
		else if (!active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]]))
			Result = 0;
		else
			Result = 1;
	}
	// Flood (Mage Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["summon_rain"] && CountEquippedClassArmor()[Classes::MAGE] >= 2 && AriCurrentGmRoomIsDungeonFloor() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0)
		Result = 0;
	// Elemental Seal (Mage Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::MAGE] >= 3 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] > 0 || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] > 0 || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] > 0)
			Result = 0;
		else if (active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]] / 2))
			Result = 0;
		else if (!active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["full_restore"]]))
			Result = 0;
		else
			Result = 1;
	}
	// Quake (Mage Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::MAGE] >= 4 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0)
			Result = 0;
		else if (active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["growth"]] / 2))
			Result = 0;
		else if (!active_floor_enchantments.contains(FloorEnchantments::FEY) && ari_resource_to_value_map[AriResources::MANA] < (spell_id_to_default_cost_map[spell_name_to_id_map["growth"]]))
			Result = 0;
		else
			Result = 1;
	}
	// Predict (Oracle Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] > 0)
			Result = 0;
		else
			Result = 1;
	}
	// Condemn (Oracle Set Bonus)
	else if (Arguments[0]->ToInt64() == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5 && AriCurrentGmRoomIsDungeonFloor())
	{
		if (offering_chance_occurred || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0)
			Result = 0;
		else
			Result = 1;
	}

	return Result;
}

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

RValue& GmlScriptGetMoveSpeedCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MOVE_SPEED));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Gravity
	if (active_floor_enchantments.contains(FloorEnchantments::GRAVITY))
		Result = 1.25;

	// Haste
	if (active_floor_enchantments.contains(FloorEnchantments::HASTE))
		Result = 3.0;

	// Spirit Surge
	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE))
		Result = 4.0;

	return Result; // 2.0 is default run speed
}

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
				double damage = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * configuration.gloom_damage_dealt_modifier); // 50% increased damage
				*Arguments[0]->GetRefMember("damage") = damage;
			}
			else
			{
				double damage = Arguments[0]->GetMember("damage").ToDouble();
				int penalty = std::trunc(damage * configuration.gloom_damage_received_modifier); // 50% reduced damage
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
