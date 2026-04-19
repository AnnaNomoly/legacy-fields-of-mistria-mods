#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::UI;
using namespace State::Maps;

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
		const std::string localization_key = Arguments[0]->ToString();
		auto armor_set_bonuses = GetArmorSetBonuses();

		// Full Restore
		if (localization_key.contains("spells/full_restore"))
		{
			// Dark Seal (Dark Knight Set Bonus)
			if (armor_set_bonuses.dark_knight.DarkSeal())
			{
				if (localization_key == "spells/full_restore/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Siphon Life/name");
				else if (localization_key == "spells/full_restore/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Siphon Life/description");
				else if (localization_key == "spells/full_restore/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Siphon Life/type");
			}
			// Elemental Seal (Mage Set Bonus)
			else if (armor_set_bonuses.mage.ElementalSeal() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
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
			else if (armor_set_bonuses.oracle.FullSet())
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
			if (armor_set_bonuses.mage.Flood())
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
			if (armor_set_bonuses.mage.Quake())
			{
				if (localization_key == "spells/growth/name")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Quake/name");
				else if (localization_key == "spells/growth/description")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Quake/description");
				else if (localization_key == "spells/growth/type")
					*Arguments[0] = RValue("Spells/Mods/Deep Dungeon/Quake/type");
			}
			// Condemn (Oracle Set Bonus)
			else if (armor_set_bonuses.oracle.FullSet())
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
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (game_is_active)
	{
		const std::string localization_key = Arguments[0]->ToString();

		if (AriCurrentGmRoomIsDungeonFloor() && localization_key == "npcs/seridia/name")
		{
			Result = RValue("Priestess");
			return Result;
		}

		// Orbs
		if (crafting_menu_open && (localization_key == TIDE_CAVERNS_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY || localization_key == DEEP_EARTH_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY || localization_key == LAVA_CAVES_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY || localization_key == RUINS_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY))
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
		else if (localization_key == HEALTH_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY)
		{
			std::string result_str = Result.ToString();
			std::string value_str = std::to_string(Config::config.health_salve_potency);

			size_t value_placeholder_index = result_str.find(VALUE_PLACEHOLDER_TEXT);
			if (value_placeholder_index != std::string::npos)
				result_str.replace(value_placeholder_index, VALUE_PLACEHOLDER_TEXT.length(), value_str);
			Result = RValue(result_str);
			return Result;
		}
		// Stamina Salve
		else if (localization_key == STAMINA_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY)
		{
			std::string result_str = Result.ToString();
			std::string value_str = std::to_string(Config::config.stamina_salve_potency);

			size_t value_placeholder_index = result_str.find(VALUE_PLACEHOLDER_TEXT);
			if (value_placeholder_index != std::string::npos)
				result_str.replace(value_placeholder_index, VALUE_PLACEHOLDER_TEXT.length(), value_str);
			Result = RValue(result_str);
			return Result;
		}
		// Mana Salve
		else if (localization_key == MANA_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY)
		{
			std::string result_str = Result.ToString();
			std::string value_str = std::to_string(Config::config.mana_salve_potency);

			size_t value_placeholder_index = result_str.find(VALUE_PLACEHOLDER_TEXT);
			if (value_placeholder_index != std::string::npos)
				result_str.replace(value_placeholder_index, VALUE_PLACEHOLDER_TEXT.length(), value_str);
			Result = RValue(result_str);
			return Result;
		}
		// Floor Enchantments
		else if (localization_key == FLOOR_ENCHANTMENT_PLACEHOLDER_TEXT_KEY)
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
		else if (localization_key == OFFERINGS_PLACEHOLDER_TEXT_KEY || localization_key == CONDEMN_PLACEHOLDER_TEXT_KEY)
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
		else if (localization_key == CLERIC_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open && !store_menu_open)
		{
			int cleric_armor_pieces_equipped = GetArmorSetBonuses().cleric.equipped;

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
		else if (localization_key == DARK_KNIGHT_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open && !store_menu_open)
		{
			int dark_knight_armor_pieces_equipped = GetArmorSetBonuses().dark_knight.equipped;

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
		else if (localization_key == MAGE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open && !store_menu_open)
		{
			int mage_armor_pieces_equipped = GetArmorSetBonuses().mage.equipped;

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
		else if (localization_key == PALADIN_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open && !store_menu_open)
		{
			int paladin_armor_pieces_equipped = GetArmorSetBonuses().paladin.equipped;

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
		else if (localization_key == ROGUE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open && !store_menu_open)
		{
			int rogue_armor_pieces_equipped = GetArmorSetBonuses().rogue.equipped;

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
		// Oracle Armor
		else if (localization_key == ORACLE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY && !crafting_menu_open && !store_menu_open)
		{
			int oracle_armor_pieces_equipped = GetArmorSetBonuses().oracle.equipped;
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
