//#include <math.h>
#include <random>
#include <complex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <nlohmann/json.hpp>
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
static const char* const VERSION = "1.0.0";
static const char* const GML_SCRIPT_GET_LOCALIZER = "gml_Script_get@Localizer@Localizer";
static const char* const GML_SCRIPT_SPAWN_LADDER = "gml_Script_spawn_ladder@DungeonRunner@DungeonRunner";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
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
static const char* const DISABLE_DUNGEON_LIFT_JSON_KEY = "disable_dungeon_lift"; // Controls the dungeon lift
static const char* const RESTRICT_PERKS_JSON_KEY = "restrict_perks"; // Determines if perks are restricted in the dungeon
static const char* const RESTRICT_ITEMS_JSON_KEY = "restrict_items"; // Determines if items are restricted in the dungeon
static const char* const RESTRICT_ARMOR_JSON_KEY = "restrict_armor"; // Determines if armor is restricted in the dungeon
static const char* const RESTRICT_TOOLS_JSON_KEY = "restrict_tools"; // Determines if tools are restricted in the dungeon
static const char* const RESTRICT_WEAPONS_JSON_KEY = "restrict_weapons"; // Determines if weapons are restricted in the dungeon
static const char* const LIMIT_SALVES_JSON_KEY = "limit_salves"; // Determines if salves have a single use limit per floor
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
static const char* const MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY = "mistpool_equipment_store_price"; // Controls the price of mistpool equipment in stores
static const char* const SALVES_STORE_PRICE_JSON_KEY = "salves_store_price"; // Controls the price of salves in stores

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
static const int THREE_MINUTES_IN_SECONDS = 180;
static const int ONE_HOUR_IN_SECONDS = 3600;
static const int TRAP_ACTIVATION_DISTANCE = 16;

// Configuration defaults
static const bool DEFAULT_DISABLE_DUNGEON_LIFT = true;
static const bool DEFAULT_RESTRICT_PERKS = true;
static const bool DEFAULT_RESTRICT_ITEMS = true;
static const bool DEFAULT_RESTRICT_ARMOR = true;
static const bool DEFAULT_RESTRICT_TOOLS = true;
static const bool DEFAULT_RESTRICT_WEAPONS = true;
static const bool DEFAULT_LIMIT_SALVES = true;
static const int DEFAULT_HEALTH_SALVE_POTENCY = 30;
static const int DEFAULT_STAMINA_SALVE_POTENCY = 30;
static const int DEFAULT_MANA_SALVE_POTENCY = 1;
static const double DEFAULT_SUSTAINING_POTION_DURATION_MODIFIER = 0.5;
static const bool DEFAULT_RANDOMIZE_DUNGEON_MUSIC = true;
static const int DEFAULT_RANDOM_DREAD_BEAST_SPAWN_CHANCE = 5; // TODO: Tune this.
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
static const int DEFAULT_GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT = 50;
static const double DEFAULT_METEOR_TRAP_SCALING_FACTOR = 2.5;
static const int DEFAULT_MISTPOOL_EQUIPMENT_STORE_PRICE = 500;
static const int DEFAULT_SALVES_STORE_PRICE = 50;

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

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static RValue __YYTK;
static bool load_on_start = true;
static bool localize_mod_text = false;
static bool game_is_active = false;
static bool unlock_recipes = true;
static bool crafting_menu_open = false;
static bool journal_menu_open = false;
static bool drop_biome_reward = false;
static bool biome_reward_disabled = false;
static bool dread_beast_configured = false;
static bool sigil_item_used = false;
static bool greater_sigil_item_used = false;
static bool lift_key_used = false;
static bool orb_item_used = false;
static bool inner_fire_cast = false;
static bool reckoning_applied = false;
static bool fairy_buff_applied = false;
static bool stoneskin_applied = false;
static bool is_restoration_tracked_interval = false;
static bool is_second_wind_tracked_interval = false;
static bool is_fumigate_tracked_interval = false;
static bool is_deep_wounds_tracked_interval = false;
static bool offering_chance_occurred = false;
static bool obj_dungeon_elevator_focused = false;
static bool obj_dungeon_ladder_down_focused = false;
static double ari_x = -1;
static double ari_y = -1;
static double ari_facing_dir = -1;
static int floor_number = 0;
static int unmodified_base_health = -1;
static int hp_penalty_amount = -1;
static int floor_start_time = 0;
static int current_time_in_seconds = -1;
static int time_of_last_restoration_tick = -1;
static int time_of_last_second_wind_tick = -1;
static int time_of_last_fumigate_tick = -1;
static int time_of_last_deep_wounds_tick = -1;
static int time_of_last_outbreak_tick = -1;
static int held_item_id = -1;
static int frailty_hit_counter = 0;
static int grudge_counter = 0;
static int deep_wounds_damage_pool = 0;
static int stoneskin_shield_amount = 0;
static int spirit_link_combined_health_pool = 0;
static int sigil_of_silence_count = 0;
static int sigil_of_alteration_count = 0;
static int dread_beast_monster_id = -1;
static int dread_beasts_configured = 0;
static int boss_monsters_configured = 0;
static BossBattle boss_battle = BossBattle::NONE;
static TreasureSpot treasure_spot = TreasureSpot();
static std::string ari_current_location = "";
static std::string ari_current_gm_room = "";
static std::unordered_set<int> orb_items = {};
static std::unordered_set<int> lift_key_items = {};
static std::unordered_set<int> restricted_items = {};
static std::unordered_set<int> deep_dungeon_items = {};
static std::map<Sigils, int> sigil_to_item_id_map = {};
static std::map<int, Sigils> item_id_to_sigil_map = {};
static std::map<GreaterSigils, int> greater_sigil_to_item_id_map = {};
static std::map<int, GreaterSigils> item_id_to_greater_sigil_map = {};
static std::map<std::string, int> perk_name_to_id_map = {};
static std::map<std::string, int> spell_name_to_id_map = {};
static std::map<int, int> spell_id_to_default_cost_map = {};
static std::map<std::string, int> salve_name_to_id_map = {}; // TODO: Remove and use item_name_to_id_map instead.
static std::map<int, std::string> object_id_to_name_map = {};
static std::map<std::string, int> player_state_to_id_map = {};
static std::map<std::string, std::map<std::string, int>> monster_category_to_state_id_map = {};
static std::map<std::string, int> monster_name_to_id_map = {};
static std::map<int, std::string> monster_id_to_name_map = {};
static std::map<std::string, int> tutorial_name_to_id_map = {};
static std::map<std::string, int> infusion_name_to_id_map = {};
static std::map<std::string, int> status_effect_name_to_id_map = {};
static std::map<std::string, int> item_name_to_id_map = {};
static std::map<std::string, int> bark_name_to_id_map = {};
std::unordered_set<std::pair<int, int>, pair_hash> floor_trap_positions = {};
static std::unordered_set<int> salves_used = {};
static std::map<Traps, std::pair<int, int>> active_traps = {}; // Holds the active traps and the position they most recently triggered at.
static std::vector<CustomAOE> meteor_aoes = {};
static std::vector<CustomAOE> gaze_aoes = {};
static std::vector<CustomAOE> void_aoes = {};
static std::vector<RevealedFloorTrap> revealed_floor_traps = {};
static std::unordered_set<Sigils> active_sigils = {};
static std::unordered_set<GreaterSigils> active_greater_sigils = {};
static std::unordered_set<Offerings> queued_offerings = {};
static std::unordered_set<Offerings> active_offerings = {};
static std::unordered_set<FloorEnchantments> active_floor_enchantments = {};
static std::map<Traps, int> active_traps_to_value_map = {}; // Used to track value's for active traps.
static std::map<Offerings, std::string> offerings_to_localized_string_map = {};
static std::map<Classes, std::string> classes_to_localized_armor_description_string_map = {};
static std::map<FloorEnchantments, std::string> floor_enchantments_to_localized_string_map = {};
static std::map<Classes, std::map<std::string, std::string>> class_armor_to_localized_string_map = {};
static std::map<AriResources, int> ari_resource_to_value_map = {}; // Used to track Ari's current resource levels.
static std::map<AriResources, bool> ari_resource_to_penalty_map = {}; // Used to track penalties from offerings to apply.
static std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map; // Vector<CInstance*> holds references to Self and Other for each script.
static std::map<std::string, std::unordered_set<int>> dungeon_biome_to_candidate_monsters_map = {}; // Holds the candidate monster spawns for each dungeon biome.
static std::map<int, std::string> floor_number_to_biome_name_map = {}; // Maps floor numbers to the dungeon biome name.
static std::vector<int> initial_floor_monsters = {}; // Holds the IDs of monsters spawned on the current floor.
static std::vector<CInstance*> current_floor_monsters = {}; // Holds CInstance refs to all monsters on the current floor.
static std::map<std::string, uint64_t> notification_name_to_last_display_time_map = {}; // Tracks when a notification was last displayed.
static std::map<int, RValue> item_id_to_prototype_map = {}; // Used to serialize LiveItem instances for a given item prototype.
static std::map<Classes, std::map<ManagedSetBonuses, int>> class_name_to_set_bonus_effect_value_map; // Used to track values for specific class set bonus effects. Example: Cleric->afflatus_misery->70.

// Attack pattern data for Stalagmites
static std::vector<std::vector<double>> donut_aoe_points;
static std::vector<std::vector<double>> pb_aoe_points;
static std::vector<std::vector<double>> cross_aoe_points;
static std::vector<std::vector<double>> x_aoe_points;
static std::vector<std::vector<double>> checkerboard_room_wide_a_points;
static std::vector<std::vector<double>> checkerboard_room_wide_b_points;

// GUI
static double window_width = 0;
static double window_height = 0;
static bool show_dashes = false;
static bool show_danger_banner = false;
static bool fade_initialized = false;
static uint64_t fade_start_time = 0;

// Configuration Options
static struct Configuration {
	bool disable_dungeon_lift = DEFAULT_DISABLE_DUNGEON_LIFT;
	bool restrict_perks = DEFAULT_RESTRICT_PERKS;
	bool restrict_items = DEFAULT_RESTRICT_ITEMS;
	bool restrict_armor = DEFAULT_RESTRICT_ARMOR;
	bool restrict_tools = DEFAULT_RESTRICT_TOOLS;
	bool restrict_weapons = DEFAULT_RESTRICT_WEAPONS;
	bool limit_salves = DEFAULT_LIMIT_SALVES;
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
	int mistpool_equipment_store_price = DEFAULT_MISTPOOL_EQUIPMENT_STORE_PRICE;
	int salves_store_price = DEFAULT_SALVES_STORE_PRICE;
};
static Configuration configuration = Configuration();

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}

json CreateConfigJson(bool use_defaults)
{
	json config_json = {
		{ DISABLE_DUNGEON_LIFT_JSON_KEY, use_defaults ? DEFAULT_DISABLE_DUNGEON_LIFT : configuration.disable_dungeon_lift },
		{ RESTRICT_PERKS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_PERKS : configuration.restrict_perks },
		{ RESTRICT_ITEMS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_ITEMS : configuration.restrict_items },
		{ RESTRICT_ARMOR_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_ARMOR : configuration.restrict_armor },
		{ RESTRICT_TOOLS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_TOOLS : configuration.restrict_tools },
		{ RESTRICT_WEAPONS_JSON_KEY, use_defaults ? DEFAULT_RESTRICT_WEAPONS : configuration.restrict_weapons },
		{ LIMIT_SALVES_JSON_KEY, use_defaults ? DEFAULT_LIMIT_SALVES : configuration.limit_salves },
		{ HEALTH_SALVE_POTENCY_JSON_KEY, use_defaults ? DEFAULT_HEALTH_SALVE_POTENCY : configuration.health_salve_potency },
		{ STAMINA_SALVE_POTENCY_JSON_KEY, use_defaults ? DEFAULT_STAMINA_SALVE_POTENCY : configuration.stamina_salve_potency },
		{ MANA_SALVE_POTENCY_JSON_KEY, use_defaults ? DEFAULT_MANA_SALVE_POTENCY : configuration.mana_salve_potency },
		{ SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_SUSTAINING_POTION_DURATION_MODIFIER : configuration.sustaining_potion_duration_modifier },
		{ RANDOMIZE_DUNGEON_MUSIC_JSON_KEY, use_defaults ? DEFAULT_RANDOMIZE_DUNGEON_MUSIC : configuration.randomize_dungeon_music },
		{ RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY, use_defaults ? DEFAULT_RANDOM_DREAD_BEAST_SPAWN_CHANCE : configuration.random_dread_beast_spawn_chance },
		{ OFFERING_EVENT_CHANCE_JSON_KEY, use_defaults ? DEFAULT_OFFERING_EVENT_CHANCE : configuration.offering_event_chance },
		{ OFFERING_HEALTH_REQUIREMENT_JSON_KEY, use_defaults ? DEFAULT_OFFERING_HEALTH_REQUIREMENT : configuration.offering_health_requirement },
		{ OFFERING_STAMINA_REQUIREMENT_JSON_KEY, use_defaults ? DEFAULT_OFFERING_STAMINA_REQUIREMENT : configuration.offering_stamina_requirement },
		{ OFFERING_MANA_REQUIREMENT_JSON_KEY, use_defaults ? DEFAULT_OFFERING_MANA_REQUIREMENT : configuration.offering_mana_requirement },
		{ CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY, use_defaults ? DEFAULT_CURSED_ARMOR_DROP_CHANCE_MODIFIER : configuration.cursed_armor_drop_chance_modifier },
		{ SOUL_STONE_DROP_CHANCE_JSON_KEY, use_defaults ? DEFAULT_SOUL_STONE_DROP_CHANCE : configuration.soul_stone_drop_chance },
		{ LIFT_KEY_DROP_CHANCE_JSON_KEY, use_defaults ? DEFAULT_LIFT_KEY_DROP_CHANCE : configuration.lift_key_drop_chance },
		{ ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY, use_defaults ? DEFAULT_ENABLE_BOSS_FIGHT_RESTRICTIONS : configuration.enable_boss_fight_restrictions },
		{ CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_CONFUSING_TRAP_DURATION_SECONDS : configuration.confusing_trap_duration_seconds },
		{ DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_DISORIENTING_TRAP_DURATION_SECONDS : configuration.disorienting_trap_duration_seconds },
		{ EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY, use_defaults ? DEFAULT_EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT : configuration.exploding_trap_current_health_damage_percent },
		{ INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY, use_defaults ? DEFAULT_INHIBITING_TRAP_DURATION_SECONDS : configuration.inhibiting_trap_duration_seconds },
		{ LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY, use_defaults ? DEFAULT_LURING_TRAP_MONSTER_SPAWN_COUNT : configuration.luring_trap_monster_spawn_count },
		{ GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY, use_defaults ? DEFAULT_GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT : configuration.gaze_trap_max_health_damage_percent },
		{ METEOR_TRAP_SCALING_FACTOR_JSON_KEY, use_defaults ? DEFAULT_METEOR_TRAP_SCALING_FACTOR : configuration.meteor_trap_scaling_factor },
		{ MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY, use_defaults ? DEFAULT_MISTPOOL_EQUIPMENT_STORE_PRICE : configuration.mistpool_equipment_store_price },
		{ SALVES_STORE_PRICE_JSON_KEY, use_defaults ? DEFAULT_SALVES_STORE_PRICE : configuration.salves_store_price }
	};
	return config_json;
}

void CreateOrLoadConfigFile()
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
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
			std::filesystem::create_directory(mod_data_folder);
		}

		// Try to find the mod_data/DeepDungeon directory.
		std::string deep_dungeon_folder = mod_data_folder + "\\DeepDungeon";
		if (!std::filesystem::exists(deep_dungeon_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DeepDungeon\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, deep_dungeon_folder.c_str());
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
					g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.c_str());
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Add values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
				}
				else
				{
					// Try loading the disable_dungeon_lift value.
					if (json_object.contains(DISABLE_DUNGEON_LIFT_JSON_KEY) && json_object.at(DISABLE_DUNGEON_LIFT_JSON_KEY).is_boolean())
						configuration.disable_dungeon_lift = json_object[DISABLE_DUNGEON_LIFT_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, DISABLE_DUNGEON_LIFT_JSON_KEY, config_file.c_str());

					// Try loading the restrict_perks value.
					if (json_object.contains(RESTRICT_PERKS_JSON_KEY) && json_object.at(RESTRICT_PERKS_JSON_KEY).is_boolean())
						configuration.restrict_perks = json_object[RESTRICT_PERKS_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RESTRICT_PERKS_JSON_KEY, config_file.c_str());

					// Try loading the restrict_items value.
					if (json_object.contains(RESTRICT_ITEMS_JSON_KEY) && json_object.at(RESTRICT_ITEMS_JSON_KEY).is_boolean())
						configuration.restrict_items = json_object[RESTRICT_ITEMS_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RESTRICT_ITEMS_JSON_KEY, config_file.c_str());

					// Try loading the restrict_armor value.
					if (json_object.contains(RESTRICT_ARMOR_JSON_KEY) && json_object.at(RESTRICT_ARMOR_JSON_KEY).is_boolean())
						configuration.restrict_armor = json_object[RESTRICT_ARMOR_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RESTRICT_ARMOR_JSON_KEY, config_file.c_str());

					// Try loading the restrict_tools value.
					if (json_object.contains(RESTRICT_TOOLS_JSON_KEY) && json_object.at(RESTRICT_TOOLS_JSON_KEY).is_boolean())
						configuration.restrict_tools = json_object[RESTRICT_TOOLS_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RESTRICT_TOOLS_JSON_KEY, config_file.c_str());

					// Try loading the restrict_weapons value.
					if (json_object.contains(RESTRICT_WEAPONS_JSON_KEY) && json_object.at(RESTRICT_WEAPONS_JSON_KEY).is_boolean())
						configuration.restrict_weapons = json_object[RESTRICT_WEAPONS_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RESTRICT_WEAPONS_JSON_KEY, config_file.c_str());

					// Try loading the limit_salves value.
					if (json_object.contains(LIMIT_SALVES_JSON_KEY) && json_object.at(LIMIT_SALVES_JSON_KEY).is_boolean())
						configuration.limit_salves = json_object[LIMIT_SALVES_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, LIMIT_SALVES_JSON_KEY, config_file.c_str());

					// Try loading the health_salve_potency value.
					if (json_object.contains(HEALTH_SALVE_POTENCY_JSON_KEY) && json_object.at(HEALTH_SALVE_POTENCY_JSON_KEY).is_number_integer())
					{
						int health_salve_potency = json_object[HEALTH_SALVE_POTENCY_JSON_KEY];
						if (health_salve_potency > 0 && health_salve_potency <= 999)
							configuration.health_salve_potency = health_salve_potency;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, HEALTH_SALVE_POTENCY_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, HEALTH_SALVE_POTENCY_JSON_KEY, config_file.c_str());

					// Try loading the stamina_salve_potency value.
					if (json_object.contains(STAMINA_SALVE_POTENCY_JSON_KEY) && json_object.at(STAMINA_SALVE_POTENCY_JSON_KEY).is_number_integer())
					{
						int stamina_salve_potency = json_object[STAMINA_SALVE_POTENCY_JSON_KEY];
						if (stamina_salve_potency > 0 && stamina_salve_potency <= 999)
							configuration.stamina_salve_potency = stamina_salve_potency;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, STAMINA_SALVE_POTENCY_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, STAMINA_SALVE_POTENCY_JSON_KEY, config_file.c_str());

					// Try loading the mana_salve_potency value.
					if (json_object.contains(MANA_SALVE_POTENCY_JSON_KEY) && json_object.at(MANA_SALVE_POTENCY_JSON_KEY).is_number_integer())
					{
						int mana_salve_potency = json_object[MANA_SALVE_POTENCY_JSON_KEY];
						if (mana_salve_potency > 0 && mana_salve_potency <= 999)
							configuration.mana_salve_potency = mana_salve_potency;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, MANA_SALVE_POTENCY_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, MANA_SALVE_POTENCY_JSON_KEY, config_file.c_str());

					// Try loading the sustaining_potion_duration_modifier value.
					if (json_object.contains(SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY) && json_object.at(SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY).is_number_float())
					{
						double sustaining_potion_duration_modifier = json_object[SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY];
						if (sustaining_potion_duration_modifier >= 0.5 && sustaining_potion_duration_modifier <= 2.0)
							configuration.sustaining_potion_duration_modifier = sustaining_potion_duration_modifier;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SUSTAINING_POTION_DURATION_MODIFIER_JSON_KEY, config_file.c_str());

					// Try loading the randomize_dungeon_music value.
					if (json_object.contains(RANDOMIZE_DUNGEON_MUSIC_JSON_KEY) && json_object.at(RANDOMIZE_DUNGEON_MUSIC_JSON_KEY).is_boolean())
						configuration.randomize_dungeon_music = json_object[RANDOMIZE_DUNGEON_MUSIC_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RANDOMIZE_DUNGEON_MUSIC_JSON_KEY, config_file.c_str());

					// Try loading the random_dread_beast_spawn_chance value.
					if (json_object.contains(RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY) && json_object.at(RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY).is_number_integer())
					{
						int random_dread_beast_spawn_chance = json_object[RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY];
						if (random_dread_beast_spawn_chance >= 0 && random_dread_beast_spawn_chance <= 100)
							configuration.random_dread_beast_spawn_chance = random_dread_beast_spawn_chance;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, RANDOM_DREAD_BEAST_SPAWN_CHANCE_JSON_KEY, config_file.c_str());

					// Try loading the offering_event_chance value.
					if (json_object.contains(OFFERING_EVENT_CHANCE_JSON_KEY) && json_object.at(OFFERING_EVENT_CHANCE_JSON_KEY).is_number_integer())
					{
						int offering_event_chance = json_object[OFFERING_EVENT_CHANCE_JSON_KEY];
						if (offering_event_chance >= 0 && offering_event_chance <= 100)
							configuration.offering_event_chance = offering_event_chance;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_EVENT_CHANCE_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_EVENT_CHANCE_JSON_KEY, config_file.c_str());

					// Try loading the offering_health_requirement value.
					if (json_object.contains(OFFERING_HEALTH_REQUIREMENT_JSON_KEY) && json_object.at(OFFERING_HEALTH_REQUIREMENT_JSON_KEY).is_number_integer())
					{
						int offering_health_requirement = json_object[OFFERING_HEALTH_REQUIREMENT_JSON_KEY];
						if (offering_health_requirement >= 0 && offering_health_requirement <= 90)
							configuration.offering_health_requirement = offering_health_requirement;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_HEALTH_REQUIREMENT_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_HEALTH_REQUIREMENT_JSON_KEY, config_file.c_str());

					// Try loading the offering_stamina_requirement value.
					if (json_object.contains(OFFERING_STAMINA_REQUIREMENT_JSON_KEY) && json_object.at(OFFERING_STAMINA_REQUIREMENT_JSON_KEY).is_number_integer())
					{
						int offering_stamina_requirement = json_object[OFFERING_STAMINA_REQUIREMENT_JSON_KEY];
						if (offering_stamina_requirement >= 0 && offering_stamina_requirement <= 90)
							configuration.offering_stamina_requirement = offering_stamina_requirement;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_STAMINA_REQUIREMENT_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_STAMINA_REQUIREMENT_JSON_KEY, config_file.c_str());

					// Try loading the offering_mana_requirement value.
					if (json_object.contains(OFFERING_MANA_REQUIREMENT_JSON_KEY) && json_object.at(OFFERING_MANA_REQUIREMENT_JSON_KEY).is_number_integer())
					{
						int offering_mana_requirement = json_object[OFFERING_MANA_REQUIREMENT_JSON_KEY];
						if (offering_mana_requirement >= 0 && offering_mana_requirement <= 16)
							configuration.offering_mana_requirement = offering_mana_requirement;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_MANA_REQUIREMENT_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, OFFERING_MANA_REQUIREMENT_JSON_KEY, config_file.c_str());

					// Try loading the cursed_armor_drop_chance_modifier value.
					if (json_object.contains(CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY) && json_object.at(CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY).is_number_float())
					{
						double cursed_armor_drop_chance_modifier = json_object[CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY];
						if (cursed_armor_drop_chance_modifier >= 1.0 && cursed_armor_drop_chance_modifier <= 10.0)
							configuration.cursed_armor_drop_chance_modifier = cursed_armor_drop_chance_modifier;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, CURSED_ARMOR_DROP_CHANCE_MODIFIER_JSON_KEY, config_file.c_str());

					// Try loading the soul_stone_drop_chance value.
					if (json_object.contains(SOUL_STONE_DROP_CHANCE_JSON_KEY) && json_object.at(SOUL_STONE_DROP_CHANCE_JSON_KEY).is_number_integer())
					{
						int soul_stone_drop_chance = json_object[SOUL_STONE_DROP_CHANCE_JSON_KEY];
						if (soul_stone_drop_chance >= 35 && soul_stone_drop_chance <= 100)
							configuration.soul_stone_drop_chance = soul_stone_drop_chance;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SOUL_STONE_DROP_CHANCE_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SOUL_STONE_DROP_CHANCE_JSON_KEY, config_file.c_str());

					// Try loading the lift_key_drop_chance value.
					if (json_object.contains(LIFT_KEY_DROP_CHANCE_JSON_KEY) && json_object.at(LIFT_KEY_DROP_CHANCE_JSON_KEY).is_number_integer())
					{
						int lift_key_drop_chance = json_object[LIFT_KEY_DROP_CHANCE_JSON_KEY];
						if (lift_key_drop_chance >= 2 && lift_key_drop_chance <= 100)
							configuration.lift_key_drop_chance = lift_key_drop_chance;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, LIFT_KEY_DROP_CHANCE_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, LIFT_KEY_DROP_CHANCE_JSON_KEY, config_file.c_str());

					// Try loading the enable_boss_fight_restrictions value.
					if (json_object.contains(ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY) && json_object.at(ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY).is_boolean())
						configuration.enable_boss_fight_restrictions = json_object[ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY];
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, ENABLE_BOSS_FIGHT_RESTRICTIONS_JSON_KEY, config_file.c_str());

					// Try loading confusing_trap_duration_seconds
					if (json_object.contains(CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
					{
						int confusing_trap_duration_seconds = json_object[CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY];
						if (confusing_trap_duration_seconds >= 0 && confusing_trap_duration_seconds <= 1200)
							configuration.confusing_trap_duration_seconds = confusing_trap_duration_seconds;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, CONFUSING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

					// Try loading disorienting_trap_duration_seconds
					if (json_object.contains(DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
					{
						int disorienting_trap_duration_seconds = json_object[DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY];
						if (disorienting_trap_duration_seconds >= 0 && disorienting_trap_duration_seconds <= 600)
							configuration.disorienting_trap_duration_seconds = disorienting_trap_duration_seconds;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, DISORIENTING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

					// Try loading exploding_trap_current_health_damage_percent
					if (json_object.contains(EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY) && json_object.at(EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY).is_number_integer())
					{
						int exploding_trap_current_health_damage_percent = json_object[EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY];
						if (exploding_trap_current_health_damage_percent >= 0 && exploding_trap_current_health_damage_percent <= 80)
							configuration.exploding_trap_current_health_damage_percent = exploding_trap_current_health_damage_percent;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, EXPLODING_TRAP_CURRENT_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());

					// Try loading luring_trap_monster_spawn_count
					if (json_object.contains(LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY) && json_object.at(LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY).is_number_integer())
					{
						int luring_trap_monster_spawn_count = json_object[LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY];
						if (luring_trap_monster_spawn_count >= 0 && luring_trap_monster_spawn_count <= 2)
							configuration.luring_trap_monster_spawn_count = luring_trap_monster_spawn_count;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, LURING_TRAP_MONSTER_SPAWN_COUNT_JSON_KEY, config_file.c_str());

					// Try loading inhibiting_trap_duration_seconds
					if (json_object.contains(INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY) && json_object.at(INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY).is_number_integer())
					{
						int inhibiting_trap_duration_seconds = json_object[INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY];
						if (inhibiting_trap_duration_seconds >= 0 && inhibiting_trap_duration_seconds <= 900)
							configuration.inhibiting_trap_duration_seconds = inhibiting_trap_duration_seconds;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, INHIBITING_TRAP_DURATION_SECONDS_JSON_KEY, config_file.c_str());

					// Try loading gaze_trap_max_health_damage_percent
					if (json_object.contains(GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY) && json_object.at(GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY).is_number_integer())
					{
						int gaze_trap_max_health_damage_percent = json_object[GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY];
						if (gaze_trap_max_health_damage_percent >= 0 && gaze_trap_max_health_damage_percent <= 99)
							configuration.gaze_trap_max_health_damage_percent = gaze_trap_max_health_damage_percent;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, GAZE_TRAP_MAX_HEALTH_DAMAGE_PERCENT_JSON_KEY, config_file.c_str());

					// Try loading meteor_trap_scaling_factor
					if (json_object.contains(METEOR_TRAP_SCALING_FACTOR_JSON_KEY) && json_object.at(METEOR_TRAP_SCALING_FACTOR_JSON_KEY).is_number_float())
					{
						double meteor_trap_scaling_factor = json_object[METEOR_TRAP_SCALING_FACTOR_JSON_KEY];
						if (meteor_trap_scaling_factor == 0 || meteor_trap_scaling_factor == 1 || meteor_trap_scaling_factor == 1.5 || meteor_trap_scaling_factor == 2 || meteor_trap_scaling_factor == 2.5 || meteor_trap_scaling_factor == 3)
							configuration.meteor_trap_scaling_factor = meteor_trap_scaling_factor;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, METEOR_TRAP_SCALING_FACTOR_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, METEOR_TRAP_SCALING_FACTOR_JSON_KEY, config_file.c_str());

					// Try loading mistpool_equipment_store_price
					if (json_object.contains(MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY) && json_object.at(MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY).is_number_integer())
					{
						int mistpool_equipment_store_price = json_object[MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY];
						if (mistpool_equipment_store_price >= 1 && mistpool_equipment_store_price <= 500)
							configuration.mistpool_equipment_store_price = mistpool_equipment_store_price;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, MISTPOOL_EQUIPMENT_STORE_PRICE_JSON_KEY, config_file.c_str());

					// Try loading salves_store_price
					if (json_object.contains(SALVES_STORE_PRICE_JSON_KEY) && json_object.at(SALVES_STORE_PRICE_JSON_KEY).is_number_integer())
					{
						int salves_store_price = json_object[SALVES_STORE_PRICE_JSON_KEY];
						if (salves_store_price >= 1 && salves_store_price <= 50)
							configuration.salves_store_price = salves_store_price;
						else
							g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SALVES_STORE_PRICE_JSON_KEY, config_file.c_str());
					}
					else
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing or invalid \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SALVES_STORE_PRICE_JSON_KEY, config_file.c_str());

				}

				update_config_file = true;
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DeepDungeon.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();
		}

		if (update_config_file)
		{
			json config_json = CreateConfigJson(false);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << config_json << std::endl;
			out_stream.close();
		}
	}
	catch (...)
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);

		eptr = std::current_exception();
		PrintError(eptr);
	}
}


