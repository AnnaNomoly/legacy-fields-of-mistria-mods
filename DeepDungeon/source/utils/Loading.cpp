#include "Utils.h"

using namespace State::Maps;

static const std::map<std::string, Sigils> item_name_to_sigil_map = {
	{ SIGIL_OF_ALTERATION_NAME, Sigils::ALTERATION },
	{ SIGIL_OF_CONCEALMENT_NAME, Sigils::CONCEALMENT },
	{ SIGIL_OF_FORTIFICATION_NAME, Sigils::FORTIFICATION },
	{ SIGIL_OF_FORTUNE_NAME, Sigils::FORTUNE },
	{ SIGIL_OF_PROTECTION_NAME, Sigils::PROTECTION },
	{ SIGIL_OF_RAGE_NAME, Sigils::RAGE },
	{ SIGIL_OF_REDEMPTION_NAME, Sigils::REDEMPTION },
	{ SIGIL_OF_SAFETY_NAME, Sigils::SAFETY },
	{ SIGIL_OF_SERENITY_NAME, Sigils::SERENITY },
	{ SIGIL_OF_SILENCE_NAME, Sigils::SILENCE },
	{ SIGIL_OF_STRENGTH_NAME, Sigils::STRENGTH },
	{ SIGIL_OF_TEMPTATION_NAME, Sigils::TEMPTATION },
	{ SIGIL_OF_SIGHT_NAME, Sigils::SIGHT },
	{ SIGIL_OF_INTUITION_NAME, Sigils::INTUITION }
};

static const std::map<std::string, GreaterSigils> item_name_to_greater_sigil_map = {
	{ GREATER_SIGIL_OF_BENEDICTION_NAME, GreaterSigils::BENEDICTION },
	{ GREATER_SIGIL_OF_ASTRAL_FLOW_NAME, GreaterSigils::ASTRAL_FLOW },
	{ GREATER_SIGIL_OF_CHAIN_SPELL_NAME, GreaterSigils::CHAIN_SPELL },
	{ GREATER_SIGIL_OF_SPIRIT_SURGE_NAME, GreaterSigils::SPIRIT_SURGE },
	{ GREATER_SIGIL_OF_MEIKYO_SHISUI_NAME, GreaterSigils::MEIKYO_SHISUI }
};

void ModifyMonsterPrototypes()
{
	size_t array_length;
	RValue monster_prototypes = global_instance->GetMember("__monster_prototypes");
	g_ModuleInterface->GetArraySize(monster_prototypes, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* monster_prototype;
		g_ModuleInterface->GetArrayEntry(monster_prototypes, i, monster_prototype);

		if (!StructVariableExists(*monster_prototype, "monster_id") || !StructVariableExists(*monster_prototype, "hp") || !StructVariableExists(*monster_prototype, "damage"))
			continue;

		int monster_id = monster_prototype->GetMember("monster_id").ToInt64();
		if (monster_id == static_cast<int>(MMAPI::Monster::Ids::Barrel))
			continue;

		if (!monster_id_to_original_hp_map.contains(monster_id))
			monster_id_to_original_hp_map[monster_id] = monster_prototype->GetMember("hp").ToDouble();
		*monster_prototype->GetRefMember("hp") = std::trunc(monster_id_to_original_hp_map[monster_id] * Config::config.experimental_monster_base_stat_difficulty_modifier);

		if (!monster_id_to_original_damage_map.contains(monster_id))
			monster_id_to_original_damage_map[monster_id] = monster_prototype->GetMember("damage").ToDouble();
		*monster_prototype->GetRefMember("damage") = std::trunc(monster_id_to_original_damage_map[monster_id] * Config::config.experimental_monster_base_stat_difficulty_modifier);

		if (StructVariableExists(*monster_prototype, "projectile_damage"))
		{
			if (!monster_id_to_original_projectile_damage_map.contains(monster_id))
				monster_id_to_original_projectile_damage_map[monster_id] = monster_prototype->GetMember("projectile_damage").ToDouble();
			*monster_prototype->GetRefMember("projectile_damage") = std::trunc(monster_id_to_original_projectile_damage_map[monster_id] * Config::config.experimental_monster_base_stat_difficulty_modifier);
		}
	}
}

