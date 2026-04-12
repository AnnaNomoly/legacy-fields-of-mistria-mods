#pragma once

#include <random>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <map>
#include <vector>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <pcg/pcg_random.hpp>
#include <magic_enum/magic_enum.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
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


static const char* const MOD_NAME = "DeepDungeon";
static const char* const VERSION = "1.0.0-BETA-3";
static const char* const GML_SCRIPT_GET_LOCALIZER = "gml_Script_get@Localizer@Localizer";
static const char* const GML_SCRIPT_SPAWN_LADDER = "gml_Script_spawn_ladder@DungeonRunner@DungeonRunner";
static const char* const GML_SCRIPT_TELEPORT_ARI_TO_ROOM = "gml_Script_ari_teleport_to_room";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const char* const GML_SCRIPT_CLOSE_TEXTBOX = "gml_Script_begin_close@TextboxMenu@TextboxMenu";
static const char* const GML_SCRIPT_PLAY_CONVERSATION = "gml_Script_play_conversation";
static const char* const GML_SCRIPT_SCENE_AUDIO_PLAYER_STOP = "gml_Script_stop@SceneAudioPlayer@SceneAudioPlayer";
static const char* const GML_SCRIPT_SPAWN_TUTORIAL = "gml_Script_spawn_tutorial";
static const char* const GML_SCRIPT_UPDATE_TOOLBAR_MENU = "gml_Script_update@ToolbarMenu@ToolbarMenu";
static const char* const GML_SCRIPT_ENTER_DUNGEON = "gml_Script_enter_dungeon";
static const char* const GML_SCRIPT_INVENTORY_COUNT_ITEM = "gml_Script_item_id_quantity@anon@4106@__Inventory@Inventory";
static const char* const GML_SCRIPT_INVENTORY_REMOVE_ITEM = "gml_Script_remove@anon@2021@__Inventory@Inventory";
static const char* const GML_SCRIPT_GET_UNIFIED_TIME = "gml_Script_unified_time@Calendar@Calendar";
static const char* const GML_SCRIPT_CANCEL_STATUS_EFFECT = "gml_Script_cancel@StatusEffectManager@StatusEffectManager";
static const char* const GML_SCRIPT_REGISTER_STATUS_EFFECT = "gml_Script_register@StatusEffectManager@StatusEffectManager";
static const char* const GML_SCRIPT_VITALS_MENU_SET_HEALTH = "gml_Script_set_health@VitalsMenu@VitalsMenu";
static const char* const GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH = "gml_Script_set_max_health@VitalsMenu@VitalsMenu";
static const char* const GML_SCRIPT_GET_MAX_HEALTH = "gml_Script_get_max_health@Ari@Ari";
static const char* const GML_SCRIPT_GET_HEALTH = "gml_Script_get_health@Ari@Ari";
static const char* const GML_SCRIPT_SET_HEALTH = "gml_Script_set_health@Ari@Ari";
static const char* const GML_SCRIPT_MODIFY_HEALTH = "gml_Script_modify_health@Ari@Ari";
static const char* const GML_SCRIPT_GET_STAMINA = "gml_Script_get_stamina@Ari@Ari";
static const char* const GML_SCRIPT_MODIFY_STAMINA = "gml_Script_modify_stamina@Ari@Ari";
static const char* const GML_SCRIPT_GET_MANA = "gml_Script_get_mana@Ari@Ari";
static const char* const GML_SCRIPT_MODIFY_MANA = "gml_Script_modify_mana@Ari@Ari";
static const char* const GML_SCRIPT_CAST_SPELL = "gml_Script_cast_spell";
static const char* const GML_SCRIPT_SPAWN_MONSTER = "gml_Script_spawn_monster";
static const char* const GML_SCRIPT_CAN_CAST_SPELL = "gml_Script_can_cast_spell";
static const char* const GML_SCRIPT_GET_MOVE_SPEED = "gml_Script_get_move_speed@Ari@Ari";
static const char* const GML_SCRIPT_DAMAGE = "gml_Script_damage@gml_Object_obj_damage_receiver_Create_0";
static const char* const GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE = "gml_Script_update@StatusEffectManager@StatusEffectManager";
static const char* const GML_SCRIPT_TAKE_PRESS = "gml_Script_take_press@Input@Input";
static const char* const GML_SCRIPT_CHECK_VALUE = "gml_Script_check_value@Input@Input";
static const char* const GML_SCRIPT_ATTEMPT_INTERACT = "gml_Script_attempt_interact@gml_Object_par_interactable_Create_0";
static const char* const GML_SCRIPT_USE_ITEM = "gml_Script_use_item";
static const char* const GML_SCRIPT_HELD_ITEM = "gml_Script_held_item@Ari@Ari";
static const char* const GML_SCRIPT_DROP_ITEM = "gml_Script_drop_item";
static const char* const GML_SCRIPT_GET_MINUTES = "gml_Script_update@Clock@Clock";
static const char* const GML_SCRIPT_PLAY_TEXT = "gml_Script_play_text@TextboxMenu@TextboxMenu";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
static const char* const GML_SCRIPT_TRY_LOCATION_ID_TO_STRING = "gml_Script_try_location_id_to_string";
static const char* const GML_SCRIPT_ON_DUNGEON_ROOM_START = "gml_Script_on_room_start@DungeonRunner@DungeonRunner";
static const char* const GML_SCRIPT_GO_TO_ROOM = "gml_Script_goto_gm_room";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM = "gml_Script_get_equipment_bonus_from@Ari@Ari";
static const char* const GML_SCRIPT_HUD_SHOULD_SHOW = "gml_Script_hud_should_show";
static const char* const GML_SCRIPT_ON_DRAW_GUI = "gml_Script_on_draw_gui@Display@Display";
static const char* const GML_SCRIPT_DISPLAY_RESIZE = "gml_Script_resize_amount@Display@Display";
static const char* const GML_SCRIPT_GET_ITEM_UI_ICON = "gml_Script_get_ui_icon@anon@4244@LiveItem@LiveItem";
static const char* const GML_SCRIPT_CREATE_ITEM_PROTOTYPES = "gml_Script_create_item_prototypes";
static const char* const GML_SCRIPT_DESERIALIZE_LIVE_ITEM = "gml_Script_deserialize_live_item";
static const char* const GML_SCRIPT_DESERIALIZE_INVENTORY = "gml_Script_deserialize@anon@6096@__Inventory@Inventory";
static const char* const GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION = "gml_Script_get_treasure_from_distribution";
static const char* const GML_SCRIPT_CRAFTING_MENU_INITIALIZE = "gml_Script_initialize@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_CRAFTING_MENU_CLOSE = "gml_Script_on_close@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_JOURNAL_MENU_INITIALIZE = "gml_Script_initialize@JournalMenu@JournalMenu";
static const char* const GML_SCRIPT_JOURNAL_MENU_CLOSE = "gml_Script_on_close@JournalMenu@JournalMenu";
static const char* const GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR = "gml_Script_vertigo_draw_with_color";
static const char* const GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY = "gml_Script_play@SceneAudioPlayer@SceneAudioPlayer";
static const char* const GML_SCRIPT_FIND_NPC_BLIP_NOISE = "gml_Script_find_npc_blip_noise";
static const char* const GML_SCRIPT_SAVE_GAME = "gml_Script_save_game";
static const char* const GML_SCRIPT_ARI_FACE_DIR = "gml_Script_face_dir@gml_Object_obj_ari_Create_0";
static const char* const GML_SCRIPT_ON_BEGIN_STEP = "gml_Script_on_begin_step@Anchor@Anchor";
static const char* const GML_SCRIPT_RECIPE_GENERATE_INFUSIONS = "gml_Script_generate_infusions@Recipe@Recipe";
static const char* const GML_SCRIPT_BARK_EMITTER = "gml_Script_BarkEmitter";
static const char* const GML_SCRIPT_BARK_EMITTER_EMIT = "gml_Script_emit@BarkEmitter@BarkEmitter";
static const char* const GML_SCRIPT_T2_READ = "gml_Script_read@T2r@T2r";
static const char* const CONFIG_VERSION_JSON_KEY = "__config_version"; // The config version
static const char* const DISABLE_DUNGEON_LIFT_JSON_KEY = "disable_dungeon_lift"; // Controls the dungeon lift
static const char* const RESTRICT_PERKS_JSON_KEY = "restrict_perks"; // Determines if perks are restricted in the dungeon
static const char* const RESTRICT_ITEMS_JSON_KEY = "restrict_items"; // Determines if items are restricted in the dungeon
static const char* const RESTRICT_ARMOR_JSON_KEY = "restrict_armor"; // Determines if armor is restricted in the dungeon
static const char* const RESTRICT_TOOLS_JSON_KEY = "restrict_tools"; // Determines if tools are restricted in the dungeon
static const char* const RESTRICT_WEAPONS_JSON_KEY = "restrict_weapons"; // Determines if weapons are restricted in the dungeon
static const char* const HEALTH_SALVE_LIMIT_JSON_KEY = "health_salve_limit"; // Controls how many health salves may be used per floor
static const char* const STAMINA_SALVE_LIMIT_JSON_KEY = "stamina_salve_limit"; // Controls how many stamina salves may be used per floor
static const char* const MANA_SALVE_LIMIT_JSON_KEY = "mana_salve_limit"; // Controls how many mana salves may be used per floor
static const char* const HEALTH_SALVE_POTENCY_JSON_KEY = "health_salve_potency"; // Controls how much the health salve restores
static const char* const STAMINA_SALVE_POTENCY_JSON_KEY = "stamina_salve_potency"; // Controls how much the stamina salve restores
static const char* const MANA_SALVE_POTENCY_JSON_KEY = "mana_salve_potency"; // Controls how much the mana salve restores
static const char* const SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY = "sustaining_potion_duration_modifier"; // Controls the duration of the sustaining potion
static const char* const RANDOMIZE_DUNGEON_MUSIC_JSON_KEY = "randomize_dungeon_music"; // Controls music randomizer
static const char* const RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY = "random_dread_beast_spawn_chance"; // Controls the random (not Offering) dread beast spawn chance
static const char* const OFFERING_EVENT_CHANCE_JSON_KEY = "offering_event_chance"; // Controls the chance for offering events
static const char* const OFFERING_HEALTH_REQUIREMENT_JSON_KEY = "offering_health_requirement"; // Controls the HP needed for an offering
static const char* const OFFERING_STAMINA_REQUIREMENT_JSON_KEY = "offering_stamina_requirement"; // Controls the STA needed for an offering
static const char* const OFFERING_MANA_REQUIREMENT_JSON_KEY = "offering_mana_requirement"; // Controls the MP needed for an offering
static const char* const CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY = "cursed_armor_drop_chance_modifier"; // Controls cursed armor drop rate
static const char* const SOUL_STONE_DROP_CHANCE_JSON_KEY = "soul_stone_drop_chance"; // Controls soul stone drop rate
static const char* const LIFT_KEY_DROP_CHANCE_JSON_KEY = "lift_key_drop_chance"; // Controls lift key drop rates
static const char* const ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY = "enable_boss_fight_restrictions"; // Controls if sigils and spells are restricted during boss fights
static const char* const CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY = "confusing_trap_duration_seconds"; // Controls the duration in seconds for confusing traps
static const char* const DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY = "disorienting_trap_duration_seconds"; // Controls the duration in seconds for disorienting traps
static const char* const EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY = "exploding_trap_current_health_damage_percent"; // Controls the health percentage damage for exploding traps
static const char* const INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY = "inhibiting_trap_duration_seconds"; // Controls the duration in seconds for inhibiting traps
static const char* const LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY = "luring_trap_monster_spawn_count"; // Controls the number of monsters spawned for luring traps
static const char* const GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY = "gaze_trap_max_health_damage_percent"; // Controls the number of monsters spawned for luring traps
static const char* const METEOR_TRAP_SCALING_FACTOR_JSON_KEY = "meteor_trap_scaling_factor"; // Controls the scaling factor of the meteor trap
static const char* const VOID_TRAP_DURATION_SECONDS_JSON_KEY = "void_trap_duration_seconds"; // Controls the duration in seconds for void traps
static const char* const MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY = "mistpool_equipment_store_price"; // Controls the price of mistpool equipment in stores
static const char* const SALVES_STORE_PRICE_JSON_KEY = "salves_store_price"; // Controls the price of salves in stores
static const char* const DREAD_BEAST_DAMAGE_MODIFIER_JSON_KEY = "dread_beast_damage_modifier"; // Controls the damage of dread beasts
static const char* const DREAD_BEAST_HEALTH_MODIFIER_JSON_KEY = "dread_beast_health_modifier"; // Controls the health of dread beasts
static const char* const GLOOM_DAMAGE_DEALT_MODIFIER_JSON_KEY = "gloom_monster_damage_dealt_modifier"; // Controls the damage multiplier granted by Gloom
static const char* const GLOOM_DAMAGE_RECEIVED_MODIFIER_JSON_KEY = "gloom_monster_damage_received_modifier"; // Controls the damage mitigation granted by Gloom
static const char* const GLOOM_HEALTH_MODIFIER_JSON_KEY = "gloom_monster_health_modifier"; // Controls the health bonus granted by Gloom
static const char* const EXPERIMENTAL_MAX_HEALTH_BUG_FIX_JSON_KEY = "experimental_max_health_bug_fix"; // Controls the experimental fix for restoring proper max health
static const char* const EXPERIMENTAL_EXTRA_FLOOR_ENCHANTMENTS_AND_OFFERINGS_JSON_KEY = "experimental_extra_floor_enchantments_and_offerings"; // Controls the experimental option to always enable the Oracle exclusive floor enchantments and offerings
static const char* const EXPERIMENTAL_MONSTER_BASE_STAT_DIFFICULTY_MODIFIER_JSON_KEY = "experimental_monster_base_stat_difficulty_modifier"; // Controls the experimental option to scale base monster health and damage

