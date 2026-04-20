#include "Globals.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::UI;
using namespace State::Maps;

// Engine globals — no namespace.
YYTKInterface* g_ModuleInterface = nullptr;
CInstance* global_instance = nullptr;
RValue __YYTK;

namespace State {

	namespace Player {
		ChallengeModeProgress challenge_mode_progress = {};
		bool load_on_start = true;
		bool is_new_game = false;
		bool is_challenge_mode = false;
		bool in_whirl_pool = false;
		bool localize_mod_text = false;
		bool game_is_active = false;
		bool unlock_recipes = true;
		bool sigil_item_used = false;
		bool greater_sigil_item_used = false;
		bool salve_item_used = false;
		bool lift_key_used = false;
		bool orb_item_used = false;
		bool dread_contract_used = false;
		bool heart_crystal_used = false;
		bool inner_fire_cast = false;
		bool reckoning_applied = false;
		bool fairy_buff_applied = false;
		bool stoneskin_applied = false;
		bool is_restoration_tracked_interval = false;
		bool is_second_wind_tracked_interval = false;
		bool is_fumigate_tracked_interval = false;
		bool is_deep_wounds_tracked_interval = false;
		double ari_x = -1;
		double ari_y = -1;
		double ari_facing_dir = -1;
		int held_item_id = -1;
		int unmodified_base_health = -1;
		int hp_penalty_amount = -1;
		std::string save_prefix = "";
		std::string ari_current_location = "";
		std::string ari_current_gm_room = "";
		std::map<AriResources, int> ari_resource_to_value_map = {};
		std::map<AriResources, bool> ari_resource_to_penalty_map = {};
	}

	namespace Floor {
		int floor_number = 0;
		int floor_start_time = 0;
		int current_time_in_seconds = -1;
		int time_of_last_restoration_tick = -1;
		int time_of_last_second_wind_tick = -1;
		int time_of_last_fumigate_tick = -1;
		int time_of_last_deep_wounds_tick = -1;
		int time_of_last_outbreak_tick = -1;
		bool time_stopped = false;
		int64_t time_stopped_tick_accumulator = 0;
		bool drop_biome_reward = false;
		bool biome_reward_disabled = false;
		bool dread_beast_configured = false;
		bool offering_chance_occurred = false;
		int dread_beast_monster_id = -1;
		int dread_beasts_configured = 0;
		int boss_monsters_configured = 0;
		BossBattle boss_battle = BossBattle::NONE;
		TreasureSpot treasure_spot = TreasureSpot();
		std::unordered_set<int> active_dread_contracts = {};
		std::unordered_set<Sigils> active_sigils = {};
		std::unordered_set<GreaterSigils> active_greater_sigils = {};
		std::unordered_set<Offerings> queued_offerings = {};
		std::unordered_set<Offerings> active_offerings = {};
		std::unordered_set<FloorEnchantments> active_floor_enchantments = {};
		std::unordered_set<std::pair<int, int>, pair_hash> floor_trap_positions = {};
		std::map<Traps, std::pair<int, int>> active_traps = {};
		std::map<Traps, int> active_traps_to_value_map = {};
		std::vector<CustomAOE> meteor_aoes = {};
		std::vector<CustomAOE> gaze_aoes = {};
		std::vector<CustomAOE> void_aoes = {};
		std::vector<RevealedFloorTrap> revealed_floor_traps = {};
		std::vector<int> initial_floor_monsters = {};
		std::vector<CInstance*> current_floor_monsters = {};
		std::map<std::string, int> salves_used = {};
	}

	namespace Combat {
		int frailty_hit_counter = 0;
		int grudge_counter = 0;
		int deep_wounds_damage_pool = 0;
		int stoneskin_shield_amount = 0;
		int spirit_link_combined_health_pool = 0;
		int sigil_of_silence_count = 0;
		int sigil_of_alteration_monster_id = -1;
		std::map<Classes, std::map<ManagedSetBonuses, int>> class_name_to_set_bonus_effect_value_map;
		std::vector<std::vector<double>> donut_aoe_points;
		std::vector<std::vector<double>> pb_aoe_points;
		std::vector<std::vector<double>> cross_aoe_points;
		std::vector<std::vector<double>> x_aoe_points;
		std::vector<std::vector<double>> checkerboard_room_wide_a_points;
		std::vector<std::vector<double>> checkerboard_room_wide_b_points;
	}

	namespace UI {
		double window_width = 0;
		double window_height = 0;
		bool show_dashes = false;
		bool show_danger_banner = false;
		bool fade_initialized = false;
		uint64_t fade_start_time = 0;
		bool crafting_menu_open = false;
		bool journal_menu_open = false;
		bool store_menu_open = false;
		bool obj_dragonshrine_focused = false;
		bool obj_dungeon_elevator_focused = false;
		bool obj_dungeon_ladder_down_focused = false;
	}

	namespace Maps {
		std::unordered_set<int> orb_items = {};
		std::unordered_set<int> salve_items = {};
		std::unordered_set<int> lift_key_items = {};
		std::unordered_set<int> restricted_items = {};
		std::unordered_set<int> deep_dungeon_items = {};
		std::unordered_set<int> default_sword_items = {};
		std::unordered_set<int> dread_contract_items = {};
		std::map<Sigils, int> sigil_to_item_id_map = {};
		std::map<int, Sigils> item_id_to_sigil_map = {};
		std::map<GreaterSigils, int> greater_sigil_to_item_id_map = {};
		std::map<int, GreaterSigils> item_id_to_greater_sigil_map = {};
		std::map<std::string, int> perk_name_to_id_map = {};
		std::map<std::string, int> spell_name_to_id_map = {};
		std::map<int, int> spell_id_to_default_cost_map = {};
		std::map<std::string, int> salve_name_to_id_map = {};
		std::map<int, std::string> object_id_to_name_map = {};
		std::map<std::string, int> player_state_to_id_map = {};
		std::map<std::string, std::map<std::string, int>> monster_category_to_state_id_map = {};
		std::map<std::string, int> monster_name_to_id_map = {};
		std::map<int, std::string> monster_id_to_name_map = {};
		std::map<std::string, int> tutorial_name_to_id_map = {};
		std::map<std::string, int> infusion_name_to_id_map = {};
		std::map<std::string, int> status_effect_name_to_id_map = {};
		std::map<std::string, int> location_name_to_id_map = {};
		std::map<std::string, int> item_name_to_id_map = {};
		std::map<std::string, int> bark_name_to_id_map = {};
		std::map<Offerings, std::string> offerings_to_localized_string_map = {};
		std::map<Classes, std::string> classes_to_localized_armor_description_string_map = {};
		std::map<FloorEnchantments, std::string> floor_enchantments_to_localized_string_map = {};
		std::map<Classes, std::map<std::string, std::string>> class_armor_to_localized_string_map = {};
		std::map<std::string, uint64_t> notification_name_to_last_display_time_map = {};
		std::map<int, RValue> item_id_to_prototype_map = {};
		std::map<std::string, std::unordered_set<int>> dungeon_biome_to_candidate_monsters_map = {};
		std::map<int, std::string> floor_number_to_biome_name_map = {};
		std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map;
	}

} // namespace State

Config::Configuration Config::config = Config::Configuration();