void ResetFade()
{
	fade_initialized = false;
	fade_start_time = 0;
}

void ResetCustomDrawFields()
{
	show_dashes = false;
	show_danger_banner = false;
	ResetFade();
}

bool GameIsPaused()
{
	RValue paused = global_instance->GetMember("__pause_status");
	return paused.ToInt64() > 0;
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return struct_exists.ToBoolean();
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void StructVariableRemove(RValue the_struct, const char* variable_name)
{
	if (StructVariableExists(the_struct, variable_name))
	{
		RValue struct_exists = g_ModuleInterface->CallBuiltin(
			"struct_remove",
			{ the_struct, variable_name }
		);
	}
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return global_variable_exists.ToBoolean();
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists("__YYTK"))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet("__YYTK", __YYTK);
	}
	else
		__YYTK = GlobalVariableGet("__YYTK");
}

void CreateModInfoInGlobalYYTKVariable()
{
	if (!StructVariableExists(__YYTK, MOD_NAME))
	{
		RValue deep_dungeon;
		RValue version = VERSION;
		RValue floor = floor_number;
		g_ModuleInterface->GetRunnerInterface().StructCreate(&deep_dungeon);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "version", &version);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "floor", &floor);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&__YYTK, MOD_NAME, &deep_dungeon);
	}
}

double GetWindowWidth()
{
	RValue window_width = g_ModuleInterface->CallBuiltin("window_get_width", {});
	return window_width.ToDouble();
}

double GetWindowHeight()
{
	RValue window_height = g_ModuleInterface->CallBuiltin("window_get_height", {});
	return window_height.ToDouble();
}

uint64_t GetCurrentSystemTime() {
	return duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int euclidean_modulo(int a, int m)
{
	int r = a % m;
	return (r < 0) ? r + m : r;
}

std::vector<double> linspace(double start, double end, std::size_t num)
{
	std::vector<double> result;
	result.reserve(num);

	if (num == 0)
		return result;

	if (num == 1)
	{
		result.push_back(start);
		return result;
	}

	double step = (end - start) / static_cast<double>(num - 1);
	for (std::size_t i = 0; i < num; ++i)
		result.push_back(start + step * static_cast<double>(i));

	return result;
}

double round_n(double value, int decimals)
{
	double factor = std::pow(10.0, decimals);
	return std::round(value * factor) / factor;
}

std::complex<double> round_complex(const std::complex<double>& z, int decimals)
{
	return {
		round_n(z.real(), decimals),
		round_n(z.imag(), decimals)
	};
}

std::vector<std::vector<double>> generate_circle(const std::vector<int>& Ns)
{
	static constexpr double PI = 3.14159265358979323846;
	std::vector<std::vector<double>> points = {};

	for (int N : Ns)
	{
		auto ts = linspace(0.0, 1.0, static_cast<std::size_t>(N));

		for (double t : ts)
		{
			std::complex<double> z = static_cast<double>(N - 1) * std::exp(std::complex<double>(0.0, 2.0 * PI * t));
			std::complex<double> c = round_complex(z, 2);
			points.push_back({ c.real(), c.imag() });
		}
	}

	return points;
}

std::vector<std::vector<double>> generate_cross(int threshold, int lower_bound, int upper_bound, int increment, bool rotate)
{
	static const std::complex<double> SQRT_I = std::sqrt(std::complex<double>(0.0, 1.0));

	std::vector<std::vector<double>> points;
	for (int y = lower_bound; y <= upper_bound; y += increment)
	{
		for (int x = lower_bound; x <= upper_bound; x += increment)
		{
			if (min(std::abs(x), std::abs(y)) < threshold && (std::abs(x) + std::abs(y)) != 0)
			{
				std::complex<double> z(x, y);
				std::complex<double> c;

				if (!rotate)
					c = round_complex(z, 2); // Cross
				else
					c = round_complex(z * SQRT_I, 2); // X

				points.push_back({ c.real(), c.imag() });
			}
		}
	}

	return points;
}

std::vector<std::vector<double>> generate_checkerboard()
{
	std::vector<std::vector<double>> positions;

	for (int y = -24; y <= 24; ++y)
	{
		for (int x = -24; x <= 24; ++x)
		{
			int xm = euclidean_modulo(x, 10);
			int ym = euclidean_modulo(y, 10);

			if ((xm < 5 && ym < 5) || (xm >= 5 && ym >= 5))
				positions.push_back({ 8.0 * (x + 0.5), 8.0 * (y + 0.5) });
		}
	}

	return positions;
}

std::vector<std::vector<double>> generate_inverted_checkerboard()
{
	std::vector<std::vector<double>> positions;

	for (int y = -24; y <= 24; ++y)
	{
		for (int x = -24; x <= 24; ++x)
		{
			int xm = euclidean_modulo(x, 10);
			int ym = euclidean_modulo(y, 10);

			if (!((xm < 5 && ym < 5) || (xm >= 5 && ym >= 5)))
				positions.push_back({ 8.0 * (x + 0.5), 8.0 * (y + 0.5) });
		}
	}

	return positions;
}

double GetDistance(int x1, int y1, int x2, int y2)
{
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

std::pair<int, int> GetVector(int x1, int y1, int x2, int y2)
{
	return { x2 - x1, y2 - y1 };
}

int CalculateMeteorDamage(double distance)
{
	const double lethalRadius = 32.0;
	const double maxDistance = 256.0;

	if (configuration.meteor_trap_scaling_factor == 0)
		return 0;

	if (distance <= lethalRadius)
		return 100; 

	if (distance >= maxDistance)
		return 1;

	double t = (distance - lethalRadius) / (maxDistance - lethalRadius);
	double damage = 100.0f * std::pow(1.0f - t, configuration.meteor_trap_scaling_factor);

	return max(1, static_cast<int>(damage));
}

bool FacingTrap(int ariX, int ariY, int trapX, int trapY)
{
	static constexpr double PI = 3.14159265358979323846;

	std::pair<int, int> v = GetVector(ariX, ariY, trapX, trapY);
	double rad = ari_facing_dir * PI / 180.0;

	double forwardX = cos(rad);
	double forwardY = -sin(rad);

	float dot = forwardX * v.first + forwardY * v.second;
	bool inFrontHalfPlane = dot >= 0.0f;
	return inFrontHalfPlane;
}

std::pair<int, int> GetCenterOffset(int screen_center_x, int screen_center_y, int image_width, int image_height)
{
	int offset_x = screen_center_x - image_width / 2;
	int offset_y = screen_center_y - image_height / 2;
	return { offset_x, offset_y };
}

std::vector<double> GetCenter(std::vector<double> topLeft, std::vector<double> bottomRight)
{
	double x = (topLeft[0] + bottomRight[0]) / 2.0;
	double y = (topLeft[1] + bottomRight[1]) / 2.0;
	std::vector<double> center = { x, y };
	return center;
}

void DrawRectangle(int color, float x1, float y1, float x2, float y2, bool outline)
{
	g_ModuleInterface->CallBuiltin(
		"draw_set_color", {
		 color
		}
	);

	g_ModuleInterface->CallBuiltin(
		"draw_rectangle", {
			x1, y1, x2, y2, outline
		}
	);
}

void DrawImage(int x, int y, int transparency)
{
	RValue sprite_index = g_ModuleInterface->CallBuiltin(
		"asset_get_index", {
			"aldarian_danger_banner"
		}
	);

	g_ModuleInterface->CallBuiltin(
		"draw_sprite_ext", {
			sprite_index, -1, x, y, 1, 1, 0, 16777215, transparency / 100.0
		}
	);
}

void FadeInImage(double seconds_per_cycle, int repeat_count) {
	// Window corners
	std::vector<double> window_top_left = { 0.0, 0.0 };
	std::vector<double> window_top_right = { window_width, 0.0 };
	std::vector<double> window_bottom_left = { 0.0, window_height };
	std::vector<double> window_bottom_right = { window_width, window_height };

	// Window center
	auto center = GetCenter(window_top_left, window_bottom_right);
	auto centered_offset = GetCenterOffset(center[0], center[1], 400, 120);

	if (!fade_initialized) {
		fade_start_time = GetCurrentSystemTime();
		fade_initialized = true;
	}

	double cycle_ms = seconds_per_cycle * 1000.0;
	uint64_t elapsed = GetCurrentSystemTime() - fade_start_time;

	int current_cycle = elapsed / cycle_ms;

	if (current_cycle >= repeat_count)
		return;

	double cycle_position = static_cast<double>(elapsed % static_cast<int>(cycle_ms));
	double half_cycle = cycle_ms / 2.0;
	int transparency = 0;

	if (cycle_position <= half_cycle) {
		// Fade in: 0 -> 100
		transparency = static_cast<int>((cycle_position / half_cycle) * 100);
	}
	else {
		// Fade out: 100 -> 0
		double fade_out_pos = cycle_position - half_cycle;
		transparency = static_cast<int>(((half_cycle - fade_out_pos) / half_cycle) * 100);
	}

	transparency = std::clamp(transparency, 0, 100);
	DrawImage(centered_offset.first, centered_offset.second, transparency);
}

void DrawDashedBorder(
	float dash_len,
	float dash_thk,
	float speed,          // pixels per second
	float screen_width,
	float screen_height,
	uint64_t current_time_ms
) {
	// Static dash state
	static std::vector<float> dash_positions;  // positions around perimeter
	static bool initialized = false;

	float top_len = screen_width;
	float right_len = screen_height;
	float bottom_len = screen_width;
	float left_len = screen_height;
	float perimeter = 2.0f * (screen_width + screen_height);

	// Desired dash spacing (avg)
	float target_spacing = perimeter / 40.0f;

	if (!initialized) {
		// Compute dash count per edge
		int top_count = static_cast<int>(std::floor(top_len / target_spacing));
		int right_count = static_cast<int>(std::floor(right_len / target_spacing));
		int bottom_count = static_cast<int>(std::floor(bottom_len / target_spacing));
		int left_count = static_cast<int>(std::floor(left_len / target_spacing));

		// Store total dashes spaced along perimeter
		dash_positions.clear();
		for (int i = 0; i < top_count; ++i)
			dash_positions.push_back((top_len / top_count) * i);
		for (int i = 0; i < right_count; ++i)
			dash_positions.push_back(top_len + (right_len / right_count) * i);
		for (int i = 0; i < bottom_count; ++i)
			dash_positions.push_back(top_len + right_len + (bottom_len / bottom_count) * i);
		for (int i = 0; i < left_count; ++i)
			dash_positions.push_back(top_len + right_len + bottom_len + (left_len / left_count) * i);

		initialized = true;
	}

	// Time-based offset
	static uint64_t last_time = current_time_ms;
	float delta_sec = (current_time_ms - last_time) / 1000.0f;
	last_time = current_time_ms;

	static float offset = 0.0f;
	offset += speed * delta_sec;
	if (offset > perimeter) offset -= perimeter;

	// Draw all dashes with animated offset
	for (float base_pos : dash_positions) {
		float pos = base_pos + offset;
		if (pos >= perimeter) pos -= perimeter;

		if (pos < top_len) {
			float x = pos;
			DrawRectangle(255, x, 0.0f, x + dash_len, dash_thk, false);
		}
		else if (pos < top_len + right_len) {
			float y = pos - top_len;
			DrawRectangle(255, screen_width - dash_thk, y, screen_width, y + dash_len, false);
		}
		else if (pos < top_len + right_len + bottom_len) {
			float x = screen_width - (pos - (top_len + right_len));
			DrawRectangle(255, x, screen_height - dash_thk, x + dash_len, screen_height, false);
		}
		else {
			float y = screen_height - (pos - (top_len + right_len + bottom_len));
			DrawRectangle(255, 0.0f, y, dash_thk, y + dash_len, false);
		}
	}
}

void PlaySoundEffect(const char* sound_name, int priority, double gain)
{
	const auto sound_index = g_ModuleInterface->CallBuiltin(
		"asset_get_index",
		{ sound_name }
	);

	g_ModuleInterface->CallBuiltin(
		"audio_play_sound",
		{ sound_index, 100, false, gain }
	);
}

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

bool FairyBuffIsActive()
{
	RValue ari = global_instance->GetMember("__ari");
	RValue status_effects = ari.GetMember("status_effects");
	RValue effects = status_effects.GetMember("effects");
	RValue inner = effects.GetMember("inner");

	if (StructVariableExists(inner, std::to_string(status_effect_name_to_id_map["fairy"]).c_str()))
	{
		RValue fairy_status = inner.GetMember(std::to_string(status_effect_name_to_id_map["fairy"]));
		return fairy_status.m_Kind == VALUE_OBJECT;
	}

	return false;
}

double GetInvulnerabilityHits()
{
	RValue ari = global_instance->GetMember("__ari");
	return ari.GetMember("invulnerable_hits").ToDouble();
}

void SetInvulnerabilityHits(double amount)
{
	RValue ari = *global_instance->GetRefMember("__ari");
	double invulnerability_hits = ari.GetMember("invulnerable_hits").ToDouble();

	if (amount == 0)
		*ari.GetRefMember("invulnerable_hits") = amount;
	else
		*ari.GetRefMember("invulnerable_hits") = invulnerability_hits + amount;
}

void SetFireBreathTime(double value)
{
	// Stop the fire breath spell
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("fire_breath_time") = value;
}

void DisableAllPerks()
{
	std::unordered_set<int> perks_to_disable = {};

	std::vector<std::string> struct_field_names = {};
	auto GetStructFieldNames = [&](IN const char* MemberName, IN OUT RValue* Value) {
		struct_field_names.push_back(MemberName);
		return false;
	};

	RValue dragon_shrine_data = global_instance->GetMember("__dragon_shrine_data");
	RValue inner = dragon_shrine_data.GetMember("inner");

	// Combat Perks
	RValue combat = inner.GetMember("combat");
	g_ModuleInterface->EnumInstanceMembers(combat, GetStructFieldNames);
	for (std::string field_name : struct_field_names)
	{
		if (field_name.contains("tier"))
		{
			size_t array_length;
			RValue tier = combat.GetMember(field_name);
			g_ModuleInterface->GetArraySize(tier, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* entry;
				g_ModuleInterface->GetArrayEntry(tier, i, entry);

				perks_to_disable.insert(entry->GetMember("perk").ToInt64());
			}
		}
	}

	// Mining Perks
	struct_field_names = {};
	RValue mining = inner.GetMember("mining");
	g_ModuleInterface->EnumInstanceMembers(mining, GetStructFieldNames);
	for (std::string field_name : struct_field_names)
	{
		if (field_name.contains("tier"))
		{
			size_t array_length;
			RValue tier = mining.GetMember(field_name);
			g_ModuleInterface->GetArraySize(tier, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* entry;
				g_ModuleInterface->GetArrayEntry(tier, i, entry);

				perks_to_disable.insert(entry->GetMember("perk").ToInt64());
			}
		}
	}

	// Cooking Perks
	perks_to_disable.insert(perk_name_to_id_map["snacktime"]);

	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue __ari_perks_active = *__ari.GetRefMember("perks_active");

	for (int perk : perks_to_disable)
		__ari_perks_active[perk] = false;
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
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to look up enemy name: %s", MOD_NAME, VERSION, enemy_name.c_str());
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
	std::unordered_set<std::string> lift_key_names = { 
		UPPER_MINES_KEY_F5_NAME, UPPER_MINES_KEY_F10_NAME, UPPER_MINES_KEY_F15_NAME,
		TIDE_CAVERNS_KEY_F20_NAME, TIDE_CAVERNS_KEY_F25_NAME, TIDE_CAVERNS_KEY_F30_NAME,TIDE_CAVERNS_KEY_F35_NAME,
		DEEP_EARTH_KEY_F40_NAME, DEEP_EARTH_KEY_F45_NAME, DEEP_EARTH_KEY_F50_NAME, DEEP_EARTH_KEY_F55_NAME,
		LAVA_CAVES_KEY_F60_NAME, LAVA_CAVES_KEY_F65_NAME, LAVA_CAVES_KEY_F70_NAME, LAVA_CAVES_KEY_F75_NAME,
		RUINS_KEY_F80_NAME, RUINS_KEY_F85_NAME, RUINS_KEY_F90_NAME, RUINS_KEY_F95_NAME, RUINS_KEY_F100_NAME
	};
	std::unordered_set<std::string> orb_item_names = { TIDE_CAVERNS_ORB, DEEP_EARTH_ORB, LAVA_CAVES_ORB, RUINS_ORB }; // TODO: Add other orbs
	std::vector<std::string> custom_potions = { SUSTAINING_POTION_NAME, HEALTH_SALVE_NAME, STAMINA_SALVE_NAME, MANA_SALVE_NAME }; // TODO: Change to unordered_set
	std::vector<std::string> cursed_armor = { CURSED_HELMET_NAME, CURSED_CHESTPIECE_NAME, CURSED_PANTS_NAME, CURSED_BOOTS_NAME, CURSED_GLOVES_NAME }; // TODO: Change to unordered_set

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

			// Custom potions
			for (std::string custom_potion : custom_potions)
			{
				if (item_name == custom_potion)
				{
					deep_dungeon_items.insert(item_id);
					salve_name_to_id_map[item_name] = item_id;
				}
			}

			// All consumable items (except Deep Dungeon items)
			if (configuration.restrict_items && !deep_dungeon_items.contains(item_id))
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

					if (configuration.restrict_armor && array_element->ToString() == "armor")
						*item->GetRefMember("defense") = 0;

					if (array_element->ToString() == "weapon")
					{
						if (item_name == MISTPOOL_SWORD_NAME)
							deep_dungeon_items.insert(item_id);
						else if (configuration.restrict_weapons)
						{
							*item->GetRefMember("damage") = 0;
							restricted_items.insert(item_id);
						}
					}

					if (configuration.restrict_tools && array_element->ToString() == "pick_axe")
					{
						if (item_name != MISTPOOL_PICK_AXE_NAME)
							restricted_items.insert(item_id);
					}

					if (configuration.restrict_items && array_element->ToString() == "bomb")
					{
						*item->GetRefMember("damage") = 0;
						*item->GetRefMember("bomb")->GetRefMember("damage") = 0;
						restricted_items.insert(item_id);
					}
				}
			}

			// All snake oils
			if (configuration.restrict_items && item_name.contains("snake_oil"))
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
	SetItemHealthModifier(item_name_to_id_map[HEALTH_SALVE_NAME], configuration.health_salve_potency);
	SetItemStaminaModifier(item_name_to_id_map[STAMINA_SALVE_NAME], configuration.stamina_salve_potency);
	SetItemManaModifier(item_name_to_id_map[MANA_SALVE_NAME], configuration.mana_salve_potency);

	SetItemShopPrice(item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], configuration.mistpool_equipment_store_price);
	SetItemShopPrice(item_name_to_id_map[MISTPOOL_SWORD_NAME], configuration.mistpool_equipment_store_price);
	for (std::string armor_name : MISTPOOL_ARMOR_NAMES)
		SetItemShopPrice(item_name_to_id_map[armor_name], configuration.mistpool_equipment_store_price);
	for (const auto& pair : salve_name_to_id_map)
		SetItemShopPrice(pair.second, configuration.salves_store_price);
}

void MarkDungeonTutorialUnseen()
{
	RValue ari = *global_instance->GetRefMember("__ari");
	RValue tutorials_seen = *ari.GetRefMember("tutorials_seen");

	RValue* mines_tutorial;
	g_ModuleInterface->GetArrayEntry(tutorials_seen, tutorial_name_to_id_map["mines"], mines_tutorial);

	*mines_tutorial = false; // TODO: This works, but should only be called once per save file.
}

void ModifyMistpoolWeaponSprites()
{
	// Sprite indexes for the "Scrap Metal Sword" which is used as the Mistpool Sword.
	RValue spr_weapon_sword_scrap_metal_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_s" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_s" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_s" });
	RValue spr_weapon_sword_scrap_metal_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_down_attack_e" });
	RValue spr_ui_item_tool_scrap_metal_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_scrap_metal_sword" });

	if (floor_number < 20)
	{
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t0_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t0_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t0_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t0_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t0_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t0_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t0_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t0_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t0_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t0_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t0_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t0_sword });
	}
	else if (floor_number < 40)
	{
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t1_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t1_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t1_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t1_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t1_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t1_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t1_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t1_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t1_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t1_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t1_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t1_sword });
	}
	else if (floor_number < 60)
	{
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t2_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t2_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t2_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t2_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t2_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t2_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t2_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t2_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t2_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t2_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t2_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t2_sword });
	}
	else
	{
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t3_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t3_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t3_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t3_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t3_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t3_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t3_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t3_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t3_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t3_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t3_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t3_sword });
	}
}

void ModifyMistpoolPickaxeSprites()
{
	// Sprite indexes for the "Worn Pickaxe" which is used as the Mistpool Pickaxe.
	RValue spr_tool_pick_axe_worn_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_e" });
	RValue spr_tool_pick_axe_worn_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_n" });
	RValue spr_tool_pick_axe_worn_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_s" });
	RValue spr_ui_item_tool_rusty_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_rusty_pickaxe" });

	if (floor_number < 20)
	{
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t0_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t0_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t0_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t0_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t0_pickaxe });
	}
	else if (floor_number < 40)
	{
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t1_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t1_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t1_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t1_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t1_pickaxe });
	}
	else if (floor_number < 60)
	{
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t2_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t2_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t2_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t2_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t2_pickaxe });
	}
	else
	{
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t3_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t3_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t3_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t3_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t3_pickaxe });
	}
}