static const std::string SIGIL_OF_ALTERATION_NAME = "sigil_of_alteration";
static const std::string SIGIL_OF_CONCEALMENT_NAME = "sigil_of_concealment";
static const std::string SIGIL_OF_FORTIFICATION_NAME = "sigil_of_fortification";
static const std::string SIGIL_OF_FORTUNE_NAME = "sigil_of_fortune";
static const std::string SIGIL_OF_PROTECTION_NAME = "sigil_of_protection";
static const std::string SIGIL_OF_RAGE_NAME = "sigil_of_rage";
static const std::string SIGIL_OF_REDEMPTION_NAME = "sigil_of_redemption";
static const std::string SIGIL_OF_SAFETY_NAME = "sigil_of_safety";
static const std::string SIGIL_OF_SERENITY_NAME = "sigil_of_serenity";
static const std::string SIGIL_OF_SILENCE_NAME = "sigil_of_silence";
static const std::string SIGIL_OF_STRENGTH_NAME = "sigil_of_strength";
static const std::string SIGIL_OF_TEMPTATION_NAME = "sigil_of_temptation";
static const std::string SIGIL_OF_SIGHT_NAME = "sigil_of_sight";
static const std::string SIGIL_OF_INTUITION_NAME = "sigil_of_intuition";
static const std::string GREATER_SIGIL_OF_BENEDICTION_NAME = "greater_sigil_of_benediction";
static const std::string GREATER_SIGIL_OF_ASTRAL_FLOW_NAME = "greater_sigil_of_astral_flow";
static const std::string GREATER_SIGIL_OF_CHAIN_SPELL_NAME = "greater_sigil_of_chain_spell";
static const std::string GREATER_SIGIL_OF_SPIRIT_SURGE_NAME = "greater_sigil_of_spirit_surge";
static const std::string GREATER_SIGIL_OF_MEIKYO_SHISUI_NAME = "greater_sigil_of_meikyo_shisui";
static const std::string SUSTAINING_POTION_NAME = "sustaining_potion";
static const std::string HEALTH_SALVE_NAME = "health_salve";
static const std::string STAMINA_SALVE_NAME = "stamina_salve";
static const std::string MANA_SALVE_NAME = "mana_salve";
static const std::string MISTPOOL_SWORD_NAME = "sword_scrap_metal";
static const std::string MISTPOOL_PICK_AXE_NAME = "pick_axe_worn";
static const std::string MISTPOOL_HELMET_NAME = "scrap_metal_helmet";
static const std::string MISTPOOL_CHESTPIECE_NAME = "scrap_metal_chestpiece";
static const std::string MISTPOOL_PANTS_NAME = "scrap_metal_pants";
static const std::string MISTPOOL_BOOTS_NAME = "scrap_metal_boots";
static const std::string MISTPOOL_GLOVES_NAME = "scrap_metal_ring";
static const std::string CURSED_HELMET_NAME = "cursed_helmet";
static const std::string CURSED_CHESTPIECE_NAME = "cursed_chestpiece";
static const std::string CURSED_PANTS_NAME = "cursed_pants";
static const std::string CURSED_BOOTS_NAME = "cursed_boots";
static const std::string CURSED_GLOVES_NAME = "cursed_gloves";
static const std::string CLERIC_HELMET_NAME = "cleric_helmet";
static const std::string CLERIC_CHESTPIECE_NAME = "cleric_chestpiece";
static const std::string CLERIC_GLOVES_NAME = "cleric_gloves";
static const std::string CLERIC_PANTS_NAME = "cleric_pants";
static const std::string CLERIC_BOOTS_NAME = "cleric_boots";
static const std::string DARK_KNIGHT_HELMET_NAME = "dark_knight_helmet";
static const std::string DARK_KNIGHT_CHESTPIECE_NAME = "dark_knight_chestpiece";
static const std::string DARK_KNIGHT_GLOVES_NAME = "dark_knight_gloves";
static const std::string DARK_KNIGHT_PANTS_NAME = "dark_knight_pants";
static const std::string DARK_KNIGHT_BOOTS_NAME = "dark_knight_boots";
static const std::string MAGE_HELMET_NAME = "mage_helmet";
static const std::string MAGE_CHESTPIECE_NAME = "mage_chestpiece";
static const std::string MAGE_GLOVES_NAME = "mage_gloves";
static const std::string MAGE_PANTS_NAME = "mage_pants";
static const std::string MAGE_BOOTS_NAME = "mage_boots";
static const std::string PALADIN_HELMET_NAME = "paladin_helmet";
static const std::string PALADIN_CHESTPIECE_NAME = "paladin_chestpiece";
static const std::string PALADIN_GLOVES_NAME = "paladin_gloves";
static const std::string PALADIN_PANTS_NAME = "paladin_pants";
static const std::string PALADIN_BOOTS_NAME = "paladin_boots";
static const std::string ROGUE_HELMET_NAME = "rogue_helmet";
static const std::string ROGUE_CHESTPIECE_NAME = "rogue_chestpiece";
static const std::string ROGUE_GLOVES_NAME = "rogue_gloves";
static const std::string ROGUE_PANTS_NAME = "rogue_pants";
static const std::string ROGUE_BOOTS_NAME = "rogue_boots";
static const std::string ORACLE_HELMET_NAME = "oracle_helmet";
static const std::string ORACLE_CHESTPIECE_NAME = "oracle_chestpiece";
static const std::string ORACLE_GLOVES_NAME = "oracle_gloves";
static const std::string ORACLE_PANTS_NAME = "oracle_pants";
static const std::string ORACLE_BOOTS_NAME = "oracle_boots";
static const std::string TREASURE_CHEST_WOOD_NAME = "treasure_chest_wood";
static const std::string TREASURE_CHEST_COPPER_NAME = "treasure_chest_copper";
static const std::string TREASURE_CHEST_SILVER_NAME = "treasure_chest_silver";
static const std::string TREASURE_CHEST_GOLD_NAME = "treasure_chest_gold";
static const std::string UPPER_MINES_KEY_F5_NAME = "upper_mines_key_f5";
static const std::string UPPER_MINES_KEY_F10_NAME = "upper_mines_key_f10";
static const std::string UPPER_MINES_KEY_F15_NAME = "upper_mines_key_f15";
static const std::string TIDE_CAVERNS_KEY_F20_NAME = "tide_caverns_key_f20";
static const std::string TIDE_CAVERNS_KEY_F25_NAME = "tide_caverns_key_f25";
static const std::string TIDE_CAVERNS_KEY_F30_NAME = "tide_caverns_key_f30";
static const std::string TIDE_CAVERNS_KEY_F35_NAME = "tide_caverns_key_f35";
static const std::string DEEP_EARTH_KEY_F40_NAME = "deep_earth_key_f40";
static const std::string DEEP_EARTH_KEY_F45_NAME = "deep_earth_key_f45";
static const std::string DEEP_EARTH_KEY_F50_NAME = "deep_earth_key_f50";
static const std::string DEEP_EARTH_KEY_F55_NAME = "deep_earth_key_f55";
static const std::string LAVA_CAVES_KEY_F60_NAME = "lava_caves_key_f60";
static const std::string LAVA_CAVES_KEY_F65_NAME = "lava_caves_key_f65";
static const std::string LAVA_CAVES_KEY_F70_NAME = "lava_caves_key_f70";
static const std::string LAVA_CAVES_KEY_F75_NAME = "lava_caves_key_f75";
static const std::string RUINS_KEY_F80_NAME = "ruins_key_f80";
static const std::string RUINS_KEY_F85_NAME = "ruins_key_f85";
static const std::string RUINS_KEY_F90_NAME = "ruins_key_f90";
static const std::string RUINS_KEY_F95_NAME = "ruins_key_f95";
static const std::string RUINS_KEY_F100_NAME = "ruins_key_f100";
static const std::string TIDE_CAVERNS_ORB = "tide_caverns_orb";
static const std::string DEEP_EARTH_ORB = "deep_earth_orb";
static const std::string LAVA_CAVES_ORB = "lava_caves_orb";
static const std::string RUINS_ORB = "ruins_orb";
static const std::string SOUL_STONE_CLERIC = "soul_stone_cleric";
static const std::string SOUL_STONE_DARK_KNIGHT = "soul_stone_dark_knight";
static const std::string SOUL_STONE_MAGE = "soul_stone_mage";
static const std::string SOUL_STONE_PALADIN = "soul_stone_paladin";
static const std::string SOUL_STONE_ROGUE = "soul_stone_rogue";
static const std::string SOUL_STONE_ORACLE = "soul_stone_oracle";
static const std::string SAVING_DISABLED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/saving_disabled";
static const std::string LIFT_KEY_RESTRICTED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/lift_key_restricted";
static const std::string ORB_RESTRICTED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/orb_restricted";
static const std::string SIGIL_RESTRICTED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/sigil_restricted";
static const std::string GREATER_SIGIL_RESTRICTED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/greater_sigil_restricted";
static const std::string SIGIL_LIMIT_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/sigil_limit";
static const std::string GREATER_SIGIL_LIMIT_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/greater_sigil_limit";
static const std::string SALVE_LIMIT_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/salve_limit";
static const std::string ITEM_PENALTY_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/item_penalty";
static const std::string ITEM_PROHIBITED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/item_prohibited";
static const std::string ITEM_RESTRICTED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/item_restricted";
static const std::string CONCEALMENT_LOST_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Sigils/concealment/deactivated";
static const std::string AMNESIA_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/amnesia";
static const std::string BOSS_BATTLE_SPELL_RESTRICTION_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/boss_spell_restriction";
static const std::string INHIBITED_PENALTY_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/inhibited_penalty";
static const std::string MALFUNCTION_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/malfunction";
static const std::string CONFUSING_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/confusing";
static const std::string DISORIENTING_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/disorienting";
static const std::string EXPLODING_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/exploding";
static const std::string INHIBITING_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/inhibiting";
static const std::string LURING_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/luring";
static const std::string METEOR_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/meteor";
static const std::string GAZE_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/gaze";
static const std::string VOID_TRAP_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Traps/void";
static const std::string PREDICT_SPELL_CAST_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/predict";
static const std::string PROPHECY_FORTIFICATION_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Prophecy/fortification";
static const std::string PROPHECY_STRENGTH_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Prophecy/strength";
static const std::string PROPHECY_PROTECTION_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Prophecy/protection";
static const std::string PROPHECY_CONCEALMENT_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Prophecy/concealment";
static const std::string PROPHECY_SAFETY_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Prophecy/safety";
static const std::string TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Treasure Spot/not_present";
static const std::string TREASURE_SPOT_SPAWNED_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Treasure Spot/spawned";
static const std::string TREASURE_SPOT_FOUND_NOTIFICATION_KEY = "Notifications/Mods/Deep Dungeon/Treasure Spot/found";
static const std::string FLOOR_ENCHANTMENT_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/floor_enchantments";
static const std::string DREAD_BEAST_WARNING_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/dread_beast_warning";
static const std::string FLOOR_TEN_CONVERSATION_KEY = "Conversations/floor_10/mines_floor_ten";
static const std::string FLOOR_ENCHANTMENT_AND_DREAD_BEAST_WARNING_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/dread_beast_warning_and_floor_enchantments";
static const std::string ELEVATOR_LOCKED_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/Progression Mode/elevator_locked";
static const std::string BOSS_BATTLE_TIDE_CAVERNS_ORB_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/Boss Battles/tide_caverns_orb";
static const std::string BOSS_BATTLE_DEEP_EARTH_ORB_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/Boss Battles/deep_earth_orb";
static const std::string BOSS_BATTLE_LAVA_CAVES_ORB_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/Boss Battles/lava_caves_orb";
static const std::string BOSS_BATTLE_RUINS_ORB_CONVERSATION_KEY = "Conversations/Mods/Deep Dungeon/Boss Battles/ruins_orb";
static const std::string CONDEMN_PLACEHOLDER_TEXT_KEY = "Conversations/Mods/Deep Dungeon/placeholders/condemn/result";
static const std::string OFFERINGS_PLACEHOLDER_TEXT_KEY = "Conversations/Mods/Deep Dungeon/placeholders/offerings/result";
static const std::string FLOOR_ENCHANTMENT_PLACEHOLDER_TEXT_KEY = "Conversations/Mods/Deep Dungeon/placeholders/floor_enchantments/init";
static const std::string DREAD_BEAST_WARNING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Special/dread";
static const std::string GLOOM_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/gloom";
static const std::string HP_PENALTY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/hp_penalty";
static const std::string EXHAUSTION_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/exhaustion";
static const std::string AMNESIA_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/amnesia";
static const std::string ITEM_PENALTY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/item_penalty";
static const std::string DISTORTION_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/distortion";
static const std::string DAMAGE_DOWN_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/damage_down";
static const std::string GRAVITY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Negative/gravity";
static const std::string FEY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Positive/fey";
static const std::string RESTORATION_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Positive/restoration";
static const std::string SECOND_WIND_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Positive/second_wind";
static const std::string HASTE_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Positive/haste";
static const std::string FUMIGATE_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/fumigate";
static const std::string FRAILTY_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/frailty";
static const std::string GRUDGE_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/grudge";
static const std::string DEEP_WOUNDS_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/deep_wounds";
static const std::string BLINK_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/blink";
static const std::string STONESKIN_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/stoneskin";
static const std::string PHALANX_FLOOR_ENCHANTMENT_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Floor Enchantments/Predict/phalanx";
static const std::string DREAD_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Negative/dread";
static const std::string INNER_FIRE_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Positive/inner_fire";
static const std::string LEECH_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Positive/leech";
static const std::string PERIL_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Negative/peril";
static const std::string RECKONING_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Negative/reckoning";
static const std::string OUTBREAK_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Condemn/outbreak";
static const std::string SPIRIT_LINK_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Condemn/spirit_link";
static const std::string SPIKES_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Condemn/spikes";
static const std::string REFLECT_OFFERING_LOCALIZED_TEXT_KEY = "Conversations/Mods/Deep Dungeon/Offerings/Condemn/reflect";
static const std::string TIDE_CAVERNS_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Orbs/tide_caverns_orb/description";
static const std::string DEEP_EARTH_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Orbs/deep_earth_orb/description";
static const std::string LAVA_CAVES_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Orbs/lava_caves_orb/description";
static const std::string RUINS_ORB_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Orbs/ruins_orb/description";
static const std::string HEALTH_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Salves/health_salve/description";
static const std::string STAMINA_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Salves/stamina_salve/description";
static const std::string MANA_SALVE_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Salves/mana_salve/description";
static const std::string VALUE_PLACEHOLDER_TEXT = "<VALUE>";
static const std::string CLERIC_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Armor/cleric/description";
static const std::string DARK_KNIGHT_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Armor/dark_knight/description";
static const std::string MAGE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Armor/mage/description";
static const std::string PALADIN_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Armor/paladin/description";
static const std::string ROGUE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Armor/rogue/description";
static const std::string ORACLE_ARMOR_DESCRIPTION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Armor/oracle/description";
static const std::string SET_PIECES_EQUIPPED_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/set_bonuses/equipped";
static const std::string CLERIC_SET_BONUS_AUTO_REGEN_ONE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Cleric/set_bonuses/auto_regen/1";
static const std::string CLERIC_SET_BONUS_AUTO_REGEN_TWO_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Cleric/set_bonuses/auto_regen/2";
static const std::string CLERIC_SET_BONUS_AUTO_REGEN_THREE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Cleric/set_bonuses/auto_regen/3";
static const std::string CLERIC_SET_BONUS_DIVINE_SEAL_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Cleric/set_bonuses/divine_seal";
static const std::string CLERIC_SET_BONUS_AFFLATUS_MISERY_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Cleric/set_bonuses/afflatus_misery";
static const std::string DARK_KNIGHT_SET_BONUS_DRAIN_ONE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Dark Knight/set_bonuses/drain/1";
static const std::string DARK_KNIGHT_SET_BONUS_DRAIN_TWO_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Dark Knight/set_bonuses/drain/2";
static const std::string DARK_KNIGHT_SET_BONUS_DRAIN_THREE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Dark Knight/set_bonuses/drain/3";
static const std::string DARK_KNIGHT_SET_BONUS_DARK_SEAL_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Dark Knight/set_bonuses/dark_seal";
static const std::string DARK_KNIGHT_SET_BONUS_SOUL_EATER_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Dark Knight/set_bonuses/soul_eater";
static const std::string MAGE_SET_BONUS_ASPIR_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Mage/set_bonuses/aspir";
static const std::string MAGE_SET_BONUS_FLOOD_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Mage/set_bonuses/flood";
static const std::string MAGE_SET_BONUS_ELEMENTAL_SEAL_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Mage/set_bonuses/elemental_seal";
static const std::string MAGE_SET_BONUS_QUAKE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Mage/set_bonuses/quake";
static const std::string MAGE_SET_BONUS_MANA_FONT_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Mage/set_bonuses/mana_font";
static const std::string PALADIN_SET_BONUS_HOLY_CIRCLE_ONE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Paladin/set_bonuses/holy_circle/1";
static const std::string PALADIN_SET_BONUS_HOLY_CIRCLE_TWO_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Paladin/set_bonuses/holy_circle/2";
static const std::string PALADIN_SET_BONUS_HOLY_CIRCLE_THREE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Paladin/set_bonuses/holy_circle/3";
static const std::string PALADIN_SET_BONUS_TEMPERANCE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Paladin/set_bonuses/temperance";
static const std::string PALADIN_SET_BONUS_HALLOWED_GROUND_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Paladin/set_bonuses/hallowed_ground";
static const std::string ROGUE_SET_BONUS_FLEE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Rogue/set_bonuses/flee";
static const std::string ROGUE_SET_BONUS_HIDE_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Rogue/set_bonuses/hide";
static const std::string ROGUE_SET_BONUS_SNEAK_ATTACK_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Rogue/set_bonuses/sneak_attack";
static const std::string ROGUE_SET_BONUS_DISARM_TRAP_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Rogue/set_bonuses/disarm_trap";
static const std::string ROGUE_SET_BONUS_TREASURE_HUNTER_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Rogue/set_bonuses/treasure_hunter";
static const std::string ORACLE_SET_BONUS_PREDICT_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Oracle/set_bonuses/predict";
static const std::string ORACLE_SET_BONUS_CONDEMN_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Oracle/set_bonuses/condemn";
static const std::string ORACLE_SET_BONUS_DIVINATION_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Oracle/set_bonuses/divination";
static const std::string ORACLE_SET_BONUS_BLESSED_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Oracle/set_bonuses/blessed";
static const std::string ORACLE_SET_BONUS_PROPHECY_LOCALIZED_TEXT_KEY = "Items/Mods/Deep Dungeon/Classes/Oracle/set_bonuses/prophecy";

