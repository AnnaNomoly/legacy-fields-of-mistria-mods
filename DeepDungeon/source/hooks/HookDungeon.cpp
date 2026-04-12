#include "../utils/Utils.h"
#include "../patterns/MonsterPatterns.h"

RValue& GmlScriptPlayConversationCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && AriCurrentGmRoomIsDungeonFloor() && FLOOR_TEN_CONVERSATION_KEY == Arguments[1]->ToString())
		return Result;

	if (game_is_active && ari_current_gm_room == "rm_mines_entry" && Arguments[1]->ToString() == "Conversations/gameplay_triggered/inspect_seridia_statue")
	{
		if (!script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
			*Arguments[1] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/unavailable";
		else
			*Arguments[1] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange";
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_CONVERSATION));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);
	return Result;
}

RValue& GmlScriptGetMinutesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_MINUTES));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

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
		if (active_floor_enchantments.contains(FloorEnchantments::RESTORATION))
		{
			if (!is_restoration_tracked_interval && (current_time_in_seconds - time_of_last_restoration_tick) >= TWO_MINUTES_IN_SECONDS)
			{
				is_restoration_tracked_interval = true;
				time_of_last_restoration_tick = current_time_in_seconds;
			}
		}

		// Second Wind
		if (active_floor_enchantments.contains(FloorEnchantments::SECOND_WIND))
		{
			if (!is_second_wind_tracked_interval && (current_time_in_seconds - time_of_last_second_wind_tick) >= TWO_MINUTES_IN_SECONDS)
			{
				is_second_wind_tracked_interval = true;
				time_of_last_second_wind_tick = current_time_in_seconds;
			}
		}

		// Fumigate
		if (active_floor_enchantments.contains(FloorEnchantments::FUMIGATE))
		{
			if (!is_fumigate_tracked_interval && (current_time_in_seconds - time_of_last_fumigate_tick) >= TWO_MINUTES_AND_THIRTY_SECONDS)
			{
				is_fumigate_tracked_interval = true;
				time_of_last_fumigate_tick = current_time_in_seconds;
			}
		}

		// Deep Wounds
		if (active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS))
		{
			if (!is_deep_wounds_tracked_interval && (current_time_in_seconds - time_of_last_deep_wounds_tick) >= TWO_MINUTES_IN_SECONDS)
			{
				is_deep_wounds_tracked_interval = true;
				time_of_last_deep_wounds_tick = current_time_in_seconds;
			}
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

RValue& GmlScriptGetLocalizerCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (localize_mod_text)
	{
		localize_mod_text = false;

		floor_enchantments_to_localized_string_map[FloorEnchantments::GLOOM] = LocalizeString(Self, Other, GLOOM_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::HP_PENALTY] = LocalizeString(Self, Other, HP_PENALTY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::EXHAUSTION] = LocalizeString(Self, Other, EXHAUSTION_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::AMNESIA] = LocalizeString(Self, Other, AMNESIA_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::ITEM_PENALTY] = LocalizeString(Self, Other, ITEM_PENALTY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::DISTORTION] = LocalizeString(Self, Other, DISTORTION_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::DAMAGE_DOWN] = LocalizeString(Self, Other, DAMAGE_DOWN_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::GRAVITY] = LocalizeString(Self, Other, GRAVITY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::FEY] = LocalizeString(Self, Other, FEY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::RESTORATION] = LocalizeString(Self, Other, RESTORATION_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::SECOND_WIND] = LocalizeString(Self, Other, SECOND_WIND_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::HASTE] = LocalizeString(Self, Other, HASTE_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::FUMIGATE] = LocalizeString(Self, Other, FUMIGATE_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::FRAILTY] = LocalizeString(Self, Other, FRAILTY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::GRUDGE] = LocalizeString(Self, Other, GRUDGE_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::DEEP_WOUNDS] = LocalizeString(Self, Other, DEEP_WOUNDS_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::BLINK] = LocalizeString(Self, Other, BLINK_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::STONESKIN] = LocalizeString(Self, Other, STONESKIN_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();
		floor_enchantments_to_localized_string_map[FloorEnchantments::PHALANX] = LocalizeString(Self, Other, PHALANX_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY).ToString();

		offerings_to_localized_string_map[Offerings::DREAD] = LocalizeString(Self, Other, DREAD_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::INNER_FIRE] = LocalizeString(Self, Other, INNER_FIRE_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::LEECH] = LocalizeString(Self, Other, LEECH_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::PERIL] = LocalizeString(Self, Other, PERIL_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::RECKONING] = LocalizeString(Self, Other, RECKONING_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::OUTBREAK] = LocalizeString(Self, Other, OUTBREAK_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::SPIRIT_LINK] = LocalizeString(Self, Other, SPIRIT_LINK_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::SPIKES] = LocalizeString(Self, Other, SPIKES_OFFERING_LOCALIZED_TEXT_KEY).ToString();
		offerings_to_localized_string_map[Offerings::REFLECT] = LocalizeString(Self, Other, REFLECT_OFFERING_LOCALIZED_TEXT_KEY).ToString();

		classes_to_localized_armor_description_string_map[Classes::CLERIC] = LocalizeString(Self, Other, CLERIC_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY).ToString();
		classes_to_localized_armor_description_string_map[Classes::DARK_KNIGHT] = LocalizeString(Self, Other, DARK_KNIGHT_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY).ToString();
		classes_to_localized_armor_description_string_map[Classes::MAGE] = LocalizeString(Self, Other, MAGE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY).ToString();
		classes_to_localized_armor_description_string_map[Classes::PALADIN] = LocalizeString(Self, Other, PALADIN_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY).ToString();
		classes_to_localized_armor_description_string_map[Classes::ROGUE] = LocalizeString(Self, Other, ROGUE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY).ToString();
		classes_to_localized_armor_description_string_map[Classes::ORACLE] = LocalizeString(Self, Other, ORACLE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY).ToString();
	}
	else if (game_is_active && AriCurrentGmRoomIsDungeonFloor())
	{
		std::string localization_key = Arguments[0]->ToString();

		// Full Restore
		if (localization_key.contains("spells/full_restore"))
		{
			// Dark Seal (Dark Knight Set Bonus)
			if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			{
				if (localization_key == "spells/full_restore/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Siphon Life/name");
				else if (localization_key == "spells/full_restore/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Siphon Life/description");
				else if (localization_key == "spells/full_restore/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Siphon Life/type");
			}
			// Elemental Seal (Mage Set Bonus)
			else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
			{
				ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);

				if (localization_key == "spells/full_restore/name")
				{
					if (elemental_seal_effect == ElementalSealEffects::FIRE)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enfire/name");
					else if (elemental_seal_effect == ElementalSealEffects::ICE)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enblizzard/name");
					else if (elemental_seal_effect == ElementalSealEffects::VENOM)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enpoison/name");
				}
				else if (localization_key == "spells/full_restore/description")
				{
					if (elemental_seal_effect == ElementalSealEffects::FIRE)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enfire/description");
					else if (elemental_seal_effect == ElementalSealEffects::ICE)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enblizzard/description");
					else if (elemental_seal_effect == ElementalSealEffects::VENOM)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enpoison/description");
				}
				else if (localization_key == "spells/full_restore/type")
				{
					if (elemental_seal_effect == ElementalSealEffects::FIRE)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enfire/type");
					else if (elemental_seal_effect == ElementalSealEffects::ICE)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enblizzard/type");
					else if (elemental_seal_effect == ElementalSealEffects::VENOM)
						*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Enpoison/type");
				}
			}
			// Predict (Oracle Set Bonus)
			else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			{
				if (localization_key == "spells/full_restore/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Predict/name");
				else if (localization_key == "spells/full_restore/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Predict/description");
				else if (localization_key == "spells/full_restore/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Predict/type");
			}
		}
		// Summon Rain
		else if (localization_key.contains("spells/summon_rain"))
		{
			// Flood (Mage Set Bonus)
			if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			{
				if (localization_key == "spells/summon_rain/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Flood/name");
				else if (localization_key == "spells/summon_rain/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Flood/description");
				else if (localization_key == "spells/summon_rain/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Flood/type");
			}
		}
		// Growth
		else if (localization_key.contains("spells/growth"))
		{
			// Quake (Mage Set Bonus)
			if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			{
				if (localization_key == "spells/growth/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Quake/name");
				else if (localization_key == "spells/growth/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Quake/description");
				else if (localization_key == "spells/growth/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Quake/type");
			}
			// Condemn (Oracle Set Bonus)
			else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			{
				if (localization_key == "spells/growth/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Condemn/name");
				else if (localization_key == "spells/growth/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Condemn/description");
				else if (localization_key == "spells/growth/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Condemn/type");
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_LOCALIZER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active)
	{
		if (AriCurrentGmRoomIsDungeonFloor() && Arguments[0]->ToString() == "npcs/seridia/name")
		{
			Result = RValue("Priestess");
			return Result;
		}

		// Orbs
		// TODO: Other orbs when added
		if (crafting_menu_open && (Arguments[0]->ToString() == TIDE_CAVERNS_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY || Arguments[0]->ToString() == DEEP_EARTH_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY || Arguments[0]->ToString() == LAVA_CAVES_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY || Arguments[0]->ToString() == RUINS_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY))
		{
			std::string result_str = Result.ToString();

			size_t index = result_str.find("\n\n");
			if (index == std::string::npos)
				return Result;

			result_str = result_str.substr(0, index);
			Result = RValue(result_str);
			return Result;
		}
		// Health Salve
		else if (Arguments[0]->ToString() == HEALTH_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY)
		{
			std::string result_str = Result.ToString();
			std::string value_str = std::to_string(configuration.health_salve_potency);

			size_t value_placeholder_index = result_str.find(VALUE_PLACEHOLDER_TEXT);
			if (value_placeholder_index != std::string::npos)
				result_str.replace(value_placeholder_index, VALUE_PLACEHOLDER_TEXT.length(), value_str);
			Result = RValue(result_str);
			return Result;
		}
		// Stamina Salve
		else if (Arguments[0]->ToString() == STAMINA_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY)
		{
			std::string result_str = Result.ToString();
			std::string value_str = std::to_string(configuration.stamina_salve_potency);

			size_t value_placeholder_index = result_str.find(VALUE_PLACEHOLDER_TEXT);
			if (value_placeholder_index != std::string::npos)
				result_str.replace(value_placeholder_index, VALUE_PLACEHOLDER_TEXT.length(), value_str);
			Result = RValue(result_str);
			return Result;
		}
		// Mana Salve
		else if (Arguments[0]->ToString() == MANA_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY)
		{
			std::string result_str = Result.ToString();
			std::string value_str = std::to_string(configuration.mana_salve_potency);

			size_t value_placeholder_index = result_str.find(VALUE_PLACEHOLDER_TEXT);
			if (value_placeholder_index != std::string::npos)
				result_str.replace(value_placeholder_index, VALUE_PLACEHOLDER_TEXT.length(), value_str);
			Result = RValue(result_str);
			return Result;
		}
		// Floor Enchantments
		else if (Arguments[0]->ToString() == FLOOR_ENCHANTMENT_PLACEHOLDER_TEXT_KEY)
		{
			std::string custom_text = "";
			bool add_newline = false;

			// Group 1 Enchantments
			for (const auto& enchantment : GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS)
			{
				if (active_floor_enchantments.contains(enchantment))
				{
					add_newline = true;
					custom_text += floor_enchantments_to_localized_string_map[enchantment];
					break;
				}
			}

			// Group 2 Enchantments
			for (const auto& enchantment : GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS)
			{
				if (active_floor_enchantments.contains(enchantment))
				{
					if (add_newline)
						custom_text += "\n";

					add_newline = true;
					custom_text += floor_enchantments_to_localized_string_map[enchantment];
					break;
				}
			}

			// Group 3 Enchantments
			for (const auto& enchantment : GROUP_THREE_FLOOR_ENCHANTMENTS)
			{
				if (active_floor_enchantments.contains(enchantment))
				{
					if (add_newline)
						custom_text += "\n";

					custom_text += floor_enchantments_to_localized_string_map[enchantment];
					break;
				}
			}

			Result = RValue(custom_text);
			return Result;
		}
		// Offerings & Condemn (Oracle Set Bonus)
		else if (Arguments[0]->ToString() == OFFERINGS_PLACEHOLDER_TEXT_KEY || Arguments[0]->ToString() == CONDEMN_PLACEHOLDER_TEXT_KEY)
		{
			std::string custom_text = "";
			for (auto it = queued_offerings.begin(); it != queued_offerings.end();)
			{
				custom_text += offerings_to_localized_string_map[*it];

				if (++it != queued_offerings.end())
					custom_text += "\n";
			}

			Result = RValue(custom_text);
			return Result;
		}
		// Cleric Armor
		else if (Arguments[0]->ToString() == CLERIC_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open)
		{
			int cleric_armor_pieces_equipped = CountEquippedClassArmor()[Classes::CLERIC];

			std::string custom_text = classes_to_localized_armor_description_string_map[Classes::CLERIC];
			custom_text += "\n\n" + LocalizeString(Self, Other, SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY).ToString() + " [" + std::to_string(cleric_armor_pieces_equipped) + "/5]";
			if (cleric_armor_pieces_equipped >= 1 && cleric_armor_pieces_equipped < 3)
				custom_text += "\n- " + LocalizeString(Self, Other, CLERIC_SET_BONUS_AUTO_REGEN_ONE_LOCALIZED_TEXT_KEY).ToString();
			else if (cleric_armor_pieces_equipped >= 3 && cleric_armor_pieces_equipped < 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, CLERIC_SET_BONUS_AUTO_REGEN_TWO_LOCALIZED_TEXT_KEY).ToString() + "";
				custom_text += "\n- " + LocalizeString(Self, Other, CLERIC_SET_BONUS_DIVINE_SEAL_LOCALIZED_TEXT_KEY).ToString() + "";
			}
			else if (cleric_armor_pieces_equipped == 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, CLERIC_SET_BONUS_AUTO_REGEN_THREE_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, CLERIC_SET_BONUS_DIVINE_SEAL_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, CLERIC_SET_BONUS_AFFLATUS_MISERY_LOCALIZED_TEXT_KEY).ToString();
			}

			Result = RValue(custom_text);
			return Result;
		}
		// Dark Knight Armor
		else if (Arguments[0]->ToString() == DARK_KNIGHT_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open)
		{
			int dark_knight_armor_pieces_equipped = CountEquippedClassArmor()[Classes::DARK_KNIGHT];

			std::string custom_text = classes_to_localized_armor_description_string_map[Classes::DARK_KNIGHT];
			custom_text += "\n\n" + LocalizeString(Self, Other, SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY).ToString() + " [" + std::to_string(dark_knight_armor_pieces_equipped) + "/5]";
			if (dark_knight_armor_pieces_equipped >= 1 && dark_knight_armor_pieces_equipped < 3)
				custom_text += "\n- " + LocalizeString(Self, Other, DARK_KNIGHT_SET_BONUS_DRAIN_ONE_LOCALIZED_TEXT_KEY).ToString();
			else if (dark_knight_armor_pieces_equipped >= 3 && dark_knight_armor_pieces_equipped < 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, DARK_KNIGHT_SET_BONUS_DRAIN_TWO_LOCALIZED_TEXT_KEY).ToString() + "";
				custom_text += "\n- " + LocalizeString(Self, Other, DARK_KNIGHT_SET_BONUS_DARK_SEAL_LOCALIZED_TEXT_KEY).ToString() + "";
			}
			else if (dark_knight_armor_pieces_equipped == 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, DARK_KNIGHT_SET_BONUS_DRAIN_THREE_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, DARK_KNIGHT_SET_BONUS_DARK_SEAL_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, DARK_KNIGHT_SET_BONUS_SOUL_EATER_LOCALIZED_TEXT_KEY).ToString();
			}

			Result = RValue(custom_text);
			return Result;
		}
		// Mage Armor
		else if (Arguments[0]->ToString() == MAGE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open)
		{
			int mage_armor_pieces_equipped = CountEquippedClassArmor()[Classes::MAGE];

			std::string custom_text = classes_to_localized_armor_description_string_map[Classes::MAGE];
			custom_text += "\n\n" + LocalizeString(Self, Other, SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY).ToString() + " [" + std::to_string(mage_armor_pieces_equipped) + "/5]";
			if (mage_armor_pieces_equipped >= 1)
				custom_text += "\n- " + LocalizeString(Self, Other, MAGE_SET_BONUS_ASPIR_LOCALIZED_TEXT_KEY).ToString();
			if (mage_armor_pieces_equipped >= 2)
				custom_text += "\n- " + LocalizeString(Self, Other, MAGE_SET_BONUS_FLOOD_LOCALIZED_TEXT_KEY).ToString();
			if (mage_armor_pieces_equipped >= 3)
				custom_text += "\n- " + LocalizeString(Self, Other, MAGE_SET_BONUS_ELEMENTAL_SEAL_LOCALIZED_TEXT_KEY).ToString();
			if (mage_armor_pieces_equipped >= 4)
				custom_text += "\n- " + LocalizeString(Self, Other, MAGE_SET_BONUS_QUAKE_LOCALIZED_TEXT_KEY).ToString();
			if (mage_armor_pieces_equipped == 5)
				custom_text += "\n- " + LocalizeString(Self, Other, MAGE_SET_BONUS_MANA_FONT_LOCALIZED_TEXT_KEY).ToString();

			Result = RValue(custom_text);
			return Result;
		}
		// Paladin Armor
		else if (Arguments[0]->ToString() == PALADIN_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open)
		{
			int paladin_armor_pieces_equipped = CountEquippedClassArmor()[Classes::PALADIN];

			std::string custom_text = classes_to_localized_armor_description_string_map[Classes::PALADIN];
			custom_text += "\n\n" + LocalizeString(Self, Other, SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY).ToString() + " [" + std::to_string(paladin_armor_pieces_equipped) + "/5]";
			if (paladin_armor_pieces_equipped >= 1 && paladin_armor_pieces_equipped < 3)
				custom_text += "\n- " + LocalizeString(Self, Other, PALADIN_SET_BONUS_HOLY_CIRCLE_ONE_LOCALIZED_TEXT_KEY).ToString();
			else if (paladin_armor_pieces_equipped >= 3 && paladin_armor_pieces_equipped < 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, PALADIN_SET_BONUS_HOLY_CIRCLE_TWO_LOCALIZED_TEXT_KEY).ToString() + "";
				custom_text += "\n- " + LocalizeString(Self, Other, PALADIN_SET_BONUS_TEMPERANCE_LOCALIZED_TEXT_KEY).ToString() + "";
			}
			else if (paladin_armor_pieces_equipped == 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, PALADIN_SET_BONUS_HOLY_CIRCLE_THREE_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, PALADIN_SET_BONUS_TEMPERANCE_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, PALADIN_SET_BONUS_HALLOWED_GROUND_LOCALIZED_TEXT_KEY).ToString();
			}

			Result = RValue(custom_text);
			return Result;
		}
		// Rogue Armor
		else if (Arguments[0]->ToString() == ROGUE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open)
		{
			int rogue_armor_pieces_equipped = CountEquippedClassArmor()[Classes::ROGUE];

			std::string custom_text = classes_to_localized_armor_description_string_map[Classes::ROGUE];
			custom_text += "\n\n" + LocalizeString(Self, Other, SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY).ToString() + " [" + std::to_string(rogue_armor_pieces_equipped) + "/5]";
			if (rogue_armor_pieces_equipped >= 1)
				custom_text += "\n- " + LocalizeString(Self, Other, ROGUE_SET_BONUS_FLEE_LOCALIZED_TEXT_KEY).ToString();
			if (rogue_armor_pieces_equipped >= 2)
				custom_text += "\n- " + LocalizeString(Self, Other, ROGUE_SET_BONUS_HIDE_LOCALIZED_TEXT_KEY).ToString();
			if (rogue_armor_pieces_equipped >= 3)
				custom_text += "\n- " + LocalizeString(Self, Other, ROGUE_SET_BONUS_SNEAK_ATTACK_LOCALIZED_TEXT_KEY).ToString();
			if (rogue_armor_pieces_equipped >= 4)
				custom_text += "\n- " + LocalizeString(Self, Other, ROGUE_SET_BONUS_DISARM_TRAP_LOCALIZED_TEXT_KEY).ToString();
			if (rogue_armor_pieces_equipped == 5)
				custom_text += "\n- " + LocalizeString(Self, Other, ROGUE_SET_BONUS_TREASURE_HUNTER_LOCALIZED_TEXT_KEY).ToString();

			Result = RValue(custom_text);
			return Result;
		}
		else if (Arguments[0]->ToString() == ORACLE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open)
		{
			int oracle_armor_pieces_equipped = CountEquippedClassArmor()[Classes::ORACLE];
			std::string custom_text = classes_to_localized_armor_description_string_map[Classes::ORACLE];
			custom_text += "\n\n" + LocalizeString(Self, Other, SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY).ToString() + " [" + std::to_string(oracle_armor_pieces_equipped) + "/5]";
			if (oracle_armor_pieces_equipped == 5)
			{
				custom_text += "\n- " + LocalizeString(Self, Other, ORACLE_SET_BONUS_PREDICT_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, ORACLE_SET_BONUS_CONDEMN_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, ORACLE_SET_BONUS_DIVINATION_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, ORACLE_SET_BONUS_BLESSED_LOCALIZED_TEXT_KEY).ToString();
				custom_text += "\n- " + LocalizeString(Self, Other, ORACLE_SET_BONUS_PROPHECY_LOCALIZED_TEXT_KEY).ToString();
			}

			Result = RValue(custom_text);
			return Result;
		}
	}

	return Result;
}

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!game_is_active)
	{
		game_is_active = true;
		//MarkDungeonTutorialUnseen(); // TODO: Only do this once per save file.
	}

	if (unlock_recipes)
	{
		// TODO: Unlock more recipes as added
		unlock_recipes = false;
		for (std::string armor_name : CLASS_ARMOR_NAMES)
			UnlockRecipe(item_name_to_id_map[armor_name], Self, Other);
		for (std::string orb_name : ORB_NAMES)
			UnlockRecipe(item_name_to_id_map[orb_name], Self, Other);
		for (auto& entry : sigil_to_item_id_map)
			UnlockRecipe(entry.second, Self, Other);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_WEATHER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptTryLocationIdToStringCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && Result.m_Kind == VALUE_STRING)
		ari_current_location = Result.ToString();

	return Result;
}

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
		if (random < configuration.random_dread_beast_spawn_chance && floor_number > 1 && !ari_current_gm_room.contains("seal"))
			active_offerings.insert(Offerings::DREAD);
	}
	show_dashes = active_offerings.contains(Offerings::DREAD) || boss_battle != BossBattle::NONE;
	show_danger_banner = active_offerings.contains(Offerings::DREAD) || boss_battle != BossBattle::NONE;
	if (active_offerings.contains(Offerings::DREAD))
		SelectDreadBeast(Self, Other);
	initial_floor_monsters.clear();

	if (configuration.restrict_perks)
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
	if (configuration.disable_dungeon_lift && (ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_seridias_chamber") && ari_current_gm_room != "rm_void_seal" && !biome_reward_disabled)
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

RValue& GmlScriptGoToRoomCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ResetCustomDrawFields();
	revealed_floor_traps.clear();
	meteor_aoes.clear();
	gaze_aoes.clear();
	void_aoes.clear();

	// Teleport Ari to the ritual chamber for boss battles.
	if (boss_battle == BossBattle::TIDE_CAVERNS_ORB && !ari_current_gm_room.contains("ritual_chamber"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_tide_ritual_chamber" });
	else if (boss_battle == BossBattle::DEEP_EARTH_ORB && !ari_current_gm_room.contains("ritual"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_deep_ritual_chamber" });
	else if (boss_battle == BossBattle::LAVA_CAVES_ORB && !ari_current_gm_room.contains("ritual"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_lava_ritual_chamber" });
	else if (boss_battle == BossBattle::RUINS_ORB && !ari_current_gm_room.contains("ritual"))
		*Arguments[0] = g_ModuleInterface->CallBuiltin("asset_get_index", { "rm_mines_ruins_ritual_chamber" });
	// End Boss Battles when leaving the ritual floor.
	else if (boss_battle != BossBattle::NONE && ari_current_gm_room.contains("ritual"))
		boss_battle = BossBattle::NONE;

	// If leaving the void seal, prohibit the key from spawning in progression mode.
	if (configuration.disable_dungeon_lift && ari_current_gm_room == "rm_void_seal")
		biome_reward_disabled = true;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GO_TO_ROOM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	RValue gm_room = Result.GetMember("gm_room");
	RValue room_name = g_ModuleInterface->CallBuiltin("room_get_name", { gm_room });
	ari_current_gm_room = room_name.ToString();

	if ((ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_priestess_quarters" || ari_current_gm_room == "rm_seridias_chamber") && ari_current_gm_room != "rm_mines_entry")
		SetFloorNumber();
	else
		floor_number = 0;

	// Store the floor number in the global instance for other mods.
	*__YYTK.GetRefMember(MOD_NAME)->GetRefMember("floor") = floor_number;

	ModifyMistpoolWeaponSprites();
	ModifyMistpoolPickaxeSprites();
	ModifyBarkSprites();

	if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
		UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

	// Stop music for dungeon floors to force a new song to play.
	if (configuration.randomize_dungeon_music && floor_number > 0 && script_name_to_reference_map.contains(GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY))
		SceneAudioPlayerStop(script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY][0], script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY][1]);

	// Reset any floor specific set bonus effects.
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

	// Reset Max HP Adjustments
	hp_penalty_amount = -1;
	if (unmodified_base_health != -1)
	{
		SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], unmodified_base_health);
		int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

		VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], unmodified_base_health);
		VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, unmodified_base_health);
		unmodified_base_health = -1;
	}

	// Max HP bug fix (experimental)
	if (configuration.experimental_max_health_bug_fix)
	{
		int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
		if (floor_number == 100)
		{
			if (max_health < 200)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 200);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 200);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 200);
			}
		}
		else if (floor_number > 80)
		{
			if (max_health < 180)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 180);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 180);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 180);
			}
		}
		else if (floor_number > 60)
		{
			if (max_health < 160)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 160);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 160);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 160);
			}
		}
		else if (floor_number > 40)
		{
			if (max_health < 140)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 140);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 140);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 140);
			}
		}
		else if (floor_number > 20)
		{
			if (max_health < 120)
			{
				SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], 120);
				int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

				VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], 120);
				VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, 120);
			}
		}
	}

	if (floor_number != 0 && (treasure_spot.state == TreasureSpot::SPAWNED || treasure_spot.state == TreasureSpot::FOUND))
		treasure_spot = TreasureSpot();

	if (ari_current_location == "dungeon" && (!ari_current_gm_room.contains("rm_mines") || ari_current_gm_room == "rm_mines_entry")) // TODO: Don't use ari_current_location
	{
		// TODO: Run logic to actually undo all active floor enchantments.
		// TOOD: Remove all buffs.
		ResetCustomDrawFields();
		salves_used.clear();
		active_sigils.clear();
		active_greater_sigils.clear();
		active_floor_enchantments.clear();
		active_offerings.clear(); // Different than OnDungeonRoomStart
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
		CancelAllStatusEffects();
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
		sigil_of_alteration_monster_id = 0;
		dread_beast_monster_id = -1;
		dread_beasts_configured = 0;
		boss_monsters_configured = 0;
		class_name_to_set_bonus_effect_value_map.clear();
		initial_floor_monsters.clear();
		treasure_spot = TreasureSpot();

		// Reset Oracle set bonus effects.
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] = 0;
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] = 0;

	}
	else
		active_offerings.clear();

	return Result;
}

RValue& GmlScriptSpawnLadderCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (ari_current_gm_room.contains("seal") || ari_current_gm_room.contains("ritual_chamber"))
		return Result;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SPAWN_LADDER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetUnifiedTimeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_GET_UNIFIED_TIME))
		script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_UNIFIED_TIME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