void ScaleMistpoolWeapon(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");

	RValue* sword_scrap_metal;
	g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map["sword_scrap_metal"], sword_scrap_metal);

	if (in_dungeon)
	{
		int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
		int damage = (modified_floor_number / 4) + 3;
		*sword_scrap_metal->GetRefMember("damage") = damage;
	}
	else
		*sword_scrap_metal->GetRefMember("damage") = 1;
}

void ScaleMistpoolArmor(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");
	for (std::string mistpool_armor_name : MISTPOOL_ARMOR_NAMES)
	{
		RValue* mistpool_armor_piece;
		g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[mistpool_armor_name], mistpool_armor_piece);

		if (in_dungeon)
		{
			int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 20 : floor_number;
			int defense = modified_floor_number / 20;
			*mistpool_armor_piece->GetRefMember("defense") = defense;
		}
		else
			*mistpool_armor_piece->GetRefMember("defense") = 0;
	}
}

void ScaleMistpoolPickaxe(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");

	RValue* pick_axe_worn;
	g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], pick_axe_worn);

	if (in_dungeon)
	{
		if (floor_number < 20)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 2 : 1;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 1 : 0;
		}
		else if (floor_number < 40)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 3 : 2;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 2 : 1;
		}
		else if (floor_number < 60)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 4 : 3;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 3 : 2;
		}

		else if (floor_number < 80)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 5 : 4;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 4 : 3;
		}

		else if (floor_number < 100)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 6 : 5;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 5 : 4;
		}
	}
	else
	{
		*pick_axe_worn->GetRefMember("damage") = 1;
		*pick_axe_worn->GetRefMember("quality") = 0;
	}
}

void ScaleClassArmor(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");
	for (std::string class_armor_name : CLASS_ARMOR_NAMES)
	{
		RValue* class_armor_piece;
		g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[class_armor_name], class_armor_piece);

		if (in_dungeon)
		{
			int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 20 : floor_number;
			int defense = modified_floor_number / 20;
			*class_armor_piece->GetRefMember("defense") = defense;
		}
		else
			*class_armor_piece->GetRefMember("defense") = 0;
	}
}

int GetRandomSoulStone()
{
	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> random_soul_stone_distribution(0, SOUL_STONE_NAMES.size() - 1);
	return item_name_to_id_map[SOUL_STONE_NAMES[random_soul_stone_distribution(random_generator)]];
}

std::map<Classes, int> CountEquippedClassArmor()
{
	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	std::map<Classes, int> class_armor_equipped = {};
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "prototype"))
			{
				RValue prototype = item.GetMember("prototype");
				if (StructVariableExists(prototype, "recipe_key"))
				{
					RValue recipe_key = prototype.GetMember("recipe_key");
					for (const auto& class_armor : CLASS_NAME_TO_ARMOR_NAMES_MAP)
					{
						if (CLASS_NAME_TO_ARMOR_NAMES_MAP.at(class_armor.first).contains(recipe_key.ToString()))
							class_armor_equipped[class_armor.first]++;
					}
				}
			}
		}
	}

	return class_armor_equipped;
}

std::map<int, int> GetClassArmorInfusions()
{
	std::map<int, int> class_armor_infusions = {};

	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "infusion") && StructVariableExists(item, "prototype"))
			{
				RValue infusion = item.GetMember("infusion");
				RValue prototype = item.GetMember("prototype");
				if (IsNumeric(infusion) && IsObject(prototype) && StructVariableExists(prototype, "recipe_key") && CLASS_ARMOR_NAMES.contains(prototype.GetMember("recipe_key").ToString()))
					class_armor_infusions[infusion.ToInt64()]++;
			}
		}
	}

	return class_armor_infusions;
}

int GetClericAutoRegenPotency()
{
	int cleric_armor_pieces_equipped = CountEquippedClassArmor()[Classes::CLERIC];
	if (cleric_armor_pieces_equipped == 0)
		return 0;
	if (cleric_armor_pieces_equipped < 3)
		return 1;
	if (cleric_armor_pieces_equipped < 5)
		return 2;
	if (cleric_armor_pieces_equipped == 5)
		return 3;
}

double GetDarkKnightDrainPotency()
{
	int dark_knight_pieces_equipped = CountEquippedClassArmor()[Classes::DARK_KNIGHT];
	if (dark_knight_pieces_equipped == 0)
		return 0;
	if (dark_knight_pieces_equipped < 3)
		return 0.03;
	if (dark_knight_pieces_equipped < 5)
		return 0.05;
	if (dark_knight_pieces_equipped == 5)
		return 0.08;
}

double GetPaladinHolyCirclePotency()
{
	int paladin_pieces_equipped = CountEquippedClassArmor()[Classes::PALADIN];
	if (paladin_pieces_equipped == 0)
		return 0;
	if (paladin_pieces_equipped < 3)
		return 0.05;
	if (paladin_pieces_equipped < 5)
		return 0.10;
	if (paladin_pieces_equipped == 5)
		return 0.15;
}

int ScaleTemperanceDamage(int current_health, int max_health, int damage)
{
	if (max_health <= 0)
		return damage;

	current_health = std::clamp(current_health, 0, max_health);

	int health_pct = (current_health * 100) / max_health;

	static const struct {
		int health;
		int multiplier;
	} breakpoints[] = {
		{100,  20},  // -80%
		{95,   40},
		{90,   60},
		{85,   80},
		{80,  100},  // 0%
		{75,  110},
		{70,  120},
		{65,  130},
		{60,  140},
		{55,  150},
		{40,  160},
		{35,  170},
		{30,  180},
		{25,  190},
		{20,  200},
		{15,  210},
		{10,  220},
		{5,   230},
		{0,   240}
	};

	constexpr int count = sizeof(breakpoints) / sizeof(breakpoints[0]);

	for (int i = 0; i < count - 1; ++i)
	{
		int h1 = breakpoints[i].health;
		int m1 = breakpoints[i].multiplier;
		int h2 = breakpoints[i + 1].health;
		int m2 = breakpoints[i + 1].multiplier;

		if (health_pct <= h1 && health_pct >= h2)
		{
			int numerator = (health_pct - h2) * (m1 - m2);
			int denominator = (h1 - h2);
			int multiplier = m2 + numerator / denominator;

			return (damage * multiplier) / 100;
		}
	}

	return (damage * breakpoints[count - 1].multiplier) / 100;
}

ElementalSealEffects GetRandomElementalSealEffect()
{
	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> random_elemental_seal_effect_distribution(0, magic_enum::enum_count<ElementalSealEffects>() - 1);
	return magic_enum::enum_value<ElementalSealEffects>(random_elemental_seal_effect_distribution(random_generator));
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

void ModifySpellCosts(bool reset_cost, bool in_dungeon) {
	size_t array_length = 0;
	RValue spells = global_instance->GetMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		int cost = reset_cost ? spell_id_to_default_cost_map[i] : spell_id_to_default_cost_map[i] / 2;
		if (in_dungeon && i == spell_name_to_id_map["growth"])
			cost = reset_cost ? spell_id_to_default_cost_map[i] / 2 : spell_id_to_default_cost_map[i] / 4;
		if (active_greater_sigils.contains(GreaterSigils::CHAIN_SPELL))
			cost = 0;
		if (i == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			cost = 0;
		if (i == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			cost = 0;

		*array_element->GetRefMember("cost") = cost;
	}
}

RValue LocalizeString(CInstance* Self, CInstance* Other, std::string localization_key)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(localization_key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void SpawnLadder(CInstance* Self, CInstance* Other, int64_t x_coord, int64_t y_coord)
{
	CScript* gml_Script_spawn_ladder = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_LADDER,
		(PVOID*)&gml_Script_spawn_ladder
	);

	RValue x = (x_coord * 2) / 16;
	RValue y = (y_coord * 2) / 16;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* rvalue_array[2] = { x_ptr, y_ptr };
	RValue retval;
	gml_Script_spawn_ladder->m_Functions->m_ScriptFunction(
		Self,
		Other,
		retval,
		2,
		rvalue_array
	);
}

void SpawnMonster(CInstance* Self, CInstance* Other, int room_x, int room_y, int monster_id)
{
	CScript* gml_script_spawn_monster = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_MONSTER,
		(PVOID*)&gml_script_spawn_monster
	);

	RValue x = room_x;
	RValue y = room_y;
	RValue monster = monster_id;

	RValue result;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* monster_ptr = &monster;
	RValue* arguments[3] = { x_ptr, y_ptr, monster_ptr };

	gml_script_spawn_monster->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		arguments
	);
}

void ModifyRockClodAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster)
{
	const enum class Patterns {
		WALL, // Shoots a wall of 10 pellets repeatedly 5 times
		SPIN, // Rotates 18-degrees at a time while shooting 5 pellets in a line at various angles
		SPLIT // Shoots a single pellet that then splits into many that repeatedly split
	};

	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> random_pattern_distribution(0, magic_enum::enum_count<Patterns>() - 1);

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			Patterns pattern = magic_enum::enum_value<Patterns>(random_pattern_distribution(random_generator));
			if (is_boss_battle)
			{
				pattern = Patterns::WALL;
				boss_monsters_configured++;
			}

			if (pattern == Patterns::WALL)
			{
				// Shoots a wall of 10 pellets repeatedly 5 times
				if(!is_outbreak)
					StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

				StructVariableSet(config_clone, "launcher", false);
				StructVariableSet(config_clone, "attack_sequence", 5.0);
				StructVariableSet(config_clone, "attack_legion", 10.0);
				StructVariableSet(config_clone, "attack_sequence_turn", -1.0);
				StructVariableSet(config_clone, "attack_sequence_image_speed", -1.0);
				StructVariableSet(config_clone, "projectile_speed", 3.0);
				StructVariableSet(config_clone, "split_distance", -1.0);
				StructVariableSet(config_clone, "split_depth", -1.0);
				StructVariableSet(config_clone, "split_angle", -1.0);
				StructVariableSet(monster, "config", config_clone);
				StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
			}
			else if (pattern == Patterns::SPIN)
			{
				// Rotates 18-degrees at a time while shooting 5 pellets in a small cone
				double attack_sequence = 20;
				double attack_legion = 3; // 5
				double attack_sequence_turn = 18;
				double split_distance = 20; // 5
				double split_depth = 2;
				double split_angle = 40;

				if (!is_outbreak)
					StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

				StructVariableSet(config_clone, "launcher", false);
				StructVariableSet(config_clone, "attack_sequence", attack_sequence);
				StructVariableSet(config_clone, "attack_legion", attack_legion);
				StructVariableSet(config_clone, "attack_sequence_turn", attack_sequence_turn);
				StructVariableSet(config_clone, "attack_sequence_image_speed", 3.0);
				StructVariableSet(config_clone, "projectile_speed", 3.0);
				StructVariableSet(config_clone, "split_distance", split_distance);
				StructVariableSet(config_clone, "split_depth", split_depth);
				StructVariableSet(config_clone, "split_angle", split_angle);
				StructVariableSet(monster, "config", config_clone);
				StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
			}
			else if (pattern == Patterns::SPLIT)
			{
				// Shoots a single pellet that then splits into many that repeatedly split
				double attack_sequence = 3;
				double attack_legion = 1;
				double attack_sequence_turn = -1;
				double split_distance = 20;
				double split_depth = 5;
				double split_angle = 20;

				if (!is_outbreak)
					StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

				StructVariableSet(config_clone, "launcher", false);
				StructVariableSet(config_clone, "attack_sequence", attack_sequence);
				StructVariableSet(config_clone, "attack_legion", attack_legion);
				StructVariableSet(config_clone, "attack_sequence_turn", attack_sequence_turn);
				StructVariableSet(config_clone, "attack_sequence_image_speed", -1.0);
				StructVariableSet(config_clone, "projectile_speed", 3.0);
				StructVariableSet(config_clone, "split_distance", split_distance);
				StructVariableSet(config_clone, "split_depth", split_depth);
				StructVariableSet(config_clone, "split_angle", split_angle);
				StructVariableSet(monster, "config", config_clone);
				StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
			}
		}
	}
}

void ModifyStalagmiteAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster)
{
	const enum class Modes {
		DONUT_PB,
		CROSS_X,
		CHECKERBOARD
	};

	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> zero_to_one_distribution(0, 1);
	std::uniform_int_distribution<size_t> random_mode_distribution(0, magic_enum::enum_count<Modes>() - 1);

	// Setup the state tracker variable.
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	// Setup the attack pattern mode.
	RValue attack_pattern_mode_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__attack_pattern_mode" });
	if (!attack_pattern_mode_exists.ToBoolean())
	{
		if (is_boss_battle)
		{
			Modes mode = Modes::CHECKERBOARD;
			StructVariableSet(monster, "__deep_dungeon__attack_pattern_mode", magic_enum::enum_name(mode));
			boss_monsters_configured++;
		}
		else
		{
			Modes mode = magic_enum::enum_value<Modes>(zero_to_one_distribution(random_generator)); // Only use DONUT_PB and CROSS_X modes for Dread Beasts.
			StructVariableSet(monster, "__deep_dungeon__attack_pattern_mode", magic_enum::enum_name(mode));
		}
	}
	Modes attack_pattern_mode = magic_enum::enum_cast<Modes>(monster.GetMember("__deep_dungeon__attack_pattern_mode").ToString()).value();

	// Control the attack patterns.
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			// Randomly choose starting attack. Each pattern has two alternating attacks.
			int starting_pattern = zero_to_one_distribution(random_generator);

			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (attack_pattern_mode == Modes::DONUT_PB)
			{
				if (starting_pattern == 0) // Donut AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { donut_aoe_points.size() });
					for (int i = 0; i < donut_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, donut_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, donut_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
				else if (starting_pattern == 1) // PB AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { pb_aoe_points.size() });
					for (int i = 0; i < pb_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, pb_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, pb_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
			}
			else if (attack_pattern_mode == Modes::CROSS_X)
			{
				if (starting_pattern == 0) // Cross AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { cross_aoe_points.size() });
					for (int i = 0; i < cross_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, cross_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, cross_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
				else if (starting_pattern == 1) // X AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { x_aoe_points.size() });
					for (int i = 0; i < x_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, x_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, x_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
			}
			else if (attack_pattern_mode == Modes::CHECKERBOARD)
			{
				if (starting_pattern == 0) // Checkerboard AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_a_points.size() });
					for (int i = 0; i < checkerboard_room_wide_a_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_a_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_a_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
				else if (starting_pattern == 1) // Checkerboard Inverted AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_b_points.size() });
					for (int i = 0; i < checkerboard_room_wide_b_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_b_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_b_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
			}
		}
	}

	if (custom_attack_pattern_exists.ToBoolean() && StructVariableExists(monster, "fsm"))
	{
		RValue state_id = monster.GetMember("fsm").GetMember("state").GetMember("state_id");
		if (state_id.ToInt64() == monster_category_to_state_id_map["mite"]["attack"])
			*monster.GetRefMember("__deep_dungeon__wait_to_change_attack_pattern") = false;

		if (state_id.ToInt64() == monster_category_to_state_id_map["mite"]["tired"] && !wait_to_change_attack_pattern.ToBoolean())
		{
			*monster.GetRefMember("__deep_dungeon__wait_to_change_attack_pattern") = true;

			int custom_attack_pattern = monster.GetMember("__deep_dungeon__custom_attack_pattern").ToInt64() + 1;
			if (custom_attack_pattern > 1)
				custom_attack_pattern = 0;

			if (attack_pattern_mode == Modes::DONUT_PB)
			{
				if (custom_attack_pattern == 0) // Donut AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { donut_aoe_points.size() });
						for (int i = 0; i < donut_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, donut_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, donut_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
				else if (custom_attack_pattern == 1) // PB AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { pb_aoe_points.size() });
						for (int i = 0; i < pb_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, pb_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, pb_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
			}
			else if (attack_pattern_mode == Modes::CROSS_X)
			{
				if (custom_attack_pattern == 0) // Cross AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { cross_aoe_points.size() });
						for (int i = 0; i < cross_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, cross_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, cross_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
				else if (custom_attack_pattern == 1) // X AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { x_aoe_points.size() });
						for (int i = 0; i < x_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, x_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, x_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
			}
			else if (attack_pattern_mode == Modes::CHECKERBOARD)
			{
				if (custom_attack_pattern == 0) // Checkerboard AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_a_points.size() });
						for (int i = 0; i < checkerboard_room_wide_a_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_a_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_a_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
				else if (custom_attack_pattern == 1) // Checkerboard Inverted AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_b_points.size() });
						for (int i = 0; i < checkerboard_room_wide_b_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_b_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_b_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
			}

			*monster.GetRefMember("__deep_dungeon__custom_attack_pattern") = custom_attack_pattern;
		}
	}
}

void ModifySaplingAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id)
{
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "sticky", true);
			StructVariableSet(config_clone, "free_fly", true);
			StructVariableSet(config_clone, "air_speed_modifier", 0.6);
			StructVariableSet(config_clone, "use_circle", true);
			StructVariableSet(config_clone, "speed", 0.75);
			// "jump_speed": -2.0,
			StructVariableSet(config_clone, "attack_radius", 624);
			StructVariableSet(config_clone, "max_jump_radius", 624);
			StructVariableSet(config_clone, "aggro_radius", 624);
			StructVariableSet(config_clone, "sap_children_birth_timer", 30);
			StructVariableSet(config_clone, "sap_children_birth_distance", 15);
			StructVariableSet(config_clone, "sap_children", 3);
			StructVariableSet(config_clone, "sap_children_species", RValue(monster_id_to_name_map[monster_id]));

			RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 15 }); // 45
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 25 }); // 55
			StructVariableSet(config_clone, "acknowledgment", acknowledgment);

			RValue tired = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { tired, 0, 10 }); // 90
			g_ModuleInterface->CallBuiltin("array_set", { tired, 1, 30 }); // 110
			StructVariableSet(config_clone, "tired", tired);

			RValue windup = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup, 0, 1 }); // 35
			g_ModuleInterface->CallBuiltin("array_set", { windup, 1, 1 }); // 55
			StructVariableSet(config_clone, "windup", windup);

			if (monster_id == monster_name_to_id_map["sapling_pink"])
			{
				StructVariableSet(config_clone, "hyper_armor", 3);
			}

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyShroomAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "spew_lava", true);
			StructVariableSet(config_clone, "lava_damage", 1);
			StructVariableSet(config_clone, "lava_angle", 90);
			StructVariableSet(config_clone, "lava_distance", 16);
			StructVariableSet(config_clone, "lava_timer", 500); // 300
			StructVariableSet(config_clone, "lava_count", 4);
			StructVariableSet(config_clone, "attack_radius", 704);
			StructVariableSet(config_clone, "hide_radius", 360);
			StructVariableSet(config_clone, "shadow_threshold", 0.18); // 0.2
			StructVariableSet(config_clone, "fade_in_rate", 0.18); // 0.2
			StructVariableSet(config_clone, "fade_out_rate", 0.18); // 0.2
			StructVariableSet(config_clone, "windup_friction", 0.93); // 0.93
			StructVariableSet(config_clone, "push_force", 500);
			StructVariableSet(config_clone, "ari_bounce_distance", 500);

			RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 1 });
			StructVariableSet(config_clone, "acknowledgment", acknowledgment);

			RValue tired = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { tired, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { tired, 1, 1 });
			StructVariableSet(config_clone, "tired", tired);

			RValue windup = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { windup, 1, 1 });
			StructVariableSet(config_clone, "windup", windup);

			RValue wiggle = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { wiggle, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { wiggle, 1, 1 });
			StructVariableSet(config_clone, "wiggle", wiggle);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyEnchanternAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "charge_speed", 3);
			StructVariableSet(config_clone, "flee_speed", 2);
			StructVariableSet(config_clone, "attack_radius", 384);
			StructVariableSet(config_clone, "drops_balls", true);

			RValue charge_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { charge_timer, 0, 360 }); // 540
			g_ModuleInterface->CallBuiltin("array_set", { charge_timer, 1, 400 }); // 600
			StructVariableSet(config_clone, "charge_timer", charge_timer);

			RValue projectile_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { projectile_timer, 0, 200 }); // 400
			g_ModuleInterface->CallBuiltin("array_set", { projectile_timer, 1, 400 }); // 800
			StructVariableSet(config_clone, "projectile_timer", projectile_timer);

			RValue flee_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 0, 180 }); // 120
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 1, 200 }); // 160
			StructVariableSet(config_clone, "flee_timer", flee_timer);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifySpiritAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			if (!is_outbreak)
				StructVariableSet(config_clone, "projectile_damage", config_clone.GetMember("projectile_damage").ToDouble() * 2);

			RValue idle_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { idle_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { idle_duration, 1, 2 });
			StructVariableSet(config_clone, "idle_duration", idle_duration);

			RValue tired_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { tired_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { tired_duration, 1, 2 });
			StructVariableSet(config_clone, "tired_duration", tired_duration);
			
			RValue teleport_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_duration, 1, 2 });
			StructVariableSet(config_clone, "teleport_duration", teleport_duration);

			RValue teleport_distance_from_player = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_distance_from_player, 0, 10 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_distance_from_player, 1, 30 });
			StructVariableSet(config_clone, "teleport_distance_from_player", teleport_distance_from_player);

			if (monster_id == monster_name_to_id_map["spirit_purple"])
			{
				//StructVariableSet(config_clone, "belt_size", 4);
				StructVariableSet(config_clone, "projectile_speed", 4.5);
				StructVariableSet(config_clone, "projectile_turn_rate", 0.4);
				StructVariableSet(config_clone, "projectile_distance", 24); // Distance they rotate at from the spirit
				StructVariableSet(config_clone, "rotation_speed", 8);
				StructVariableSet(config_clone, "shot_rate", 60);
				StructVariableSet(config_clone, "pre_attack_wait", 60);

				RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
				g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 10 });
				g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 20 });
				StructVariableSet(config_clone, "acknowledgment", acknowledgment);
			}

			if (monster_id == monster_name_to_id_map["spirit"])
			{
				StructVariableSet(config_clone, "projectile_turn_rate", 0.8);

				RValue projectile_life_time = g_ModuleInterface->CallBuiltin("array_create", { 2 });
				g_ModuleInterface->CallBuiltin("array_set", { projectile_life_time, 0, 240 });
				g_ModuleInterface->CallBuiltin("array_set", { projectile_life_time, 1, 240 });
				StructVariableSet(config_clone, "projectile_life_time", projectile_life_time);
			}

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyCatAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "charge_range", 192);
			StructVariableSet(config_clone, "attack_movement_speed", 8);

			RValue windup_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 0, 15 });
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 1, 30 });
			StructVariableSet(config_clone, "windup_duration", windup_duration);

			RValue attack_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 0, 180 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 1, 180 });
			StructVariableSet(config_clone, "attack_duration", attack_duration);

			RValue attack_stall_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_stall_duration, 0, 15 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_stall_duration, 1, 30 });
			StructVariableSet(config_clone, "attack_stall_duration", attack_stall_duration);

			if (monster_id == monster_name_to_id_map["cat"])
			{
				StructVariableSet(config_clone, "petrified_duration", 60);
				StructVariableSet(config_clone, "petrified_shakes", 10);
			}

			if (monster_id == monster_name_to_id_map["cat_void"])
			{
				*config_clone.GetRefMember("light_hater")->GetRefMember("light_health") = 99999;
			}

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyBatAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "speed", 3);
			StructVariableSet(config_clone, "flee_speed", 3); // -1
			StructVariableSet(config_clone, "attack_radius", 144);
			StructVariableSet(config_clone, "variant_attack", true);

			RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 9 }); // 45
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 11 }); // 55
			StructVariableSet(config_clone, "acknowledgment", acknowledgment);

			RValue flee_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 0, 90 }); // 30
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 1, 120 }); // 45
			StructVariableSet(config_clone, "flee_timer", flee_timer);

			RValue attack_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 1, 2 });
			StructVariableSet(config_clone, "attack_duration", attack_duration);

			RValue windup_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 0, 5 }); // 15
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 1, 10 }); // 30
			StructVariableSet(config_clone, "windup_duration", windup_duration);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyTomeAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "acknowledgment", 15); // 32
			StructVariableSet(config_clone, "flying_speed", 6); // 2.4
			StructVariableSet(config_clone, "flying_timeout", 450); // 220
			StructVariableSet(config_clone, "steering", 0.8); // 0.6
			StructVariableSet(config_clone, "charge_up_time", 1); // 100
			StructVariableSet(config_clone, "idle_override_len", 1); // 60
			StructVariableSet(config_clone, "wind_attack_duration", 60); // 135
			StructVariableSet(config_clone, "stun_star_duration", 1); // 60
			StructVariableSet(config_clone, "stun_blink_duration", 1); // 180
			StructVariableSet(config_clone, "blink_timing", 1); // 60
			StructVariableSet(config_clone, "blink_timing_decrement", 1); // 10
			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyRockStackAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "speed", 10); // 1.5
			StructVariableSet(config_clone, "aggro_radius", 624); // 256
			StructVariableSet(config_clone, "air_speed", 10); // 1.25
			StructVariableSet(config_clone, "air_speed_starting", 10); // 0
			StructVariableSet(config_clone, "air_speed_max", 10); // 0.02
			StructVariableSet(config_clone, "rising_duration", 10); // 30
			StructVariableSet(config_clone, "fall_speed", 15); // 5 // 1
			StructVariableSet(config_clone, "hop_threshold", 9999); // 48

			RValue fell_stall = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { fell_stall, 0, 1 }); // 30 // 60
			g_ModuleInterface->CallBuiltin("array_set", { fell_stall, 1, 2 }); // 45 // 75
			StructVariableSet(config_clone, "fell_stall", fell_stall);

			RValue air_stall = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { air_stall, 0, 75 }); // 150
			g_ModuleInterface->CallBuiltin("array_set", { air_stall, 1, 150 }); // 250
			StructVariableSet(config_clone, "air_stall", air_stall);

			RValue air_wait = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { air_wait, 0, 1 }); // 15 // 30
			g_ModuleInterface->CallBuiltin("array_set", { air_wait, 1, 2 }); // 30 // 60
			StructVariableSet(config_clone, "air_wait", air_wait);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyGriffinStatueAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", config_clone.GetMember("damage").ToDouble() * 2);

			StructVariableSet(config_clone, "aggro_radius", 624); // 360
			StructVariableSet(config_clone, "speed", 10); // 0.6
			StructVariableSet(config_clone, "stomp_frames", 24); // 6
			StructVariableSet(config_clone, "bonus_damage", 13); // 5
			StructVariableSet(config_clone, "tumble_hit_reduction", 0.4); // 0.75
			StructVariableSet(config_clone, "tumble_hit_speed_reduction", 10); // 1
			StructVariableSet(config_clone, "tumble_hit_speed", 10); // 6
			StructVariableSet(config_clone, "tumble_spd_reduction", 0.4); // 0.2
			StructVariableSet(config_clone, "move_speed", is_boss_battle ? 6 : 3);
			StructVariableSet(config_clone, "chase_rate", 5); // 20
			StructVariableSet(config_clone, "jump_speed_gain", 1); // 0.25
			StructVariableSet(config_clone, "gravity_gain", 1); // 0.25

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyDreadBeastAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster)
{
	int monster_id = monster.GetMember("monster_id").ToInt64();
	if (monster_id == monster_name_to_id_map["rockclod"] || monster_id == monster_name_to_id_map["rockclod_blue"] || monster_id == monster_name_to_id_map["rockclod_green"] || monster_id == monster_name_to_id_map["rockclod_red"]) // TODO: "rockclod_purple" if/when implemented
		ModifyRockClodAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["stalagmite"] || monster_id == monster_name_to_id_map["stalagmite_green"] || monster_id == monster_name_to_id_map["stalagmite_purple"])
		ModifyStalagmiteAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["sapling"] || monster_id == monster_name_to_id_map["sapling_cool"] || monster_id == monster_name_to_id_map["sapling_blue"] || monster_id == monster_name_to_id_map["sapling_purple"] || monster_id == monster_name_to_id_map["sapling_orange"] || monster_id == monster_name_to_id_map["sapling_pink"])
		ModifySaplingAttackPatterns(is_boss_battle, is_outbreak, monster, monster_id);
	if (monster_id == monster_name_to_id_map["mushroom"] || monster_id == monster_name_to_id_map["mushroom_green"] || monster_id == monster_name_to_id_map["mushroom_blue"] || monster_id == monster_name_to_id_map["mushroom_purple"])
		ModifyShroomAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["enchantern"] || monster_id == monster_name_to_id_map["enchantern_blue"])
		ModifyEnchanternAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["spirit"] || monster_id == monster_name_to_id_map["spirit_purple"])
		ModifySpiritAttackPatterns(is_boss_battle, is_outbreak, monster, monster_id);
	if (monster_id == monster_name_to_id_map["cat"] || monster_id == monster_name_to_id_map["cat_void"])
		ModifyCatAttackPatterns(is_boss_battle, is_outbreak, monster, monster_id);
	if (monster_id == monster_name_to_id_map["bat"] || monster_id == monster_name_to_id_map["bat_blue"])
		ModifyBatAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["tome"])
		ModifyTomeAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["rock_stack"])
		ModifyRockStackAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["griffin_statue"])
		ModifyGriffinStatueAttackPatterns(is_boss_battle, is_outbreak, monster);
}