static const int TWO_MINUTES_IN_SECONDS = 120;
static const int TWO_MINUTES_AND_THIRTY_SECONDS = 150;
static const int THREE_MINUTES_IN_SECONDS = 180;
static const int TWENTY_FIVE_MINUTES_IN_SECONDS = 1600;
static const int THIRTY_MINUTES_IN_SECONDS = 1800;
static const int TRAP_ACTIVATION_DISTANCE = 16;

// Configuration defaults
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
static const int DEFAULT_OFFERING_HEALTH_REQUIREMENT = 25; // TODO: Tune this.
static const int DEFAULT_OFFERING_STAMINA_REQUIREMENT = 20; // TODO: Tune this.
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

static enum class BossBattle {
	NONE,
	TIDE_CAVERNS_ORB,
	DEEP_EARTH_ORB,
	LAVA_CAVES_ORB,
	RUINS_ORB
};

static enum class Classes {
	CLERIC,
	MAGE,
	PALADIN,
	DARK_KNIGHT,
	ROGUE,
	ORACLE
};

static enum class ManagedSetBonuses { // Set bonuses that have actively managed values.
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

static enum class ElementalSealEffects {
	FIRE = 1,
	ICE = 2,
	VENOM = 3
};

static enum class AriResources {
	HEALTH,
	MAX_HEALTH,
	STAMINA,
	MANA
};

static enum class DungeonBiomes {
	UPPER,
	TIDE_CAVERNS,
	DEEP_EARTH,
	LAVA_CAVES,
	RUINS
};

static enum class FloorEnchantments {
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

static enum class Offerings {
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

static enum class Sigils {
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

static enum class GreaterSigils {
	BENEDICTION,
	ASTRAL_FLOW,
	CHAIN_SPELL,
	SPIRIT_SURGE,
	MEIKYO_SHISUI
};

static enum class Traps {
	CONFUSING,
	DISORIENTING,
	EXPLODING,
	INHIBITING,
	LURING,
	METEOR,
	GAZE,
	_VOID
};

static enum class CustomAOETypes {
	METEOR,
	GAZE,
	_VOID
};

static struct CustomAOE {
	int x;
	int y;
	int spawned_time;
	int duration;
	int last_application;
	bool is_active;
	RValue instance;
	CustomAOETypes type;
};

static struct RevealedFloorTrap {
	int x;
	int y;
	bool is_active;
	RValue instance;
};

static struct TreasureSpot {
	enum TreasureSpotState { INACTIVE, WAITING_TO_SPAWN, SPAWNED, FOUND };

	int x;
	int y;
	int floors_to_descend;
	bool is_active;
	RValue instance;
	TreasureSpotState state = INACTIVE;
};

static const std::unordered_set<std::string> DUNGEON_TREASURE_CHEST_NAMES = {
	TREASURE_CHEST_WOOD_NAME,
	TREASURE_CHEST_COPPER_NAME,
	TREASURE_CHEST_SILVER_NAME,
	TREASURE_CHEST_GOLD_NAME
};

static const std::vector<std::string> MISTPOOL_ARMOR_NAMES = {
	MISTPOOL_HELMET_NAME,
	MISTPOOL_CHESTPIECE_NAME,
	MISTPOOL_PANTS_NAME,
	MISTPOOL_BOOTS_NAME,
	MISTPOOL_GLOVES_NAME
};

static const std::unordered_set<std::string> CLASS_ARMOR_NAMES = {
	CLERIC_HELMET_NAME, CLERIC_CHESTPIECE_NAME, CLERIC_GLOVES_NAME, CLERIC_PANTS_NAME, CLERIC_BOOTS_NAME,
	DARK_KNIGHT_HELMET_NAME, DARK_KNIGHT_CHESTPIECE_NAME, DARK_KNIGHT_GLOVES_NAME, DARK_KNIGHT_PANTS_NAME, DARK_KNIGHT_BOOTS_NAME,
	MAGE_HELMET_NAME, MAGE_CHESTPIECE_NAME, MAGE_GLOVES_NAME, MAGE_PANTS_NAME, MAGE_BOOTS_NAME,
	PALADIN_HELMET_NAME, PALADIN_CHESTPIECE_NAME, PALADIN_GLOVES_NAME, PALADIN_PANTS_NAME, PALADIN_BOOTS_NAME,
	ROGUE_HELMET_NAME, ROGUE_CHESTPIECE_NAME, ROGUE_GLOVES_NAME, ROGUE_PANTS_NAME, ROGUE_BOOTS_NAME,
	ORACLE_HELMET_NAME, ORACLE_CHESTPIECE_NAME, ORACLE_GLOVES_NAME, ORACLE_PANTS_NAME, ORACLE_BOOTS_NAME
};

static const std::map<Classes, std::unordered_set<std::string>> CLASS_NAME_TO_ARMOR_NAMES_MAP = {
	{ Classes::CLERIC, { CLERIC_HELMET_NAME, CLERIC_CHESTPIECE_NAME, CLERIC_GLOVES_NAME, CLERIC_PANTS_NAME, CLERIC_BOOTS_NAME } },
	{ Classes::DARK_KNIGHT, { DARK_KNIGHT_HELMET_NAME, DARK_KNIGHT_CHESTPIECE_NAME, DARK_KNIGHT_GLOVES_NAME, DARK_KNIGHT_PANTS_NAME, DARK_KNIGHT_BOOTS_NAME } },
	{ Classes::MAGE, { MAGE_HELMET_NAME, MAGE_CHESTPIECE_NAME, MAGE_GLOVES_NAME, MAGE_PANTS_NAME, MAGE_BOOTS_NAME } },
	{ Classes::PALADIN, { PALADIN_HELMET_NAME, PALADIN_CHESTPIECE_NAME, PALADIN_GLOVES_NAME, PALADIN_PANTS_NAME, PALADIN_BOOTS_NAME } },
	{ Classes::ROGUE, { ROGUE_HELMET_NAME, ROGUE_CHESTPIECE_NAME, ROGUE_GLOVES_NAME, ROGUE_PANTS_NAME, ROGUE_BOOTS_NAME } },
	{ Classes::ORACLE, { ORACLE_HELMET_NAME, ORACLE_CHESTPIECE_NAME, ORACLE_GLOVES_NAME, ORACLE_PANTS_NAME, ORACLE_BOOTS_NAME } }
};

static const std::vector<std::string> SOUL_STONE_NAMES = {
	SOUL_STONE_CLERIC,
	SOUL_STONE_DARK_KNIGHT,
	SOUL_STONE_MAGE,
	SOUL_STONE_PALADIN,
	SOUL_STONE_ROGUE,
	SOUL_STONE_ORACLE
};

static const std::vector<std::string> ORB_NAMES = {
	// TODO: Add other orbs
	TIDE_CAVERNS_ORB,
	DEEP_EARTH_ORB,
	LAVA_CAVES_ORB,
	RUINS_ORB
};

static const std::vector<FloorEnchantments> GROUP_ONE_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::HP_PENALTY,
	FloorEnchantments::EXHAUSTION,
	FloorEnchantments::GRAVITY,
	FloorEnchantments::RESTORATION,
	FloorEnchantments::SECOND_WIND,
	FloorEnchantments::HASTE,
};

static const std::vector<FloorEnchantments> GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::HP_PENALTY,
	FloorEnchantments::EXHAUSTION,
	FloorEnchantments::GRAVITY,
	FloorEnchantments::RESTORATION,
	FloorEnchantments::SECOND_WIND,
	FloorEnchantments::HASTE,
	FloorEnchantments::FUMIGATE,
	FloorEnchantments::DEEP_WOUNDS,
	FloorEnchantments::BLINK,
	FloorEnchantments::STONESKIN,
	FloorEnchantments::PHALANX
};

static const std::vector<FloorEnchantments> GROUP_TWO_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::AMNESIA,
	FloorEnchantments::ITEM_PENALTY,
	FloorEnchantments::DISTORTION,
	FloorEnchantments::DAMAGE_DOWN,
};

static const std::vector<FloorEnchantments> GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::AMNESIA,
	FloorEnchantments::ITEM_PENALTY,
	FloorEnchantments::DISTORTION,
	FloorEnchantments::DAMAGE_DOWN,
	FloorEnchantments::FRAILTY,
	FloorEnchantments::GRUDGE
};

