#pragma once

// All heavy standard library and third-party headers are in pch.h (precompiled header).
// pch.h is injected first into every TU via ForcedIncludeFiles in the project settings.
#include "config/Config.h"
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;


// C++ 17 Boost-style hash combine
struct pair_hash {
	std::size_t operator()(const std::pair<int, int>& p) const noexcept {
		std::size_t seed = 0;

		auto h1 = std::hash<int>{}(p.first);
		seed ^= h1 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

		auto h2 = std::hash<int>{}(p.second);
		seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

		return seed;
	}
};


// GML script names, item names, notification/conversation keys, timing constants,
// floor enchantment groups, trap spawn points, and function declarations are in
// Utils.h (via constants/ and data/ headers). Include Utils.h in .cpp files, not Globals.h directly.

enum class BossBattle {
	NONE,
	CLEARED,
	TIDE_CAVERNS_ORB,
	DEEP_EARTH_ORB,
	LAVA_CAVES_ORB,
	RUINS_ORB
};

enum class Classes {
	CLERIC,
	MAGE,
	PALADIN,
	DARK_KNIGHT,
	ROGUE,
	ORACLE
};

enum class ManagedSetBonuses { // Set bonuses that have actively managed values.
	AUTO_REGEN, // Cleric
	AFFLATUS_MISERY, // Cleric
	DARK_SEAL, // Dark Knight
	DRAIN, // Dark Knight
	SOUL_EATER, // Dark Knight
	ASPIR, // Mage
	FLOOD, // Mage
	ELEMENTAL_SEAL, // Mage
	ENFIRE, // Mage
	ENBLIZZARD, // Mage
	ENPOISON, // Mage
	QUAKE, // Mage
	MANA_FONT, // Mage
	FLEE, // Rogue
	PREDICT, // Oracle
	CONDEMN, // Oracle
	BLESSED // Oracle
};

enum class ElementalSealEffects {
	FIRE = 1,
	ICE = 2,
	VENOM = 3
};

enum class AriResources {
	HEALTH,
	MAX_HEALTH,
	STAMINA,
	MANA
};

enum class DungeonBiomes {
	UPPER,
	TIDE_CAVERNS,
	DEEP_EARTH,
	LAVA_CAVES,
	RUINS
};

enum class FloorEnchantments {
	// Negative effects
	HP_PENALTY, // Group 1
	EXHAUSTION, // Group 1
	AMNESIA, // Group 2
	ITEM_PENALTY, // Group 2
	DISTORTION, // Group 2
	DAMAGE_DOWN, // Group 2
	GRAVITY, // Group 1
	GLOOM, // Group 3

	// Positive effects
	RESTORATION, // Group 1
	SECOND_WIND, // Group 1
	HASTE, // Group 1
	FEY, // Group 3

	// Predict Exclusive Effects
	FUMIGATE, // Group 1
	FRAILTY, // Group 2
	GRUDGE, // Group 2
	DEEP_WOUNDS, // Group 1
	BLINK, // Group 1
	STONESKIN, // Group 1
	PHALANX // Group 1
};

enum class Offerings {
	DREAD,
	INNER_FIRE,
	LEECH,
	PERIL,
	RECKONING,

	// Condemn Exclusive Offerings
	OUTBREAK,
	SPIRIT_LINK,
	SPIKES,
	REFLECT
};

enum class Sigils {
	RAGE,
	STRENGTH,
	FORTIFICATION,
	PROTECTION,
	SERENITY,
	SAFETY,
	SILENCE,
	FORTUNE,
	REDEMPTION,
	ALTERATION,
	CONCEALMENT,
	TEMPTATION,
	SIGHT,
	INTUITION
};

enum class GreaterSigils {
	BENEDICTION,
	ASTRAL_FLOW,
	CHAIN_SPELL,
	SPIRIT_SURGE,
	MEIKYO_SHISUI
};

enum class Traps {
	CONFUSING,
	DISORIENTING,
	EXPLODING,
	INHIBITING,
	LURING,
	METEOR,
	GAZE,
	_VOID
};

enum class CustomAOETypes {
	METEOR,
	GAZE,
	_VOID
};

struct CustomAOE {
	int x;
	int y;
	int spawned_time;
	int duration;
	int last_application;
	bool is_active;
	RValue instance;
	CustomAOETypes type;
};

