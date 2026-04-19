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

void LoadPerks()
{
	size_t array_length;
	RValue perk_names = global_instance->GetMember("__perk__");
	g_ModuleInterface->GetArraySize(perk_names, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* perk_name;
		g_ModuleInterface->GetArrayEntry(perk_names, i, perk_name);

		perk_name_to_id_map[perk_name->ToString()] = i;
	}
}

void LoadTutorials()
{
	size_t array_length;
	RValue tutorials = global_instance->GetMember("__tutorial__");
	g_ModuleInterface->GetArraySize(tutorials, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* tutorial;
		g_ModuleInterface->GetArrayEntry(tutorials, i, tutorial);

		tutorial_name_to_id_map[tutorial->ToString()] = i;
	}
}

void LoadPlayerStates()
{
	size_t array_length;
	RValue player_states = global_instance->GetMember("__player_state__");
	g_ModuleInterface->GetArraySize(player_states, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* player_state;
		g_ModuleInterface->GetArrayEntry(player_states, i, player_state);

		player_state_to_id_map[player_state->ToString()] = i;
	}
}

void LoadMonsterStates()
{
	// NOTE: Using monster category names from: __monster_category__

	// Mushroom States
	size_t shroom_states_length;
	RValue shroom_states = global_instance->GetMember("__mushroom_state__");
	g_ModuleInterface->GetArraySize(shroom_states, shroom_states_length);
	for (size_t i = 0; i < shroom_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(shroom_states, i, state);

		monster_category_to_state_id_map["shroom"][state->ToString()] = i;
	}

	// Rock Clod States
	size_t rock_clod_states_length;
	RValue rock_clod_states = global_instance->GetMember("__rockclod_state__");
	g_ModuleInterface->GetArraySize(rock_clod_states, rock_clod_states_length);
	for (size_t i = 0; i < rock_clod_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(rock_clod_states, i, state);

		monster_category_to_state_id_map["clod"][state->ToString()] = i;
	}

	// Sapling States
	size_t sapling_states_length;
	RValue sapling_states = global_instance->GetMember("__sapling_state__");
	g_ModuleInterface->GetArraySize(sapling_states, sapling_states_length);
	for (size_t i = 0; i < sapling_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(sapling_states, i, state);

		monster_category_to_state_id_map["sap"][state->ToString()] = i;
	}

	// Enchantern States
	size_t enchantern_states_length;
	RValue enchantern_states = global_instance->GetMember("__enchantern_state__");
	g_ModuleInterface->GetArraySize(enchantern_states, enchantern_states_length);
	for (size_t i = 0; i < enchantern_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(enchantern_states, i, state);

		monster_category_to_state_id_map["enchantern"][state->ToString()] = i;
	}

	// Stalagmite States
	size_t mite_states_length;
	RValue mite_states = global_instance->GetMember("__mite_state__");
	g_ModuleInterface->GetArraySize(mite_states, mite_states_length);
	for (size_t i = 0; i < mite_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(mite_states, i, state);

		monster_category_to_state_id_map["mite"][state->ToString()] = i;
	}

	// Bat States
	size_t bat_states_length;
	RValue bat_states = global_instance->GetMember("__bat_state__");
	g_ModuleInterface->GetArraySize(bat_states, bat_states_length);
	for (size_t i = 0; i < bat_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(bat_states, i, state);

		monster_category_to_state_id_map["bat"][state->ToString()] = i;
	}

	// Mimic States
	size_t mimic_states_length;
	RValue mimic_states = global_instance->GetMember("__mimic_state__");
	g_ModuleInterface->GetArraySize(mimic_states, mimic_states_length);
	for (size_t i = 0; i < mimic_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(mimic_states, i, state);

		monster_category_to_state_id_map["mimic"][state->ToString()] = i;
	}

	// Spirit States
	size_t spirit_states_length;
	RValue spirit_states = global_instance->GetMember("__spirit_state__");
	g_ModuleInterface->GetArraySize(spirit_states, spirit_states_length);
	for (size_t i = 0; i < spirit_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(spirit_states, i, state);

		monster_category_to_state_id_map["spirit"][state->ToString()] = i;
	}

	// Cat States
	size_t cat_states_length;
	RValue cat_states = global_instance->GetMember("__cat_state__");
	g_ModuleInterface->GetArraySize(cat_states, cat_states_length);
	for (size_t i = 0; i < cat_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(cat_states, i, state);

		monster_category_to_state_id_map["cat"][state->ToString()] = i;
	}

	// Rock Stack States
	size_t rock_stack_states_length;
	RValue rock_stack_states = global_instance->GetMember("__rock_stack_state__");
	g_ModuleInterface->GetArraySize(rock_stack_states, rock_stack_states_length);
	for (size_t i = 0; i < rock_stack_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(rock_stack_states, i, state);

		monster_category_to_state_id_map["rock_stack"][state->ToString()] = i;
	}

	// Statue States
	size_t statue_states_length;
	RValue statue_states = global_instance->GetMember("__statue_state__");
	g_ModuleInterface->GetArraySize(statue_states, statue_states_length);
	for (size_t i = 0; i < statue_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(statue_states, i, state);

		monster_category_to_state_id_map["statue"][state->ToString()] = i;
	}

	// Tome States
	size_t tome_states_length;
	RValue tome_states = global_instance->GetMember("__tome_state__");
	g_ModuleInterface->GetArraySize(tome_states, tome_states_length);
	for (size_t i = 0; i < tome_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(tome_states, i, state);

		monster_category_to_state_id_map["tome"][state->ToString()] = i;
	}

	// TODO: New monsters as added.
}