void UnlockRecipe(int item_id, CInstance* Self, CInstance* Other)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue recipe_unlocks = *__ari.GetRefMember("recipe_unlocks");

	if (recipe_unlocks[item_id].m_Real == 0.0)
		recipe_unlocks[item_id] = 1.0; // This value is ultimately what unlocks the recipe.
}

void UnlockLiftKeyRecipe(CInstance* Self, CInstance* Other)
{
	if (!configuration.disable_dungeon_lift)
		return;

	if (floor_number == 5)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F5_NAME], Self, Other);
	else if (floor_number == 10)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F10_NAME], Self, Other);
	else if (floor_number == 15)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F15_NAME], Self, Other);
	else if (floor_number == 20)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], Self, Other);
	else if (floor_number == 25)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME], Self, Other);
	else if (floor_number == 30)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME], Self, Other);
	else if (floor_number == 35)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME], Self, Other);
	else if (floor_number == 40)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], Self, Other);
	else if (floor_number == 45)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME], Self, Other);
	else if (floor_number == 50)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME], Self, Other);
	else if (floor_number == 55)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME], Self, Other);
	else if (floor_number == 60)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], Self, Other);
	else if (floor_number == 65)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME], Self, Other);
	else if (floor_number == 70)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME], Self, Other);
	else if (floor_number == 75)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME], Self, Other);
	else if (floor_number == 80)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F80_NAME], Self, Other);
	else if (floor_number == 85)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F85_NAME], Self, Other);
	else if (floor_number == 90)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F90_NAME], Self, Other);
	else if (floor_number == 95)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F95_NAME], Self, Other);
	else if (floor_number == 100)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F100_NAME], Self, Other);
}

void CreateNotification(bool ignore_cooldown, std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	uint64_t current_system_time = GetCurrentSystemTime();
	if (ignore_cooldown || current_system_time > notification_name_to_last_display_time_map[notification_localization_str] + 5000)
	{
		CScript* gml_script_create_notification = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			GML_SCRIPT_CREATE_NOTIFICATION,
			(PVOID*)&gml_script_create_notification
		);

		RValue result;
		RValue notification = RValue(notification_localization_str);
		RValue* notification_ptr = &notification;
		gml_script_create_notification->m_Functions->m_ScriptFunction(
			Self,
			Other,
			result,
			1,
			{ &notification_ptr }
		);

		notification_name_to_last_display_time_map[notification_localization_str] = current_system_time;
	}
}

void PlayConversation(std::string conversation_localization_str, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_play_conversation = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_CONVERSATION,
		(PVOID*)&gml_script_play_conversation
	);

	RValue zero = 0;
	RValue conversation = RValue(conversation_localization_str);
	RValue undefined;

	RValue* zero_ptr = &zero;
	RValue* conversation_ptr = &conversation;
	RValue* undefined_ptr = &undefined;

	RValue result;
	RValue* arguments[4] = { zero_ptr, conversation_ptr, undefined_ptr, undefined_ptr };

	gml_script_play_conversation->m_Functions->m_ScriptFunction(
		Self,
		Self,
		result,
		4,
		arguments
	);
}

void EmitBark(CInstance* Self, CInstance* Other, RValue bark_id, RValue bark_type)
{
	CScript* gml_script_register_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_BARK_EMITTER_EMIT,
		(PVOID*)&gml_script_register_status_effect
	);

	RValue result;
	RValue* bark_id_ptr = &bark_id;
	RValue* bark_type_ptr = &bark_type;
	RValue* argument_array[2] = { bark_id_ptr, bark_type_ptr };

	gml_script_register_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		2,
		argument_array
	);
}

void SceneAudioPlayerStop(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SCENE_AUDIO_PLAYER_STOP,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

void SpawnTutorial(std::string tutorial_name, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_TUTORIAL,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = tutorial_name_to_id_map[tutorial_name];
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);
}

void UpdateToolbarMenu(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_update_toolbar_menu = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_UPDATE_TOOLBAR_MENU,
		(PVOID*)&gml_script_update_toolbar_menu
	);

	RValue result;
	gml_script_update_toolbar_menu->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

RValue DeserializeLiveItem(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_deserialize_live_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DESERIALIZE_LIVE_ITEM,
		(PVOID*)&gml_script_deserialize_live_item
	);

	std::map<std::string, RValue> rvalue_map = {
		{ "cosmetic", RValue() }, // UNDEFINED
		{ "item_id", RValue("sword_scrap_metal")}, // STRING
		{ "infusion", RValue() }, // UNDEFINED
		{ "animal_cosmetic", RValue() }, // UNDEFINED
		{ "date_photo", RValue() }, // UNDEFINED
		{ "inner_item", RValue() }, // UNDEFINED
		{ "gold_to_gain", RValue() }, // UNDEFINED
		{ "auto_use", false }, // BOOL
		{ "pet_cosmetic_set_name", RValue() } // UNDEFINED
	};

	RValue result;
	RValue input = rvalue_map;
	RValue* input_ptr = &input;
	gml_script_deserialize_live_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void DropItem(int item_id, double x_coord, double y_coord, CInstance* Self, CInstance* Other)
{
	RValue item = DeserializeLiveItem(Self, Other);
	*item.GetRefMember("prototype") = item_id_to_prototype_map[item_id];
	*item.GetRefMember("item_id") = item_id;

	CScript* gml_script_drop_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DROP_ITEM,
		(PVOID*)&gml_script_drop_item
	);

	RValue x = x_coord;
	RValue y = y_coord;
	RValue undefined;

	RValue* item_ptr = &item;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* undefined_ptr = &undefined;

	RValue result;
	RValue* arguments[4] = { item_ptr, x_ptr, y_ptr, undefined_ptr };

	gml_script_drop_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		4,
		arguments
	);
}

void DropLiftKey()
{
	if(floor_number >= 95)
		DropItem(item_name_to_id_map[RUINS_KEY_F95_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 90)
		DropItem(item_name_to_id_map[RUINS_KEY_F90_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 85)
		DropItem(item_name_to_id_map[RUINS_KEY_F85_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 80)
		DropItem(item_name_to_id_map[RUINS_KEY_F80_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 75)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 70)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 65)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 60)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 55)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 50)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 45)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 40)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 35)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 30)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 25)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 20)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 15)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F15_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 10)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F10_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 5)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F5_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
}

void EnterDungeon(double dungeon_level, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_enter_dungeon = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ENTER_DUNGEON,
		(PVOID*)&gml_script_enter_dungeon
	);

	RValue result;
	RValue level = dungeon_level;
	RValue undefined;
	RValue* level_ptr = &level;
	RValue* undefined_ptr = &undefined;
	RValue* arguments[3] = { level_ptr, undefined_ptr, undefined_ptr };

	gml_script_enter_dungeon->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		arguments
	);
}

RValue InventoryCountItem(int item_id, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_inventory_count_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INVENTORY_COUNT_ITEM,
		(PVOID*)&gml_script_inventory_count_item
	);

	RValue result;
	RValue item = item_id;
	RValue* item_ptr = &item;

	gml_script_inventory_count_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &item_ptr }
	);

	return result;
}

void InventoryRemoveItem(int item_id, int quantity, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_inventory_remove_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INVENTORY_REMOVE_ITEM,
		(PVOID*)&gml_script_inventory_remove_item
	);

	RValue result;
	RValue item = item_id;
	RValue amount = quantity;
	RValue* item_ptr = &item;
	RValue* amount_ptr = &amount;
	RValue* arguments[2] = { item_ptr, amount_ptr };

	gml_script_inventory_remove_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		2,
		arguments
	);
}

bool AriCurrentGmRoomIsDungeonFloor()
{
	if (boss_battle != BossBattle::NONE)
		return true;
	return ari_current_gm_room.contains("rm_mines") && ari_current_gm_room != "rm_mines_entry" && !ari_current_gm_room.contains("seal");
}

void SetFloorNumber()
{
	// Update the floor number.
	if (ari_current_gm_room == "rm_mines_upper_floor1")
		floor_number = 1;
	else if (ari_current_gm_room == "rm_mines_upper_elevator5")
		floor_number = 5;
	else if (ari_current_gm_room == "rm_mines_upper_elevator10")
		floor_number = 10;
	else if (ari_current_gm_room == "rm_mines_upper_elevator15")
		floor_number = 15;
	else if (ari_current_gm_room == "rm_water_seal" || ari_current_gm_room == "rm_mines_tide_ritual_chamber")
		floor_number = 20;
	else if (ari_current_gm_room == "rm_mines_tide_floor21")
		floor_number = 21;
	else if (ari_current_gm_room == "rm_mines_tide_elevator25")
		floor_number = 25;
	else if (ari_current_gm_room == "rm_mines_tide_elevator30")
		floor_number = 30;
	else if (ari_current_gm_room == "rm_mines_tide_elevator35")
		floor_number = 35;
	else if (ari_current_gm_room == "rm_earth_seal" || ari_current_gm_room == "rm_mines_deep_ritual_chamber")
		floor_number = 40;
	else if (ari_current_gm_room == "rm_mines_deep_41")
		floor_number = 41;
	else if (ari_current_gm_room == "rm_mines_deep_45")
		floor_number = 45;
	else if (ari_current_gm_room == "rm_mines_deep_50")
		floor_number = 50;
	else if (ari_current_gm_room == "rm_mines_deep_55")
		floor_number = 55;
	else if (ari_current_gm_room == "rm_fire_seal" || ari_current_gm_room == "rm_mines_lava_ritual_chamber")
		floor_number = 60;
	else if (ari_current_gm_room == "rm_mines_lava_61")
		floor_number = 61;
	else if (ari_current_gm_room == "rm_mines_lava_65")
		floor_number = 65;
	else if (ari_current_gm_room == "rm_mines_lava_70")
		floor_number = 70;
	else if (ari_current_gm_room == "rm_mines_lava_75")
		floor_number = 75;
	else if (ari_current_gm_room == "rm_ruins_seal" || ari_current_gm_room == "rm_mines_ruins_ritual_chamber")
		floor_number = 80;
	else if (ari_current_gm_room == "rm_mines_ruins_85")
		floor_number = 85;
	else if (ari_current_gm_room == "rm_priestess_quarters")
		floor_number = 90;
	else if (ari_current_gm_room == "rm_mines_ruins_95")
		floor_number = 95;
	else
		floor_number++;
}

RValue GetDynamicItemSprite(int item_id)
{
	if (item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
	{
		if (active_traps.contains(Traps::INHIBITING) || !AriCurrentGmRoomIsDungeonFloor())
		{
			if (floor_number < 20)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword_disabled" });
			else if (floor_number < 40)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword_disabled" });
			else if (floor_number < 60)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword_disabled" });
		}
		else
		{
			if (floor_number < 20)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword" });
			else if (floor_number < 40)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword" });
			else if (floor_number < 60)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword" });
		}
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_pickaxe" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_pickaxe" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_pickaxe" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_pickaxe" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_4" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::ALTERATION])
	{
		if (active_sigils.contains(Sigils::ALTERATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_alteration_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_alteration" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::CONCEALMENT])
	{
		if (active_sigils.contains(Sigils::CONCEALMENT) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_concealment_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_concealment" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::FORTIFICATION])
	{
		if (active_sigils.contains(Sigils::FORTIFICATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortification_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortification" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::FORTUNE])
	{
		if (active_sigils.contains(Sigils::FORTUNE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortune_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortune" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::PROTECTION])
	{
		if (active_sigils.contains(Sigils::PROTECTION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || GetInvulnerabilityHits() > 0 || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_protection_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_protection" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::RAGE])
	{
		if (active_sigils.contains(Sigils::RAGE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_rage_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_rage" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::REDEMPTION])
	{
		if (active_sigils.contains(Sigils::REDEMPTION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || FairyBuffIsActive() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_redemption_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_redemption" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SAFETY])
	{
		if (active_sigils.contains(Sigils::SAFETY) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_safety_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_safety" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SERENITY])
	{
		if (active_sigils.contains(Sigils::SERENITY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_serenity_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_serenity" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SILENCE])
	{
		if (active_sigils.contains(Sigils::SILENCE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_silence_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_silence" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::STRENGTH])
	{
		if (active_sigils.contains(Sigils::STRENGTH) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_strength_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_strength" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::TEMPTATION])
	{
		if (active_sigils.contains(Sigils::TEMPTATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0 || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_temptation_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_temptation" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SIGHT])
	{
		if (active_sigils.contains(Sigils::SIGHT) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_sight_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_sight" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::INTUITION])
	{
		if (active_sigils.contains(Sigils::INTUITION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_intuition_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_intuition" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_benediction_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_benediction" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_astral_flow_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_astral_flow" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_chain_spell_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_chain_spell" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_spirit_surge_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_spirit_surge" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_meikyo_shisui_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_meikyo_shisui" });
	}
	else if (item_id == salve_name_to_id_map[SUSTAINING_POTION_NAME])
	{
		if (active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_potion_sustain_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_potion_sustain" });
	}
	else if (item_id == salve_name_to_id_map[HEALTH_SALVE_NAME])
	{
		if ((configuration.limit_salves && salves_used.contains(item_id)) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_health_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_health" });
	}
	else if (item_id == salve_name_to_id_map[STAMINA_SALVE_NAME])
	{
		if ((configuration.limit_salves && salves_used.contains(item_id)) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_stamina_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_stamina" });
	}
	else if (item_id == salve_name_to_id_map[MANA_SALVE_NAME])
	{
		if ((configuration.limit_salves && salves_used.contains(item_id)) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_mana_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_mana" });
	}
}

RValue GetDynamicUiSprite(std::string sprite_name)
{
	// OnDungeonRoomStart Conversation Sprite Overrides
	if (!journal_menu_open) // floor_start_time == current_time_in_seconds
	{
		// Priestess Portrait Replacement
		if (sprite_name.contains("spr_portrait_seridia") && !sprite_name.contains("flashback"))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_portrait_seridia_flashback_priestess_closed_eyes" });
		// Heart Insert Icon Conversation Replacement
		else if (sprite_name == "spr_ui_dialogue_namebar_heartinsert")
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_dialogue_namebar_circleinsert" });
		// Heart & Circle Icon Conversation Replacement
		else if (sprite_name.contains("spr_ui_dialogue_heart_") || sprite_name.contains("spr_ui_dialogue_circle_"))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_dialogue_circle_purple" });
	}

	// Full Restore (Spell Icon)
	if (sprite_name == "spr_ui_journal_magic_restore_spell_icon_main")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_spell_icon_main" });
		}
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_spell_icon_main" });
			}
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_spell_icon_main" });
			}
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_spell_icon_main" });
			}
		}
		// Predict (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_spell_icon_main" });
		}
		// Full Restore Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_restore_spell_icon_disabled" });
	}
	// Summon Rain (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_rain_spell_icon_main")
	{
		// Flood (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_spell_icon_main" });
		}
		// Summon Rain Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_rain_spell_icon_disabled" });
	}
	// Growth (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_growth_spell_icon_main")
	{
		// Quake (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_spell_icon_main" });
		}
		// Condemn (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0 || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_spell_icon_main" });
		}
		// Growth Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_growth_spell_icon_disabled" });
	}
	// Fire Breath (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_fire_spell_icon_main")
	{
		// Fire Breath Disabled
		if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_fire_spell_icon_disabled" });
	}
	// Dungeon Backplate
	else if (sprite_name == "spr_ui_dungeon_backplate")
	{
		// Floor Enchantments & Offerings
		if (!active_floor_enchantments.empty() || !active_offerings.empty())
		{
			std::string sprite_name = "backplate";
			std::string group_one_enchantment_str = "";
			std::string group_two_enchantment_str = "";
			std::string group_three_enchantment_str = "";
			std::string offering_str = "";

			for (FloorEnchantments floor_enchantment : active_floor_enchantments)
			{
				auto group_one_enchantment = std::find(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.begin(), GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_one_enchantment != GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.end())
					group_one_enchantment_str += magic_enum::enum_name(floor_enchantment);

				auto group_two_enchantment = std::find(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.begin(), GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_two_enchantment != GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.end())
					group_two_enchantment_str += magic_enum::enum_name(floor_enchantment);

				auto group_three_enchantment = std::find(GROUP_THREE_FLOOR_ENCHANTMENTS.begin(), GROUP_THREE_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_three_enchantment != GROUP_THREE_FLOOR_ENCHANTMENTS.end())
					group_three_enchantment_str += magic_enum::enum_name(floor_enchantment);
			}

			for (Offerings offering : active_offerings)
				offering_str += magic_enum::enum_name(offering);

			if (!group_one_enchantment_str.empty())
				sprite_name += "_" + group_one_enchantment_str;
			if (!group_two_enchantment_str.empty())
				sprite_name += "_" + group_two_enchantment_str;
			if (!group_three_enchantment_str.empty())
				sprite_name += "_" + group_three_enchantment_str;
			if (!offering_str.empty())
				sprite_name += "_" + offering_str;

			std::transform(sprite_name.begin(), sprite_name.end(), sprite_name.begin(), [](unsigned char c) { return std::tolower(c); });
			return g_ModuleInterface->CallBuiltin("asset_get_index", { RValue(sprite_name) });
		}
		// Empty
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { RValue("backplate_empty") });
	}
	// Full Restore (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_restore_card_icon")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_card_icon" });
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_card_icon" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_card_icon" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_card_icon" });
		}
		// Predict (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_card_icon" });
	}
	// Summon Rain (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_rain_card_icon")
	{
		// Flood (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_card_icon" });
	}
	// Growth (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_growth_card_icon")
	{
		// Quake (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_card_icon" });
		// Condemn (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_card_icon" });
	}
	// Full Restore (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_restore")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_siphon_life" });
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enfire" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enblizzard" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enpoison" });
		}
		// Predict (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_predict" });
	}
	// Summon Rain (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_rain")
	{
		// Flood (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_flood" });
	}
	// Growth (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_growth")
	{
		// Quake (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_quake" });
		// Condemn (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_condemn" });
	}
	// Spell Card Backplate
	else if (sprite_name == "spr_ui_journal_magic_card_backplate")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_sipon_life_card_backplate" });
		// Flood (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_flood_card_backplate" });
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enfire_card_backplate" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enblizzard_card_backplate" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enpoison_card_backplate" });
		}
		// Quake (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_quake_card_backplate" });
	}
	return RValue();
}

std::unordered_set<FloorEnchantments> RandomFloorEnchantments(bool is_first_floor, DungeonBiomes dungeon_biome)
{
	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

	std::unordered_set<FloorEnchantments> random_floor_enchantments = {};

	if (is_first_floor)
	{
		const std::vector<FloorEnchantments> FIRST_FLOOR_POSSIBLE_ENCHANTMENTS = {
			FloorEnchantments::RESTORATION, FloorEnchantments::SECOND_WIND, FloorEnchantments::HASTE
		};

		std::uniform_int_distribution<size_t> first_floor_distribution(0, FIRST_FLOOR_POSSIBLE_ENCHANTMENTS.size() - 1);
		return { FIRST_FLOOR_POSSIBLE_ENCHANTMENTS[first_floor_distribution(random_generator)] };
	}

	if (dungeon_biome == DungeonBiomes::UPPER)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 70% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 70)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 45% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 45)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 20% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 20)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 50) // 50% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 50% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 50% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 50)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 25% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 25)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::TIDE_CAVERNS)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 85% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 60% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 60)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 45% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 45)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 60) // 60% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 40% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 40% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 40)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 25% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 25)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 60) // 60% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 40% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::DEEP_EARTH)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 85% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 50% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 50)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 70) // 70% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 30% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 45% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 45)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 65% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 65)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 30% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 30)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 70) // 70% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 30% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::LAVA_CAVES)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 80% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 80)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 95% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 95)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 55% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 55)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 80) // 80% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 20% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 60% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 60)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 75% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 75)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 35% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 35)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 80) // 80% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 20% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::RUINS)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 85% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 95% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 95)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 60% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 60)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 90) // 90% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 10% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 75% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 75)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 40% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 40)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 90) // 90% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 10% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	return random_floor_enchantments;
}

void GenerateFloorTraps()
{
	if (TRAP_SPAWN_POINTS.contains(ari_current_gm_room))
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
		std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

		if (spawn_points.empty())
			return;

		int min_traps = 2;
		int biome_adjusted_max_traps = (floor_number / 20) + 2;

		std::uniform_int_distribution<size_t> traps_for_room_distribution(min_traps, biome_adjusted_max_traps);
		int random_trap_count = traps_for_room_distribution(random_generator);

		// Disarm Trap (Rogue Set Bonus)
		if (CountEquippedClassArmor()[Classes::ROGUE] >= 4)
			random_trap_count -= 2;

		// Peril
		if (active_offerings.contains(Offerings::PERIL))
			random_trap_count += 2;

		int count = 0;
		for (int i = 0; i < random_trap_count; i++)
		{
			std::uniform_int_distribution<size_t> random_number_of_traps_for_current_room(0, spawn_points.size() - 1);
			int random_index = random_number_of_traps_for_current_room(random_generator);
			floor_trap_positions.insert(spawn_points[random_index]);
			spawn_points.erase(spawn_points.begin() + random_index);
		}
	}
}

void GenerateTreasureSpot(CInstance* Self, CInstance* Other)
{
	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> zero_to_seven_distribution(0, 7);

	int biome_adjusted_max_traps_with_peril = (floor_number / 20) + 4;
	int floors_to_descend = zero_to_seven_distribution(random_generator);

	if (floor_trap_positions.size() == biome_adjusted_max_traps_with_peril)
		floors_to_descend++;

	treasure_spot.floors_to_descend = floors_to_descend;
	treasure_spot.state = TreasureSpot::WAITING_TO_SPAWN;

	if (treasure_spot.floors_to_descend > 0)
		CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
}

std::vector<int> GenerateRandomMonstersIdsForCurrentFloor(int monsters_to_spawn, const int monster_id_to_exclude = -1)
{
	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::vector<int> candidate_monsters(dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].begin(), dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].end());

	if (monster_id_to_exclude != -1)
		candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_id_to_exclude), candidate_monsters.end());
	
	// Don't spawn griffin statues for luring traps.
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["griffin_statue"]), candidate_monsters.end());

	std::vector<int> random_monsters = {};
	for (int i = 0; i < monsters_to_spawn; i++)
	{
		std::uniform_int_distribution<size_t> random_monster_distribution(0, candidate_monsters.size() - 1);
		random_monsters.push_back(candidate_monsters[random_monster_distribution(random_generator)]);
	}

	return random_monsters;
}

void SpawnDreadBeast(CInstance* Self, CInstance* Other)
{
	if (TRAP_SPAWN_POINTS.contains(ari_current_gm_room))
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
		std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

		if (spawn_points.empty())
			return;

		std::uniform_int_distribution<size_t> trap_spawn_points_distribution(0, spawn_points.size() - 1);
		int random_index = trap_spawn_points_distribution(random_generator);
		std::pair<int, int> spawn_point = spawn_points[random_index];

		// TODO: Update this as dread beast logic is implemented
		std::vector<std::string> possible_dread_beast_monsters = {};
		if (floor_number < 20)
			possible_dread_beast_monsters = { "rockclod", "sapling", "sapling_cool", "mushroom", "enchantern" };
		else if (floor_number < 40)
			possible_dread_beast_monsters = { "rockclod_blue", "sapling_blue", "mushroom_green", "enchantern_blue", "stalagmite", "bat" };
		else if (floor_number < 60)
			possible_dread_beast_monsters = { "rockclod_green", "sapling_purple", "mushroom_blue", "stalagmite_green", "bat_blue" };
		else if (floor_number < 80)
			possible_dread_beast_monsters = { "rockclod_red", "sapling_orange", "mushroom_purple", "stalagmite_purple", "spirit", "cat" };
		else
			possible_dread_beast_monsters = { "sapling_pink", "spirit_purple", "cat_void", "rock_stack", "tome" }; // TODO: "rockclod_purple" if/when implemented

		std::uniform_int_distribution<size_t> random_dread_beast_distribution(0, possible_dread_beast_monsters.size() - 1);
		random_index = random_dread_beast_distribution(random_generator);
		int monster_id = monster_name_to_id_map[possible_dread_beast_monsters[random_index]];

		dread_beast_monster_id = monster_id;
		SpawnMonster(Self, Other, spawn_point.first, spawn_point.second, monster_id);
	}
}

void SelectDreadBeast(CInstance* Self, CInstance* Other)
{
	// TODO: Update this as Dread Beasts are implemented by removing the prune statements below.
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["barrel"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["copperclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["goldclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["ironclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["mimic"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["mistrilclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["rock_stack_lava"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["sapling_orange_mini"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["silverclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["rockclod_purple"]), initial_floor_monsters.end()); // TODO: don't remove if/when implemented (MONSTER NOT IMPLEMENTED)

	if (initial_floor_monsters.size() > 0)
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
		std::uniform_int_distribution<int> initial_floor_monster_distribution(0, initial_floor_monsters.size() - 1);

		dread_beast_monster_id = initial_floor_monsters[initial_floor_monster_distribution(random_generator)];
	}
	else
		SpawnDreadBeast(Self, Other);
}

RValue GetUnifiedTime(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_unified_time = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_UNIFIED_TIME,
		(PVOID*)&gml_script_get_unified_time
	);

	RValue result;
	gml_script_get_unified_time->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void CancelStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id)
{
	CScript* gml_script_cancel_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CANCEL_STATUS_EFFECT,
		(PVOID*)&gml_script_cancel_status_effect
	);

	RValue result;
	RValue* status_effect_id_ptr = &status_effect_id;

	gml_script_cancel_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &status_effect_id_ptr }
	);
}

void CancelAllStatusEffects()
{
	std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

	// Remove all status effects
	for (int i = 0; i <= status_effect_name_to_id_map.size(); i++)
		CancelStatusEffect(refs[0], refs[1], i);
}

void RegisterStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id, RValue amount, RValue start, RValue finish)
{
	CScript* gml_script_register_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_REGISTER_STATUS_EFFECT,
		(PVOID*)&gml_script_register_status_effect
	);

	RValue result;
	RValue* status_effect_id_ptr = &status_effect_id;
	RValue* amount_ptr = &amount;
	RValue* start_ptr = &start;
	RValue* finish_ptr = &finish;
	RValue* argument_array[4] = { status_effect_id_ptr, amount_ptr, start_ptr, finish_ptr };

	gml_script_register_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		4,
		argument_array
	);
}

RValue GetHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_HEALTH,
		(PVOID*)&gml_script_get_health
	);

	RValue result;
	gml_script_get_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void SetHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_set_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SET_HEALTH,
		(PVOID*)&gml_script_set_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_set_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

void ModifyHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_HEALTH,
		(PVOID*)&gml_script_modify_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_modify_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

RValue GetMaxHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MAX_HEALTH,
		(PVOID*)&gml_script_get_max_health
	);

	RValue result;
	gml_script_get_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void SetMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("base_health") = value;

	RValue current_health = GetHealth(Self, Other);
	if (current_health.ToInt64() > value)
		SetHealth(Self, Other, value);
}

int ModifyMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	int max_health = __ari.GetMember("base_health").ToInt64() + value;
	*__ari.GetRefMember("base_health") = max_health;

	return max_health;
}

void VitalsMenuSetHealth(CInstance* Self, CInstance* Other, int current_health_value, int max_health_value)
{
	CScript* gml_script_vitals_menu_set_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_health
	);

	RValue result;
	RValue current_health = current_health_value;
	RValue max_health = max_health_value;
	RValue* current_health_ptr = &current_health;
	RValue* max_health_ptr = &max_health;
	RValue arg2 = false;
	RValue* arg2_ptr = &arg2;
	RValue* argument_array[3] = { current_health_ptr, max_health_ptr, arg2_ptr };

	gml_script_vitals_menu_set_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		argument_array
	);
}

void VitalsMenuSetMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_vitals_menu_set_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_max_health
	);

	RValue result;
	RValue max_health = value;
	RValue* max_health_ptr = &max_health;

	gml_script_vitals_menu_set_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &max_health_ptr }
	);
}