struct RevealedFloorTrap {
	int x;
	int y;
	bool is_active;
	RValue instance;
};

struct TreasureSpot {
	enum TreasureSpotState { INACTIVE, WAITING_TO_SPAWN, SPAWNED, FOUND };

	int x;
	int y;
	int floors_to_descend;
	bool is_active;
	RValue instance;
	TreasureSpotState state = INACTIVE;
};


struct ChallengeModeProgress {
	int highest_floor_reached{0};
	cista::raw::hash_map<cista::raw::cstring, int> starting_inventory{};
};

// Engine globals — YYTK/Aurie-layer objects, no namespace.
extern YYTKInterface* g_ModuleInterface;
extern CInstance* global_instance;
extern RValue __YYTK;

namespace State {

	// Player character state: position, resources, per-run item-use flags, resource tracking.
	namespace Player {
		extern ChallengeModeProgress challenge_mode_progress;
		extern std::optional<Config::Configuration> saved_config;
		extern bool load_on_start;
		extern bool is_new_game;
		extern bool is_challenge_mode;
		extern bool in_whirl_pool;
		extern bool localize_mod_text;
		extern bool game_is_active;
		extern bool unlock_recipes;
		extern bool sigil_item_used;
		extern bool greater_sigil_item_used;
		extern bool salve_item_used;
		extern bool lift_key_used;
		extern bool orb_item_used;
		extern bool dread_contract_used;
		extern bool heart_crystal_used;
		extern bool inner_fire_cast;
		extern bool reckoning_applied;
		extern bool fairy_buff_applied;
		extern bool stoneskin_applied;
		extern bool is_restoration_tracked_interval;
		extern bool is_second_wind_tracked_interval;
		extern bool is_fumigate_tracked_interval;
		extern bool is_deep_wounds_tracked_interval;
		extern double ari_x;
		extern double ari_y;
		extern double ari_facing_dir;
		extern int held_item_id;
		extern int unmodified_base_health;
		extern int hp_penalty_amount;
		extern std::string save_prefix;
		extern std::string ari_current_location;
		extern std::string ari_current_gm_room;
		extern std::map<AriResources, int> ari_resource_to_value_map;
		extern std::map<AriResources, bool> ari_resource_to_penalty_map;
	}

	// Floor-scoped state: enchantments, sigils, offerings, traps, monsters, boss battles.
	namespace Floor {
		extern int floor_number;
		extern int floor_start_time;
		extern int current_time_in_seconds;
		extern int time_of_last_restoration_tick;
		extern int time_of_last_second_wind_tick;
		extern int time_of_last_fumigate_tick;
		extern int time_of_last_deep_wounds_tick;
		extern int time_of_last_outbreak_tick;
		extern bool time_stopped;
		extern int64_t time_stopped_tick_accumulator;
		extern bool drop_biome_reward;
		extern bool biome_reward_disabled;
		extern bool dread_beast_configured;
		extern bool offering_chance_occurred;
		extern int dread_beast_monster_id;
		extern int dread_beasts_configured;
		extern int boss_monsters_configured;
		extern BossBattle boss_battle;
		extern TreasureSpot treasure_spot;
		extern std::unordered_set<int> active_dread_contracts;
		extern std::unordered_set<Sigils> active_sigils;
		extern std::unordered_set<GreaterSigils> active_greater_sigils;
		extern std::unordered_set<Offerings> queued_offerings;
		extern std::unordered_set<Offerings> active_offerings;
		extern std::unordered_set<FloorEnchantments> active_floor_enchantments;
		extern std::unordered_set<std::pair<int, int>, pair_hash> floor_trap_positions;
		extern std::map<Traps, std::pair<int, int>> active_traps;
		extern std::map<Traps, int> active_traps_to_value_map;
		extern std::vector<CustomAOE> meteor_aoes;
		extern std::vector<CustomAOE> gaze_aoes;
		extern std::vector<CustomAOE> void_aoes;
		extern std::vector<RevealedFloorTrap> revealed_floor_traps;
		extern std::vector<int> initial_floor_monsters;
		extern std::vector<CInstance*> current_floor_monsters;
		extern std::map<std::string, int> salves_used;
	}