void LoadDungeonBiomeCandidateMonsters()
{
	RValue biomes = global_instance->GetMember("__fiddle").GetMember("__inner").GetMember("inner").GetMember("dungeons").GetMember("dungeons").GetMember("biomes");
	int max_floors = global_instance->GetMember("__fiddle").GetMember("__inner").GetMember("inner").GetMember("dungeons").GetMember("misc").GetMember("max_floors").ToInt64() + 1;

	size_t biomes_length;
	g_ModuleInterface->GetArraySize(biomes, biomes_length);

	for (size_t i = 0; i < biomes_length; i++)
	{
		RValue* biome;
		g_ModuleInterface->GetArrayEntry(biomes, i, biome);

		int floor = biome->GetMember("floor").ToInt64();
		std::string biome_name = biome->GetMember("name").ToString();
		RValue enemies = biome->GetMember("votes").GetMember("enemy");

		size_t enemies_length;
		g_ModuleInterface->GetArraySize(enemies, enemies_length);

		for (size_t j = 0; j < enemies_length; j++)
		{
			RValue* enemy;
			g_ModuleInterface->GetArrayEntry(enemies, j, enemy);

			std::string enemy_name = enemy->GetMember("object").ToString();

			if (auto monster = MMAPI::Monster::TryFromInternalName(enemy_name))
				dungeon_biome_to_candidate_monsters_map[biome_name].insert(static_cast<int>(*monster));
			else
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to look up enemy name: %s", MOD_NAME, MOD_VERSION, enemy_name.c_str());
		}

		if (i < biomes_length - 1)
		{
			RValue* next_biome;
			g_ModuleInterface->GetArrayEntry(biomes, i + 1, next_biome);

			int next_biome_floor = next_biome->GetMember("floor").ToInt64();
			for (size_t j = floor; j < next_biome_floor; j++)
				floor_number_to_biome_name_map[j] = biome_name;
		}
		else
		{
			for (size_t j = floor; j <= max_floors; j++)
				floor_number_to_biome_name_map[j] = biome_name;
		}
	}
}

void LoadObjectIds()
{
	size_t array_length;
	RValue objects = global_instance->GetMember("__object_id__");
	g_ModuleInterface->GetArraySize(objects, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* object;
		g_ModuleInterface->GetArrayEntry(objects, i, object);

		object_id_to_name_map[i] = object->ToString();
	}
}

