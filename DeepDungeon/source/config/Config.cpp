#include "Config.h"
#include "../Globals.h"

namespace Config
{
	void PrintError(std::exception_ptr eptr)
	{
		try {
			if (eptr) {
				std::rethrow_exception(eptr);
			}
		}
		catch (const std::exception& e) {
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, MOD_VERSION, e.what());
		}
	}

	json CreateJson(bool use_defaults)
	{
		json config_json = {
			{ VERSION_JSON_KEY, use_defaults ? CONFIG_VERSION : config.config_version },
			{ DISABLE_DUNGEON_LIFT_JSON_KEY, use_defaults ? DEFAULT_DISABLE_DUNGEON_LIFT : config.disable_dungeon_lift },
			{ RESTRICT_PERKS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_PERKS : config.restrict_perks },
			{ RESTRICT_ITEMS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_ITEMS : config.restrict_items },
			{ RESTRICT_ARMOR_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_ARMOR : config.restrict_armor },
			{ RESTRICT_TOOLS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_TOOLS : config.restrict_tools },
			{ RESTRICT_WEAPONS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_WEAPONS : config.restrict_weapons },
			{ HEALTH_SALVE_LIMIT_JSON_KEY, use_defaults ? DEFAULT_HEALTH_SALVE_LIMIT : config.health_salve_limit },
			{ STAMINA_SALVE_LIMIT_JSON_KEY, use_defaults ? DEFAULT_STAMINA_SALVE_LIMIT : config.stamina_salve_limit },
			{ MANA_SALVE_LIMIT_JSON_KEY, use_defaults ? DEFAULT_MANA_SALVE_LIMIT : config.mana_salve_limit },
			{ HEALTH_SALVE_POTENCY_JSON_KEY, use_defaults ? DEFAULT_HEALTH_SALVE_POTENCY : config.health_salve_potency },
			{ STAMINA_SALVE_POTENCY_JSON_KEY, use_defaults ? DEFAULT_STAMINA_SALVE_POTENCY : config.stamina_salve_potency },
			{ MANA_SALVE_POTENCY_JSON_KEY, use_defaults ? DEFAULT_MANA_SALVE_POTENCY : config.mana_salve_potency },
			{ SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_SUSTAINING_POTION_DURATION_MODIFIER : config.sustaining_potion_duration_modifier },
			{ RANDOMIZE_DUNGEON_MUSIC_JSON_KEY, use_defaults ? DEFAULT_RANDOMIZE_DUNGEON_MUSIC : config.randomize_dungeon_music },
			{ RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY, use_defaults ? DEFAULT_RANDOM_DREAD_BEAST_SPAWN_CHANCE : config.random_dread_beast_spawn_chance },
			{ OFFERING_EVENT_CHANCE_JSON_KEY, use_defaults ? DEFAULT_OFFERING_EVENT_CHANCE : config.offering_event_chance },
			{ OFFERING_HEALTH_REQUIREMENT_JSON_KEY, use_defaults ? DEFAULT_OFFERING_HEALTH_REQUIREMENT : config.offering_health_requirement },
			{ OFFERING_STAMINA_REQUIREMENT_JSON_KEY, use_defaults ? DEFAULT_OFFERING_STAMINA_REQUIREMENT : config.offering_stamina_requirement },
			{ OFFERING_MANA_REQUIREMENT_JSON_KEY, use_defaults ? DEFAULT_OFFERING_MANA_REQUIREMENT : config.offering_mana_requirement },
			{ CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_CURSED_ARMOR_DROP_CHANCE_MODIFIER : config.cursed_armor_drop_chance_modifier },
			{ SOUL_STONE_DROP_CHANCE_JSON_KEY, use_defaults ? DEFAULT_SOUL_STONE_DROP_CHANCE : config.soul_stone_drop_chance },
			{ LIFT_KEY_DROP_CHANCE_JSON_KEY, use_defaults ? DEFAULT_LIFT_KEY_DROP_CHANCE : config.lift_key_drop_chance },
			{ ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY, use_defaults ? DEFAULT_ENABLE_BOSS_FIGHT_RESTRICTIONS : config.enable_boss_fight_restrictions },
			{ CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_CONFUSING_TRAP_DURATION_SECONDS : config.confusing_trap_duration_seconds },
			{ DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_DISORIENTING_TRAP_DURATION_SECONDS : config.disorienting_trap_duration_seconds },
			{ EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY, use_defaults ? DEFAULT_EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT : config.exploding_trap_current_health_damage_percent },
			{ INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_INHIBITING_TRAP_DURATION_SECONDS : config.inhibiting_trap_duration_seconds },
			{ LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY, use_defaults ? DEFAULT_LURING_TRAP_MONSTER_SPAWN_COUNT : config.luring_trap_monster_spawn_count },
			{ GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY, use_defaults ? DEFAULT_GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT : config.gaze_trap_max_health_damage_percent },
			{ METEOR_TRAP_SCALING_FACTOR_JSON_KEY, use_defaults ? DEFAULT_METEOR_TRAP_SCALING_FACTOR : config.meteor_trap_scaling_factor },
			{ VOID_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_VOID_TRAP_DURATION_SECONDS : config.void_trap_duration_seconds },
			{ MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY, use_defaults ? DEFAULT_MISTPOOL_EQUIPMENT_STORE_PRICE : config.mistpool_equipment_store_price },
			{ SALVES_STORE_PRICE_JSON_KEY, use_defaults ? DEFAULT_SALVES_STORE_PRICE : config.salves_store_price },
			{ DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_DREAD_BEAST_DAMAGE_MODIFIER : config.dread_beast_damage_modifier },
			{ DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_DREAD_BEAST_HEALTH_MODIFIER : config.dread_beast_health_modifier },
			{ GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_GLOOM_DAMAGE_DEALT_MODIFIER : config.gloom_damage_dealt_modifier },
			{ GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_GLOOM_DAMAGE_RECEIVED_MODIFIER : config.gloom_damage_received_modifier },
			{ GLOOM_HEALTH_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_GLOOM_HEALTH_MODIFIER : config.gloom_health_modifier },
			{ EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY, use_defaults ? DEFAULT_EXPERIMENTAL_MAX_HEALTH_BUG_FIX : config.experimental_max_health_bug_fix },
			{ EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY, use_defaults ? DEFAULT_EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS : config.experimental_extra_floor_enchantments_and_offerings },
			{ EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER : config.experimental_monster_base_stat_difficulty_modifier },
			{ EXPERIMENTAL_STOP_TIME_IN_DUNGEON_JSON_KEY, use_defaults ? DEFAULT_EXPERIMENTAL_STOP_TIME_IN_DUNGEON : config.experimental_stop_time_in_dungeon }
		};
		return config_json;
	}

	void Load()
	{
		// Load config file.
		std::exception_ptr eptr;
		try
		{
			// Try to find the mod_data directory.
			std::string current_dir = std::filesystem::current_path().string();
			std::string mod_data_folder = current_dir + "\\mod_data";
			if (!std::filesystem::exists(mod_data_folder))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, MOD_VERSION, mod_data_folder.c_str());
				std::filesystem::create_directory(mod_data_folder);
			}

			// Try to find the mod_data/DeepDungeon directory.
			std::string deep_dungeon_folder = mod_data_folder + "\\DeepDungeon";
			if (!std::filesystem::exists(deep_dungeon_folder))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DeepDungeon\" directory was not found. Creating directory: %s", MOD_NAME, MOD_VERSION, deep_dungeon_folder.c_str());
				std::filesystem::create_directory(deep_dungeon_folder);
			}

			// Try to find the mod_data/DeepDungeon/DeepDungeon.json config file.
			bool update_config_file = false;
			std::string config_file = deep_dungeon_folder + "\\" + "DeepDungeon.json";
			std::ifstream in_stream(config_file);
			if (in_stream.good())
			{
				try
				{
					json json_object = json::parse(in_stream);

					// Check if the json_object is empty.
					if (json_object.empty())
					{
						config.config_version = 0;
						g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, MOD_VERSION, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Writing DEFAULT VALUES to mod configuration file: %s!", MOD_NAME, MOD_VERSION, config_file.c_str());
					}
					else
					{
						// Try loading the config_version value.
						bool missing_version = false;
						if (json_object.contains(VERSION_JSON_KEY) && json_object.at(VERSION_JSON_KEY).is_number_integer())
						{
							int config_version = json_object[VERSION_JSON_KEY];
							if (config_version <= 0 || config_version > CONFIG_VERSION)
							{
								missing_version = true;
								config.config_version = 0;
								g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, VERSION_JSON_KEY, config_file.c_str());
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Writing DEFAULT VALUES to mod configuration file: %s!", MOD_NAME, MOD_VERSION, config_file.c_str());
							}
							else
								config.config_version = config_version;
						}
						else
						{
							missing_version = true;
							config.config_version = 0;
							g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, VERSION_JSON_KEY, config_file.c_str());
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Writing DEFAULT VALUES to mod configuration file: %s!", MOD_NAME, MOD_VERSION, DISABLE_DUNGEON_LIFT_JSON_KEY, config_file.c_str());
						}

						if (!missing_version)
						{
							// Try loading the disable_dungeon_lift value.
							if (json_object.contains(DISABLE_DUNGEON_LIFT_JSON_KEY) && json_object.at(DISABLE_DUNGEON_LIFT_JSON_KEY).is_boolean())
								config.disable_dungeon_lift = json_object[DISABLE_DUNGEON_LIFT_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DISABLE_DUNGEON_LIFT_JSON_KEY, config_file.c_str());

							// Try loading the restrict_perks value.
							if (json_object.contains(RESTRICT_PERKS_JSON_KEY) && json_object.at(RESTRICT_PERKS_JSON_KEY).is_boolean())
								config.restrict_perks = json_object[RESTRICT_PERKS_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RESTRICT_PERKS_JSON_KEY, config_file.c_str());

							// Try loading the restrict_items value.
							if (json_object.contains(RESTRICT_ITEMS_JSON_KEY) && json_object.at(RESTRICT_ITEMS_JSON_KEY).is_boolean())
								config.restrict_items = json_object[RESTRICT_ITEMS_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RESTRICT_ITEMS_JSON_KEY, config_file.c_str());

							// Try loading the restrict_armor value.
							if (json_object.contains(RESTRICT_ARMOR_JSON_KEY) && json_object.at(RESTRICT_ARMOR_JSON_KEY).is_boolean())
								config.restrict_armor = json_object[RESTRICT_ARMOR_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RESTRICT_ARMOR_JSON_KEY, config_file.c_str());

							// Try loading the restrict_tools value.
							if (json_object.contains(RESTRICT_TOOLS_JSON_KEY) && json_object.at(RESTRICT_TOOLS_JSON_KEY).is_boolean())
								config.restrict_tools = json_object[RESTRICT_TOOLS_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RESTRICT_TOOLS_JSON_KEY, config_file.c_str());

							// Try loading the restrict_weapons value.
							if (json_object.contains(RESTRICT_WEAPONS_JSON_KEY) && json_object.at(RESTRICT_WEAPONS_JSON_KEY).is_boolean())
								config.restrict_weapons = json_object[RESTRICT_WEAPONS_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RESTRICT_WEAPONS_JSON_KEY, config_file.c_str());

							// Try loading the health_salve_limit value.
							if (json_object.contains(HEALTH_SALVE_LIMIT_JSON_KEY) && json_object.at(HEALTH_SALVE_LIMIT_JSON_KEY).is_number_integer())
							{
								int health_salve_limit = json_object[HEALTH_SALVE_LIMIT_JSON_KEY];
								if (health_salve_limit > 0 && health_salve_limit <= 999)
									config.health_salve_limit = health_salve_limit;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, HEALTH_SALVE_LIMIT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, HEALTH_SALVE_LIMIT_JSON_KEY, config_file.c_str());

							// Try loading the stamina_salve_limit value.
							if (json_object.contains(STAMINA_SALVE_LIMIT_JSON_KEY) && json_object.at(STAMINA_SALVE_LIMIT_JSON_KEY).is_number_integer())
							{
								int stamina_salve_limit = json_object[STAMINA_SALVE_LIMIT_JSON_KEY];
								if (stamina_salve_limit > 0 && stamina_salve_limit <= 999)
									config.stamina_salve_limit = stamina_salve_limit;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, STAMINA_SALVE_LIMIT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, STAMINA_SALVE_LIMIT_JSON_KEY, config_file.c_str());

							// Try loading the mana_salve_limit value.
							if (json_object.contains(MANA_SALVE_LIMIT_JSON_KEY) && json_object.at(MANA_SALVE_LIMIT_JSON_KEY).is_number_integer())
							{
								int mana_salve_limit = json_object[MANA_SALVE_LIMIT_JSON_KEY];
								if (mana_salve_limit > 0 && mana_salve_limit <= 999)
									config.mana_salve_limit = mana_salve_limit;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, MANA_SALVE_LIMIT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, MANA_SALVE_LIMIT_JSON_KEY, config_file.c_str());

							// Try loading the health_salve_potency value.
							if (json_object.contains(HEALTH_SALVE_POTENCY_JSON_KEY) && json_object.at(HEALTH_SALVE_POTENCY_JSON_KEY).is_number_integer())
							{
								int health_salve_potency = json_object[HEALTH_SALVE_POTENCY_JSON_KEY];
								if (health_salve_potency > 0 && health_salve_potency <= 999)
									config.health_salve_potency = health_salve_potency;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, HEALTH_SALVE_POTENCY_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, HEALTH_SALVE_POTENCY_JSON_KEY, config_file.c_str());

							// Try loading the stamina_salve_potency value.
							if (json_object.contains(STAMINA_SALVE_POTENCY_JSON_KEY) && json_object.at(STAMINA_SALVE_POTENCY_JSON_KEY).is_number_integer())
							{
								int stamina_salve_potency = json_object[STAMINA_SALVE_POTENCY_JSON_KEY];
								if (stamina_salve_potency > 0 && stamina_salve_potency <= 999)
									config.stamina_salve_potency = stamina_salve_potency;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, STAMINA_SALVE_POTENCY_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, STAMINA_SALVE_POTENCY_JSON_KEY, config_file.c_str());

							// Try loading the mana_salve_potency value.
							if (json_object.contains(MANA_SALVE_POTENCY_JSON_KEY) && json_object.at(MANA_SALVE_POTENCY_JSON_KEY).is_number_integer())
							{
								int mana_salve_potency = json_object[MANA_SALVE_POTENCY_JSON_KEY];
								if (mana_salve_potency > 0 && mana_salve_potency <= 999)
									config.mana_salve_potency = mana_salve_potency;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, MANA_SALVE_POTENCY_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, MANA_SALVE_POTENCY_JSON_KEY, config_file.c_str());

							// Try loading the sustaining_potion_duration_modifier value.
							if (json_object.contains(SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY) && json_object.at(SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY).is_number())
							{
								double sustaining_potion_duration_modifier = json_object[SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY];
								if (sustaining_potion_duration_modifier >= 0.5 && sustaining_potion_duration_modifier <= 2.0)
									config.sustaining_potion_duration_modifier = sustaining_potion_duration_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the randomize_dungeon_music value.
							if (json_object.contains(RANDOMIZE_DUNGEON_MUSIC_JSON_KEY) && json_object.at(RANDOMIZE_DUNGEON_MUSIC_JSON_KEY).is_boolean())
								config.randomize_dungeon_music = json_object[RANDOMIZE_DUNGEON_MUSIC_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RANDOMIZE_DUNGEON_MUSIC_JSON_KEY, config_file.c_str());

							// Try loading the random_dread_beast_spawn_chance value.
							if (json_object.contains(RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY) && json_object.at(RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY).is_number_integer())
							{
								int random_dread_beast_spawn_chance = json_object[RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY];
								if (random_dread_beast_spawn_chance >= 0 && random_dread_beast_spawn_chance <= 100)
									config.random_dread_beast_spawn_chance = random_dread_beast_spawn_chance;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY, config_file.c_str());

							// Try loading the offering_event_chance value.
							if (json_object.contains(OFFERING_EVENT_CHANCE_JSON_KEY) && json_object.at(OFFERING_EVENT_CHANCE_JSON_KEY).is_number_integer())
							{
								int offering_event_chance = json_object[OFFERING_EVENT_CHANCE_JSON_KEY];
								if (offering_event_chance >= 0 && offering_event_chance <= 100)
									config.offering_event_chance = offering_event_chance;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_EVENT_CHANCE_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_EVENT_CHANCE_JSON_KEY, config_file.c_str());

							// Try loading the offering_health_requirement value.
							if (json_object.contains(OFFERING_HEALTH_REQUIREMENT_JSON_KEY) && json_object.at(OFFERING_HEALTH_REQUIREMENT_JSON_KEY).is_number_integer())
							{
								int offering_health_requirement = json_object[OFFERING_HEALTH_REQUIREMENT_JSON_KEY];
								if (offering_health_requirement >= 0 && offering_health_requirement <= 90)
									config.offering_health_requirement = offering_health_requirement;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_HEALTH_REQUIREMENT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_HEALTH_REQUIREMENT_JSON_KEY, config_file.c_str());

							// Try loading the offering_stamina_requirement value.
							if (json_object.contains(OFFERING_STAMINA_REQUIREMENT_JSON_KEY) && json_object.at(OFFERING_STAMINA_REQUIREMENT_JSON_KEY).is_number_integer())
							{
								int offering_stamina_requirement = json_object[OFFERING_STAMINA_REQUIREMENT_JSON_KEY];
								if (offering_stamina_requirement >= 0 && offering_stamina_requirement <= 90)
									config.offering_stamina_requirement = offering_stamina_requirement;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_STAMINA_REQUIREMENT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_STAMINA_REQUIREMENT_JSON_KEY, config_file.c_str());

							// Try loading the offering_mana_requirement value.
							if (json_object.contains(OFFERING_MANA_REQUIREMENT_JSON_KEY) && json_object.at(OFFERING_MANA_REQUIREMENT_JSON_KEY).is_number_integer())
							{
								int offering_mana_requirement = json_object[OFFERING_MANA_REQUIREMENT_JSON_KEY];
								if (offering_mana_requirement >= 0 && offering_mana_requirement <= 16)
									config.offering_mana_requirement = offering_mana_requirement;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_MANA_REQUIREMENT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, OFFERING_MANA_REQUIREMENT_JSON_KEY, config_file.c_str());

							// Try loading the cursed_armor_drop_chance_modifier value.
							if (json_object.contains(CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY) && json_object.at(CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY).is_number())
							{
								double cursed_armor_drop_chance_modifier = json_object[CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY];
								if (cursed_armor_drop_chance_modifier >= 1.0 && cursed_armor_drop_chance_modifier <= 10.0)
									config.cursed_armor_drop_chance_modifier = cursed_armor_drop_chance_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the soul_stone_drop_chance value.
							if (json_object.contains(SOUL_STONE_DROP_CHANCE_JSON_KEY) && json_object.at(SOUL_STONE_DROP_CHANCE_JSON_KEY).is_number_integer())
							{
								int soul_stone_drop_chance = json_object[SOUL_STONE_DROP_CHANCE_JSON_KEY];
								if (soul_stone_drop_chance >= 35 && soul_stone_drop_chance <= 100)
									config.soul_stone_drop_chance = soul_stone_drop_chance;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, SOUL_STONE_DROP_CHANCE_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, SOUL_STONE_DROP_CHANCE_JSON_KEY, config_file.c_str());

							// Try loading the lift_key_drop_chance value.
							if (json_object.contains(LIFT_KEY_DROP_CHANCE_JSON_KEY) && json_object.at(LIFT_KEY_DROP_CHANCE_JSON_KEY).is_number_integer())
							{
								int lift_key_drop_chance = json_object[LIFT_KEY_DROP_CHANCE_JSON_KEY];
								if (lift_key_drop_chance >= 2 && lift_key_drop_chance <= 100)
									config.lift_key_drop_chance = lift_key_drop_chance;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, LIFT_KEY_DROP_CHANCE_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, LIFT_KEY_DROP_CHANCE_JSON_KEY, config_file.c_str());

							// Try loading the enable_boss_fight_restrictions value.
							if (json_object.contains(ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY) && json_object.at(ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY).is_boolean())
								config.enable_boss_fight_restrictions = json_object[ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY, config_file.c_str());

							// Try loading confusing_trap_duration_seconds
							if (json_object.contains(CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
							{
								int confusing_trap_duration_seconds = json_object[CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY];
								if (confusing_trap_duration_seconds >= 0 && confusing_trap_duration_seconds <= 1200)
									config.confusing_trap_duration_seconds = confusing_trap_duration_seconds;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

							// Try loading disorienting_trap_duration_seconds
							if (json_object.contains(DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
							{
								int disorienting_trap_duration_seconds = json_object[DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY];
								if (disorienting_trap_duration_seconds >= 0 && disorienting_trap_duration_seconds <= 600)
									config.disorienting_trap_duration_seconds = disorienting_trap_duration_seconds;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

							// Try loading exploding_trap_current_health_damage_percent
							if (json_object.contains(EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY) && json_object.at(EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY).is_number_integer())
							{
								int exploding_trap_current_health_damage_percent = json_object[EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY];
								if (exploding_trap_current_health_damage_percent >= 0 && exploding_trap_current_health_damage_percent <= 80)
									config.exploding_trap_current_health_damage_percent = exploding_trap_current_health_damage_percent;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());

							// Try loading luring_trap_monster_spawn_count
							if (json_object.contains(LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY) && json_object.at(LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY).is_number_integer())
							{
								int luring_trap_monster_spawn_count = json_object[LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY];
								if (luring_trap_monster_spawn_count >= 0 && luring_trap_monster_spawn_count <= 2)
									config.luring_trap_monster_spawn_count = luring_trap_monster_spawn_count;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY, config_file.c_str());

							// Try loading inhibiting_trap_duration_seconds
							if (json_object.contains(INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
							{
								int inhibiting_trap_duration_seconds = json_object[INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY];
								if (inhibiting_trap_duration_seconds >= 0 && inhibiting_trap_duration_seconds <= 900)
									config.inhibiting_trap_duration_seconds = inhibiting_trap_duration_seconds;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

							// Try loading gaze_trap_max_health_damage_percent
							if (json_object.contains(GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY) && json_object.at(GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY).is_number_integer())
							{
								int gaze_trap_max_health_damage_percent = json_object[GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY];
								if (gaze_trap_max_health_damage_percent >= 0 && gaze_trap_max_health_damage_percent <= 99)
									config.gaze_trap_max_health_damage_percent = gaze_trap_max_health_damage_percent;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());

							// Try loading meteor_trap_scaling_factor
							if (json_object.contains(METEOR_TRAP_SCALING_FACTOR_JSON_KEY) && json_object.at(METEOR_TRAP_SCALING_FACTOR_JSON_KEY).is_number())
							{
								double meteor_trap_scaling_factor = json_object[METEOR_TRAP_SCALING_FACTOR_JSON_KEY];
								if (meteor_trap_scaling_factor == 0 || meteor_trap_scaling_factor == 1 || meteor_trap_scaling_factor == 1.5 || meteor_trap_scaling_factor == 2 || meteor_trap_scaling_factor == 2.5 || meteor_trap_scaling_factor == 3)
									config.meteor_trap_scaling_factor = meteor_trap_scaling_factor;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, METEOR_TRAP_SCALING_FACTOR_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, METEOR_TRAP_SCALING_FACTOR_JSON_KEY, config_file.c_str());

							// Try loading void_trap_duration_seconds
							if (json_object.contains(VOID_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(VOID_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
							{
								int void_trap_duration_seconds = json_object[VOID_TRAP_DURATION_SECONDS_JSON_KEY];
								if (void_trap_duration_seconds >= 0 && void_trap_duration_seconds <= 1800)
									config.void_trap_duration_seconds = void_trap_duration_seconds;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, VOID_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, VOID_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

							// Try loading mistpool_equipment_store_price
							if (json_object.contains(MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY) && json_object.at(MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY).is_number_integer())
							{
								int mistpool_equipment_store_price = json_object[MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY];
								if (mistpool_equipment_store_price >= 1 && mistpool_equipment_store_price <= 500)
									config.mistpool_equipment_store_price = mistpool_equipment_store_price;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY, config_file.c_str());

							// Try loading salves_store_price
							if (json_object.contains(SALVES_STORE_PRICE_JSON_KEY) && json_object.at(SALVES_STORE_PRICE_JSON_KEY).is_number_integer())
							{
								int salves_store_price = json_object[SALVES_STORE_PRICE_JSON_KEY];
								if (salves_store_price >= 1 && salves_store_price <= 50)
									config.salves_store_price = salves_store_price;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, SALVES_STORE_PRICE_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, SALVES_STORE_PRICE_JSON_KEY, config_file.c_str());

							// Try loading the dread_beast_damage_modifier value.
							if (json_object.contains(DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY) && json_object.at(DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY).is_number())
							{
								double dread_beast_damage_modifier = json_object[DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY];
								if (dread_beast_damage_modifier >= 1.0 && dread_beast_damage_modifier <= 2.0)
									config.dread_beast_damage_modifier = dread_beast_damage_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the dread_beast_health_modifier value.
							if (json_object.contains(DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY) && json_object.at(DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY).is_number())
							{
								double dread_beast_health_modifier = json_object[DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY];
								if (dread_beast_health_modifier >= 1.0 && dread_beast_health_modifier <= 3.0)
									config.dread_beast_health_modifier = dread_beast_health_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the gloom_damage_dealt_modifier value.
							if (json_object.contains(GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY) && json_object.at(GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY).is_number())
							{
								double gloom_damage_dealt_modifier = json_object[GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY];
								if (gloom_damage_dealt_modifier >= 1.0 && gloom_damage_dealt_modifier <= 1.5)
									config.gloom_damage_dealt_modifier = gloom_damage_dealt_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the gloom_damage_received_modifier value.
							if (json_object.contains(GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY) && json_object.at(GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY).is_number())
							{
								double gloom_damage_received_modifier = json_object[GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY];
								if (gloom_damage_received_modifier >= 0.5 && gloom_damage_received_modifier <= 1.0)
									config.gloom_damage_received_modifier = gloom_damage_received_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the gloom_health_modifier value.
							if (json_object.contains(GLOOM_HEALTH_MODIFIER_JSON_KEY) && json_object.at(GLOOM_HEALTH_MODIFIER_JSON_KEY).is_number())
							{
								double gloom_health_modifier = json_object[GLOOM_HEALTH_MODIFIER_JSON_KEY];
								if (gloom_health_modifier >= 1.0 && gloom_health_modifier <= 1.5)
									config.gloom_health_modifier = gloom_health_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GLOOM_HEALTH_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, GLOOM_HEALTH_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the experimental_max_health_bug_fix value.
							if (json_object.contains(EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY) && json_object.at(EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY).is_boolean())
								config.experimental_max_health_bug_fix = json_object[EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY, config_file.c_str());

							// Try loading the experimental_extra_floor_enchantments_and_offerings value.
							if (json_object.contains(EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY) && json_object.at(EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY).is_boolean())
								config.experimental_extra_floor_enchantments_and_offerings = json_object[EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY, config_file.c_str());

							// Try loading the experimental_monster_base_stat_difficulty_modifier value.
							if (json_object.contains(EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY) && json_object.at(EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY).is_number())
							{
								double experimental_monster_base_stat_difficulty_modifier = json_object[EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY];
								if (experimental_monster_base_stat_difficulty_modifier >= 1.0 && experimental_monster_base_stat_difficulty_modifier <= 3.0)
									config.experimental_monster_base_stat_difficulty_modifier = experimental_monster_base_stat_difficulty_modifier;
								else
									g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY, config_file.c_str());
							}
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY, config_file.c_str());

							// Try loading the experimental_stop_time_in_dungeon value.
							if (json_object.contains(EXPERIMENTAL_STOP_TIME_IN_DUNGEON_JSON_KEY) && json_object.at(EXPERIMENTAL_STOP_TIME_IN_DUNGEON_JSON_KEY).is_boolean())
								config.experimental_stop_time_in_dungeon = json_object[EXPERIMENTAL_STOP_TIME_IN_DUNGEON_JSON_KEY];
							else
								g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, MOD_VERSION, EXPERIMENTAL_STOP_TIME_IN_DUNGEON_JSON_KEY, config_file.c_str());
						}
					}

					update_config_file = true;
				}
				catch (...)
				{
					eptr = std::current_exception();
					PrintError(eptr);

					g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, MOD_VERSION, config_file.c_str());
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, MOD_VERSION);
				}

				in_stream.close();
			}
			else
			{
				in_stream.close();

				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DeepDungeon.json\" file was not found. Creating file: %s", MOD_NAME, MOD_VERSION, config_file.c_str());

				json default_config_json = CreateJson(true);
				std::ofstream out_stream(config_file);
				out_stream << std::setw(4) << default_config_json << std::endl;
				out_stream.close();
			}

			if (update_config_file)
			{
				bool use_defaults = false;
				if (config.config_version == 0)
					use_defaults = true;

				json config_json = CreateJson(use_defaults);
				std::ofstream out_stream(config_file);
				out_stream << std::setw(4) << config_json << std::endl;
				out_stream.close();
			}
		}
		catch (...)
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, MOD_VERSION);

			eptr = std::current_exception();
			PrintError(eptr);
		}
	}

	void OverrideWithDefaultsForChallengeMode()
	{
		State::Player::saved_config = config;
		config = Configuration{};
	}

	void RestoreAfterChallengeMode()
	{
		if (State::Player::saved_config.has_value())
		{
			config = *State::Player::saved_config;
			State::Player::saved_config = std::nullopt;
		}
	}

} // namespace Config