	// Combat mechanics: counters, set bonus tracking, stalagmite AOE attack patterns.
	namespace Combat {
		extern int frailty_hit_counter;
		extern int grudge_counter;
		extern int deep_wounds_damage_pool;
		extern int stoneskin_shield_amount;
		extern int spirit_link_combined_health_pool;
		extern int sigil_of_silence_count;
		extern int sigil_of_alteration_monster_id;
		extern std::map<Classes, std::map<ManagedSetBonuses, int>> class_name_to_set_bonus_effect_value_map;
		extern std::vector<std::vector<double>> donut_aoe_points;
		extern std::vector<std::vector<double>> pb_aoe_points;
		extern std::vector<std::vector<double>> cross_aoe_points;
		extern std::vector<std::vector<double>> x_aoe_points;
		extern std::vector<std::vector<double>> checkerboard_room_wide_a_points;
		extern std::vector<std::vector<double>> checkerboard_room_wide_b_points;
	}

	// UI and menu state: window dimensions, draw flags, menu open/focus state.
	namespace UI {
		extern double window_width;
		extern double window_height;
		extern bool show_dashes;
		extern bool show_danger_banner;
		extern bool fade_initialized;
		extern uint64_t fade_start_time;
		extern bool crafting_menu_open;
		extern bool journal_menu_open;
		extern bool store_menu_open;
		extern bool obj_dragonshrine_focused;
		extern bool obj_dungeon_elevator_focused;
		extern bool obj_dungeon_ladder_down_focused;
	}

	// Lookup maps: all bidirectional ID<->name mappings, item/sigil sets, localization maps.
	namespace Maps {
		extern std::unordered_set<int> orb_items;
		extern std::unordered_set<int> salve_items;
		extern std::unordered_set<int> lift_key_items;
		extern std::unordered_set<int> restricted_items;
		extern std::unordered_set<int> deep_dungeon_items;
		extern std::unordered_set<int> default_sword_items;
		extern std::unordered_set<int> dread_contract_items;
		extern std::map<Sigils, int> sigil_to_item_id_map;
		extern std::map<int, Sigils> item_id_to_sigil_map;
		extern std::map<GreaterSigils, int> greater_sigil_to_item_id_map;
		extern std::map<int, GreaterSigils> item_id_to_greater_sigil_map;
		extern std::map<std::string, int> perk_name_to_id_map;
		extern std::map<std::string, int> spell_name_to_id_map;
		extern std::map<int, int> spell_id_to_default_cost_map;
		extern std::map<std::string, int> salve_name_to_id_map; // TODO: Remove and use item_name_to_id_map instead.
		extern std::map<int, std::string> object_id_to_name_map;
		extern std::map<std::string, int> player_state_to_id_map;
		extern std::map<std::string, std::map<std::string, int>> monster_category_to_state_id_map;
		extern std::map<std::string, int> monster_name_to_id_map;
		extern std::map<int, std::string> monster_id_to_name_map;
		extern std::map<std::string, int> tutorial_name_to_id_map;
		extern std::map<std::string, int> infusion_name_to_id_map;
		extern std::map<std::string, int> status_effect_name_to_id_map;
		extern std::map<std::string, int> location_name_to_id_map;
		extern std::map<std::string, int> item_name_to_id_map;
		extern std::map<int, std::string> item_id_to_name_map;
		extern std::map<std::string, int> bark_name_to_id_map;
		extern std::map<Offerings, std::string> offerings_to_localized_string_map;
		extern std::map<Classes, std::string> classes_to_localized_armor_description_string_map;
		extern std::map<FloorEnchantments, std::string> floor_enchantments_to_localized_string_map;
		extern std::map<Classes, std::map<std::string, std::string>> class_armor_to_localized_string_map;
		extern std::map<std::string, uint64_t> notification_name_to_last_display_time_map;
		extern std::map<int, RValue> item_id_to_prototype_map;
		extern std::map<int, double> item_id_to_original_defense_map;
		extern std::map<int, double> item_id_to_original_damage_map;
		extern std::map<int, double> item_id_to_original_bomb_damage_map;
		extern std::map<int, double> monster_id_to_original_hp_map;
		extern std::map<int, double> monster_id_to_original_damage_map;
		extern std::map<int, double> monster_id_to_original_projectile_damage_map;
		extern std::map<std::string, std::unordered_set<int>> dungeon_biome_to_candidate_monsters_map;
		extern std::map<int, std::string> floor_number_to_biome_name_map;
		extern std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map;
	}

} // namespace State