static const std::vector<FloorEnchantments> GROUP_THREE_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::GLOOM,
	FloorEnchantments::FEY
};

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


static const std::map<std::string, std::vector<std::pair<int, int>>> TRAP_SPAWN_POINTS = {
	{ "rm_mines_upper_floor1", {
		{ 224 + 8, 144 + 8 },
		{ 192 + 8, 208 + 8 },
		{ 176 + 8, 368 + 8 },
		{ 288 + 8, 352 + 8 },
		{ 384 + 8, 288 + 8 },
		{ 368 + 8, 208 + 8 },
	}},
	{ "rm_mines_upper_elevator5", {
		{ 64 + 8, 240 + 8 },
		{ 176 + 8, 192 + 8 },
		{ 272 + 8, 96 + 8 },
		{ 352 + 8, 64 + 8 },
		{ 400 + 8, 128 + 8 },
		{ 336 + 8, 160 + 8 },
	}},
	{ "rm_mines_upper_elevator10", {
		{ 352 + 8, 352 + 8 },
		{ 400 + 8, 208 + 8 },
		{ 320 + 8, 240 + 8 },
		{ 96 + 8, 256 + 8 },
		{ 112 + 8, 160 + 8 },
		{ 128 + 8, 80 + 8 },
	}},
	{ "rm_mines_upper_elevator15", {
		{ 80 + 8, 144 + 8 },
		{ 112 + 8, 288 + 8 },
		{ 224 + 8, 368 + 8 },
		{ 304 + 8, 128 + 8 },
		{ 352 + 8, 208 + 8 },
		{ 352 + 8, 352 + 8 },
	}},
	{ "rm_mines_upper_slimetime", {
		{ 352 + 8, 320 + 8 },
		{ 272 + 8, 432 + 8 },
		{ 192 + 8, 384 + 8 },
		{ 144 + 8, 320 + 8 },
		{ 176 + 8, 256 + 8 },
		{ 256 + 8, 208 + 8 },
	}},
	{ "rm_mines_upper_sock", {
		{ 208 + 8, 192 + 8 },
		{ 240 + 8, 256 + 8 },
		{ 240 + 8, 416 + 8 },
		{ 272 + 8, 496 + 8 },
		{ 352 + 8, 560 + 8 },
		{ 400 + 8, 496 + 8 },
	}},
	{ "rm_mines_upper_boomerang", {
		{ 304 + 8, 336 + 8 },
		{ 240 + 8, 304 + 8 },
		{ 128 + 8, 224 + 8 },
		{ 208 + 8, 160 + 8 },
		{ 272 + 8, 160 + 8 },
		{ 320 + 8, 128 + 8 },
	}},
	{ "rm_mines_upper_worm", {
		{ 112 + 8, 144 + 8 },
		{ 304 + 8, 160 + 8 },
		{ 384 + 8, 240 + 8 },
		{ 416 + 8, 336 + 8 },
		{ 400 + 8, 432 + 8 },
		{ 528 + 8, 464 + 8 },
	}},
	{ "rm_mines_upper_ponds", {
		{ 304 + 8, 432 + 8 },
		{ 384 + 8, 512 + 8 },
		{ 192 + 8, 368 + 8 },
		{ 368 + 8, 320 + 8 },
		{ 304 + 8, 240 + 8 },
		{ 352 + 8, 128 + 8 },
	}},
	{ "rm_mines_upper_pond", {
		{ 192 + 8, 400 + 8 },
		{ 192 + 8, 240 + 8 },
		{ 336 + 8, 144 + 8 },
		{ 448 + 8, 288 + 8 },
		{ 352 + 8, 384 + 8 },
		{ 416 + 8, 448 + 8 },
	}},
	{ "rm_mines_upper_snake", {
		{ 320 + 8, 144 + 8 },
		{ 192 + 8, 112 + 8 },
		{ 160 + 8, 336 + 8 },
		{ 400 + 8, 336 + 8 },
		{ 448 + 8, 448 + 8 },
		{ 336 + 8, 512 + 8 },
	}},
	{ "rm_mines_upper_four", {
		{ 144 + 8, 144 + 8 },
		{ 224 + 8, 368 + 8 },
		{ 384 + 8, 352 + 8 },
		{ 464 + 8, 272 + 8 },
		{ 464 + 8, 448 + 8 },
		{ 560 + 8, 512 + 8 },
	}},
	{ "rm_mines_upper_amoeba", {
		{ 272 + 8, 464 + 8 },
		{ 352 + 8, 400 + 8 },
		{ 512 + 8, 432 + 8 },
		{ 496 + 8, 304 + 8 },
		{ 528 + 8, 240 + 8 },
		{ 640 + 8, 256 + 8 },
	}},
	{ "rm_mines_upper_wishbone", {
		{ 256 + 8, 512 + 8 },
		{ 352 + 8, 464 + 8 },
		{ 336 + 8, 368 + 8 },
		{ 272 + 8, 240 + 8 },
		{ 400 + 8, 160 + 8 },
		{ 480 + 8, 256 + 8 },
	}},
	{ "rm_mines_upper_canada", {
		{ 192 + 8, 320 + 8 },
		{ 208 + 8, 208 + 8 },
		{ 336 + 8, 192 + 8 },
		{ 336 + 8, 272 + 8 },
		{ 448 + 8, 208 + 8 },
		{ 464 + 8, 336 + 8 },
	}},
	{ "rm_mines_upper_staple", {
		{ 496 + 8, 320 + 8 },
		{ 416 + 8, 208 + 8 },
		{ 336 + 8, 256 + 8 },
		{ 240 + 8, 192 + 8 },
		{ 144 + 8, 224 + 8 },
		{ 176 + 8, 368 + 8 },
	}},
	{ "rm_mines_upper_pillars", {
		{ 112 + 8, 48 + 8 },
		{ 336 + 8, 112 + 8 },
		{ 176 + 8, 208 + 8 },
		{ 48 + 8, 240 + 8 },
		{ 240 + 8, 336 + 8 },
		{ 368 + 8, 288 + 8 },
	}},
	{ "rm_mines_upper_path", {
		{ 208 + 8, 352 + 8 },
		{ 368 + 8, 256 + 8 },
		{ 384 + 8, 112 + 8 },
		{ 272 + 8, 192 + 8 },
		{ 160 + 8, 64 + 8 },
		{ 80 + 8, 224 + 8 },
	}},
	{ "rm_mines_upper_stream", {
		{ 320 + 8, 80 + 8 },
		{ 272 + 8, 160 + 8 },
		{ 192 + 8, 112 + 8 },
		{ 176 + 8, 288 + 8 },
		{ 208 + 8, 352 + 8 },
		{ 80 + 8, 352 + 8 },
	}},
	{ "rm_mines_upper_muscle", {
		{ 192 + 8, 544 + 8 },
		{ 272 + 8, 496 + 8 },
		{ 320 + 8, 416 + 8 },
		{ 400 + 8, 304 + 8 },
		{ 416 + 8, 224 + 8 },
		{ 336 + 8, 160 + 8 },
	}},
	{ "rm_mines_upper_crossroad", {
		{ 416 + 8, 208 + 8 },
		{ 144 + 8, 224 + 8 },
		{ 192 + 8, 304 + 8 },
		{ 240 + 8, 400 + 8 },
		{ 416 + 8, 432 + 8 },
		{ 208 + 8, 480 + 8 },
	}},
	{ "rm_mines_upper_pits", {
		{ 128 + 8, 320 + 8 },
		{ 160 + 8, 384 + 8 },
		{ 240 + 8, 400 + 8 },
		{ 256 + 8, 320 + 8 },
		{ 288 + 8, 480 + 8 },
		{ 368 + 8, 320 + 8 },
	}},
	{ "rm_mines_upper_formerelevator", {
		{ 80 + 8, 224 + 8 },
		{ 112 + 8, 64 + 8 },
		{ 176 + 8, 304 + 8 },
		{ 320 + 8, 96 + 8 },
		{ 368 + 8, 320 + 8 },
		{ 400 + 8, 192 + 8 },
	}},
	{ "rm_mines_tide_fork", {
		{ 128 + 8, 176 + 8 },
		{ 368 + 8, 352 + 8 },
		{ 592 + 8, 384 + 8 },
		{ 768 + 8, 240 + 8 },
		{ 512 + 8, 512 + 8 },
		{ 576 + 8, 640 + 8 },
	}},
	{ "rm_mines_tide_whirly", {
		{ 656 + 8, 512 + 8 },
		{ 464 + 8, 576 + 8 },
		{ 448 + 8, 448 + 8 },
		{ 224 + 8, 464 + 8 },
		{ 176 + 8, 448 + 8 },
		{ 672 + 8, 272 + 8 },
	}},
	{ "rm_mines_tide_islands", {
		{ 144 + 8, 384 + 8 },
		{ 336 + 8, 352 + 8 },
		{ 400 + 8, 352 + 8 },
		{ 368 + 8, 272 + 8 },
		{ 608 + 8, 272 + 8 },
		{ 576 + 8, 160 + 8 },
	}},
	{ "rm_mines_tide_paths", {
		{ 336 + 8, 448 + 8 },
		{ 512 + 8, 464 + 8 },
		{ 624 + 8, 432 + 8 },
		{ 640 + 8, 304 + 8 },
		{ 624 + 8, 112 + 8 },
		{ 368 + 8, 352 + 8 },
	}},
	{ "rm_mines_tide_milestone", {
		{ 128 + 8, 272 + 8 },
		{ 224 + 8, 272 + 8 },
		{ 176 + 8, 192 + 8 },
		{ 112 + 8, 112 + 8 },
		{ 176 + 8, 112 + 8 },
		{ 240 + 8, 112 + 8 },
	}},
	{ "rm_mines_tide_oasis", {
		{ 128 + 8, 240 + 8 },
		{ 176 + 8, 288 + 8 },
		{ 224 + 8, 208 + 8 },
		{ 256 + 8, 112 + 8 },
		{ 288 + 8, 256 + 8 },
		{ 352 + 8, 176 + 8 },
	}},
	{ "rm_mines_tide_chamber", {
		{ 224 + 8, 560 + 8 },
		{ 224 + 8, 384 + 8 },
		{ 304 + 8, 464 + 8 },
		{ 352 + 8, 336 + 8 },
		{ 352 + 8, 560 + 8 },
		{ 448 + 8, 480 + 8 },
	}},
	{ "rm_mines_tide_elevator25", {
		{ 272 + 8, 336 + 8 },
		{ 208 + 8, 432 + 8 },
		{ 256 + 8, 528 + 8 },
		{ 336 + 8, 496 + 8 },
		{ 416 + 8, 480 + 8 },
		{ 368 + 8, 400 + 8 },
	}},
	{ "rm_mines_tide_elevator30", {
		{ 224 + 8, 368 + 8 },
		{ 160 + 8, 464 + 8 },
		{ 208 + 8, 528 + 8 },
		{ 304 + 8, 528 + 8 },
		{ 336 + 8, 464 + 8 },
		{ 448 + 8, 384 + 8 },
	}},
	{ "rm_mines_tide_elevator35", {
		{ 352 + 8, 320 + 8 },
		{ 304 + 8, 336 + 8 },
		{ 352 + 8, 416 + 8 },
		{ 192 + 8, 368 + 8 },
		{ 176 + 8, 432 + 8 },
		{ 256 + 8, 512 + 8 },
	}},
	{ "rm_mines_tide_floor21", {
		{ 464 + 8, 208 + 8 },
		{ 336 + 8, 272 + 8 },
		{ 368 + 8, 368 + 8 },
		{ 224 + 8, 384 + 8 },
		{ 288 + 8, 432 + 8 },
		{ 192 + 8, 464 + 8 },
	}},
	{ "rm_mines_tide_basic1", {
		{ 576 + 8, 272 + 8 },
		{ 448 + 8, 336 + 8 },
		{ 432 + 8, 464 + 8 },
		{ 496 + 8, 560 + 8 },
		{ 656 + 8, 512 + 8 },
		{ 816 + 8, 496 + 8 },
	}},
	{ "rm_mines_tide_shrine1", {
		{ 304 + 8, 400 + 8 },
		{ 352 + 8, 224 + 8 },
		{ 480 + 8, 208 + 8 },
		{ 528 + 8, 288 + 8 },
		{ 480 + 8, 384 + 8 },
		{ 768 + 8, 320 + 8 },
	}},
	{ "rm_mines_tide_whirlpool1", {
		{ 864 + 8, 560 + 8 },
		{ 848 + 8, 400 + 8 },
		{ 832 + 8, 288 + 8 },
		{ 688 + 8, 224 + 8 },
		{ 672 + 8, 368 + 8 },
		{ 464 + 8, 384 + 8 },
	}},
	{ "rm_mines_tide_basic2", {
		{ 224 + 8, 208 + 8 },
		{ 192 + 8, 560 + 8 },
		{ 480 + 8, 352 + 8 },
		{ 608 + 8, 272 + 8 },
		{ 704 + 8, 432 + 8 },
		{ 512 + 8, 544 + 8 },
	}},
	{ "rm_mines_tide_switch2", {
		{ 240 + 8, 432 + 8 },
		{ 96 + 8, 592 + 8 },
		{ 320 + 8, 608 + 8 },
		{ 496 + 8, 656 + 8 },
		{ 416 + 8, 496 + 8 },
		{ 432 + 8, 128 + 8 },
	}},
	{ "rm_mines_tide_whirlpool2", {
		{ 368 + 8, 416 + 8 },
		{ 432 + 8, 368 + 8 },
		{ 512 + 8, 400 + 8 },
		{ 720 + 8, 592 + 8 },
		{ 832 + 8, 576 + 8 },
		{ 864 + 8, 640 + 8 },
	}},
	{ "rm_mines_tide_switch1", {
		{ 368 + 8, 304 + 8 },
		{ 288 + 8, 464 + 8 },
		{ 112 + 8, 368 + 8 },
		{ 176 + 8, 608 + 8 },
		{ 448 + 8, 624 + 8 },
		{ 496 + 8, 400 + 8 },
	}},
	{ "rm_mines_tide_shrine3", {
		{ 656 + 8, 544 + 8 },
		{ 560 + 8, 464 + 8 },
		{ 416 + 8, 400 + 8 },
		{ 432 + 8, 208 + 8 },
		{ 624 + 8, 176 + 8 },
		{ 704 + 8, 240 + 8 },
	}},
	{ "rm_mines_tide_shrine2", {
		{ 144 + 8, 512 + 8 },
		{ 176 + 8, 608 + 8 },
		{ 464 + 8, 352 + 8 },
		{ 528 + 8, 432 + 8 },
		{ 624 + 8, 352 + 8 },
		{ 688 + 8, 160 + 8 },
	}},
	{ "rm_mines_tide_whirlpool3", {
		{ 96 + 8, 624 + 8 },
		{ 192 + 8, 528 + 8 },
		{ 384 + 8, 400 + 8 },
		{ 560 + 8, 416 + 8 },
		{ 800 + 8, 208 + 8 },
		{ 832 + 8, 144 + 8 },
	}},
	{ "rm_mines_tide_basic3", {
		{ 192 + 8, 544 + 8 },
		{ 336 + 8, 560 + 8 },
		{ 560 + 8, 560 + 8 },
		{ 704 + 8, 416 + 8 },
		{ 624 + 8, 272 + 8 },
		{ 464 + 8, 304 + 8 },
	}},
	{ "rm_mines_tide_basic4", {
		{ 144 + 8, 320 + 8 },
		{ 160 + 8, 496 + 8 },
		{ 304 + 8, 560 + 8 },
		{ 480 + 8, 336 + 8 },
		{ 688 + 8, 352 + 8 },
		{ 672 + 8, 528 + 8 },
	}},
	{ "rm_mines_tide_whirlpool4", {
		{ 208 + 8, 672 + 8 },
		{ 176 + 8, 528 + 8 },
		{ 96 + 8, 448 + 8 },
		{ 160 + 8, 336 + 8 },
		{ 448 + 8, 224 + 8 },
		{ 864 + 8, 160 + 8 },
	}},
	{ "rm_mines_tide_switch3", {
		{ 224 + 8, 112 + 8 },
		{ 416 + 8, 112 + 8 },
		{ 224 + 8, 448 + 8 },
		{ 160 + 8, 608 + 8 },
		{ 400 + 8, 544 + 8 },
		{ 480 + 8, 656 + 8 },
	}},
	{ "rm_mines_tide_switch4", {
		{ 128 + 8, 208 + 8 },
		{ 240 + 8, 192 + 8 },
		{ 192 + 8, 368 + 8 },
		{ 160 + 8, 608 + 8 },
		{ 400 + 8, 560 + 8 },
		{ 384 + 8, 672 + 8 },
	}},
	{ "rm_mines_deep_runner", {
		{ 496 + 8, 128 + 8 },
		{ 640 + 8, 176 + 8 },
		{ 720 + 8, 112 + 8 },
		{ 896 + 8, 128 + 8 },
		{ 992 + 8, 176 + 8 },
		{ 1248 + 8, 144 + 8 },
	}},
	{ "rm_mines_deep_winding", {
		{ 464 + 8, 384 + 8 },
		{ 224 + 8, 560 + 8 },
		{ 528 + 8, 656 + 8 },
		{ 608 + 8, 400 + 8 },
		{ 544 + 8, 256 + 8 },
		{ 704 + 8, 112 + 8 },
	}},
	{ "rm_mines_deep_leap", {
		{ 448 + 8, 192 + 8 },
		{ 272 + 8, 288 + 8 },
		{ 144 + 8, 480 + 8 },
		{ 544 + 8, 432 + 8 },
		{ 432 + 8, 544 + 8 },
		{ 512 + 8, 592 + 8 },
	}},
	{ "rm_mines_deep_chambers", {
		{ 128 + 8, 432 + 8 },
		{ 304 + 8, 400 + 8 },
		{ 336 + 8, 176 + 8 },
		{ 464 + 8, 512 + 8 },
		{ 368 + 8, 768 + 8 },
		{ 400 + 8, 896 + 8 },
	}},
	{ "rm_mines_deep_spiral", {
		{ 144 + 8, 560 + 8 },
		{ 160 + 8, 384 + 8 },
		{ 352 + 8, 128 + 8 },
		{ 464 + 8, 256 + 8 },
		{ 720 + 8, 432 + 8 },
		{ 608 + 8, 496 + 8 },
	}},
	{ "rm_mines_deep_wishbone", {
		{ 768 + 8, 512 + 8 },
		{ 736 + 8, 320 + 8 },
		{ 592 + 8, 368 + 8 },
		{ 448 + 8, 432 + 8 },
		{ 464 + 8, 224 + 8 },
		{ 240 + 8, 304 + 8 },
	}},
	{ "rm_mines_deep_key", {
		{ 240 + 8, 288 + 8 },
		{ 400 + 8, 400 + 8 },
		{ 624 + 8, 432 + 8 },
		{ 832 + 8, 336 + 8 },
		{ 768 + 8, 576 + 8 },
		{ 1184 + 8, 272 + 8 },
	}},
	{ "rm_mines_deep_scorpio", {
		{ 240 + 8, 368 + 8 },
		{ 304 + 8, 272 + 8 },
		{ 384 + 8, 368 + 8 },
		{ 512 + 8, 304 + 8 },
		{ 704 + 8, 496 + 8 },
		{ 784 + 8, 416 + 8 },
	}},
	{ "rm_mines_deep_butterfly", {
		{ 640 + 8, 240 + 8 },
		{ 528 + 8, 256 + 8 },
		{ 160 + 8, 240 + 8 },
		{ 240 + 8, 528 + 8 },
		{ 400 + 8, 464 + 8 },
		{ 576 + 8, 528 + 8 },
	}},
	{ "rm_mines_deep_41", {
		{ 432 + 8, 176 + 8 },
		{ 256 + 8, 272 + 8 },
		{ 320 + 8, 384 + 8 },
		{ 416 + 8, 384 + 8 },
		{ 512 + 8, 448 + 8 },
		{ 560 + 8, 352 + 8 },
	}},
	{ "rm_mines_deep_45", {
		{ 368 + 8, 352 + 8 },
		{ 512 + 8, 336 + 8 },
		{ 544 + 8, 240 + 8 },
		{ 672 + 8, 272 + 8 },
		{ 624 + 8, 384 + 8 },
		{ 576 + 8, 528 + 8 },
	}},
	{ "rm_mines_deep_50", {
		{ 656 + 8, 368 + 8 },
		{ 512 + 8, 448 + 8 },
		{ 416 + 8, 512 + 8 },
		{ 384 + 8, 304 + 8 },
		{ 448 + 8, 224 + 8 },
		{ 512 + 8, 288 + 8 },
	}},
	{ "rm_mines_deep_55", {
		{ 592 + 8, 464 + 8 },
		{ 736 + 8, 448 + 8 },
		{ 832 + 8, 416 + 8 },
		{ 832 + 8, 224 + 8 },
		{ 688 + 8, 144 + 8 },
		{ 528 + 8, 256 + 8 },
	}},
	{ "rm_mines_deep_switch1", {
		{ 464 + 8, 368 + 8 },
		{ 400 + 8, 208 + 8 },
		{ 272 + 8, 160 + 8 },
		{ 208 + 8, 368 + 8 },
		{ 288 + 8, 432 + 8 },
		{ 400 + 8, 592 + 8 },
	}},
	{ "rm_mines_deep_switch2", {
		{ 208 + 8, 288 + 8 },
		{ 192 + 8, 624 + 8 },
		{ 368 + 8, 624 + 8 },
		{ 352 + 8, 416 + 8 },
		{ 448 + 8, 432 + 8 },
		{ 464 + 8, 336 + 8 },
	}},
	{ "rm_mines_deep_switch3", {
		{ 672 + 8, 608 + 8 },
		{ 512 + 8, 416 + 8 },
		{ 576 + 8, 352 + 8 },
		{ 672 + 8, 416 + 8 },
		{ 752 + 8, 240 + 8 },
		{ 816 + 8, 288 + 8 },
	}},
	{ "rm_mines_deep_switch4", {
		{ 704 + 8, 528 + 8 },
		{ 768 + 8, 368 + 8 },
		{ 624 + 8, 448 + 8 },
		{ 496 + 8, 384 + 8 },
		{ 432 + 8, 416 + 8 },
		{ 384 + 8, 624 + 8 },
	}},
	{ "rm_mines_deep_whirlpool1", {
		{ 528 + 8, 192 + 8 },
		{ 640 + 8, 560 + 8 },
		{ 576 + 8, 624 + 8 },
		{ 464 + 8, 656 + 8 },
		{ 384 + 8, 624 + 8 },
		{ 304 + 8, 560 + 8 },
	}},
	{ "rm_mines_deep_whirlpool2", {
		{ 272 + 8, 224 + 8 },
		{ 160 + 8, 240 + 8 },
		{ 256 + 8, 560 + 8 },
		{ 384 + 8, 624 + 8 },
		{ 592 + 8, 624 + 8 },
		{ 752 + 8, 608 + 8 },
	}},
	{ "rm_mines_deep_shrine2", {
		{ 304 + 8, 480 + 8 },
		{ 368 + 8, 384 + 8 },
		{ 480 + 8, 368 + 8 },
		{ 608 + 8, 464 + 8 },
		{ 656 + 8, 560 + 8 },
		{ 496 + 8, 640 + 8 },
	}},
	{ "rm_mines_deep_shrine1", {
		{ 480 + 8, 240 + 8 },
		{ 592 + 8, 320 + 8 },
		{ 592 + 8, 400 + 8 },
		{ 640 + 8, 464 + 8 },
		{ 800 + 8, 464 + 8 },
		{ 752 + 8, 256 + 8 },
	}},
	{ "rm_mines_deep_shrine3", {
		{ 320 + 8, 272 + 8 },
		{ 384 + 8, 368 + 8 },
		{ 464 + 8, 256 + 8 },
		{ 640 + 8, 272 + 8 },
		{ 576 + 8, 336 + 8 },
		{ 592 + 8, 448 + 8 },
	}},
	{ "rm_mines_deep_shrine4", {
		{ 400 + 8, 512 + 8 },
		{ 288 + 8, 480 + 8 },
		{ 336 + 8, 384 + 8 },
		{ 192 + 8, 272 + 8 },
		{ 560 + 8, 192 + 8 },
		{ 496 + 8, 144 + 8 },
	}},
	{ "rm_mines_lava_bullseye", {
		{ 96 + 8, 256 + 8 },
		{ 192 + 8, 240 + 8 },
		{ 240 + 8, 288 + 8 },
		{ 256 + 8, 192 + 8 },
		{ 304 + 8, 240 + 8 },
		{ 384 + 8, 256 + 8 },
		{ 272 + 8, 368 + 8 },
	}},
	{ "rm_mines_lava_generic", {
		{ 256 + 8, 560 + 8 },
		{ 480 + 8, 560 + 8 },
		{ 576 + 8, 512 + 8 },
		{ 816 + 8, 512 + 8 },
		{ 448 + 8, 288 + 8 },
		{ 496 + 8, 208 + 8 },
		{ 192 + 8, 464 + 8 },
	}},
	{ "rm_mines_lava_world", {
		{ 192 + 8, 160 + 8 },
		{ 448 + 8, 160 + 8 },
		{ 480 + 8, 224 + 8 },
		{ 208 + 8, 448 + 8 },
		{ 432 + 8, 352 + 8 },
		{ 496 + 8, 464 + 8 },
		{ 416 + 8, 256 + 8 }, 
	}},
	{ "rm_mines_lava_elements", {
		{ 352 + 8, 576 + 8 },
		{ 352 + 8, 432 + 8 },
		{ 288 + 8, 368 + 8 },
		{ 368 + 8, 352 + 8 },
		{ 448 + 8, 368 + 8 },
		{ 368 + 8, 192 + 8 },
		{ 368 + 8, 288 + 8 },
	}},
	{ "rm_mines_lava_arena", {
		{ 400 + 8, 384 + 8 },
		{ 240 + 8, 384 + 8 },
		{ 544 + 8, 416 + 8 },
		{ 160 + 8, 576 + 8 },
		{ 384 + 8, 640 + 8 },
		{ 544 + 8, 576 + 8 },
		{ 336 + 8, 320 + 8 },
	}},
	{ "rm_mines_lava_61", {
		{ 144 + 8, 224 + 8 },
		{ 400 + 8, 240 + 8 },
		{ 480 + 8, 256 + 8 },
		{ 304 + 8, 352 + 8 },
		{ 272 + 8, 432 + 8 },
		{ 368 + 8, 416 + 8 },
		{ 464 + 8, 336 + 8 },
	}},
	{ "rm_mines_lava_65", {
		{ 384 + 8, 176 + 8 },
		{ 352 + 8, 288 + 8 },
		{ 352 + 8, 400 + 8 },
		{ 464 + 8, 432 + 8 },
		{ 528 + 8, 416 + 8 },
		{ 624 + 8, 288 + 8 },
		{ 560 + 8, 192 + 8 },
	}},
	{ "rm_mines_lava_70", {
		{ 384 + 8, 208 + 8 },
		{ 368 + 8, 368 + 8 },
		{ 496 + 8, 416 + 8 },
		{ 592 + 8, 288 + 8 },
		{ 656 + 8, 208 + 8 },
		{ 576 + 8, 160 + 8 },
		{ 592 + 8, 384 + 8 },
	}},
	{ "rm_mines_lava_75", {
		{ 544 + 8, 144 + 8 },
		{ 400 + 8, 160 + 8 },
		{ 416 + 8, 256 + 8 },
		{ 448 + 8, 320 + 8 },
		{ 560 + 8, 352 + 8 },
		{ 640 + 8, 208 + 8 },
		{ 480 + 8, 400 + 8 },
	}},
	{ "rm_mines_lava_switch1", {
		{ 352 + 8, 384 + 8 },
		{ 576 + 8, 400 + 8 },
		{ 704 + 8, 432 + 8 },
		{ 768 + 8, 528 + 8 },
		{ 768 + 8, 368 + 8 },
		{ 784 + 8, 288 + 8 },
		{ 528 + 8, 304 + 8 },
	}},
	{ "rm_mines_lava_switch2", {
		{ 256 + 8, 480 + 8 },
		{ 304 + 8, 416 + 8 },
		{ 432 + 8, 432 + 8 },
		{ 544 + 8, 400 + 8 },
		{ 528 + 8, 336 + 8 },
		{ 416 + 8, 256 + 8 },
		{ 656 + 8, 400 + 8 },
	}},
	{ "rm_mines_lava_switch3", {
		{ 272 + 8, 192 + 8 },
		{ 512 + 8, 256 + 8 },
		{ 416 + 8, 304 + 8 },
		{ 544 + 8, 416 + 8 },
		{ 464 + 8, 496 + 8 },
		{ 272 + 8, 560 + 8 },
		{ 256 + 8, 288 + 8 },
	}},
	{ "rm_mines_lava_switch4", {
		{ 464 + 8, 192 + 8 },
		{ 368 + 8, 272 + 8 },
		{ 288 + 8, 304 + 8 },
		{ 448 + 8, 368 + 8 },
		{ 576 + 8, 432 + 8 },
		{ 320 + 8, 592 + 8 },
		{ 368 + 8, 496 + 8 },
	}},
	{ "rm_mines_lava_switch5", {
		{ 512 + 8, 144 + 8 },
		{ 256 + 8, 352 + 8 },
		{ 512 + 8, 336 + 8 },
		{ 720 + 8, 384 + 8 },
		{ 672 + 8, 544 + 8 },
		{ 528 + 8, 656 + 8 },
		{ 496 + 8, 80 + 8 },
	}},
	{ "rm_mines_lava_basic1", {
		{ 768 + 8, 176 + 8 },
		{ 880 + 8, 320 + 8 },
		{ 736 + 8, 320 + 8 },
		{ 720 + 8, 432 + 8 },
		{ 800 + 8, 480 + 8 },
		{ 224 + 8, 512 + 8 },
		{ 624 + 8, 368 + 8 },
	}},
	{ "rm_mines_lava_basic2", {
		{ 336 + 8, 240 + 8 },
		{ 224 + 8, 416 + 8 },
		{ 336 + 8, 480 + 8 },
		{ 432 + 8, 384 + 8 },
		{ 240 + 8, 656 + 8 },
		{ 800 + 8, 400 + 8 },
		{ 336 + 8, 368 + 8 },
	}},
	{ "rm_mines_lava_basic3", {
		{ 320 + 8, 240 + 8 },
		{ 448 + 8, 368 + 8 },
		{ 256 + 8, 464 + 8 },
		{ 352 + 8, 480 + 8 },
		{ 288 + 8, 656 + 8 },
		{ 368 + 8, 640 + 8 },
		{ 224 + 8, 384 + 8 },
	}},
	{ "rm_mines_lava_basic4", {
		{ 432 + 8, 400 + 8 },
		{ 544 + 8, 320 + 8 },
		{ 640 + 8, 416 + 8 },
		{ 784 + 8, 160 + 8 },
		{ 752 + 8, 400 + 8 },
		{ 736 + 8, 592 + 8 },
		{ 528 + 8, 160 + 8 },
	}},
	{ "rm_mines_lava_basic5", {
		{ 304 + 8, 304 + 8 },
		{ 528 + 8, 176 + 8 },
		{ 560 + 8, 256 + 8 },
		{ 544 + 8, 400 + 8 },
		{ 544 + 8, 560 + 8 },
		{ 736 + 8, 288 + 8 },
		{ 688 + 8, 432 + 8 },
	}},
	{ "rm_mines_lava_shrine1", {
		{ 272 + 8, 416 + 8 },
		{ 368 + 8, 384 + 8 },
		{ 528 + 8, 320 + 8 },
		{ 672 + 8, 368 + 8 },
		{ 752 + 8, 352 + 8 },
		{ 848 + 8, 272 + 8 },
		{ 544 + 8, 416 + 8 },
	}},
	{ "rm_mines_lava_shrine2", {
		{ 288 + 8, 224 + 8 },
		{ 384 + 8, 208 + 8 },
		{ 480 + 8, 192 + 8 },
		{ 608 + 8, 208 + 8 },
		{ 688 + 8, 208 + 8 },
		{ 720 + 8, 336 + 8 },
		{ 752 + 8, 544 + 8 },
	}},
	{ "rm_mines_lava_shrine3", {
		{ 256 + 8, 176 + 8 },
		{ 352 + 8, 288 + 8 },
		{ 352 + 8, 400 + 8 },
		{ 176 + 8, 464 + 8 },
		{ 336 + 8, 512 + 8 },
		{ 272 + 8, 560 + 8 },
		{ 352 + 8, 144 + 8 },
	}},
	{ "rm_mines_lava_shrine4", {
		{ 288 + 8, 160 + 8 },
		{ 304 + 8, 240 + 8 },
		{ 288 + 8, 544 + 8 },
		{ 464 + 8, 336 + 8 },
		{ 640 + 8, 320 + 8 },
		{ 608 + 8, 608 + 8 },
		{ 464 + 8, 592 + 8 },
	}},
	{ "rm_mines_ruins_basic1", {
		{ 432 + 8, 224 + 8 },
		{ 528 + 8, 208 + 8 },
		{ 224 + 8, 544 + 8 },
		{ 400 + 8, 528 + 8 },
		{ 480 + 8, 384 + 8 },
		{ 480 + 8, 608 + 8 },
		{ 592 + 8, 512 + 8 },
		{ 704 + 8, 528 + 8 },
	}},
	{ "rm_mines_ruins_85", {
		{ 576 + 8, 192 + 8 },
		{ 320 + 8, 560 + 8 },
		{ 448 + 8, 528 + 8 },
		{ 480 + 8, 384 + 8 },
		{ 592 + 8, 352 + 8 },
		{ 688 + 8, 384 + 8 },
		{ 672 + 8, 528 + 8 },
		{ 848 + 8, 528 + 8 },
	}},
	{ "rm_mines_ruins_large_switch", {
		{ 480 + 8, 176 + 8 },
		{ 448 + 8, 400 + 8 },
		{ 496 + 8, 544 + 8 },
		{ 640 + 8, 544 + 8 },
		{ 720 + 8, 304 + 8 },
		{ 784 + 8, 544 + 8 },
		{ 832 + 8, 256 + 8 },
		{ 880 + 8, 480 + 8 },
	}},
	{ "rm_mines_ruins_81", {
		{ 176 + 8, 304 + 8 },
		{ 272 + 8, 352 + 8 },
		{ 448 + 8, 336 + 8 },
		{ 544 + 8, 352 + 8 },
		{ 608 + 8, 400 + 8 },
		{ 704 + 8, 336 + 8 },
		{ 880 + 8, 304 + 8 },
		{ 960 + 8, 352 + 8 },
	}},
	{ "rm_mines_ruins_95", {
		{ 576 + 8, 192 + 8 },
		{ 304 + 8, 528 + 8 },
		{ 448 + 8, 544 + 8 },
		{ 560 + 8, 496 + 8 },
		{ 608 + 8, 416 + 8 },
		{ 720 + 8, 544 + 8 },
		{ 864 + 8, 512 + 8 },
		{ 576 + 8, 656 + 8 },
	}},
	{ "rm_mines_ruins_arena1", {
		{ 448 + 8, 288 + 8 },
		{ 560 + 8, 304 + 8 },
		{ 688 + 8, 272 + 8 },
		{ 496 + 8, 368 + 8 },
		{ 656 + 8, 384 + 8 },
		{ 576 + 8, 448 + 8 },
		{ 464 + 8, 480 + 8 },
		{ 704 + 8, 464 + 8 },
	}},
	{ "rm_mines_ruins_basic2", {
		{ 240 + 8, 288 + 8 },
		{ 256 + 8, 208 + 8 },
		{ 432 + 8, 208 + 8 },
		{ 544 + 8, 176 + 8 },
		{ 624 + 8, 224 + 8 },
		{ 784 + 8, 208 + 8 },
		{ 784 + 8, 304 + 8 },
		{ 528 + 8, 352 + 8 },
	}},
	{ "rm_mines_ruins_basic3", {
		{ 384 + 8, 496 + 8 },
		{ 416 + 8, 592 + 8 },
		{ 592 + 8, 464 + 8 },
		{ 608 + 8, 624 + 8 },
		{ 752 + 8, 576 + 8 },
		{ 816 + 8, 480 + 8 },
		{ 720 + 8, 336 + 8 },
		{ 672 + 8, 192 + 8 },
	}},
	{ "rm_mines_ruins_basic4", {
		{ 496 + 8, 112 + 8 },
		{ 672 + 8, 128 + 8 },
		{ 592 + 8, 176 + 8 },
		{ 752 + 8, 240 + 8 },
		{ 448 + 8, 288 + 8 },
		{ 688 + 8, 304 + 8 },
		{ 480 + 8, 672 + 8 },
		{ 624 + 8, 672 + 8 },
	}},
	{ "rm_mines_ruins_basic5", {
		{ 656 + 8, 304 + 8 },
		{ 464 + 8, 384 + 8 },
		{ 720 + 8, 384 + 8 },
		{ 384 + 8, 464 + 8 },
		{ 288 + 8, 496 + 8 },
		{ 496 + 8, 496 + 8 },
		{ 496 + 8, 624 + 8 },
		{ 576 + 8, 656 + 8 },
	}},
	{ "rm_mines_ruins_switch2", {
		{ 192 + 8, 160 + 8 },
		{ 528 + 8, 192 + 8 },
		{ 624 + 8, 144 + 8 },
		{ 272 + 8, 368 + 8 },
		{ 672 + 8, 384 + 8 },
		{ 192 + 8, 512 + 8 },
		{ 448 + 8, 512 + 8 },
		{ 608 + 8, 560 + 8 },
	}},
	{ "rm_mines_ruins_switch3", {
		{ 336 + 8, 160 + 8 },
		{ 320 + 8, 256 + 8 },
		{ 528 + 8, 272 + 8 },
		{ 624 + 8, 304 + 8 },
		{ 304 + 8, 480 + 8 },
		{ 192 + 8, 528 + 8 },
		{ 416 + 8, 512 + 8 },
		{ 320 + 8, 576 + 8 },
	}},
	{ "rm_mines_ruins_switch4", {
		{ 128 + 8, 272 + 8 },
		{ 592 + 8, 208 + 8 },
		{ 736 + 8, 224 + 8 },
		{ 160 + 8, 384 + 8 },
		{ 720 + 8, 384 + 8 },
		{ 128 + 8, 512 + 8 },
		{ 448 + 8, 544 + 8 },
		{ 736 + 8, 544 + 8 },
	}},
	{ "rm_mines_ruins_switch5", {
		{ 112 + 8, 192 + 8 },
		{ 272 + 8, 176 + 8 },
		{ 352 + 8, 240 + 8 },
		{ 496 + 8, 240 + 8 },
		{ 672 + 8, 256 + 8 },
		{ 448 + 8, 384 + 8 },
		{ 544 + 8, 416 + 8 },
		{ 624 + 8, 480 + 8 },
	}},
	{ "rm_mines_ruins_arena2", {
		{ 432 + 8, 304 + 8 },
		{ 592 + 8, 256 + 8 },
		{ 576 + 8, 336 + 8 },
		{ 720 + 8, 304 + 8 },
		{ 448 + 8, 432 + 8 },
		{ 432 + 8, 496 + 8 },
		{ 704 + 8, 432 + 8 },
		{ 736 + 8, 496 + 8 },
	}},
	{ "rm_mines_ruins_whirlpool1", {
		{ 272 + 8, 256 + 8 },
		{ 496 + 8, 272 + 8 },
		{ 240 + 8, 448 + 8 },
		{ 304 + 8, 512 + 8 },
		{ 480 + 8, 512 + 8 },
		{ 512 + 8, 448 + 8 },
		{ 784 + 8, 336 + 8 },
		{ 784 + 8, 448 + 8 },
	}},
	{ "rm_mines_ruins_whirlpool2", {
		{ 272 + 8, 416 + 8 },
		{ 464 + 8, 384 + 8 },
		{ 480 + 8, 464 + 8 },
		{ 608 + 8, 368 + 8 },
		{ 608 + 8, 512 + 8 },
		{ 560 + 8, 592 + 8 },
		{ 576 + 8, 688 + 8 },
		{ 688 + 8, 688 + 8 },
	}},
	{ "rm_mines_ruins_whirlpool3", {
		{ 416 + 8, 240 + 8 },
		{ 352 + 8, 336 + 8 },
		{ 400 + 8, 416 + 8 },
		{ 496 + 8, 304 + 8 },
		{ 592 + 8, 240 + 8 },
		{ 640 + 8, 336 + 8 },
		{ 576 + 8, 384 + 8 },
		{ 608 + 8, 464 + 8 },
	}},
	{ "rm_mines_ruins_whirlpool4", {
		{ 320 + 8, 128 + 8 },
		{ 512 + 8, 96 + 8 },
		{ 576 + 8, 224 + 8 },
		{ 688 + 8, 352 + 8 },
		{ 752 + 8, 416 + 8 },
		{ 816 + 8, 224 + 8 },
		{ 832 + 8, 368 + 8 },
		{ 848 + 8, 496 + 8 },
	}},
	{ "rm_mines_ruins_shrine1", {
		{ 544 + 8, 224 + 8 },
		{ 688 + 8, 208 + 8 },
		{ 832 + 8, 176 + 8 },
		{ 544 + 8, 352 + 8 },
		{ 688 + 8, 352 + 8 },
		{ 832 + 8, 352 + 8 },
		{ 496 + 8, 512 + 8 },
		{ 688 + 8, 496 + 8 },
	}},
	{ "rm_mines_ruins_whirlpool5", {
		{ 464 + 8, 192 + 8 },
		{ 608 + 8, 160 + 8 },
		{ 736 + 8, 240 + 8 },
		{ 720 + 8, 336 + 8 },
		{ 736 + 8, 592 + 8 },
		{ 672 + 8, 672 + 8 },
		{ 512 + 8, 688 + 8 },
		{ 368 + 8, 656 + 8 },
	} },
	{ "rm_mines_ruins_shrine2", {
		{ 448 + 8, 272 + 8 },
		{ 512 + 8, 288 + 8 },
		{ 256 + 8, 432 + 8 },
		{ 240 + 8, 608 + 8 },
		{ 512 + 8, 480 + 8 },
		{ 432 + 8, 608 + 8 },
		{ 608 + 8, 624 + 8 },
		{ 736 + 8, 608 + 8 },
	} },
	{ "rm_mines_ruins_shrine3", {
		{ 464 + 8, 224 + 8 },
		{ 432 + 8, 336 + 8 },
		{ 608 + 8, 256 + 8 },
		{ 720 + 8, 256 + 8 },
		{ 400 + 8, 512 + 8 },
		{ 480 + 8, 464 + 8 },
		{ 608 + 8, 496 + 8 },
		{ 720 + 8, 432 + 8 },
	} },
	{ "rm_mines_ruins_shrine4", {
		{ 336 + 8, 80 + 8 },
		{ 464 + 8, 128 + 8 },
		{ 640 + 8, 80 + 8 },
		{ 320 + 8, 288 + 8 },
		{ 480 + 8, 256 + 8 },
		{ 624 + 8, 272 + 8 },
		{ 336 + 8, 416 + 8 },
		{ 544 + 8, 400 + 8 },
	} },
	{ "rm_mines_ruins_shrine5", {
		{ 496 + 8, 176 + 8 },
		{ 448 + 8, 288 + 8 },
		{ 608 + 8, 304 + 8 },
		{ 672 + 8, 352 + 8 },
		{ 496 + 8, 432 + 8 },
		{ 528 + 8, 512 + 8 },
		{ 624 + 8, 576 + 8 },
		{ 752 + 8, 480 + 8 },
	} },
	{ "rm_mines_ruins_arena3", {
		{ 496 + 8, 272 + 8 },
		{ 656 + 8, 272 + 8 },
		{ 448 + 8, 352 + 8 },
		{ 528 + 8, 352 + 8 },
		{ 624 + 8, 336 + 8 },
		{ 704 + 8, 368 + 8 },
		{ 624 + 8, 432 + 8 },
		{ 512 + 8, 496 + 8 },
	} },
	//{ "", {
	//	{ 0 + 8, 0 + 8 },
	//	{ 0 + 8, 0 + 8 },
	//	{ 0 + 8, 0 + 8 },
	//	{ 0 + 8, 0 + 8 },
	//	{ 0 + 8, 0 + 8 },
	//	{ 0 + 8, 0 + 8 },
	//}},
};