void LoadItems()
{
	const std::unordered_set<std::string> lift_key_names = {
		UPPER_MINES_KEY_F5_NAME, UPPER_MINES_KEY_F10_NAME, UPPER_MINES_KEY_F15_NAME,
		TIDE_CAVERNS_KEY_F20_NAME, TIDE_CAVERNS_KEY_F25_NAME, TIDE_CAVERNS_KEY_F30_NAME,TIDE_CAVERNS_KEY_F35_NAME,
		DEEP_EARTH_KEY_F40_NAME, DEEP_EARTH_KEY_F45_NAME, DEEP_EARTH_KEY_F50_NAME, DEEP_EARTH_KEY_F55_NAME,
		LAVA_CAVES_KEY_F60_NAME, LAVA_CAVES_KEY_F65_NAME, LAVA_CAVES_KEY_F70_NAME, LAVA_CAVES_KEY_F75_NAME,
		RUINS_KEY_F80_NAME, RUINS_KEY_F85_NAME, RUINS_KEY_F90_NAME, RUINS_KEY_F95_NAME, RUINS_KEY_F100_NAME
	};
	const std::unordered_set<std::string> orb_item_names = { TIDE_CAVERNS_ORB, DEEP_EARTH_ORB, LAVA_CAVES_ORB, RUINS_ORB };
	const std::unordered_set<std::string> dread_contract_names = { UPPER_MINES_DREAD_CONTRACT, TIDE_CAVERNS_DREAD_CONTRACT, DEEP_EARTH_DREAD_CONTRACT, LAVA_CAVES_DREAD_CONTRACT, RUINS_DREAD_CONTRACT };
	const std::unordered_set<std::string> custom_potion_names = { SUSTAINING_POTION_NAME, HEALTH_SALVE_NAME, STAMINA_SALVE_NAME, MANA_SALVE_NAME };

	size_t array_length;
	RValue item_data = global_instance->GetMember("__item_data");
	g_ModuleInterface->GetArraySize(item_data, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* item;
		g_ModuleInterface->GetArrayEntry(item_data, i, item);

		RValue name_key = item->GetMember("name_key"); // The item's localization key
		if (name_key.m_Kind != VALUE_NULL && name_key.m_Kind != VALUE_UNDEFINED && name_key.m_Kind != VALUE_UNSET)
		{
			int item_id = item->GetMember("item_id").ToInt64();
			std::string item_name = item->GetMember("recipe_key").ToString(); // The internal item name
			item_name_to_id_map[item_name] = item_id;
			item_id_to_name_map[item_id] = item_name;

			// Sigils
			if (item_name_to_sigil_map.contains(item_name))
			{
				deep_dungeon_items.insert(item_id);
				sigil_to_item_id_map[item_name_to_sigil_map.at(item_name)] = item_id;
				item_id_to_sigil_map[item_id] = item_name_to_sigil_map.at(item_name);

				*item->GetRefMember("health_modifier") = 0;
			}

			// Greater Sigils
			if (item_name_to_greater_sigil_map.contains(item_name))
			{
				deep_dungeon_items.insert(item_id);
				greater_sigil_to_item_id_map[item_name_to_greater_sigil_map.at(item_name)] = item_id;
				item_id_to_greater_sigil_map[item_id] = item_name_to_greater_sigil_map.at(item_name);

				*item->GetRefMember("health_modifier") = 0;
			}

			// Lift keys
			if (lift_key_names.contains(item_name))
				lift_key_items.insert(item_id);

			// Orb Items
			if (orb_item_names.contains(item_name))
				orb_items.insert(item_id);

			// Dread Contracts
			if (dread_contract_names.contains(item_name))
			{
				deep_dungeon_items.insert(item_id);
				dread_contract_items.insert(item_id);
			}

			// Salve Items
			if (custom_potion_names.contains(item_name))
			{
				deep_dungeon_items.insert(item_id);
				salve_items.insert(item_id);
			}

			// All consumable items (except Deep Dungeon items)
			if (Config::config.restrict_items && !deep_dungeon_items.contains(item_id))
			{
				if (name_key.ToString().contains("cooked_dishes"))
					restricted_items.insert(item_id);
				else
				{
					RValue edible = item->GetMember("edible");
					if (edible.m_Kind == VALUE_BOOL && edible.m_Real == 1.0)
						restricted_items.insert(item_id);
				}
			}

			// Armor, Weapons, Tools, etc.
			if (StructVariableExists(*item, "tags"))
			{
				RValue tags = item->GetMember("tags");
				RValue buffer = tags.GetMember("__buffer");

				size_t array_length = 0;
				g_ModuleInterface->GetArraySize(buffer, array_length);
				for (size_t i = 0; i < array_length; i++)
				{
					RValue* array_element;
					g_ModuleInterface->GetArrayEntry(buffer, i, array_element);

					if (array_element->ToString() == "armor")
					{
						if (!item_id_to_original_defense_map.contains(item_id))
							item_id_to_original_defense_map[item_id] = item->GetMember("defense").ToDouble();
						*item->GetRefMember("defense") = Config::config.restrict_armor ? 0 : item_id_to_original_defense_map[item_id];
					}

					if (array_element->ToString() == "weapon")
					{
						if (item_name == MISTPOOL_SWORD_NAME)
							deep_dungeon_items.insert(item_id);
						else
						{
							if (!item_id_to_original_damage_map.contains(item_id))
								item_id_to_original_damage_map[item_id] = item->GetMember("damage").ToDouble();
							if (Config::config.restrict_weapons)
							{
								*item->GetRefMember("damage") = 0;
								restricted_items.insert(item_id);
								default_sword_items.insert(item_id);
							}
							else
								*item->GetRefMember("damage") = item_id_to_original_damage_map[item_id];
						}
					}

					if (Config::config.restrict_tools && array_element->ToString() == "pick_axe")
					{
						if (item_name != MISTPOOL_PICK_AXE_NAME)
							restricted_items.insert(item_id);
					}

					if (array_element->ToString() == "bomb")
					{
						if (!item_id_to_original_damage_map.contains(item_id))
							item_id_to_original_damage_map[item_id] = item->GetMember("damage").ToDouble();
						if (!item_id_to_original_bomb_damage_map.contains(item_id))
							item_id_to_original_bomb_damage_map[item_id] = item->GetMember("bomb").GetMember("damage").ToDouble();
						if (Config::config.restrict_items)
						{
							*item->GetRefMember("damage") = 0;
							*item->GetRefMember("bomb")->GetRefMember("damage") = 0;
							restricted_items.insert(item_id);
						}
						else
						{
							*item->GetRefMember("damage") = item_id_to_original_damage_map[item_id];
							*item->GetRefMember("bomb")->GetRefMember("damage") = item_id_to_original_bomb_damage_map[item_id];
						}
					}
				}
			}

			// All snake oils
			if (Config::config.restrict_items && item_name.contains("snake_oil"))
			{
				/*
				   - [string] __infusion__[1] = 'fire_sword'
				   - [string] __infusion__[4] = 'ice_sword'
				   - [string] __infusion__[16] = 'venom_sword'
				*/
				auto default_infusion = static_cast<MMAPI::Infusion::Ids>(item->GetMember("default_infusion").ToInt64());
				if (default_infusion == MMAPI::Infusion::Ids::FireSword || default_infusion == MMAPI::Infusion::Ids::IceSword || default_infusion == MMAPI::Infusion::Ids::VenomSword)
					restricted_items.insert(item_id);
			}
		}
	}
}

