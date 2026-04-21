#pragma once

#include "../constants/ModInfo.h"

// <fstream>, <iomanip>, <filesystem>, <nlohmann/json.hpp> provided by pch.h.

namespace Config
{
	using json = nlohmann::json;

	// ============================================================
	// JSON keys
	// ============================================================
	static const char* const VERSION_JSON_KEY = "__config_version";
	static const char* const DISABLE_DUNGEON_LIFT_JSON_KEY = "disable_dungeon_lift";
	static const char* const RESTRICT_PERKS_JSON_KEY = "restrict_perks";
	static const char* const RESTRICT_ITEMS_JSON_KEY = "restrict_items";
	static const char* const RESTRICT_ARMOR_JSON_KEY = "restrict_armor";
	static const char* const RESTRICT_TOOLS_JSON_KEY = "restrict_tools";
	static const char* const RESTRICT_WEAPONS_JSON_KEY = "restrict_weapons";
	static const char* const HEALTH_SALVE_LIMIT_JSON_KEY = "health_salve_limit";
	static const char* const STAMINA_SALVE_LIMIT_JSON_KEY = "stamina_salve_limit";
	static const char* const MANA_SALVE_LIMIT_JSON_KEY = "mana_salve_limit";
	static const char* const HEALTH_SALVE_POTENCY_JSON_KEY = "health_salve_potency";
	static const char* const STAMINA_SALVE_POTENCY_JSON_KEY = "stamina_salve_potency";
	static const char* const MANA_SALVE_POTENCY_JSON_KEY = "mana_salve_potency";
	static const char* const SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY = "sustaining_potion_duration_modifier";
	static const char* const RANDOMIZE_DUNGEON_MUSIC_JSON_KEY = "randomize_dungeon_music";
	static const char* const RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY = "random_dread_beast_spawn_chance";
	static const char* const OFFERING_EVENT_CHANCE_JSON_KEY = "offering_event_chance";
	static const char* const OFFERING_HEALTH_REQUIREMENT_JSON_KEY = "offering_health_requirement";
	static const char* const OFFERING_STAMINA_REQUIREMENT_JSON_KEY = "offering_stamina_requirement";
	static const char* const OFFERING_MANA_REQUIREMENT_JSON_KEY = "offering_mana_requirement";
	static const char* const CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY = "cursed_armor_drop_chance_modifier";
	static const char* const SOUL_STONE_DROP_CHANCE_JSON_KEY = "soul_stone_drop_chance";
	static const char* const LIFT_KEY_DROP_CHANCE_JSON_KEY = "lift_key_drop_chance";
	static const char* const ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY = "enable_boss_fight_restrictions";
	static const char* const CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY = "confusing_trap_duration_seconds";
	static const char* const DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY = "disorienting_trap_duration_seconds";
	static const char* const EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY = "exploding_trap_current_health_damage_percent";
	static const char* const INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY = "inhibiting_trap_duration_seconds";
	static const char* const LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY = "luring_trap_monster_spawn_count";
	static const char* const GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY = "gaze_trap_max_health_damage_percent";
	static const char* const METEOR_TRAP_SCALING_FACTOR_JSON_KEY = "meteor_trap_scaling_factor";
	static const char* const VOID_TRAP_DURATION_SECONDS_JSON_KEY = "void_trap_duration_seconds";
	static const char* const MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY = "mistpool_equipment_store_price";
	static const char* const SALVES_STORE_PRICE_JSON_KEY = "salves_store_price";
	static const char* const DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY = "dread_beast_damage_modifier";
	static const char* const DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY = "dread_beast_health_modifier";
	static const char* const GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY = "gloom_monster_damage_dealt_modifier";
	static const char* const GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY = "gloom_monster_damage_received_modifier";
	static const char* const GLOOM_HEALTH_MODIFIER_JSON_KEY = "gloom_monster_health_modifier";
	static const char* const EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY = "experimental_max_health_bug_fix";
	static const char* const EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY = "experimental_extra_floor_enchantments_and_offerings";
	static const char* const EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY = "experimental_monster_base_stat_difficulty_modifier";