RValue GetStamina(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_STAMINA,
		(PVOID*)&gml_script_get_stamina
	);

	RValue result;
	gml_script_get_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void ModifyStamina(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	RValue result;
	RValue stamina_modifier = value;
	RValue* stamina_modifier_ptr = &stamina_modifier;

	gml_script_modify_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &stamina_modifier_ptr }
	);
}

RValue GetMana(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MANA,
		(PVOID*)&gml_script_get_mana
	);

	RValue result;
	gml_script_get_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void ModifyMana(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_MANA,
		(PVOID*)&gml_script_modify_mana
	);

	RValue result;
	RValue mana_modifier = value;
	RValue* mana_modifier_ptr = &mana_modifier;

	gml_script_modify_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &mana_modifier_ptr }
	);
}

void CastSpell(CInstance* Self, CInstance* Other, int spell_id)
{
	CScript* gml_script_cast_spell = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CAST_SPELL,
		(PVOID*)&gml_script_cast_spell
	);

	RValue result;
	RValue health_modifier = spell_id;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_cast_spell->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

void RevealFloorTraps()
{
	if (active_sigils.contains(Sigils::SIGHT) && revealed_floor_traps.empty())
	{
		for (auto floor_trap = floor_trap_positions.begin(); floor_trap != floor_trap_positions.end(); floor_trap++) {
			RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
			if (instance_layer_exists)
			{
				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

				RevealedFloorTrap revealed_floor_trap = RevealedFloorTrap(floor_trap->first, floor_trap->second, true, instance);
				revealed_floor_traps.push_back(revealed_floor_trap);
			}
		}
	}

	for (RevealedFloorTrap revealed_floor_trap : revealed_floor_traps)
	{
		if (revealed_floor_trap.is_active && !floor_trap_positions.contains({ revealed_floor_trap.x, revealed_floor_trap.y }))
		{
			revealed_floor_trap.is_active = false;

			RValue revealed_floor_trap_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { revealed_floor_trap.instance });
			if (revealed_floor_trap_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { revealed_floor_trap.instance });
		}
	}
}

void ApplyFloorTraps(CInstance* Self, CInstance* Other)
{
	// Prune traps that have fully applied.
	if (active_traps.contains(Traps::CONFUSING) && active_traps_to_value_map[Traps::CONFUSING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect ended at: %d", MOD_NAME, VERSION, current_time_in_seconds);
		active_traps.erase(Traps::CONFUSING);
		active_traps_to_value_map.erase(Traps::CONFUSING);
	}
	if (active_traps.contains(Traps::DISORIENTING) && active_traps_to_value_map[Traps::DISORIENTING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect ended at: %d", MOD_NAME, VERSION, current_time_in_seconds);
		active_traps.erase(Traps::DISORIENTING);
		active_traps_to_value_map.erase(Traps::DISORIENTING);
	}
	if (active_traps.contains(Traps::INHIBITING) && active_traps_to_value_map[Traps::INHIBITING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect ended at: %d", MOD_NAME, VERSION, current_time_in_seconds);
		active_traps.erase(Traps::INHIBITING);
		active_traps_to_value_map.erase(Traps::INHIBITING);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	// Process traps in range.
	for (auto floor_trap = floor_trap_positions.begin(); floor_trap != floor_trap_positions.end();) {
		double distance = GetDistance(ari_x, ari_y, floor_trap->first, floor_trap->second);
		if (distance <= 16)
		{
			static thread_local std::mt19937 random_generator(std::random_device{}());
			std::uniform_int_distribution<size_t> random_trap_distribution(0, magic_enum::enum_count<Traps>() - 1);
			std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

			Traps trap = magic_enum::enum_value<Traps>(random_trap_distribution(random_generator));
			trap = Traps::_VOID;

			// Hallowed Ground (Paladin Set Bonus)
			bool malfunction = zero_to_ninety_nine_distribution(random_generator) < 50 ? true : false;
			if (CountEquippedClassArmor()[Classes::PALADIN] == 5 && malfunction)
			{
				PlaySoundEffect("snd_bark_heart603", 100, 1);
				CreateNotification(true, MALFUNCTION_TRAP_NOTIFICATION_KEY, Self, Other);
			}
			else
			{
				g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Trap Triggered: %s", MOD_NAME, VERSION, magic_enum::enum_name(trap).data());
				active_traps.insert({ trap, { floor_trap->first, floor_trap->second } });

				if (trap == Traps::CONFUSING)
				{
					PlaySoundEffect("snd_bark_o_o", 100, 1);
					CreateNotification(true, CONFUSING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["annoyed"], 0);

					if (!active_traps_to_value_map.contains(Traps::CONFUSING))
					{
						active_traps_to_value_map[Traps::CONFUSING] = current_time_in_seconds + configuration.confusing_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect started at: %d", MOD_NAME, VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::CONFUSING] += configuration.confusing_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect extended by: %d", MOD_NAME, VERSION, configuration.confusing_trap_duration_seconds);
					}
				}
				else if (trap == Traps::DISORIENTING)
				{
					PlaySoundEffect("snd_interactable_scan", 100, 1);
					CreateNotification(true, DISORIENTING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["annoyed"], 0);

					if (!active_traps_to_value_map.contains(Traps::DISORIENTING))
					{
						active_traps_to_value_map[Traps::DISORIENTING] = current_time_in_seconds + configuration.disorienting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect started at: %d", MOD_NAME, VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::DISORIENTING] += configuration.disorienting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect extended by: %d", MOD_NAME, VERSION, configuration.disorienting_trap_duration_seconds);
					}
				}
				else if (trap == Traps::EXPLODING)
				{
					PlaySoundEffect("snd_Explosion_CaveReverb", 100, 0.30);
					CreateNotification(true, EXPLODING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["angry"], 0);
				}
				else if (trap == Traps::INHIBITING)
				{
					PlaySoundEffect("snd_bark_surprised", 100, 1);
					CreateNotification(true, INHIBITING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["no_coin"], 0);

					if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
						UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

					if (!active_traps_to_value_map.contains(Traps::INHIBITING))
					{
						active_traps_to_value_map[Traps::INHIBITING] = current_time_in_seconds + configuration.inhibiting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect started at: %d", MOD_NAME, VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::INHIBITING] += configuration.inhibiting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect extended by: %d", MOD_NAME, VERSION, configuration.inhibiting_trap_duration_seconds);
					}
				}
				else if (trap == Traps::LURING)
				{
					std::uniform_int_distribution<int> random_position_offset_distribution(-12, 12);

					PlaySoundEffect("snd_ScrollRaise", 100, 1);
					CreateNotification(true, LURING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

					// TODO: Restrict monster spawns as necessary (stalagmite_pink? TBD)
					std::vector<int> random_monsters;
					if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(configuration.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite"]);
					else if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite_green"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(configuration.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite_green"]);
					else if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite_purple"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(configuration.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite_purple"]);
					else
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(configuration.luring_trap_monster_spawn_count);

					for (int i = 0; i < random_monsters.size(); i++)
						SpawnMonster(Self, Other, floor_trap->first + random_position_offset_distribution(random_generator), floor_trap->second + random_position_offset_distribution(random_generator), random_monsters[i]);

					active_traps.erase(Traps::LURING);
				}
				else if (trap == Traps::METEOR)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_VoidPortalSpawn", 100, 1);
						CreateNotification(true, METEOR_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });
						
						CustomAOE meteor = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 600, current_time_in_seconds, true, instance, CustomAOETypes::METEOR);
						meteor_aoes.push_back(meteor);
					}

					active_traps.erase(Traps::METEOR);
				}
				else if (trap == Traps::GAZE)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_VoidMassAppear", 100, 0.7);
						CreateNotification(true, GAZE_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });
						
						CustomAOE gaze = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 600, current_time_in_seconds, true, instance, CustomAOETypes::GAZE);
						gaze_aoes.push_back(gaze);
					}

					active_traps.erase(Traps::GAZE);
				}
				else if (trap == Traps::_VOID)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_MagicVoidLightSpell", 100, 0.3);
						CreateNotification(true, VOID_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

						CustomAOE void_aoe = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 1800, current_time_in_seconds, true, instance, CustomAOETypes::_VOID); // TODO: Make duration configurable
						void_aoes.push_back(void_aoe);
					}

					active_traps.erase(Traps::_VOID);
				}
			}

			floor_trap = floor_trap_positions.erase(floor_trap);
		}
		else
			++floor_trap;
	}
}

void ProcessCustomAOEs()
{
	for (CustomAOE& meteor : meteor_aoes)
	{
		if (meteor.is_active && current_time_in_seconds >= meteor.spawned_time + meteor.duration)
		{
			meteor.is_active = false;
			PlaySoundEffect("snd_EarthquakeImpact", 1, 1);
			PlaySoundEffect("snd_AriLowHealthWarning", 1, 1);

			double distance = GetDistance(ari_x, ari_y, meteor.x, meteor.y);
			double modifier = CalculateMeteorDamage(distance) / 100.0;

			double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
			ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);

			RValue meteor_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { meteor.instance });
			if (meteor_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { meteor.instance });
		}
	}

	for (CustomAOE& gaze : gaze_aoes)
	{
		if (gaze.is_active && current_time_in_seconds >= gaze.spawned_time + gaze.duration)
		{
			gaze.is_active = false;
			PlaySoundEffect("snd_CosmicImpact", 1, 0.30);

			bool facing_trap = FacingTrap(ari_x, ari_y, gaze.x, gaze.y);
			if (facing_trap)
			{
				double modifier = configuration.gaze_trap_max_health_damage_percent / 100.0;
				double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);
				PlaySoundEffect("snd_AriLowHealthWarning", 1, 1);
			}

			RValue gaze_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { gaze.instance });
			if (gaze_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { gaze.instance });
		}
	}

	for (CustomAOE& void_aoe : void_aoes)
	{
		if (void_aoe.is_active && current_time_in_seconds >= void_aoe.last_application + 15) // TODO: Make tick rate configurable
		{
			void_aoe.last_application = current_time_in_seconds;

			double distance = GetDistance(ari_x, ari_y, void_aoe.x, void_aoe.y);
			if (distance > 96 && distance <= 298)
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1);
		}

		if (void_aoe.is_active && current_time_in_seconds >= void_aoe.spawned_time + void_aoe.duration)
		{
			void_aoe.is_active = false;

			RValue void_trap_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { void_aoe.instance });
			if (void_trap_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { void_aoe.instance });
		}
	}
}

void ProcessTreasureSpot(CInstance* Self, CInstance* Other)
{
	if (treasure_spot.state == TreasureSpot::WAITING_TO_SPAWN && treasure_spot.floors_to_descend == 0)
	{
		int biome_adjusted_max_traps_with_peril = (floor_number / 20) + 4;
		if (floor_trap_positions.size() < biome_adjusted_max_traps_with_peril)
		{
			// Find a position for the treasure spot on the current floor.
			std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

			std::pair<int, int> treasure_spot_coordinates;
			for (std::pair<int, int> spawn_point : spawn_points)
			{
				if (floor_trap_positions.contains(spawn_point))
					continue;

				treasure_spot_coordinates = spawn_point;
				break;
			}

			RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
			if (instance_layer_exists)
			{
				PlaySoundEffect("snd_TreasureChestSpawn", 100, 0.7);
				CreateNotification(true, TREASURE_SPOT_SPAWNED_NOTIFICATION_KEY, Self, Other);

				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { treasure_spot_coordinates.first, treasure_spot_coordinates.second, RValue("Instances"), obj_assetobject });

				treasure_spot.x = treasure_spot_coordinates.first;
				treasure_spot.y = treasure_spot_coordinates.second;
				treasure_spot.is_active = true;
				treasure_spot.instance = instance;
				treasure_spot.state = TreasureSpot::SPAWNED;
			}
			else
			{
				treasure_spot.floors_to_descend++;
				CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
			}
		}
		else
		{
			treasure_spot.floors_to_descend++;
			CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
		}
		
	}

	if (treasure_spot.state == TreasureSpot::SPAWNED)
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
		std::uniform_int_distribution<size_t> random_greater_sigil_distribution(0, magic_enum::enum_count<GreaterSigils>() - 1);

		double distance = GetDistance(ari_x, ari_y, treasure_spot.x, treasure_spot.y);
		if (distance <= 8)
		{
			treasure_spot.state = TreasureSpot::FOUND;
			CreateNotification(true, TREASURE_SPOT_FOUND_NOTIFICATION_KEY, Self, Other);

			//GreaterSigils random_greater_sigil = magic_enum::enum_value<GreaterSigils>(random_greater_sigil_distribution(random_generator));
			//DropItem(greater_sigil_to_item_id_map[random_greater_sigil], ari_x, ari_y, Self, Other);
			
			if (floor_number < 20)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE], ari_x, ari_y, Self, Other);
			else if (floor_number < 40)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI], ari_x, ari_y, Self, Other);
			else if (floor_number < 60)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION], ari_x, ari_y, Self, Other);
			else if (floor_number < 80)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL], ari_x, ari_y, Self, Other);
			else if (floor_number < 100)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW], ari_x, ari_y, Self, Other);
			
			if (treasure_spot.is_active)
			{
				treasure_spot.is_active = false;

				RValue treasure_spot_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { treasure_spot.instance });
				if (treasure_spot_instance_exists.ToBoolean())
					g_ModuleInterface->CallBuiltin("instance_destroy", { treasure_spot.instance });
			}
		}
	}

}

void ApplyOfferingPenalties(CInstance* Self, CInstance* Other)
{
	if (ari_resource_to_penalty_map[AriResources::HEALTH])
		ModifyHealth(Self, Other, configuration.offering_health_requirement * -1);
	if (ari_resource_to_penalty_map[AriResources::STAMINA])
		ModifyStamina(Self, Other, configuration.offering_stamina_requirement * -1);
	if (ari_resource_to_penalty_map[AriResources::MANA])
		ModifyMana(Self, Other, configuration.offering_mana_requirement * -1);

	ari_resource_to_penalty_map.clear();
}

void TrackAriResources(CInstance* Self, CInstance* Other)
{
	ari_resource_to_value_map[AriResources::HEALTH] = GetHealth(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::MAX_HEALTH] = GetMaxHealth(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::STAMINA] = GetStamina(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::MANA] = GetMana(Self, Other).ToInt64();
}