void LoadBarkData()
{
	size_t array_length;
	RValue bark_data = global_instance->GetMember("__bark_id__");
	g_ModuleInterface->GetArraySize(bark_data, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* bark_name;
		g_ModuleInterface->GetArrayEntry(bark_data, i, bark_name);

		bark_name_to_id_map[bark_name->ToString()] = i;
	}
}

void LoadStatusEffects()
{
	size_t array_length;
	RValue status_effects = global_instance->GetMember("__status_effect_id__");
	g_ModuleInterface->GetArraySize(status_effects, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* status_effect;
		g_ModuleInterface->GetArrayEntry(status_effects, i, status_effect);

		status_effect_name_to_id_map[status_effect->ToString()] = i;
	}
}

void LoadLocations()
{
	size_t array_length;
	RValue locations = global_instance->GetMember("__location_id__");
	g_ModuleInterface->GetArraySize(locations, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* location;
		g_ModuleInterface->GetArrayEntry(locations, i, location);

		location_name_to_id_map[location->ToString()] = i;
	}
}

void LoadInfusions()
{
	size_t array_length;
	RValue infusions = global_instance->GetMember("__infusion__");
	g_ModuleInterface->GetArraySize(infusions, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* infusion;
		g_ModuleInterface->GetArrayEntry(infusions, i, infusion);

		infusion_name_to_id_map[infusion->ToString()] = i;
	}
}

void LoadMonsters()
{
	size_t array_length;
	RValue monster_names = global_instance->GetMember("__monster_id__");
	g_ModuleInterface->GetArraySize(monster_names, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* monster_name;
		g_ModuleInterface->GetArrayEntry(monster_names, i, monster_name);

		monster_name_to_id_map[monster_name->ToString()] = i;
		monster_id_to_name_map[i] = monster_name->ToString();
	}
}

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
		if (monster_id == monster_name_to_id_map["barrel"])
			continue;

		double hp = monster_prototype->GetMember("hp").ToDouble();
		hp = std::trunc(hp * Config::config.experimental_monster_base_stat_difficulty_modifier);
		*monster_prototype->GetRefMember("hp") = hp;

		double damage = monster_prototype->GetMember("damage").ToDouble();
		damage = std::trunc(damage * Config::config.experimental_monster_base_stat_difficulty_modifier);
		*monster_prototype->GetRefMember("damage") = damage;

		if (StructVariableExists(*monster_prototype, "projectile_damage"))
		{
			double projectile_damage = monster_prototype->GetMember("projectile_damage").ToDouble();
			projectile_damage = std::trunc(projectile_damage * Config::config.experimental_monster_base_stat_difficulty_modifier);
			*monster_prototype->GetRefMember("projectile_damage") = projectile_damage;
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

			if (monster_name_to_id_map.contains(enemy_name))
				dungeon_biome_to_candidate_monsters_map[biome_name].insert(monster_name_to_id_map[enemy_name]);
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
				salve_name_to_id_map[item_name] = item_id;
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

					if (Config::config.restrict_armor && array_element->ToString() == "armor")
						*item->GetRefMember("defense") = 0;

					if (array_element->ToString() == "weapon")
					{
						if (item_name == MISTPOOL_SWORD_NAME)
							deep_dungeon_items.insert(item_id);
						else if (Config::config.restrict_weapons)
						{
							*item->GetRefMember("damage") = 0;
							restricted_items.insert(item_id);
							default_sword_items.insert(item_id);
						}
					}

					if (Config::config.restrict_tools && array_element->ToString() == "pick_axe")
					{
						if (item_name != MISTPOOL_PICK_AXE_NAME)
							restricted_items.insert(item_id);
					}

					if (Config::config.restrict_items && array_element->ToString() == "bomb")
					{
						*item->GetRefMember("damage") = 0;
						*item->GetRefMember("bomb")->GetRefMember("damage") = 0;
						restricted_items.insert(item_id);
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
				int default_infusion = item->GetMember("default_infusion").ToInt64();
				if (default_infusion == infusion_name_to_id_map["fire_sword"] || default_infusion == infusion_name_to_id_map["ice_sword"] || default_infusion == infusion_name_to_id_map["venom_sword"])
					restricted_items.insert(item_id);
			}
		}
	}
}

void LoadSpellIds()
{
	size_t array_length = 0;
	RValue spell_ids = global_instance->GetMember("__spell__");
	g_ModuleInterface->GetArraySize(spell_ids, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spell_ids, i, array_element);

		spell_name_to_id_map[array_element->ToString()] = i;
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
	for (const auto& pair : salve_name_to_id_map)
		SetItemShopPrice(pair.second, Config::config.salves_store_price);
}