	// ============================================================
	// Version & defaults
	// ============================================================
	static const int CONFIG_VERSION = 1;

	static const bool DEFAULT_DISABLE_DUNGEON_LIFT = true;
	static const bool DEFAULT_RESTRICT_PERKS = true;
	static const bool DEFAULT_RESTRICT_ITEMS = true;
	static const bool DEFAULT_RESTRICT_ARMOR = true;
	static const bool DEFAULT_RESTRICT_TOOLS = true;
	static const bool DEFAULT_RESTRICT_WEAPONS = true;
	static const int DEFAULT_HEALTH_SALVE_LIMIT = 3;
	static const int DEFAULT_STAMINA_SALVE_LIMIT = 3;
	static const int DEFAULT_MANA_SALVE_LIMIT = 1;
	static const int DEFAULT_HEALTH_SALVE_POTENCY = 35;
	static const int DEFAULT_STAMINA_SALVE_POTENCY = 35;
	static const int DEFAULT_MANA_SALVE_POTENCY = 1;
	static const double DEFAULT_SUSTAINING_POTION_DURATION_MODIFIER = 0.5;
	static const bool DEFAULT_RANDOMIZE_DUNGEON_MUSIC = true;
	static const int DEFAULT_RANDOM_DREAD_BEAST_SPAWN_CHANCE = 5;
	static const int DEFAULT_OFFERING_EVENT_CHANCE = 15;
	static const int DEFAULT_OFFERING_HEALTH_REQUIREMENT = 25;
	static const int DEFAULT_OFFERING_STAMINA_REQUIREMENT = 20;
	static const int DEFAULT_OFFERING_MANA_REQUIREMENT = 1;
	static const double DEFAULT_CURSED_ARMOR_DROP_CHANCE_MODIFIER = 1.0;
	static const int DEFAULT_SOUL_STONE_DROP_CHANCE = 35;
	static const int DEFAULT_LIFT_KEY_DROP_CHANCE = 2;
	static const bool DEFAULT_ENABLE_BOSS_FIGHT_RESTRICTIONS = true;
	static const int DEFAULT_CONFUSING_TRAP_DURATION_SECONDS = 1200;
	static const int DEFAULT_DISORIENTING_TRAP_DURATION_SECONDS = 600;
	static const int DEFAULT_EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT = 50;
	static const int DEFAULT_LURING_TRAP_MONSTER_SPAWN_COUNT = 2;
	static const int DEFAULT_INHIBITING_TRAP_DURATION_SECONDS = 900;
	static const int DEFAULT_GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT = 40;
	static const double DEFAULT_METEOR_TRAP_SCALING_FACTOR = 2.5;
	static const int DEFAULT_VOID_TRAP_DURATION_SECONDS = 1200;
	static const int DEFAULT_MISTPOOL_EQUIPMENT_STORE_PRICE = 500;
	static const int DEFAULT_SALVES_STORE_PRICE = 50;
	static const double DEFAULT_DREAD_BEAST_DAMAGE_MODIFIER = 2;
	static const double DEFAULT_DREAD_BEAST_HEALTH_MODIFIER = 3;
	static const double DEFAULT_GLOOM_DAMAGE_DEALT_MODIFIER = 1.5;
	static const double DEFAULT_GLOOM_DAMAGE_RECEIVED_MODIFIER = 0.5;
	static const double DEFAULT_GLOOM_HEALTH_MODIFIER = 1.5;
	static const bool DEFAULT_EXPERIMENTAL_MAX_HEALTH_BUG_FIX = false;
	static const bool DEFAULT_EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS = false;
	static const double DEFAULT_EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER = 1.0;