void GenerateTreasureChestLoot(std::string object_name, CInstance* Self, CInstance* Other)
{
	static thread_local std::mt19937 random_generator(std::random_device{}());
	std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
	std::uniform_int_distribution<size_t> random_sigil_distribution(0, magic_enum::enum_count<Sigils>() - 1);

	// Sigils
	std::vector<int> sigil_roll_success_thresholds = {};
	if (object_name == TREASURE_CHEST_WOOD_NAME)
		sigil_roll_success_thresholds = { 50, 25, 0, 0 };
	else if (object_name == TREASURE_CHEST_COPPER_NAME)
		sigil_roll_success_thresholds = { 75, 50, 10, 0 };
	else if (object_name == TREASURE_CHEST_SILVER_NAME)
		sigil_roll_success_thresholds = { 100, 50, 25, 0 };
	else if (object_name == TREASURE_CHEST_GOLD_NAME)
		sigil_roll_success_thresholds = { 100, 100, 25, 10 };

	// Treasure Hunter (Rogue Set Bonus)
	if (CountEquippedClassArmor()[Classes::ROGUE] == 5)
		sigil_roll_success_thresholds.push_back(100);

	std::unordered_set<Sigils> sigils_spawned = {};
	for (size_t i = 0; i < sigil_roll_success_thresholds.size(); i++)
	{
		int roll_for_drop = zero_to_ninety_nine_distribution(random_generator);
		if (roll_for_drop < sigil_roll_success_thresholds[i])
		{
			Sigils random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));
			while (sigils_spawned.contains(random_sigil))
				random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));

			sigils_spawned.insert(random_sigil);
			DropItem(sigil_to_item_id_map[random_sigil], ari_x, ari_y, Self, Other);
		}
	}

	// Cursed Armor
	int cursed_armor_roll_success_threshold = 0;
	if (object_name == TREASURE_CHEST_WOOD_NAME)
		cursed_armor_roll_success_threshold = 1 * configuration.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_COPPER_NAME)
		cursed_armor_roll_success_threshold = 2 * configuration.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_SILVER_NAME)
		cursed_armor_roll_success_threshold = 3 * configuration.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_GOLD_NAME)
		cursed_armor_roll_success_threshold = 4 * configuration.cursed_armor_drop_chance_modifier;

	int roll_for_drop = zero_to_ninety_nine_distribution(random_generator);
	if (roll_for_drop < cursed_armor_roll_success_threshold)
	{
		if (floor_number < 20) // Upper Mines
			DropItem(item_name_to_id_map[CURSED_CHESTPIECE_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 40) // Tide Caverns
			DropItem(item_name_to_id_map[CURSED_HELMET_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 60) // Deep Earth
			DropItem(item_name_to_id_map[CURSED_GLOVES_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 80) // Lava Caves
			DropItem(item_name_to_id_map[CURSED_PANTS_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 100) // Ruins
			DropItem(item_name_to_id_map[CURSED_BOOTS_NAME], ari_x, ari_y, Self, Other);
	}
}

void LoadStalagmiteAttackData()
{
	const std::vector<int> smallNs = { 9, 17, 25, 33, 41, 49 };
	const std::vector<int> bigNs = { 57, 65, 73, 81, 89, 97 };

	pb_aoe_points = generate_circle(smallNs);
	donut_aoe_points = generate_circle(bigNs);
	cross_aoe_points = generate_cross(18, -96, 96, 8, false);
	x_aoe_points = generate_cross(18, -96, 96, 8, true);
	checkerboard_room_wide_a_points = generate_checkerboard();
	checkerboard_room_wide_b_points = generate_inverted_checkerboard();
}

void ResetStaticFields(bool returned_to_title_screen)
{
	if (returned_to_title_screen)
	{
		game_is_active = false;
		unlock_recipes = true;
		is_restoration_tracked_interval = false;
		is_second_wind_tracked_interval = false;
		is_fumigate_tracked_interval = false;
		is_deep_wounds_tracked_interval = false;
		ari_x = -1;
		ari_y = -1;
		ari_facing_dir = -1;
		floor_number = 0;
		floor_start_time = 0;
		current_time_in_seconds = -1;
		time_of_last_restoration_tick = -1;
		time_of_last_second_wind_tick = -1;
		time_of_last_fumigate_tick = -1;
		time_of_last_deep_wounds_tick = -1;
		time_of_last_outbreak_tick = -1;
		held_item_id = -1;
		ari_current_location = "";
		ari_current_gm_room = "";
		script_name_to_reference_map.clear();
	}

	crafting_menu_open = false;
	journal_menu_open = false;
	drop_biome_reward = false;
	biome_reward_disabled = false;
	dread_beast_configured = false;
	sigil_item_used = false;
	greater_sigil_item_used = false;
	lift_key_used = false;
	orb_item_used = false;
	inner_fire_cast = false;
	reckoning_applied = false;
	fairy_buff_applied = false;
	stoneskin_applied = false;
	offering_chance_occurred = false;
	obj_dungeon_elevator_focused = false;
	obj_dungeon_ladder_down_focused = false;
	frailty_hit_counter = 0;
	grudge_counter = 0;
	deep_wounds_damage_pool = 0;
	stoneskin_shield_amount = 0;
	spirit_link_combined_health_pool = 0;
	sigil_of_silence_count = 0;
	sigil_of_alteration_count = 0;
	dread_beast_monster_id = -1;
	dread_beasts_configured = 0;
	boss_monsters_configured = 0;
	salves_used.clear();
	active_sigils.clear();
	active_greater_sigils.clear();
	queued_offerings.clear();
	active_offerings.clear();
	active_floor_enchantments.clear();
	active_traps.clear();
	active_traps_to_value_map.clear();
	floor_trap_positions.clear();
	current_floor_monsters.clear();
	ResetCustomDrawFields();
	ModifySpellCosts(true, false); // TODO: Make sure this works. Go into the dungeon, Growth should be reduced to 4, exit to title, load back in, growth should be 8 (since you won't load into the dungeon).
}

void ObjectCallback(
	IN FWCodeEvent& CodeEvent
)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self)
		return;

	if (!self->m_Object)
		return;

	if (strstr(self->m_Object->m_Name, "obj_ari"))
	{
		if (!script_name_to_reference_map.contains("obj_ari"))
			script_name_to_reference_map["obj_ari"] = { global_instance->GetRefMember("__ari")->ToInstance(), self };

		RValue x;
		g_ModuleInterface->GetBuiltin("x", self, NULL_INDEX, x);
		ari_x = x.ToDouble();

		RValue y;
		g_ModuleInterface->GetBuiltin("y", self, NULL_INDEX, y);
		ari_y = y.ToDouble();

		// Progression Mode Biome Cleared Rewards
		if (configuration.disable_dungeon_lift && drop_biome_reward && ari_x != 0 && ari_y != 0 && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
		{
			// Upper Mines
			if (ari_current_gm_room == "rm_water_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_CHESTPIECE_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Tide Caverns
			else if (ari_current_gm_room == "rm_earth_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_HELMET_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Deep Earth
			else if (ari_current_gm_room == "rm_fire_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_GLOVES_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Lava Caves
			else if (ari_current_gm_room == "rm_ruins_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_PANTS_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[RUINS_KEY_F80_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Ruins
			else if (ari_current_gm_room == "rm_seridias_chamber")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_BOOTS_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[RUINS_KEY_F100_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
		}

		// Floor Traps
		if (active_traps.contains(Traps::EXPLODING))
		{
			// Apply damage to Ari
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			int penalty = current_health * configuration.exploding_trap_current_health_damage_percent / 100;
			SetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, current_health - penalty);

			// Apply damage to monsters
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "move"))
				{
					RValue monster_x;
					RValue monster_y;
					g_ModuleInterface->GetBuiltin("x", monster, NULL_INDEX, monster_x);
					g_ModuleInterface->GetBuiltin("y", monster, NULL_INDEX, monster_y);

					double distance = GetDistance(active_traps[Traps::EXPLODING].first, active_traps[Traps::EXPLODING].second, monster_x.ToInt64(), monster_y.ToInt64());
					if (distance <= 32)
					{
						double hit_points = monster->GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							int monster_hp_penalty = std::trunc(hit_points * configuration.exploding_trap_current_health_damage_percent / 100);
							*monster->GetRefMember("hit_points") = max(0, hit_points - monster_hp_penalty);

							if (StructVariableExists(monster, "monster_id"))
							{
								RValue monster_id = monster->GetMember("monster_id");
								if (IsNumeric(monster_id))
									g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - The Exploding Trap damaged the monster: %s", MOD_NAME, VERSION, monster_id_to_name_map[monster_id.ToInt64()].c_str());
							}
						}
					}
				}
			}

			active_traps.erase(Traps::EXPLODING);
		}

		ApplyOfferingPenalties(global_instance->GetRefMember("__ari")->ToInstance(), self);

		// Process used items.
		RValue ari = self->ToRValue();
		if (StructVariableExists(ari, "fsm"))
		{
			RValue fsm = ari.GetMember("fsm");

			if (StructVariableExists(fsm, "state"))
			{
				RValue state = fsm.GetMember("state");
				if (StructVariableExists(state, "state_id"))
				{
					RValue state_id = state.GetMember("state_id");
					if (state_id.ToInt64() == player_state_to_id_map["hold_to_use"])
					{
						if (StructVariableExists(state, "did_action"))
						{
							RValue did_action = state.GetMember("did_action");
							if (did_action.ToBoolean())
							{
								if (sigil_item_used) // Necessary since did_action==true will get called a few times when the item is used.
								{
									sigil_item_used = false;

									if (held_item_id == sigil_to_item_id_map[Sigils::ALTERATION])
										active_sigils.insert(Sigils::ALTERATION);
									if (held_item_id == sigil_to_item_id_map[Sigils::CONCEALMENT])
										active_sigils.insert(Sigils::CONCEALMENT);
									if (held_item_id == sigil_to_item_id_map[Sigils::FORTIFICATION])
										active_sigils.insert(Sigils::FORTIFICATION);
									if (held_item_id == sigil_to_item_id_map[Sigils::FORTUNE])
									{
										active_sigils.insert(Sigils::FORTUNE);
										SpawnLadder(global_instance->GetRefMember("__ari")->ToInstance(), self, ari_x, ari_y);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::PROTECTION])
									{
										std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

										active_sigils.insert(Sigils::PROTECTION);
										RegisterStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["guardians_shield"], RValue(), 1, 2147483647.0);
										SetInvulnerabilityHits(2);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::RAGE])
										active_sigils.insert(Sigils::RAGE);
									if (held_item_id == sigil_to_item_id_map[Sigils::REDEMPTION])
									{
										std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

										active_sigils.insert(Sigils::REDEMPTION);
										RegisterStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"], RValue(), 1, 2147483647.0);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::SAFETY])
									{
										floor_trap_positions.clear();
										active_sigils.insert(Sigils::SAFETY);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::SERENITY])
									{
										// Undo Fey
										if (active_floor_enchantments.contains(FloorEnchantments::FEY))
										{
											std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

											ModifySpellCosts(true, true);
											CancelStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"]);
										}

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

										active_floor_enchantments.clear();
										active_sigils.insert(Sigils::SERENITY);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::SILENCE])
										active_sigils.insert(Sigils::SILENCE);
									if (held_item_id == sigil_to_item_id_map[Sigils::STRENGTH])
										active_sigils.insert(Sigils::STRENGTH);
									if (held_item_id == sigil_to_item_id_map[Sigils::TEMPTATION])
										active_sigils.insert(Sigils::TEMPTATION);
									if (held_item_id == sigil_to_item_id_map[Sigils::SIGHT])
									{
										int unified_time = GetUnifiedTime(script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][0], script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][1]).ToInt64();
										RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["sacred_light"], 0, unified_time, unified_time + 18000);
										active_sigils.insert(Sigils::SIGHT);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::INTUITION])
									{
										GenerateTreasureSpot(global_instance->GetRefMember("__ari")->ToInstance(), self);
										active_sigils.insert(Sigils::INTUITION);
									}
										
									if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
										UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
								}
								else if (greater_sigil_item_used)
								{
									greater_sigil_item_used = false;

									if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION])
									{
										// Undo Fey
										if (active_floor_enchantments.contains(FloorEnchantments::FEY))
										{
											std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

											ModifySpellCosts(true, true);
											CancelStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"]);
										}

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

										active_floor_enchantments.clear();
										active_sigils.insert(Sigils::SERENITY);
										active_greater_sigils.insert(GreaterSigils::BENEDICTION);
										ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, 999);
									}
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW])
									{
										for (CInstance* monster : current_floor_monsters)
										{
											if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points"))
											{
												RValue monster_id = monster->GetMember("monster_id");
												double hit_points = monster->GetMember("hit_points").ToDouble();
												if (IsNumeric(monster_id) && std::isfinite(hit_points) && hit_points > 0)
													*monster->GetRefMember("hit_points") = 0;
											}
										}

										active_greater_sigils.insert(GreaterSigils::ASTRAL_FLOW);
									}
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL])
										active_greater_sigils.insert(GreaterSigils::CHAIN_SPELL);
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE])
									{
										active_greater_sigils.insert(GreaterSigils::SPIRIT_SURGE);
										ModifyStamina(global_instance->GetRefMember("__ari")->ToInstance(), self, 999);
									}
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI])
									{
										active_greater_sigils.insert(GreaterSigils::MEIKYO_SHISUI);
										ScaleMistpoolArmor(true);
										ScaleMistpoolWeapon(true);
										ScaleMistpoolPickaxe(true);
										ScaleClassArmor(true);
									}

									if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
										UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
								}
								else if (held_item_id == salve_name_to_id_map[HEALTH_SALVE_NAME] || held_item_id == salve_name_to_id_map[STAMINA_SALVE_NAME] || held_item_id == salve_name_to_id_map[MANA_SALVE_NAME])
								{
									salves_used.insert(held_item_id);

									if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
										UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
								}
								else if (lift_key_used)
								{
									lift_key_used = false;
									biome_reward_disabled = true;

									if (held_item_id == item_name_to_id_map[UPPER_MINES_KEY_F5_NAME])
										EnterDungeon(4, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[UPPER_MINES_KEY_F10_NAME])
										EnterDungeon(9, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[UPPER_MINES_KEY_F15_NAME])
										EnterDungeon(14, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME])
										EnterDungeon(19, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME])
										EnterDungeon(24, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME])
										EnterDungeon(29, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME])
										EnterDungeon(34, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME])
										EnterDungeon(39, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME])
										EnterDungeon(44, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME])
										EnterDungeon(49, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME])
										EnterDungeon(54, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME])
										EnterDungeon(59, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME])
										EnterDungeon(64, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME])
										EnterDungeon(69, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME])
										EnterDungeon(74, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F80_NAME])
										EnterDungeon(79, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F85_NAME])
										EnterDungeon(84, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F90_NAME])
										EnterDungeon(89, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F95_NAME])
										EnterDungeon(94, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F100_NAME])
										EnterDungeon(99, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
								}
								else if (orb_item_used)
								{
									orb_item_used = false;
									biome_reward_disabled = true;

									// TODO: Other orbs
									if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_ORB])
									{
										boss_battle = BossBattle::TIDE_CAVERNS_ORB;
										EnterDungeon(19, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_ORB])
									{
										boss_battle = BossBattle::DEEP_EARTH_ORB;
										EnterDungeon(39, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_ORB])
									{
										boss_battle = BossBattle::LAVA_CAVES_ORB;
										EnterDungeon(59, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
									else if (held_item_id == item_name_to_id_map[RUINS_ORB])
									{
										boss_battle = BossBattle::RUINS_ORB;
										EnterDungeon(79, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
								}
							}
						}
					}
				}
			}
		}

		// Restoration
		if (is_restoration_tracked_interval)
		{
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			if (current_health > 0)
				ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
			is_restoration_tracked_interval = false;
		}

		// Fumigate
		if (is_fumigate_tracked_interval)
		{
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			if (current_health > 0)
				ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, -1);

			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				{
					RValue monster_id = monster->GetMember("monster_id");
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (IsNumeric(monster_id) && std::isfinite(hit_points) && hit_points > 0)
						*monster->GetRefMember("hit_points") = hit_points - 1;
				}
			}

			is_fumigate_tracked_interval = false;
		}

		// Deep Wounds
		if (is_deep_wounds_tracked_interval)
		{
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			if (current_health > 0 && deep_wounds_damage_pool > 0)
			{
				int damage = std::clamp(deep_wounds_damage_pool * 10 / 100, 1, 10);
				damage = min(damage, deep_wounds_damage_pool);

				deep_wounds_damage_pool -= damage;
				ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, -1 * damage);
			}

			is_deep_wounds_tracked_interval = false;
		}

		// Drain (Dark Knight Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN] > 0)
		{
			int max_health = GetMaxHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			int recovery = max_health * GetDarkKnightDrainPotency();

			ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, recovery);
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN] = 0;
		}

		// Soul Eater (Dark Knight Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] > 0)
		{
			ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, -1 * class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER]);
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] = 0;
		}

		// Aspir (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR] > 0)
		{
			ModifyMana(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR] = 0;
		}

		// Mana Font (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT] >= 3)
		{
			ModifyMana(global_instance->GetRefMember("__ari")->ToInstance(), self, 4);
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT] = 0;
		}

		// Flood (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] > 0 && current_time_in_seconds > class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] + 90)
		{
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points > 0)
						*monster->GetRefMember("hit_points") = hit_points - 1;
				}
			}

			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = 0;
			CastSpell(global_instance->GetRefMember("__ari")->ToInstance(), self, spell_name_to_id_map["summon_rain"]);
		}

		// Second Wind
		if (is_second_wind_tracked_interval)
		{
			ModifyStamina(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
			is_second_wind_tracked_interval = false;
		}

		// Fey
		if (active_floor_enchantments.contains(FloorEnchantments::FEY))
		{
			if (!fairy_buff_applied)
			{
				std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];
				RegisterStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"], RValue(), 1, 2147483647.0);
				fairy_buff_applied = true;
			}
			ModifySpellCosts(false, true);
		}

		// Inner Fire
		if (active_offerings.contains(Offerings::INNER_FIRE) && !inner_fire_cast)
		{
			inner_fire_cast = true;
			CastSpell(global_instance->GetRefMember("__ari")->ToInstance(), self, spell_name_to_id_map["fire_breath"]); // TODO: Make sure this works using name_to_id_map
		}

		// Reckoning
		if (active_offerings.contains(Offerings::RECKONING) && !reckoning_applied)
		{
			reckoning_applied = true;
			SetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
		}

		// Stoneskin
		if (active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && !stoneskin_applied)
		{
			stoneskin_applied = true;
			
			// TODO: Tune this
			if (floor_number < 20)
				stoneskin_shield_amount = 20;
			else if (floor_number < 40)
				stoneskin_shield_amount = 40;
			else if (floor_number < 60)
				stoneskin_shield_amount = 60;
			else if (floor_number < 60)
				stoneskin_shield_amount = 80;
			else
				stoneskin_shield_amount = 100;
		}

		// Chain Spell
		if (active_greater_sigils.contains(GreaterSigils::CHAIN_SPELL))
			ModifySpellCosts(false, true);

		TrackAriResources(global_instance->GetRefMember("__ari")->ToInstance(), self);
	}

	if (strstr(self->m_Object->m_Name, "obj_monster"))
	{
		RValue monster = self->ToRValue();
		if (StructVariableExists(monster, "monster_id"))
		{
			RValue monster_id = *monster.GetRefMember("monster_id");

			bool is_valid_monster_object = false;
			for (const auto& entry : monster_name_to_id_map) {
				if (entry.second == monster_id.ToInt64()) {
					is_valid_monster_object = true;
					break;
				}
			}

			if (is_valid_monster_object)
			{
				// Mimic Loot
				if (monster_id.ToInt64() == monster_name_to_id_map["mimic"] && !StructVariableExists(monster, "__deep_dungeon__mimic_drop_sigil") && StructVariableExists(monster, "fsm"))
				{
					RValue fsm = monster.GetMember("fsm");
					if (StructVariableExists(fsm, "state"))
					{
						RValue state = fsm.GetMember("state");
						if (StructVariableExists(state, "state_id"))
						{
							RValue state_id = state.GetMember("state_id");
							if (state_id.ToInt64() == monster_category_to_state_id_map["mimic"]["gobble"] && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
							{
								static thread_local std::mt19937 random_generator(std::random_device{}());
								std::uniform_int_distribution<size_t> random_sigil_distribution(0, magic_enum::enum_count<Sigils>() - 1);

								Sigils random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));
								DropItem(sigil_to_item_id_map[random_sigil], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
								StructVariableSet(monster, "__deep_dungeon__mimic_drop_sigil", true);
							}
						}
					}
				}

				// Boss Battles
				if (boss_battle == BossBattle::TIDE_CAVERNS_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 20;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}
				else if (boss_battle == BossBattle::DEEP_EARTH_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 3;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}
				else if (boss_battle == BossBattle::LAVA_CAVES_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 3;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}
				else if (boss_battle == BossBattle::RUINS_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 3;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}

				// Dread Beasts
				if (!dread_beast_configured && monster_id.ToInt64() == dread_beast_monster_id && !StructVariableExists(monster, "__deep_dungeon__dread_beast") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points))
					{
						*monster.GetRefMember("hit_points") = hit_points * 3;
						dread_beasts_configured++;
						if (dread_beast_monster_id != monster_name_to_id_map["rock_stack"] || dread_beasts_configured == 2)
							dread_beast_configured = true;
						
						StructVariableSet(monster, "__deep_dungeon__dread_beast", true);
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Configured Dread Beast: %s", MOD_NAME, VERSION, monster_id_to_name_map[monster_id.ToInt64()].c_str());
					}
				}
				else if (StructVariableExists(monster, "__deep_dungeon__dread_beast"))
					ModifyDreadBeastAttackPatterns(false, false, monster);

				// Track the monster
				if (!StructVariableExists(monster, "__deep_dungeon__current_floor_monsters") && StructVariableExists(monster, "hit_points"))
				{
					current_floor_monsters.push_back(self);
					StructVariableSet(monster, "__deep_dungeon__current_floor_monsters", true);
					StructVariableSet(monster, "__deep_dungeon__default_hit_points", monster.GetMember("hit_points").ToDouble());
				}

				// Grudge
				if (active_floor_enchantments.contains(FloorEnchantments::GRUDGE) && !StructVariableExists(monster, "__deep_dungeon__grudge_tracked") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						grudge_counter += 1;
						StructVariableSet(monster, "__deep_dungeon__grudge_tracked", true);
					}
				}

				// Regular loot drops
				if (!ari_current_gm_room.contains("seal") && !StructVariableExists(monster, "__deep_dungeon__loot_drop") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0 && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
					{
						static thread_local std::mt19937 random_generator(std::random_device{}());
						std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
						bool drop_lift_key = zero_to_ninety_nine_distribution(random_generator) < configuration.lift_key_drop_chance ? true : false;

						if (floor_number < 20) // Upper Mines
						{
							DropItem(item_name_to_id_map["beast_coin_tiny"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (configuration.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < configuration.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 40) // Tide Caverns
						{
							DropItem(item_name_to_id_map["beast_coin_small"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (configuration.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < configuration.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 60) // Deep Earth
						{
							DropItem(item_name_to_id_map["beast_coin_medium"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (configuration.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < configuration.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 80) // Lava Caves
						{
							DropItem(item_name_to_id_map["beast_coin_large"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (configuration.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < configuration.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 100) // Ruins
						{
							DropItem(item_name_to_id_map["beast_coin_giant"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (configuration.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < configuration.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}

						StructVariableSet(monster, "__deep_dungeon__loot_drop", true);
					}
				}

				// Boss loot drops
				if (boss_battle != BossBattle::NONE && boss_monsters_configured > 0)
				{
					int boss_monsters_defeated = 0;
					for (CInstance* monster : current_floor_monsters)
					{
						if (StructVariableExists(monster, "hit_points"))
						{
							double hit_points = monster->GetMember("hit_points").ToDouble();
							if (std::isfinite(hit_points) && hit_points <= 0)
								boss_monsters_defeated++;
						}
					}

					if (boss_monsters_defeated == boss_monsters_configured)
					{
						if (script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
							DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
						boss_battle = BossBattle::NONE;
						ResetCustomDrawFields();
					}
				}

				// Aspir (Mage Set Bonus)
				if (!StructVariableExists(monster, "__deep_dungeon__aspir_proc") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						static thread_local std::mt19937 random_generator(std::random_device{}());
						std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

						bool aspir_proc = zero_to_ninety_nine_distribution(random_generator) < 15 ? true : false; // TODO: Tune this. Should Aspir have a 15% chance to proc?
						if (aspir_proc)
							class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR]++;

						StructVariableSet(monster, "__deep_dungeon__aspir_proc", true);
					}
				}

				// Holy Circle (Paladin Set Bonus)
				if (CountEquippedClassArmor()[Classes::PALADIN] > 0 && !StructVariableExists(monster, "__deep_dungeon__holy_circle_proc") && StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						double default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToDouble();
						*monster.GetRefMember("hit_points") = hit_points - std::trunc(default_hit_points * GetPaladinHolyCirclePotency());
						StructVariableSet(monster, "__deep_dungeon__holy_circle_proc", true);

						for (CInstance* current_floor_monster : current_floor_monsters)
						{
							if (StructVariableExists(current_floor_monster, "monster_id") && StructVariableExists(current_floor_monster, "hit_points") && StructVariableExists(current_floor_monster, "__deep_dungeon__default_hit_points"))
							{
								RValue current_floor_monster_id = current_floor_monster->GetMember("monster_id");
								double current_floor_monster_hit_points = current_floor_monster->GetMember("hit_points").ToDouble();
								if (IsNumeric(current_floor_monster_id) && current_floor_monster_id.ToInt64() != monster_name_to_id_map["mimic"] && IsNumeric(current_floor_monster_hit_points) && std::isfinite(current_floor_monster_hit_points) && current_floor_monster_hit_points > 0)
								{
									double current_floor_monster_default_hit_points = current_floor_monster->GetMember("__deep_dungeon__default_hit_points").ToDouble();
									*current_floor_monster->GetRefMember("hit_points") = current_floor_monster_hit_points - std::trunc(current_floor_monster_default_hit_points * GetPaladinHolyCirclePotency());
								}
							}
						}
					}
				}

				// Gloom
				if (active_floor_enchantments.contains(FloorEnchantments::GLOOM))
				{
					if (!StructVariableExists(monster, "__deep_dungeon__gloom_applied") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = std::trunc(hit_points * 1.5); // TODO: Tune this.
							StructVariableSet(monster, "__deep_dungeon__gloom_applied", true);
						}
					}
				}

				// Reckoning
				if (active_offerings.contains(Offerings::RECKONING))
				{
					if (!StructVariableExists(monster, "__deep_dungeon__reckoning_applied") && StructVariableExists(monster, "hit_points"))
					{
						if (current_time_in_seconds < floor_start_time + 30) // All enemies seem to be created with HP var initialized before floor starts
						{
							*monster.GetRefMember("hit_points") = 1;
							StructVariableSet(monster, "__deep_dungeon__reckoning_applied", true);
						}
					}
				}

				// Spirit Link
				if (active_offerings.contains(Offerings::SPIRIT_LINK) && monster_id.ToInt64() != monster_name_to_id_map["mimic"])
				{
					if (StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							if (StructVariableExists(monster, "__deep_dungeon__spirit_link_applied") && spirit_link_combined_health_pool <= 0)
								StructVariableSet(monster, "hit_points", 0);
							else if (!StructVariableExists(monster, "__deep_dungeon__spirit_link_applied"))
							{
								spirit_link_combined_health_pool += hit_points;
								StructVariableSet(monster, "__deep_dungeon__spirit_link_applied", true);
								StructVariableSet(monster, "__deep_dungeon__spirit_link_damage", 0);
								
							}
							else if (StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
							{
								int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
								int spirit_link_damage = monster.GetMember("__deep_dungeon__spirit_link_damage").ToInt64();
								if (hit_points < default_hit_points)
								{
									spirit_link_damage += (default_hit_points - hit_points);
									spirit_link_combined_health_pool -= (default_hit_points - hit_points);

									StructVariableSet(monster, "__deep_dungeon__spirit_link_damage", spirit_link_damage);
									StructVariableSet(monster, "hit_points", default_hit_points);
								}
							}
						}
					}
				}
				else if (StructVariableExists(monster, "__deep_dungeon__spirit_link_applied"))
				{
					if (StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
							int spirit_link_damage = monster.GetMember("__deep_dungeon__spirit_link_damage").ToInt64();
							
							if(default_hit_points - spirit_link_damage > 0)
								StructVariableSet(monster, "hit_points", default_hit_points - spirit_link_damage);
							else
								StructVariableSet(monster, "hit_points", 0);
						}

						StructVariableRemove(monster, "__deep_dungeon__spirit_link_applied");
					}
				}

				// Spikes
				//if (active_offerings.contains(Offerings::SPIKES))
				//{
				//	if (StructVariableExists(monster, "hit_points"))
				//	{
				//		double hit_points = monster.GetMember("hit_points").ToDouble();
				//		if (std::isfinite(hit_points))
				//		{
				//			if (!StructVariableExists(monster, "__deep_dungeon__spikes_applied"))
				//			{
				//				StructVariableSet(monster, "__deep_dungeon__spikes_applied", true);
				//				StructVariableSet(monster, "__deep_dungeon__spikes_damage", 0);
				//			}
				//			else if (StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				//			{
				//				int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
				//				int spikes_damage = monster.GetMember("__deep_dungeon__spikes_damage").ToInt64();
				//				if (hit_points < default_hit_points)
				//				{
				//					spikes_damage += (default_hit_points - hit_points - spikes_damage);
				//					StructVariableSet(monster, "__deep_dungeon__spikes_damage", spikes_damage);

				//					int reflected_amount = max(1, spikes_damage * 20 / 100);
				//					ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * reflected_amount);
				//				}
				//			}
				//		}
				//	}
				//}

				// Sigil of Concealment
				if (active_sigils.contains(Sigils::CONCEALMENT))
				{
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue hit_points = monster.GetMember("hit_points");
						if (!StructVariableExists(monster, "__deep_dungeon__conceal_hit_points"))
							StructVariableSet(monster, "__deep_dungeon__conceal_hit_points", hit_points);

						RValue original_hit_points = monster.GetMember("__deep_dungeon__conceal_hit_points");
						if (hit_points.ToDouble() == original_hit_points.ToDouble())
							StructVariableSet(monster, "aggro", false);
						else
						{
							active_sigils.erase(Sigils::CONCEALMENT);
							CreateNotification(false, CONCEALMENT_LOST_NOTIFICATION_KEY, nullptr, nullptr);

							// Sneak Attack (Rogue Set Bonus)
							if (CountEquippedClassArmor()[Classes::ROGUE] >= 2)
								*monster.GetRefMember("hit_points") = 0;

							if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
								UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
						}
					}
				}
				else
					StructVariableRemove(monster, "__deep_dungeon__conceal_hit_points");

				// Sigil of Rage
				if (active_sigils.contains(Sigils::RAGE))
				{
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue hit_points = monster.GetMember("hit_points");
						if (!StructVariableExists(monster, "__deep_dungeon__rage_hit_points"))
							StructVariableSet(monster, "__deep_dungeon__rage_hit_points", hit_points);

						RValue original_hit_points = monster.GetMember("__deep_dungeon__rage_hit_points");
						if (hit_points.ToDouble() != original_hit_points.ToDouble())
							*monster.GetRefMember("hit_points") = 0;
					}
				}
			}
		}
	}
}

RValue& GmlScriptCancelStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Protection
	if (active_sigils.contains(Sigils::PROTECTION) && Arguments[0]->ToInt64() == status_effect_name_to_id_map["guardians_shield"])
	{
		active_sigils.erase(Sigils::PROTECTION);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	// Redemption
	if (active_sigils.contains(Sigils::REDEMPTION) && Arguments[0]->ToInt64() == status_effect_name_to_id_map["fairy"])
	{
		active_sigils.erase(Sigils::REDEMPTION);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CANCEL_STATUS_EFFECT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptRegisterStatusEffectCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (AriCurrentGmRoomIsDungeonFloor() && held_item_id == item_name_to_id_map[SUSTAINING_POTION_NAME])
	{
		if (Arguments[0]->ToInt64() == status_effect_name_to_id_map["restorative"])
		{
			int finish = Arguments[3]->ToInt64();
			*Arguments[3] = finish - static_cast<int>(7200 * (1 - configuration.sustaining_potion_duration_modifier)); // Modify the duration of Restoration
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_REGISTER_STATUS_EFFECT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptVitalsMenuSetMaxHealthCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH))
		script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptModifyHealthCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Afflatus Misery (Cleric Set Bonus)
	if (Arguments[0]->ToInt64() < 0 && CountEquippedClassArmor()[Classes::CLERIC] == 5 && AriCurrentGmRoomIsDungeonFloor())
		class_name_to_set_bonus_effect_value_map[Classes::CLERIC][ManagedSetBonuses::AFFLATUS_MISERY] += abs(Arguments[0]->ToInt64());

	// Frailty
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::FRAILTY) && !is_fumigate_tracked_interval && !is_deep_wounds_tracked_interval) // Need to check for Fumigate and Deep Wounds since Frailty is a Group 2 enchant
		frailty_hit_counter += 1;

	// Deep Wounds
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::DEEP_WOUNDS) && !is_deep_wounds_tracked_interval)
		deep_wounds_damage_pool += abs(Arguments[0]->ToInt64());

	// Stoneskin
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && stoneskin_shield_amount > 0)
	{
		int damage = abs(Arguments[0]->ToInt64());
		if (stoneskin_shield_amount >= damage)
		{
			stoneskin_shield_amount -= damage;
			damage = 0;
			
		}
		else
		{
			damage -= stoneskin_shield_amount;
			stoneskin_shield_amount = 0;
		}
		*Arguments[0] = -1 * damage;
	}

	// Phalanx
	if (Arguments[0]->ToInt64() < 0 && active_floor_enchantments.contains(FloorEnchantments::PHALANX))
	{
		int damage = abs(Arguments[0]->ToInt64());
		int modifier = damage * 20 / 100;
		*Arguments[0] = -1 * (damage - modifier);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_HEALTH));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Flee (Rogue Set Bonus)
	if (Arguments[0]->ToInt64() < 0 && CountEquippedClassArmor()[Classes::ROGUE] >= 1 && AriCurrentGmRoomIsDungeonFloor() && class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] == 0)
	{
		int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
		int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

		if (current_health <= max_health * 30 / 100)
		{
			int unified_time = GetUnifiedTime(script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][0], script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][1]).ToInt64();
			RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["mine_time"], 2.0, unified_time, unified_time + 900); // 15m duration
			class_name_to_set_bonus_effect_value_map[Classes::ROGUE][ManagedSetBonuses::FLEE] = 1;
		}
	}

	return Result;
}

RValue& GmlScriptModifyStaminaCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Exhaustion
	if (active_floor_enchantments.contains(FloorEnchantments::EXHAUSTION))
	{
		if (Arguments[0]->ToDouble() < 0)
		{
			double modified_stamina_cost = Arguments[0]->ToDouble() * 2;
			*Arguments[0] = modified_stamina_cost;
		}
	}
	
	// Spirit Surge
	if (active_greater_sigils.contains(GreaterSigils::SPIRIT_SURGE) && Arguments[0]->ToDouble() < 0)
		*Arguments[0] = 0;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_MODIFY_STAMINA));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

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
		static thread_local std::mt19937 random_generator(std::random_device{}());
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
			int chance_to_activate = zero_to_ninety_nine_distribution(random_generator);

			int activation_threshold = 100;
			for (int i = 0; i < sigil_of_alteration_count; i++)
			{
				activation_threshold /= 2;
			}

			bool activate = false;
			if (activation_threshold == 100)
				activate = true;
			else if (chance_to_activate < activation_threshold)
				activate = true;

			if (activate)
			{
				int random = zero_to_ninety_nine_distribution(random_generator);

				if (random < 40) // 40% chance for flame spirit
					*Arguments[2] = monster_name_to_id_map["spirit"];
				else // 60% chance for mimic
					*Arguments[2] = monster_name_to_id_map["mimic"];
			}

			sigil_of_alteration_count++;
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
		static thread_local std::mt19937 random_generator(std::random_device{}());
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
	static thread_local std::mt19937 random_generator(std::random_device{}());

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
				double damage = std::trunc(Arguments[0]->GetMember("damage").ToDouble() * 1.5); // 50% increased damage
				*Arguments[0]->GetRefMember("damage") = damage;
			}
			else
			{
				double damage = Arguments[0]->GetMember("damage").ToDouble();
				int penalty = std::trunc(damage * 0.50); // 50% reduced damage
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

RValue& GmlScriptStatusEffectManagerUpdateCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE))
		script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptTakePressCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TAKE_PRESS));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	// Chance for an Offering event when using a ladder on a dungeon floor.
	if (game_is_active && !GameIsPaused() && obj_dungeon_ladder_down_focused && Arguments[0]->ToInt64() == 6 && Result.ToBoolean() && !offering_chance_occurred && (floor_number < 19 || floor_number % 10 != 9))
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
		std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

		int roll = zero_to_ninety_nine_distribution(random_generator);
		if (active_sigils.contains(Sigils::TEMPTATION) || roll < configuration.offering_event_chance)
		{
			const std::vector<AriResources> POSSIBLE_OFFERING_RESOURCES = {
				AriResources::HEALTH, AriResources::STAMINA, AriResources::MANA
			};
			std::uniform_int_distribution<size_t> random_offering_resource_distribution(0, POSSIBLE_OFFERING_RESOURCES.size() - 1);

			AriResources resource = POSSIBLE_OFFERING_RESOURCES[random_offering_resource_distribution(random_generator)];
			if (resource == AriResources::HEALTH && ari_resource_to_value_map[AriResources::HEALTH] > configuration.offering_health_requirement)
				PlayConversation("Conversations/Mods/Deep Dungeon/offering/health", Self, Other);
			if (resource == AriResources::STAMINA && ari_resource_to_value_map[AriResources::STAMINA] > configuration.offering_stamina_requirement)
				PlayConversation("Conversations/Mods/Deep Dungeon/offering/stamina", Self, Other);
			if (resource == AriResources::MANA && ari_resource_to_value_map[AriResources::MANA] > configuration.offering_mana_requirement)
				PlayConversation("Conversations/Mods/Deep Dungeon/offering/mana", Self, Other);

			Result = false;
		}

		offering_chance_occurred = true;
	}
	// Disable the elevator if config option set.
	else if (game_is_active && !GameIsPaused() && configuration.disable_dungeon_lift && (ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal")) && obj_dungeon_elevator_focused && Arguments[0]->ToInt64() == 6 && Result.ToBoolean())
	{
		PlayConversation(ELEVATOR_LOCKED_CONVERSATION_KEY, Self, Other);
		Result = false;
	}

	return Result;
}

RValue& GmlScriptCheckValueCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && !GameIsPaused() && active_traps.contains(Traps::CONFUSING) && Arguments[0]->ToInt64() <= 4 && Arguments[0]->ToInt64() >= 1)
	{
		// Confusion Trap
		int new_value = 0;
		if (Arguments[0]->ToInt64() == 1)
			new_value = 2;
		if (Arguments[0]->ToInt64() == 2)
			new_value = 1;
		if (Arguments[0]->ToInt64() == 3)
			new_value = 4;
		if (Arguments[0]->ToInt64() == 4)
			new_value = 3;
		*Arguments[0] = new_value;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, "gml_Script_check_value@Input@Input"));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptAttemptInteractCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && Self->m_Object != nullptr)
	{
		std::string self_name = Self->m_Object->m_Name;

		if (self_name == "obj_dungeon_ladder_down")
		{
			if (floor_number != 0)
				obj_dungeon_ladder_down_focused = true;
			else
				obj_dungeon_ladder_down_focused = false;

			obj_dungeon_elevator_focused = false;
		}
		else if (self_name == "obj_dungeon_elevator")
		{
			if (configuration.disable_dungeon_lift)
				obj_dungeon_elevator_focused = true;
			else
				obj_dungeon_elevator_focused = false;

			obj_dungeon_ladder_down_focused = false;
		}
		else
		{
			obj_dungeon_ladder_down_focused = false;
			obj_dungeon_elevator_focused = false;
		}
	}
	else
	{
		obj_dungeon_ladder_down_focused = false;
		obj_dungeon_elevator_focused = false;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ATTEMPT_INTERACT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptPlayTextCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active)
	{
		std::string localization_key = Arguments[0]->ToString();

		if (localization_key.contains("Conversations/Mods/Deep Dungeon/offering"))
		{
			bool is_offering = false;
			if (localization_key == "Conversations/Mods/Deep Dungeon/offering/2/health")
			{
				is_offering = true;
				ari_resource_to_penalty_map[AriResources::HEALTH] = true;
			}
			else if (localization_key == "Conversations/Mods/Deep Dungeon/offering/2/stamina")
			{
				is_offering = true;
				ari_resource_to_penalty_map[AriResources::STAMINA] = true;
			}
			else if (localization_key == "Conversations/Mods/Deep Dungeon/offering/2/mana")
			{
				is_offering = true;
				ari_resource_to_penalty_map[AriResources::MANA] = true;
			}

			if (is_offering)
			{
				const std::vector<Offerings> possible_offerings = {
					Offerings::DREAD,
					Offerings::INNER_FIRE,
					Offerings::LEECH,
					Offerings::PERIL,
					Offerings::RECKONING
				};

				// Pick a random offering effect
				static thread_local std::mt19937 random_generator(std::random_device{}());
				std::uniform_int_distribution<size_t> random_offering_distribution(0, possible_offerings.size() - 1);
				Offerings offering = magic_enum::enum_value<Offerings>(random_offering_distribution(random_generator));
				queued_offerings.insert(offering);
			}
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/placeholders/sigil_parchment_exchange/cursed_armor/result")
		{
			int total_cursed_armor_inventory_count = 0;
			std::map<std::string, int> cursed_armor_name_to_inventory_count = {};
			std::unordered_set<std::string> cursed_armor = { CURSED_HELMET_NAME, CURSED_CHESTPIECE_NAME, CURSED_PANTS_NAME, CURSED_BOOTS_NAME, CURSED_GLOVES_NAME };

			for (std::string cursed_armor_name : cursed_armor)
			{
				int count = InventoryCountItem(item_name_to_id_map[cursed_armor_name], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
				total_cursed_armor_inventory_count += count;
				cursed_armor_name_to_inventory_count[cursed_armor_name] = count;
			}

			if (total_cursed_armor_inventory_count == 0)
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/insufficient";
			else
			{
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/cursed_armor";

				for (size_t i = 0; i < total_cursed_armor_inventory_count * 3; i++)
					DropItem(item_name_to_id_map["sigil_parchment"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);

				for (auto& entry : cursed_armor_name_to_inventory_count)
					if (entry.second > 0)
						InventoryRemoveItem(item_name_to_id_map[entry.first], entry.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]);
			}
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/placeholders/sigil_parchment_exchange/soul_stone/result")
		{
			int total_soul_stone_inventory_count = 0;
			std::map<std::string, int> soul_stone_name_to_inventory_count = {};

			for (std::string soul_stone_name : SOUL_STONE_NAMES)
			{
				int count = InventoryCountItem(item_name_to_id_map[soul_stone_name], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
				total_soul_stone_inventory_count += count;
				soul_stone_name_to_inventory_count[soul_stone_name] = count;
			}

			if (total_soul_stone_inventory_count == 0)
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/insufficient";
			else
			{
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/soul_stone";

				for (size_t i = 0; i < total_soul_stone_inventory_count * 3; i++)
					DropItem(item_name_to_id_map["sigil_parchment"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);

				for (auto& entry : soul_stone_name_to_inventory_count)
					if (entry.second > 0)
						InventoryRemoveItem(item_name_to_id_map[entry.first], entry.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]);
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_TEXT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

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

RValue& GmlScriptUseItemCallback(
	IN CInstance* Self, // Changes depending on the invocation context. For world interactables like a fountain, Self->m_Object->m_Name == "obj_world_fountain". For Ari using an item, Self->m_Object == NULL.
	IN CInstance* Other, // Changes depending on the invocation context. For world interactables like a fountain, Other->m_Object->m_Name == "Game". For Ari using an item, Other->m_Object->m_Name == "obj_ari".
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Orbs
	if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari") && orb_items.contains(held_item_id))
	{
		if (ari_current_gm_room != "rm_mines_entry")
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use an orb at the mines entrance!", MOD_NAME, VERSION);
			CreateNotification(false, ORB_RESTRICTED_NOTIFICATION_KEY, Self, Other);
			return Result;
		}
	}

	// Lift Keys
	if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari") && lift_key_items.contains(held_item_id))
	{
		if (ari_current_gm_room != "rm_mines_entry")
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use a lift key at the mines entrance!", MOD_NAME, VERSION);
			CreateNotification(false, LIFT_KEY_RESTRICTED_NOTIFICATION_KEY, Self, Other);
			return Result;
		}
	}

	// Inhibiting Trap
	if (active_traps.contains(Traps::INHIBITING))
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			if (held_item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use the Mistpool Sword due to the Inhibiting Trap's effect!", MOD_NAME, VERSION);
				CreateNotification(false, INHIBITED_PENALTY_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}

	// Item Penalty
	if (active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY))
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			if (deep_dungeon_items.contains(held_item_id) && held_item_id != sigil_to_item_id_map[Sigils::SERENITY] && held_item_id != item_name_to_id_map[MISTPOOL_SWORD_NAME] && !item_id_to_greater_sigil_map.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use that item due to the Item Penalty floor enchantment!", MOD_NAME, VERSION);
				CreateNotification(false, ITEM_PENALTY_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}

	if (AriCurrentGmRoomIsDungeonFloor())
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			if (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
			{
				// Sigil Items Restricted
				if (item_id_to_sigil_map.contains(held_item_id))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use sigils during boss battles!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_RESTRICTED_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Great Sigils Restricted
				if (item_id_to_greater_sigil_map.contains(held_item_id))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use greater sigils during boss battles!", MOD_NAME, VERSION);
					CreateNotification(false, GREATER_SIGIL_RESTRICTED_NOTIFICATION_KEY, Self, Other);
					return Result;
				}
			}
			else
			{
				// Sigil Already Used
				if (item_id_to_sigil_map.contains(held_item_id) && active_sigils.contains(item_id_to_sigil_map[held_item_id]))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Greater Sigil Already Used
				if (item_id_to_greater_sigil_map.contains(held_item_id) && !active_greater_sigils.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - A greater sigil has already been used!", MOD_NAME, VERSION);
					CreateNotification(false, GREATER_SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Protection Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::PROTECTION] && GetInvulnerabilityHits() > 0)
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Redemption Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::REDEMPTION] && FairyBuffIsActive())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Condemn (Oracle Set Bonus)
				if (held_item_id == sigil_to_item_id_map[Sigils::TEMPTATION] && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0)
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}
			}

			// Salve Limit
			if (configuration.limit_salves && salves_used.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You have already used that type of salve on the current floor!", MOD_NAME, VERSION);
				CreateNotification(false, SALVE_LIMIT_NOTIFICATION_KEY, Self, Other);
				return Result;
			}

			// Dungeon's Curse
			if (!deep_dungeon_items.contains(held_item_id) && restricted_items.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That item is prohibited in the Deep Dungeon!", MOD_NAME, VERSION);
				CreateNotification(false, ITEM_PROHIBITED_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}
	else
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			// Deep Dungeon Exclusive Items
			if (deep_dungeon_items.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You may only use Deep Dungeon specific items inside the dungeon!", MOD_NAME, VERSION);
				CreateNotification(false, ITEM_RESTRICTED_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}

	// Sigil Item
	sigil_item_used = false;
	if (item_id_to_sigil_map.contains(held_item_id))
		sigil_item_used = true;

	// Greater Sigil Item
	greater_sigil_item_used = false;
	if (item_id_to_greater_sigil_map.contains(held_item_id))
		greater_sigil_item_used = true;

	// Lift Key Item
	lift_key_used = false;
	if (lift_key_items.contains(held_item_id))
		lift_key_used = true;

	// Orb Item
	orb_item_used = false;
	if (orb_items.contains(held_item_id))
		orb_item_used = true;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_USE_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptHeldItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_HELD_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Result.m_Kind != VALUE_UNDEFINED)
	{
		int item_id = Result.GetMember("item_id").ToInt64();
		if (held_item_id != item_id)
			held_item_id = item_id;
	}

	return Result;
}

RValue& GmlScriptDropItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
		script_name_to_reference_map[GML_SCRIPT_DROP_ITEM] = { Self, Other };

	if (ari_current_gm_room.contains("rm_mines"))
	{
		bool chance_to_spawn_glowstone = false;

		if (Arguments[0]->m_Kind == VALUE_ARRAY)
		{
			size_t array_length;
			g_ModuleInterface->GetArraySize(*Arguments[0], array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* array_element;
				g_ModuleInterface->GetArrayEntry(*Arguments[0], i, array_element);

				if (StructVariableExists(*array_element, "item_id"))
				{
					int item_id = array_element->GetMember("item_id").ToInt64();
					if (item_id == item_name_to_id_map["ore_stone"])
						chance_to_spawn_glowstone = true;
				}
			}
		}
		else if (Arguments[0]->m_Kind == VALUE_INT64 && Arguments[0]->ToInt64() == item_name_to_id_map["ore_stone"])
			chance_to_spawn_glowstone = true;

		// TODO: Should there be some RNG for dropping glowstone?
		if (chance_to_spawn_glowstone)
		{
			if (floor_number < 20) // Upper Mines
				DropItem(item_name_to_id_map["glow_stone_tiny"], ari_x, ari_y, Self, Other);
			else if (floor_number < 40) // Tide Caverns
				DropItem(item_name_to_id_map["glow_stone_small"], ari_x, ari_y, Self, Other);
			else if (floor_number < 60) // Deep Earth
				DropItem(item_name_to_id_map["glow_stone_medium"], ari_x, ari_y, Self, Other);
			else if (floor_number < 80) // Lava Caves
				DropItem(item_name_to_id_map["glow_stone_large"], ari_x, ari_y, Self, Other);
			else if (floor_number < 100) // Ruins
				DropItem(item_name_to_id_map["glow_stone_giant"], ari_x, ari_y, Self, Other);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DROP_ITEM));
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
			if (!is_fumigate_tracked_interval && (current_time_in_seconds - time_of_last_fumigate_tick) >= TWO_MINUTES_IN_SECONDS)
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
			if ((current_time_in_seconds - time_of_last_outbreak_tick) >= ONE_HOUR_IN_SECONDS)
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
			for (auto it = active_floor_enchantments.begin(); it != active_floor_enchantments.end();)
			{
				custom_text += floor_enchantments_to_localized_string_map[*it];

				if (++it != active_floor_enchantments.end())
					custom_text += "\n";
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
	if (active_offerings.empty() && ari_current_gm_room != "rm_mines_entry" && ari_current_gm_room != "rm_priestess_quarters" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure"))
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
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
	sigil_of_alteration_count = 0;

	// Track Unmodified Max HP
	unmodified_base_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

	// Toggle reward on seal rooms when dungeon lift is disabled
	if (configuration.disable_dungeon_lift && ari_current_gm_room.contains("seal") && ari_current_gm_room != "rm_void_seal" && !biome_reward_disabled)
		drop_biome_reward = true;
	biome_reward_disabled = false;

	// Hide (Rogue Set Bonus)
	if (CountEquippedClassArmor()[Classes::ROGUE] > 0)
		active_sigils.insert(Sigils::CONCEALMENT);

	// Prophecy (Oracle Set Bonus)
	if (CountEquippedClassArmor()[Classes::ORACLE] >= 5 && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] == 1)
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
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

	if (ari_current_gm_room != "rm_mines_entry" && ari_current_gm_room != "rm_priestess_quarters" && !ari_current_gm_room.contains("seal") && !ari_current_gm_room.contains("ritual") && !ari_current_gm_room.contains("treasure"))
	{
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
			time_of_last_outbreak_tick = current_time_in_seconds;
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

	if ((ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal") || ari_current_gm_room == "rm_priestess_quarters") && ari_current_gm_room != "rm_mines_entry")
		SetFloorNumber();
	else
		floor_number = 0;

	// Store the floor number in the global instance for other mods.
	*__YYTK.GetRefMember(MOD_NAME)->GetRefMember("floor") = floor_number;

	ModifyMistpoolWeaponSprites();
	ModifyMistpoolPickaxeSprites();

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
		sigil_of_alteration_count = 0;
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

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (load_on_start)
	{
		load_on_start = false;
		localize_mod_text = true;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		window_width = GetWindowWidth();
		window_height = GetWindowHeight();

		CreateOrLoadConfigFile();
		CreateOrGetGlobalYYTKVariable();
		CreateModInfoInGlobalYYTKVariable();

		LoadPerks();
		LoadSpells();
		LoadSpellIds();
		LoadStatusEffects();
		LoadInfusions();
		LoadObjectIds();
		LoadItems();
		LoadMonsters();
		LoadDungeonBiomeCandidateMonsters();
		LoadPlayerStates();
		LoadMonsterStates();
		LoadBarkData();
		LoadTutorials();
		LoadStalagmiteAttackData();
		ModifyItems();
		// TODO: Load other stuff
	}
	else
		ResetStaticFields(true);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetEquipmentBonusFromCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Argument[0]: INT64 == 3 // ID of the infusion, see __infusion__ global, 3 == Hasty
	// Argument[1]: String == "amount" // Unused

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (AriCurrentGmRoomIsDungeonFloor() && !GameIsPaused())
	{
		int infusion_id = Arguments[0]->ToInt64();

		// Leech
		if (active_offerings.contains(Offerings::LEECH) && infusion_id == infusion_name_to_id_map["leeching"])
			Result = 1;

		// Dungeon's Curse: Prevent infusions on armor and tools from applying.
		if (configuration.restrict_armor && (infusion_id == infusion_name_to_id_map["fortified"] || infusion_id == infusion_name_to_id_map["hasty"] || infusion_id == infusion_name_to_id_map["tireless"]))
			Result = 0;
		if (configuration.restrict_tools && infusion_id == infusion_name_to_id_map["lightweight"])
			Result = 0;

		// Class armor bonuses
		std::map<int, int> class_armor_infusions = GetClassArmorInfusions();
		if (class_armor_infusions.contains(infusion_id))
		{
			if (infusion_id == infusion_name_to_id_map["fortified"])
				Result = class_armor_infusions[infusion_id] * 4;
			if (infusion_id == infusion_name_to_id_map["hasty"])
				Result = class_armor_infusions[infusion_id] * 0.04;
			if (infusion_id == infusion_name_to_id_map["tireless"])
				Result = class_armor_infusions[infusion_id] * 4;
		}
	}

	// Result: REAL == 0.20 // The value of the infusion bonus from all gear (0.20 for 5 Hasty armor pieces)
	return Result;
}

RValue& GmlScriptHudShouldShowCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_HUD_SHOULD_SHOW));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (active_traps.contains(Traps::DISORIENTING))
		Result = false;

	return Result;
}

RValue& GmlScriptOnDrawGuiCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_DRAW_GUI));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (game_is_active && !GameIsPaused())
	{
		// Gloom
		auto gloom = std::find(active_floor_enchantments.begin(), active_floor_enchantments.end(), FloorEnchantments::GLOOM);
		if (gloom != active_floor_enchantments.end())
		{
			// Draw semi-transparent overlay
			g_ModuleInterface->CallBuiltin(
				"draw_set_alpha",
				{ 0.45 }
			);

			g_ModuleInterface->CallBuiltin(
				"draw_set_color", {
					8388736 // c_purple
				}
			);

			g_ModuleInterface->CallBuiltin(
				"draw_rectangle",
				{ 0, 0, window_width, window_height, false }
			);
		}

		// Danger Floor Border
		if (show_dashes)
		{
			DrawDashedBorder(
				20.0f,    // dash length in pixels
				4.0f,     // dash thickness
				80.0f,    // speed pixels per second
				window_width,
				window_height,
				GetCurrentSystemTime()
			);
		}

		// Danger Floor Banner
		if (show_danger_banner)
			FadeInImage(2, 2); // 3, 2
	}

	return Result;
}

RValue& GmlScriptDisplayResizeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DISPLAY_RESIZE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	window_width = GetWindowWidth();
	window_height = GetWindowHeight();

	return Result;
}

RValue& GmlScriptGetUiIconCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_ITEM_UI_ICON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Self != nullptr && !crafting_menu_open)
	{
		RValue self = Self->ToRValue();
		if (StructVariableExists(self, "item_id"))
		{
			int item_id = self.GetMember("item_id").ToInt64();

			bool modify_icon = false;
			if (deep_dungeon_items.contains(item_id))
				modify_icon = true;
			else if (item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME] || item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME] || item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME] || item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME] || item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME])
				modify_icon = true;
			else if (item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME])
				modify_icon = true;

			if (modify_icon)
				Result = GetDynamicItemSprite(item_id);
		}
	}

	return Result;
}

RValue& GmlScriptUpdateToolbarMenuCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
		script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_UPDATE_TOOLBAR_MENU));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptCreateItemPrototypesCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CREATE_ITEM_PROTOTYPES));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	size_t array_length;
	g_ModuleInterface->GetArraySize(Result, array_length);

	// Load all items.
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(Result, i, array_element);

		item_id_to_prototype_map[i] = *array_element;
	}

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

RValue& GmlScriptDeserializeLiveItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DESERIALIZE_LIVE_ITEM))
		script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_LIVE_ITEM] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DESERIALIZE_LIVE_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptDeserializeInventoryCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DESERIALIZE_INVENTORY))
		script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DESERIALIZE_INVENTORY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetTreasureFromDistributionCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (Self != nullptr && StructVariableExists(Self, "object_id"))
	{
		int object_id = Self->GetMember("object_id").ToInt64();
		if (object_id_to_name_map.contains(object_id))
		{
			std::string object_name = object_id_to_name_map[object_id];
			if (DUNGEON_TREASURE_CHEST_NAMES.contains(object_name) && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
				GenerateTreasureChestLoot(object_name, Self, Other);
		}
	}
	else if (Other != nullptr && StructVariableExists(Other, "object_id"))
	{
		int object_id = Other->GetMember("object_id").ToInt64();
		if (object_id_to_name_map.contains(object_id))
		{
			std::string object_name = object_id_to_name_map[object_id];
			if (DUNGEON_TREASURE_CHEST_NAMES.contains(object_name) && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
				GenerateTreasureChestLoot(object_name, Self, Other);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptCraftingMenuInitializeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CRAFTING_MENU_INITIALIZE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	crafting_menu_open = true;
	return Result;
}

RValue& GmlScriptCraftingMenuCloseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CRAFTING_MENU_CLOSE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	crafting_menu_open = false;
	return Result;
}

RValue& GmlScriptJournalMenuInitializeCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_JOURNAL_MENU_INITIALIZE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	journal_menu_open = true;
	return Result;
}

RValue& GmlScriptJournalMenuCloseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_JOURNAL_MENU_CLOSE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	journal_menu_open = false;
	return Result;
}

RValue& GmlScriptVertigoDrawWithColorCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && !crafting_menu_open && AriCurrentGmRoomIsDungeonFloor()) // TODO: Make sure the AriCurrentGmRoomIsDungeonFloor() condition is working correctly.
	{
		RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { *Arguments[0] });
		if (type.ToInt64() == 1) // asset_sprite
		{
			RValue name = g_ModuleInterface->CallBuiltin("sprite_get_name", { *Arguments[0] });
			std::string name_str = name.ToString();

			RValue dynamic_sprite = GetDynamicUiSprite(name_str);
			if (dynamic_sprite.m_Kind == VALUE_REF)
				*Arguments[0] = dynamic_sprite;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptSceneAudioPlayerPlayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY))
		script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY] = { Self, Other };

	if (game_is_active && configuration.randomize_dungeon_music && AriCurrentGmRoomIsDungeonFloor() && floor_number != 91)
	{
		static thread_local std::mt19937 random_generator(std::random_device{}());
		std::uniform_int_distribution<size_t> game_music_distribution(0, MUSIC_INTERNAL_NAMES.size() - 1);
		*Arguments[0] = RValue(MUSIC_INTERNAL_NAMES.at(game_music_distribution(random_generator)));
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptFindNpcBlipNoiseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_FIND_NPC_BLIP_NOISE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (AriCurrentGmRoomIsDungeonFloor())
	{
		std::string audio_asset_name = Result.ToString();
		if (audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipSeridiaHuman" || audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipPriestess")
			Result = "SoundEffects/NPCs/Vocal/TextBlipHeadPriestess";
	}
	
	return Result;
}

RValue& GmlScriptSaveGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (floor_number != 0)
	{
		CreateNotification(true, SAVING_DISABLED_NOTIFICATION_KEY, Self, Other);
		return Result;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SAVE_GAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptAriFaceDirCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ari_facing_dir = Arguments[0]->ToDouble();

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ARI_FACE_DIR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptOnBeginStepCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Revealed Traps
	for (int i = 0; i < revealed_floor_traps.size(); i++)
	{
		if (revealed_floor_traps[i].is_active)
		{
			RValue spr_revealed_floor_trap = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_revealed_floor_trap" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { revealed_floor_traps[i].instance, "sprite_index", spr_revealed_floor_trap });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { revealed_floor_traps[i].instance, "depth", 350 });
		}
	}

	// Treasure Spot
	if (treasure_spot.is_active)
	{
		RValue spr_treasure_spot = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_treasure_spot" });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "sprite_index", spr_treasure_spot });
		g_ModuleInterface->CallBuiltin("variable_instance_set", { treasure_spot.instance, "depth", 350 });
	}
	

	// Meteor Sprites
	for (int i = 0; i < meteor_aoes.size(); i++)
	{
		if (meteor_aoes[i].is_active)
		{
			RValue spr_trap_meteor = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_meteor" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { meteor_aoes[i].instance, "sprite_index", spr_trap_meteor });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { meteor_aoes[i].instance, "image_speed", 0.6 });
		}
	}

	// Gaze Traps
	for (int i = 0; i < gaze_aoes.size(); i++)
	{
		if (gaze_aoes[i].is_active)
		{
			RValue spr_trap_gaze = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_gaze" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "sprite_index", spr_trap_gaze });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { gaze_aoes[i].instance, "image_speed", 0.6 });
		}
	}

	// Void Traps
	for (int i = 0; i < void_aoes.size(); i++)
	{
		if (void_aoes[i].is_active)
		{
			RValue spr_trap_void = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_trap_void" });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { void_aoes[i].instance, "sprite_index", spr_trap_void });
			g_ModuleInterface->CallBuiltin("variable_instance_set", { void_aoes[i].instance, "image_speed", 0.25 }); // 0.1
			g_ModuleInterface->CallBuiltin("variable_instance_set", { void_aoes[i].instance, "depth", -1000 });
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_BEGIN_STEP));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptRecipeGenerateInfusionsCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_RECIPE_GENERATE_INFUSIONS));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (StructVariableExists(Self, "item_id"))
	{
		int item_id = Self->GetMember("item_id").ToInt64();
		if (item_id_to_sigil_map.contains(item_id))
		{
			RValue empty_array = g_ModuleInterface->CallBuiltin("array_create", { 0 });
			*Result.GetRefMember("__count") = 0;
			*Result.GetRefMember("__internal_size") = 0;
			*Result.GetRefMember("__buffer") = empty_array;
		}
	}

	return Result;
}


RValue& GmlScriptBarkEmitterCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (/*!custom_bark_playing && */(StructVariableExists(Other, "god_mode") || StructVariableExists(Other, "wimp_mode")))
		script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_BARK_EMITTER));
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

void CreateObjectCallback(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook (EVENT_OBJECT_CALL)!", MOD_NAME, VERSION);
	}
}

void CreateHookGmlScriptCancelStatusEffect(AurieStatus& status)
{
	CScript* gml_script_cancel_status_effect = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CANCEL_STATUS_EFFECT,
		(PVOID*)&gml_script_cancel_status_effect
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CANCEL_STATUS_EFFECT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CANCEL_STATUS_EFFECT,
		gml_script_cancel_status_effect->m_Functions->m_ScriptFunction,
		GmlScriptCancelStatusEffectCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CANCEL_STATUS_EFFECT);
	}
}

void CreateHookGmlScriptRegisterStatusEffect(AurieStatus& status)
{
	CScript* gml_script_register_status_effect = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_REGISTER_STATUS_EFFECT,
		(PVOID*)&gml_script_register_status_effect
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_REGISTER_STATUS_EFFECT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_REGISTER_STATUS_EFFECT,
		gml_script_register_status_effect->m_Functions->m_ScriptFunction,
		GmlScriptRegisterStatusEffectCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_REGISTER_STATUS_EFFECT);
	}
}

void CreateHookGmlScriptVitalsMenuSetMaxHealth(AurieStatus& status)
{
	CScript* gml_script_vitals_menu_set_max_health = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_max_health
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH,
		gml_script_vitals_menu_set_max_health->m_Functions->m_ScriptFunction,
		GmlScriptVitalsMenuSetMaxHealthCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH);
	}
}

void CreateHookGmlScriptModifyHealth(AurieStatus& status)
{
	CScript* gml_script_modify_health = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_HEALTH,
		(PVOID*)&gml_script_modify_health
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_HEALTH);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_MODIFY_HEALTH,
		gml_script_modify_health->m_Functions->m_ScriptFunction,
		GmlScriptModifyHealthCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_HEALTH);
	}
}

void CreateHookGmlScriptModifyStamina(AurieStatus& status)
{
	CScript* gml_script_modify_stamina = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_STAMINA);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_MODIFY_STAMINA,
		gml_script_modify_stamina->m_Functions->m_ScriptFunction,
		GmlScriptModifyStaminaCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_MODIFY_STAMINA);
	}
}

void CreateHookGmlScriptSpawnMonster(AurieStatus& status)
{
	CScript* gml_script_spawn_monster = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_MONSTER,
		(PVOID*)&gml_script_spawn_monster
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SPAWN_MONSTER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SPAWN_MONSTER,
		gml_script_spawn_monster->m_Functions->m_ScriptFunction,
		GmlScriptSpawnMonsterCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SPAWN_MONSTER);
	}
}

void CreateHookGmlScriptCanCastSpell(AurieStatus& status)
{
	CScript* gml_script_can_cast_spell = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CAN_CAST_SPELL,
		(PVOID*)&gml_script_can_cast_spell
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CAN_CAST_SPELL);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CAN_CAST_SPELL,
		gml_script_can_cast_spell->m_Functions->m_ScriptFunction,
		GmlScriptCanCastSpellCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CAN_CAST_SPELL);
	}
}

void CreateHookGmlScriptCastSpell(AurieStatus& status)
{
	CScript* gml_script_cast_spell = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CAST_SPELL,
		(PVOID*)&gml_script_cast_spell
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CAST_SPELL);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CAST_SPELL,
		gml_script_cast_spell->m_Functions->m_ScriptFunction,
		GmlScriptCastSpellCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CAST_SPELL);
	}
}

void CreateHookGmlScriptGetMoveSpeed(AurieStatus& status)
{
	CScript* gml_script_get_move_speed = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MOVE_SPEED,
		(PVOID*)&gml_script_get_move_speed
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MOVE_SPEED);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_MOVE_SPEED,
		gml_script_get_move_speed->m_Functions->m_ScriptFunction,
		GmlScriptGetMoveSpeedCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MOVE_SPEED);
	}
}

void CreateHookGmlScriptDamage(AurieStatus& status)
{
	CScript* gml_script_damage = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DAMAGE,
		(PVOID*)&gml_script_damage
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DAMAGE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DAMAGE,
		gml_script_damage->m_Functions->m_ScriptFunction,
		GmlScriptDamageCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DAMAGE);
	}
}

void CreateHookGmlScriptStatusEffectManagerUpdate(AurieStatus& status)
{
	CScript* gml_script_status_effect_manager_update = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE,
		(PVOID*)&gml_script_status_effect_manager_update
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE,
		gml_script_status_effect_manager_update->m_Functions->m_ScriptFunction,
		GmlScriptStatusEffectManagerUpdateCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE);
	}
}

void CreateHookGmlScriptTakePress(AurieStatus& status)
{
	CScript* gml_script_take_press = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TAKE_PRESS,
		(PVOID*)&gml_script_take_press
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TAKE_PRESS);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_TAKE_PRESS,
		gml_script_take_press->m_Functions->m_ScriptFunction,
		GmlScriptTakePressCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TAKE_PRESS);
	}
}

void CreateHookGmlScriptCheckValue(AurieStatus& status)
{
	CScript* gml_script_check_value = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CHECK_VALUE,
		(PVOID*)&gml_script_check_value
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHECK_VALUE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CHECK_VALUE,
		gml_script_check_value->m_Functions->m_ScriptFunction,
		GmlScriptCheckValueCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHECK_VALUE);
	}
}

void CreateHookGmlScriptAttemptInteract(AurieStatus& status)
{
	CScript* gml_script_attempt_interact = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ATTEMPT_INTERACT,
		(PVOID*)&gml_script_attempt_interact
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ATTEMPT_INTERACT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ATTEMPT_INTERACT,
		gml_script_attempt_interact->m_Functions->m_ScriptFunction,
		GmlScriptAttemptInteractCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ATTEMPT_INTERACT);
	}
}

void CreateHookGmlScriptPlayText(AurieStatus& status)
{
	CScript* gml_script_play_text = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_TEXT,
		(PVOID*)&gml_script_play_text
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_TEXT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_PLAY_TEXT,
		gml_script_play_text->m_Functions->m_ScriptFunction,
		GmlScriptPlayTextCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_TEXT);
	}
}

void CreateHookGmlScriptPlayConversation(AurieStatus& status)
{
	CScript* gml_script_play_conversation = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_CONVERSATION,
		(PVOID*)&gml_script_play_conversation
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_CONVERSATION);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_PLAY_CONVERSATION,
		gml_script_play_conversation->m_Functions->m_ScriptFunction,
		GmlScriptPlayConversationCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_CONVERSATION);
	}
}

void CreateHookGmlScriptUseItem(AurieStatus& status)
{
	CScript* gml_script_use_item = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_USE_ITEM,
		(PVOID*)&gml_script_use_item
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_USE_ITEM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_USE_ITEM,
		gml_script_use_item->m_Functions->m_ScriptFunction,
		GmlScriptUseItemCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_USE_ITEM);
	}
}

void CreateHookGmlScriptHeldItem(AurieStatus& status)
{
	CScript* gml_script_held_item = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_HELD_ITEM,
		(PVOID*)&gml_script_held_item
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_HELD_ITEM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_HELD_ITEM,
		gml_script_held_item->m_Functions->m_ScriptFunction,
		GmlScriptHeldItemCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_HELD_ITEM);
	}
}

void CreateHookGmlScriptDropItem(AurieStatus& status)
{
	CScript* gml_script_drop_item = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DROP_ITEM,
		(PVOID*)&gml_script_drop_item
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DROP_ITEM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DROP_ITEM,
		gml_script_drop_item->m_Functions->m_ScriptFunction,
		GmlScriptDropItemCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DROP_ITEM);
	}
}

void CreateHookGmlScriptGetMinutes(AurieStatus& status)
{
	CScript* gml_script_get_minutes = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MINUTES,
		(PVOID*)&gml_script_get_minutes
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MINUTES);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_MINUTES,
		gml_script_get_minutes->m_Functions->m_ScriptFunction,
		GmlScriptGetMinutesCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_MINUTES);
	}
}

void CreateHookGmlScriptGetLocalizer(AurieStatus& status)
{
	CScript* gml_script_get_localizer = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_LOCALIZER,
		gml_script_get_localizer->m_Functions->m_ScriptFunction,
		GmlScriptGetLocalizerCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}
}

void CreateHookGmlScriptGetWeather(AurieStatus& status)
{
	CScript* gml_script_get_weather = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_WEATHER,
		(PVOID*)&gml_script_get_weather
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_WEATHER,
		gml_script_get_weather->m_Functions->m_ScriptFunction,
		GmlScriptGetWeatherCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_WEATHER);
	}
}

void CreateHookGmlScriptTryLocationIdToString(AurieStatus& status)
{
	CScript* gml_script_try_location_id_to_string = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
		(PVOID*)&gml_script_try_location_id_to_string
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
		gml_script_try_location_id_to_string->m_Functions->m_ScriptFunction,
		GmlScriptTryLocationIdToStringCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING);
	}
}

void CreateHookGmlScriptOnDungeonRoomStart(AurieStatus& status)
{
	CScript* gml_script_on_dungeon_room_start = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_DUNGEON_ROOM_START,
		(PVOID*)&gml_script_on_dungeon_room_start
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DUNGEON_ROOM_START);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_DUNGEON_ROOM_START,
		gml_script_on_dungeon_room_start->m_Functions->m_ScriptFunction,
		GmlScriptOnDungeonRoomStartCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DUNGEON_ROOM_START);
	}
}

void CreateHookGmlScriptGoToRoom(AurieStatus& status)
{
	CScript* gml_script_go_to_room = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GO_TO_ROOM,
		(PVOID*)&gml_script_go_to_room
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GO_TO_ROOM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GO_TO_ROOM,
		gml_script_go_to_room->m_Functions->m_ScriptFunction,
		GmlScriptGoToRoomCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GO_TO_ROOM);
	}
}

void CreateHookGmlScriptSetupMainScreen(AurieStatus& status)
{
	CScript* gml_script_setup_main_screen = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		(PVOID*)&gml_script_setup_main_screen
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		gml_script_setup_main_screen->m_Functions->m_ScriptFunction,
		GmlScriptSetupMainScreenCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}
}

void CreateHookGmlScriptGetEquipmentBonusFrom(AurieStatus& status)
{
	CScript* gml_script_get_equipment_bonus_from = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM,
		(PVOID*)&gml_script_get_equipment_bonus_from
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM,
		gml_script_get_equipment_bonus_from->m_Functions->m_ScriptFunction,
		GmlScriptGetEquipmentBonusFromCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM);
	}
}

void CreateHookGmlScriptHudShouldShow(AurieStatus& status)
{
	CScript* gml_script_hud_should_show = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_HUD_SHOULD_SHOW,
		(PVOID*)&gml_script_hud_should_show
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_HUD_SHOULD_SHOW);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_HUD_SHOULD_SHOW,
		gml_script_hud_should_show->m_Functions->m_ScriptFunction,
		GmlScriptHudShouldShowCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_HUD_SHOULD_SHOW);
	}
}

void CreateHookGmlScriptOnDrawGui(AurieStatus& status)
{
	CScript* gml_script_on_draw_gui = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_DRAW_GUI,
		(PVOID*)&gml_script_on_draw_gui
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DRAW_GUI);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_DRAW_GUI,
		gml_script_on_draw_gui->m_Functions->m_ScriptFunction,
		GmlScriptOnDrawGuiCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_DRAW_GUI);
	}
}

void CreateHookGmlScriptDisplayResize(AurieStatus& status)
{
	CScript* gml_script_display_resize = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DISPLAY_RESIZE,
		(PVOID*)&gml_script_display_resize
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DISPLAY_RESIZE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DISPLAY_RESIZE,
		gml_script_display_resize->m_Functions->m_ScriptFunction,
		GmlScriptDisplayResizeCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DISPLAY_RESIZE);
	}
}

void CreateHookGmlScriptGetUiIcon(AurieStatus& status)
{
	CScript* gml_script_get_ui_icon = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_ITEM_UI_ICON,
		(PVOID*)&gml_script_get_ui_icon
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_ITEM_UI_ICON);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_ITEM_UI_ICON,
		gml_script_get_ui_icon->m_Functions->m_ScriptFunction,
		GmlScriptGetUiIconCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_ITEM_UI_ICON);
	}
}

void CreateHookGmlScriptUpdateToolbarMenu(AurieStatus& status)
{
	CScript* gml_script_update_toolbar_menu = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_UPDATE_TOOLBAR_MENU,
		(PVOID*)&gml_script_update_toolbar_menu
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_UPDATE_TOOLBAR_MENU);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_UPDATE_TOOLBAR_MENU,
		gml_script_update_toolbar_menu->m_Functions->m_ScriptFunction,
		GmlScriptUpdateToolbarMenuCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_UPDATE_TOOLBAR_MENU);
	}
}

void CreateHookGmlScriptCreateItemPrototypes(AurieStatus& status)
{
	CScript* gml_script_create_item_prototypes = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_ITEM_PROTOTYPES,
		(PVOID*)&gml_script_create_item_prototypes
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CREATE_ITEM_PROTOTYPES);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CREATE_ITEM_PROTOTYPES,
		gml_script_create_item_prototypes->m_Functions->m_ScriptFunction,
		GmlScriptCreateItemPrototypesCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CREATE_ITEM_PROTOTYPES);
	}
}

void CreateHookGmlScriptSpawnLadder(AurieStatus& status)
{
	CScript* gml_script_spawn_ladder = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_LADDER,
		(PVOID*)&gml_script_spawn_ladder
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SPAWN_LADDER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SPAWN_LADDER,
		gml_script_spawn_ladder->m_Functions->m_ScriptFunction,
		GmlScriptSpawnLadderCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SPAWN_LADDER);
	}
}

void CreateHookGmlScriptDeserializeLiveItem(AurieStatus& status)
{
	CScript* gml_script_deserialize_live_item = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DESERIALIZE_LIVE_ITEM,
		(PVOID*)&gml_script_deserialize_live_item
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DESERIALIZE_LIVE_ITEM);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DESERIALIZE_LIVE_ITEM,
		gml_script_deserialize_live_item->m_Functions->m_ScriptFunction,
		GmlScriptDeserializeLiveItemCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DESERIALIZE_LIVE_ITEM);
	}
}

void CreateHookGmlScriptDeserializeInventory(AurieStatus& status)
{
	CScript* gml_script_deserialize_inventory = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DESERIALIZE_INVENTORY,
		(PVOID*)&gml_script_deserialize_inventory
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DESERIALIZE_INVENTORY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DESERIALIZE_INVENTORY,
		gml_script_deserialize_inventory->m_Functions->m_ScriptFunction,
		GmlScriptDeserializeInventoryCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DESERIALIZE_INVENTORY);
	}
}

void CreateHookGmlScriptGetTreasureFromDistribution(AurieStatus& status)
{
	CScript* gml_script_get_treasure_from_distribution = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION,
		(PVOID*)&gml_script_get_treasure_from_distribution
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION,
		gml_script_get_treasure_from_distribution->m_Functions->m_ScriptFunction,
		GmlScriptGetTreasureFromDistributionCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION);
	}
}

void CreateHookGmlScriptCraftingMenuInitialize(AurieStatus& status)
{
	CScript* gml_script_get_display_name = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CRAFTING_MENU_INITIALIZE,
		(PVOID*)&gml_script_get_display_name
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_INITIALIZE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CRAFTING_MENU_INITIALIZE,
		gml_script_get_display_name->m_Functions->m_ScriptFunction,
		GmlScriptCraftingMenuInitializeCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_INITIALIZE);
	}
}

void CreateHookGmlScriptCraftingMenuClose(AurieStatus& status)
{
	CScript* gml_script_get_display_name = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CRAFTING_MENU_CLOSE,
		(PVOID*)&gml_script_get_display_name
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_CLOSE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CRAFTING_MENU_CLOSE,
		gml_script_get_display_name->m_Functions->m_ScriptFunction,
		GmlScriptCraftingMenuCloseCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CRAFTING_MENU_CLOSE);
	}
}

void CreateHookGmlScriptJournalMenuInitialize(AurieStatus& status)
{
	CScript* gml_script_journal_menu_initialize = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_JOURNAL_MENU_INITIALIZE,
		(PVOID*)&gml_script_journal_menu_initialize
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_JOURNAL_MENU_INITIALIZE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_JOURNAL_MENU_INITIALIZE,
		gml_script_journal_menu_initialize->m_Functions->m_ScriptFunction,
		GmlScriptJournalMenuInitializeCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_JOURNAL_MENU_INITIALIZE);
	}
}

void CreateHookGmlScriptJournalMenuClose(AurieStatus& status)
{
	CScript* gml_script_journal_menu_close = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_JOURNAL_MENU_CLOSE,
		(PVOID*)&gml_script_journal_menu_close
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_JOURNAL_MENU_CLOSE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_JOURNAL_MENU_CLOSE,
		gml_script_journal_menu_close->m_Functions->m_ScriptFunction,
		GmlScriptJournalMenuCloseCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_JOURNAL_MENU_CLOSE);
	}
}

void CreateHookGmlScriptVertigoDrawWithColor(AurieStatus& status)
{
	CScript* gml_script_vertigo_draw_with_color = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR,
		(PVOID*)&gml_script_vertigo_draw_with_color
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR,
		gml_script_vertigo_draw_with_color->m_Functions->m_ScriptFunction,
		GmlScriptVertigoDrawWithColorCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR);
	}
}

void CreateHookGmlScriptSceneAudioPlayerPlay(AurieStatus& status)
{
	CScript* gml_script_scene_audio_player_play = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY,
		(PVOID*)&gml_script_scene_audio_player_play
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY,
		gml_script_scene_audio_player_play->m_Functions->m_ScriptFunction,
		GmlScriptSceneAudioPlayerPlayCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY);
	}
}

void CreateHookGmlScriptFindNpcBlipNoise(AurieStatus& status)
{
	CScript* gml_script_find_npc_blip_noise = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_FIND_NPC_BLIP_NOISE,
		(PVOID*)&gml_script_find_npc_blip_noise
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_FIND_NPC_BLIP_NOISE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_FIND_NPC_BLIP_NOISE,
		gml_script_find_npc_blip_noise->m_Functions->m_ScriptFunction,
		GmlScriptFindNpcBlipNoiseCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_FIND_NPC_BLIP_NOISE);
	}
}

void CreateHookGmlScriptSaveGame(AurieStatus& status)
{
	CScript* gml_script_save_game = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SAVE_GAME,
		(PVOID*)&gml_script_save_game
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SAVE_GAME,
		gml_script_save_game->m_Functions->m_ScriptFunction,
		GmlScriptSaveGameCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
	}
}

void CreateHookGmlScriptAriFaceDir(AurieStatus& status)
{
	CScript* gml_script_ari_face_dir = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ARI_FACE_DIR,
		(PVOID*)&gml_script_ari_face_dir
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FACE_DIR);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ARI_FACE_DIR,
		gml_script_ari_face_dir->m_Functions->m_ScriptFunction,
		GmlScriptAriFaceDirCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ARI_FACE_DIR);
	}
}

void CreateHookGmlScriptOnBeginStep(AurieStatus& status)
{
	CScript* gml_script_on_begin_step = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ON_BEGIN_STEP,
		(PVOID*)&gml_script_on_begin_step
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_BEGIN_STEP);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_ON_BEGIN_STEP,
		gml_script_on_begin_step->m_Functions->m_ScriptFunction,
		GmlScriptOnBeginStepCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_ON_BEGIN_STEP);
	}
}

void CreateHookGmlScriptRecipeGenerateInfusions(AurieStatus& status)
{
	CScript* gml_script_recipe_generate_infusions = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_RECIPE_GENERATE_INFUSIONS,
		(PVOID*)&gml_script_recipe_generate_infusions
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_RECIPE_GENERATE_INFUSIONS);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_RECIPE_GENERATE_INFUSIONS,
		gml_script_recipe_generate_infusions->m_Functions->m_ScriptFunction,
		GmlScriptRecipeGenerateInfusionsCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_RECIPE_GENERATE_INFUSIONS);
	}
}

void CreateHookGmlScriptBarkEmitter(AurieStatus& status)
{
	CScript* gml_script_bark_emitter = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_BARK_EMITTER,
		(PVOID*)&gml_script_bark_emitter
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_BARK_EMITTER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_BARK_EMITTER,
		gml_script_bark_emitter->m_Functions->m_ScriptFunction,
		GmlScriptBarkEmitterCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_BARK_EMITTER);
	}
}

void CreateHookGmlScriptGetUnifiedTime(AurieStatus& status)
{
	CScript* gml_script_get_unified_time = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_UNIFIED_TIME,
		(PVOID*)&gml_script_get_unified_time
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_UNIFIED_TIME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_UNIFIED_TIME,
		gml_script_get_unified_time->m_Functions->m_ScriptFunction,
		GmlScriptGetUnifiedTimeCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_UNIFIED_TIME);
	}
}

EXPORTED AurieStatus ModuleInitialize(
	IN AurieModule* Module,
	IN const fs::path& ModulePath
)
{
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateObjectCallback(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCancelStatusEffect(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptRegisterStatusEffect(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptVitalsMenuSetMaxHealth(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptModifyHealth(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptModifyStamina(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSpawnMonster(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCanCastSpell(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCastSpell(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetMoveSpeed(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptDamage(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptStatusEffectManagerUpdate(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptTakePress(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCheckValue(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAttemptInteract(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptPlayText(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptPlayConversation(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptUseItem(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptHeldItem(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptDropItem(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetMinutes(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetLocalizer(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetWeather(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptTryLocationIdToString(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptOnDungeonRoomStart(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGoToRoom(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSetupMainScreen(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetEquipmentBonusFrom(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptHudShouldShow(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptOnDrawGui(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptDisplayResize(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetUiIcon(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptUpdateToolbarMenu(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCreateItemPrototypes(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSpawnLadder(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptDeserializeLiveItem(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptDeserializeInventory(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetTreasureFromDistribution(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCraftingMenuInitialize(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCraftingMenuClose(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptJournalMenuInitialize(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptJournalMenuClose(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptVertigoDrawWithColor(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSceneAudioPlayerPlay(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptFindNpcBlipNoise(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSaveGame(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptAriFaceDir(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptOnBeginStep(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptRecipeGenerateInfusions(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptBarkEmitter(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetUnifiedTime(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}