// TODO: Add new songs when updated
static const std::vector<std::string> MUSIC_INTERNAL_NAMES = { // As of 0.15.1
	"Music/Crystal Tracks/BarleyMoon",
	"Music/Crystal Tracks/CrystalCaves",
	"Music/Crystal Tracks/PinkTwintails",
	"Music/Events/DragonCutscene",
	"Music/Events/LightAndShadow",
	"Music/Events/SharedLight",
	"Music/Events/ShootingStarNight",
	"Music/Events/TheSeal",
	"Music/Events/ThemeOfDarkness",
	"Music/Events/TsukihotaruFinale",
	"Music/Location Tracks/AnimalFestival",
	"Music/Location Tracks/Bathhouse",
	"Music/Location Tracks/Blacksmith",
	"Music/Location Tracks/Carpenter",
	"Music/Location Tracks/Clinic",
	"Music/Location Tracks/Deep Woods",
	"Music/Location Tracks/Festival",
	"Music/Location Tracks/General Store",
	"Music/Location Tracks/HarvestFestivalTheme",
	"Music/Location Tracks/InnLessBusy",
	"Music/Location Tracks/InnMoreBusy",
	"Music/Location Tracks/MinesEntry",
	"Music/Location Tracks/Player Home/Day",
	"Music/Location Tracks/Player Home/Night",
	"Music/Location Tracks/SeridiaVoidRoom",
	"Music/Location Tracks/ShootingStarFestival",
	"Music/Location Tracks/SpringFestival",
	"Music/Location Tracks/TheFinalSeal",
	"Music/Location Tracks/VoidSeal",
	"Music/Menu Tracks/Tsukihotaru",
	"Music/Npc Tracks/Adeline",
	"Music/Npc Tracks/Balor",
	"Music/Npc Tracks/Caldarus",
	"Music/Npc Tracks/Celine",
	"Music/Npc Tracks/Eiland",
	"Music/Npc Tracks/Hayden",
	"Music/Npc Tracks/Juniper",
	"Music/Npc Tracks/March",
	"Music/Npc Tracks/Reina",
	"Music/Npc Tracks/Ryis",
	"Music/Npc Tracks/Seridia",
	"Music/Npc Tracks/Valen",
	"Music/Playlists/MinesDeepEarth",
	"Music/Playlists/MinesLavaCaves",
	"Music/Playlists/MinesTideCaverns",
	"Music/Playlists/MinesUpper",
	"Music/Playlists/MinesRuins",
	"Music/Playlists/Spring",
	"Music/Playlists/Spring Rain",
	"Music/Playlists/Summer",
	"Music/Playlists/Fall",
	"Music/Playlists/Winter",
	"Music/Playlists/Winter Snow"
};