	// ============================================================
	// Configuration struct
	// ============================================================
	struct Configuration
	{
		int config_version = CONFIG_VERSION;
		bool disable_dungeon_lift = DEFAULT_DISABLE_DUNGEON_LIFT;
		bool restrict_perks = DEFAULT_RESTRICT_PERKS;
		bool restrict_items = DEFAULT_RESTRICT_ITEMS;
		bool restrict_armor = DEFAULT_RESTRICT_ARMOR;
		bool restrict_tools = DEFAULT_RESTRICT_TOOLS;
		bool restrict_weapons = DEFAULT_RESTRICT_WEAPONS;
		int health_salve_limit = DEFAULT_HEALTH_SALVE_LIMIT;
		int stamina_salve_limit = DEFAULT_STAMINA_SALVE_LIMIT;
		int mana_salve_limit = DEFAULT_MANA_SALVE_LIMIT;
		int health_salve_potency = DEFAULT_HEALTH_SALVE_POTENCY;
		int stamina_salve_potency = DEFAULT_STAMINA_SALVE_POTENCY;
		int mana_salve_potency = DEFAULT_MANA_SALVE_POTENCY;
		double sustaining_potion_duration_modifier = DEFAULT_SUSTAINING_POTION_DURATION_MODIFIER;
		bool randomize_dungeon_music = DEFAULT_RANDOMIZE_DUNGEON_MUSIC;
		int random_dread_beast_spawn_chance = DEFAULT_RANDOM_DREAD_BEAST_SPAWN_CHANCE;
		int offering_event_chance = DEFAULT_OFFERING_EVENT_CHANCE;
		int offering_health_requirement = DEFAULT_OFFERING_HEALTH_REQUIREMENT;
		int offering_stamina_requirement = DEFAULT_OFFERING_STAMINA_REQUIREMENT;
		int offering_mana_requirement = DEFAULT_OFFERING_MANA_REQUIREMENT;
		double cursed_armor_drop_chance_modifier = DEFAULT_CURSED_ARMOR_DROP_CHANCE_MODIFIER;
		int soul_stone_drop_chance = DEFAULT_SOUL_STONE_DROP_CHANCE;
		int lift_key_drop_chance = DEFAULT_LIFT_KEY_DROP_CHANCE;
		bool enable_boss_fight_restrictions = DEFAULT_ENABLE_BOSS_FIGHT_RESTRICTIONS;
		int confusing_trap_duration_seconds = DEFAULT_CONFUSING_TRAP_DURATION_SECONDS;
		int disorienting_trap_duration_seconds = DEFAULT_DISORIENTING_TRAP_DURATION_SECONDS;
		int exploding_trap_current_health_damage_percent = DEFAULT_EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT;
		int inhibiting_trap_duration_seconds = DEFAULT_INHIBITING_TRAP_DURATION_SECONDS;
		int luring_trap_monster_spawn_count = DEFAULT_LURING_TRAP_MONSTER_SPAWN_COUNT;
		int gaze_trap_max_health_damage_percent = DEFAULT_GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT;
		double meteor_trap_scaling_factor = DEFAULT_METEOR_TRAP_SCALING_FACTOR;
		int void_trap_duration_seconds = DEFAULT_VOID_TRAP_DURATION_SECONDS;
		int mistpool_equipment_store_price = DEFAULT_MISTPOOL_EQUIPMENT_STORE_PRICE;
		int salves_store_price = DEFAULT_SALVES_STORE_PRICE;
		double dread_beast_damage_modifier = DEFAULT_DREAD_BEAST_DAMAGE_MODIFIER;
		double dread_beast_health_modifier = DEFAULT_DREAD_BEAST_HEALTH_MODIFIER;
		double gloom_damage_dealt_modifier = DEFAULT_GLOOM_DAMAGE_DEALT_MODIFIER;
		double gloom_damage_received_modifier = DEFAULT_GLOOM_DAMAGE_RECEIVED_MODIFIER;
		double gloom_health_modifier = DEFAULT_GLOOM_HEALTH_MODIFIER;
		bool experimental_max_health_bug_fix = DEFAULT_EXPERIMENTAL_MAX_HEALTH_BUG_FIX;
		bool experimental_extra_floor_enchantments_and_offerings = DEFAULT_EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS;
		double experimental_monster_base_stat_difficulty_modifier = DEFAULT_EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER;
	};

	// ============================================================
	// Global instance & functions
	// ============================================================
	extern Configuration config;

	void PrintError(std::exception_ptr eptr);
	json CreateJson(bool use_defaults);
	void Load();
	void OverrideWithDefaultsForChallengeMode();
	void RestoreAfterChallengeMode();

} // namespace Config