void LoadSpells()
{
	size_t array_length = 0;
	RValue spells = global_instance->GetMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		spell_id_to_default_cost_map[i] = array_element->GetMember("cost").ToInt64();
	}
}

void SetItemHealthModifier(int item_id, double health_modifier)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	g_ModuleInterface->CallBuiltin("struct_set", { item, "health_modifier", health_modifier });
}

void SetItemStaminaModifier(int item_id, double stamina_modifier)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	g_ModuleInterface->CallBuiltin("struct_set", { item, "stamina_modifier", stamina_modifier });
}

void SetItemManaModifier(int item_id, double mana_modifier)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	g_ModuleInterface->CallBuiltin("struct_set", { item, "mana_modifier", mana_modifier });
}

void SetItemShopPrice(int item_id, int store_price)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	RValue value = *item.GetRefMember("value");
	StructVariableSet(value, "store", store_price);
}

void ModifyItems()
{
	SetItemHealthModifier(item_name_to_id_map[HEALTH_SALVE_NAME], Config::config.health_salve_potency);
	SetItemStaminaModifier(item_name_to_id_map[STAMINA_SALVE_NAME], Config::config.stamina_salve_potency);
	SetItemManaModifier(item_name_to_id_map[MANA_SALVE_NAME], Config::config.mana_salve_potency);

	SetItemShopPrice(item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], Config::config.mistpool_equipment_store_price);
	SetItemShopPrice(item_name_to_id_map[MISTPOOL_SWORD_NAME], Config::config.mistpool_equipment_store_price);
	for (std::string armor_name : MISTPOOL_ARMOR_NAMES)
		SetItemShopPrice(item_name_to_id_map[armor_name], Config::config.mistpool_equipment_store_price);
	for (int item_id : salve_items)
		SetItemShopPrice(item_id, Config::config.salves_store_price);
}

void RefreshPrototypes()
{
	restricted_items.clear();
	default_sword_items.clear();
	LoadItems();
	ModifyItems();
	ModifyMonsterPrototypes();
}