extern YYTKInterface* g_ModuleInterface;
extern CInstance* global_instance;
extern RValue __YYTK;
extern bool load_on_start;
extern bool is_new_game;
extern bool localize_mod_text;
extern bool game_is_active;
extern bool unlock_recipes;
extern bool crafting_menu_open;
extern bool journal_menu_open;
extern bool drop_biome_reward;
extern bool biome_reward_disabled;
extern bool dread_beast_configured;
extern bool sigil_item_used;
extern bool greater_sigil_item_used;
extern bool salve_item_used;
extern bool lift_key_used;
extern bool orb_item_used;
extern bool heart_crystal_used;
extern bool inner_fire_cast;
extern bool reckoning_applied;
extern bool fairy_buff_applied;
extern bool stoneskin_applied;
extern bool is_restoration_tracked_interval;
extern bool is_second_wind_tracked_interval;
extern bool is_fumigate_tracked_interval;
extern bool is_deep_wounds_tracked_interval;
extern bool offering_chance_occurred;
extern bool obj_dragonshrine_focused;
extern bool obj_dungeon_elevator_focused;
extern bool obj_dungeon_ladder_down_focused;
extern double ari_x;
extern double ari_y;
extern double ari_facing_dir;
extern int floor_number;
extern int unmodified_base_health;
extern int hp_penalty_amount;
extern int floor_start_time;
extern int current_time_in_seconds;
extern int time_of_last_restoration_tick;
extern int time_of_last_second_wind_tick;
extern int time_of_last_fumigate_tick;
extern int time_of_last_deep_wounds_tick;
extern int time_of_last_outbreak_tick;
extern int held_item_id;
extern int frailty_hit_counter;
extern int grudge_counter;
extern int deep_wounds_damage_pool;
extern int stoneskin_shield_amount;
extern int spirit_link_combined_health_pool;
extern int sigil_of_silence_count;
extern int sigil_of_alteration_monster_id;
extern int dread_beast_monster_id;
extern int dread_beasts_configured;
extern int boss_monsters_configured;
extern BossBattle boss_battle;
extern TreasureSpot treasure_spot;
extern std::string ari_current_location;
extern std::string ari_current_gm_room;
extern std::unordered_set<int> orb_items;
extern std::unordered_set<int> salve_items;
extern std::unordered_set<int> lift_key_items;
extern std::unordered_set<int> restricted_items;
extern std::unordered_set<int> deep_dungeon_items;
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
extern std::map<std::string, int> bark_name_to_id_map;
extern std::unordered_set<std::pair<int, int>, pair_hash> floor_trap_positions;
extern std::map<std::string, int> salves_used; // TODO
extern std::map<Traps, std::pair<int, int>> active_traps; // Holds the active traps and the position they most recently triggered at.
extern std::vector<CustomAOE> meteor_aoes;
extern std::vector<CustomAOE> gaze_aoes;
extern std::vector<CustomAOE> void_aoes;
extern std::vector<RevealedFloorTrap> revealed_floor_traps;
extern std::unordered_set<Sigils> active_sigils;
extern std::unordered_set<GreaterSigils> active_greater_sigils;
extern std::unordered_set<Offerings> queued_offerings;
extern std::unordered_set<Offerings> active_offerings;
extern std::unordered_set<FloorEnchantments> active_floor_enchantments;
extern std::map<Traps, int> active_traps_to_value_map; // Used to track value's for active traps.
extern std::map<Offerings, std::string> offerings_to_localized_string_map;
extern std::map<Classes, std::string> classes_to_localized_armor_description_string_map;
extern std::map<FloorEnchantments, std::string> floor_enchantments_to_localized_string_map;
extern std::map<Classes, std::map<std::string, std::string>> class_armor_to_localized_string_map;
extern std::map<AriResources, int> ari_resource_to_value_map; // Used to track Ari's current resource levels.
extern std::map<AriResources, bool> ari_resource_to_penalty_map; // Used to track penalties from offerings to apply.
extern std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map; // Vector<CInstance*> holds references to Self and Other for each script.
extern std::map<std::string, std::unordered_set<int>> dungeon_biome_to_candidate_monsters_map; // Holds the candidate monster spawns for each dungeon biome.
extern std::map<int, std::string> floor_number_to_biome_name_map; // Maps floor numbers to the dungeon biome name.
extern std::vector<int> initial_floor_monsters; // Holds the IDs of monsters spawned on the current floor.
extern std::vector<CInstance*> current_floor_monsters; // Holds CInstance refs to all monsters on the current floor.
extern std::map<std::string, uint64_t> notification_name_to_last_display_time_map; // Tracks when a notification was last displayed.
extern std::map<int, RValue> item_id_to_prototype_map; // Used to serialize LiveItem instances for a given item prototype.
extern std::map<Classes, std::map<ManagedSetBonuses, int>> class_name_to_set_bonus_effect_value_map; // Used to track values for specific class set bonus effects. Example: Cleric->afflatus_misery->70.

// Attack pattern data for Stalagmites
extern std::vector<std::vector<double>> donut_aoe_points;
extern std::vector<std::vector<double>> pb_aoe_points;
extern std::vector<std::vector<double>> cross_aoe_points;
extern std::vector<std::vector<double>> x_aoe_points;
extern std::vector<std::vector<double>> checkerboard_room_wide_a_points;
extern std::vector<std::vector<double>> checkerboard_room_wide_b_points;

// GUI
extern double window_width;
extern double window_height;
extern bool show_dashes;
extern bool show_danger_banner;
extern bool fade_initialized;
extern uint64_t fade_start_time;


struct Configuration {
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

extern Configuration configuration;

// ============================================================
// Utility / game-interaction function forward declarations
// (implementations remain in ModuleMain.cpp)
// ============================================================
void PrintError(std::exception_ptr eptr);
void ResetFade();
void ResetCustomDrawFields();
bool GameIsPaused();
bool IsNumeric(RValue value);
bool IsObject(RValue value);
bool StructVariableExists(RValue the_struct, const char* variable_name);
RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value);
void StructVariableRemove(RValue the_struct, const char* variable_name);
bool GlobalVariableExists(const char* variable_name);
RValue GlobalVariableGet(const char* variable_name);
RValue GlobalVariableSet(const char* variable_name, RValue value);
void CreateOrGetGlobalYYTKVariable();
void CreateModInfoInGlobalYYTKVariable();
double GetWindowWidth();
double GetWindowHeight();
uint64_t GetCurrentSystemTime();
int euclidean_modulo(int a, int m);
std::vector<double> linspace(double start, double end, std::size_t num);
double round_n(double value, int decimals);
std::complex<double> round_complex(const std::complex<double>& z, int decimals);
double GetDistance(int x1, int y1, int x2, int y2);
std::pair<int, int> GetVector(int x1, int y1, int x2, int y2);
int CalculateMeteorDamage(double distance);
bool FacingTrap(int ariX, int ariY, int trapX, int trapY);
std::pair<int, int> GetCenterOffset(int screen_center_x, int screen_center_y, int image_width, int image_height);
std::vector<double> GetCenter(std::vector<double> topLeft, std::vector<double> bottomRight);
void DrawRectangle(int color, float x1, float y1, float x2, float y2, bool outline);
void DrawImage(int x, int y, int transparency);
void FadeInImage(double seconds_per_cycle, int repeat_count);
void DrawDashedBorder(float dash_len, float dash_thk, float speed, float screen_width, float screen_height, uint64_t current_time_ms);
void PlaySoundEffect(const char* sound_name, int priority, double gain);
void LoadPerks();
bool FairyBuffIsActive();
double GetInvulnerabilityHits();
void SetInvulnerabilityHits(double amount);
void SetFireBreathTime(double value);
void DisableAllPerks();
bool ItemHasBeenAcquired(int item_id);
void LoadTutorials();
void LoadPlayerStates();
void LoadMonsterStates();
void LoadBarkData();
void LoadStatusEffects();
void LoadLocations();
void LoadInfusions();
void LoadMonsters();
void ModifyMonsterPrototypes();
void LoadDungeonBiomeCandidateMonsters();
void LoadObjectIds();
void LoadItems();
void SetItemHealthModifier(int item_id, double health_modifier);
void SetItemStaminaModifier(int item_id, double stamina_modifier);
void SetItemManaModifier(int item_id, double mana_modifier);
void SetItemShopPrice(int item_id, int store_price);
void ModifyItems();
void MarkDungeonTutorialUnseen();
void ModifyMistpoolWeaponSprites();
void ModifyMistpoolPickaxeSprites();
void ModifyBarkSprites();
void ScaleMistpoolWeapon(bool in_dungeon);
void ScaleMistpoolArmor(bool in_dungeon);
void ScaleMistpoolPickaxe(bool in_dungeon);
void ScaleClassArmor(bool in_dungeon);
int GetRandomSoulStone();
std::map<Classes, int> CountEquippedClassArmor();
std::map<int, int> GetClassArmorInfusions();
int GetClericAutoRegenPotency();
double GetDarkKnightDrainPotency();
double GetPaladinHolyCirclePotency();
int ScaleTemperanceDamage(int current_health, int max_health, int damage);
ElementalSealEffects GetRandomElementalSealEffect();
void LoadSpellIds();
void LoadSpells();
RValue LocalizeString(CInstance* Self, CInstance* Other, std::string localization_key);
RValue T2Read(CInstance* Self, CInstance* Other, std::string key);
RValue DeserializeLiveItem(CInstance* Self, CInstance* Other);
RValue InventoryCountItem(int item_id, CInstance* Self, CInstance* Other);
bool AriCurrentGmRoomIsDungeonFloor();
RValue GetDynamicItemSprite(int item_id);
RValue GetDynamicUiSprite(std::string sprite_name);
std::unordered_set<FloorEnchantments> RandomFloorEnchantments(bool is_first_floor, DungeonBiomes dungeon_biome);
std::vector<int> GenerateRandomMonstersIdsForCurrentFloor(int monsters_to_spawn, const int monster_id_to_exclude = -1);
int SelectRandomMonsterForAlteration();
RValue GetUnifiedTime(CInstance* Self, CInstance* Other);
RValue GetHealth(CInstance* Self, CInstance* Other);
RValue GetMaxHealth(CInstance* Self, CInstance* Other);
int ModifyMaxHealth(CInstance* Self, CInstance* Other, int value);
RValue GetStamina(CInstance* Self, CInstance* Other);
RValue GetMana(CInstance* Self, CInstance* Other);
void UpdateToolbarMenu(CInstance* Self, CInstance* Other);
void CreateNotification(bool ignore_cooldown, std::string notification_localization_str, CInstance* Self, CInstance* Other);
void CloseTextbox(CInstance* Self, CInstance* Other);
void SpawnTutorial(std::string tutorial_name, CInstance* Self, CInstance* Other);
void PlayConversation(std::string conversation_localization_str, CInstance* Self, CInstance* Other);
void EmitBark(CInstance* Self, CInstance* Other, RValue bark_id, RValue bark_type);
void SceneAudioPlayerStop(CInstance* Self, CInstance* Other);
void EnterDungeon(double dungeon_level, CInstance* Self, CInstance* Other);
void InventoryRemoveItem(int item_id, int quantity, CInstance* Self, CInstance* Other);
void SetHealth(CInstance* Self, CInstance* Other, int value);
void ModifyHealth(CInstance* Self, CInstance* Other, int value);
void SetMaxHealth(CInstance* Self, CInstance* Other, int value);
void VitalsMenuSetHealth(CInstance* Self, CInstance* Other, int current_health_value, int max_health_value);
void VitalsMenuSetMaxHealth(CInstance* Self, CInstance* Other, int value);
void ModifyStamina(CInstance* Self, CInstance* Other, int value);
void ModifyMana(CInstance* Self, CInstance* Other, int value);
void SpawnMonster(CInstance* Self, CInstance* Other, int room_x, int room_y, int monster_id);
void SpawnLadder(CInstance* Self, CInstance* Other, int64_t x_coord, int64_t y_coord);
void CancelStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id);
void CancelAllStatusEffects();
void RegisterStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id, RValue amount, RValue start, RValue finish);
void CastSpell(CInstance* Self, CInstance* Other, int spell_id);
void DropItem(int item_id, double x_coord, double y_coord, CInstance* Self, CInstance* Other);
void DropLiftKey();
void ModifySpellCosts(bool reset_cost, bool in_dungeon);
void SetFloorNumber();
void GenerateFloorTraps();
void GenerateTreasureSpot(CInstance* Self, CInstance* Other);
void SpawnDreadBeast(CInstance* Self, CInstance* Other);
void SelectDreadBeast(CInstance* Self, CInstance* Other);
void RevealFloorTraps();
void ApplyFloorTraps(CInstance* Self, CInstance* Other);
void ProcessCustomAOEs();
void ProcessTreasureSpot(CInstance* Self, CInstance* Other);
void ProcessSpiritConcealment();
void ApplyOfferingPenalties(CInstance* Self, CInstance* Other);
void TrackAriResources(CInstance* Self, CInstance* Other);
void GenerateTreasureChestLoot(std::string object_name, CInstance* Self, CInstance* Other);
void ResetStaticFields(bool returned_to_title_screen);
void UnlockRecipe(int item_id, CInstance* Self, CInstance* Other);
void UnlockLiftKeyRecipe(CInstance* Self, CInstance* Other);
void TeleportAriToRoom(CInstance* Self, CInstance* Other, int location_id, int x_coordinate, int y_coordinate);
void LoadStalagmiteAttackData();
