#pragma once
#include <string>
#include <vector>
#include <unordered_set>

// Sigil item names
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

// Consumable item names
static const std::string SUSTAINING_POTION_NAME = "sustaining_potion";
static const std::string HEALTH_SALVE_NAME = "health_salve";
static const std::string STAMINA_SALVE_NAME = "stamina_salve";
static const std::string MANA_SALVE_NAME = "mana_salve";

// Sword item names
static const std::string WORN_SWORD_NAME = "sword_worn";
static const std::string COPPER_SWORD_NAME = "sword_copper";
static const std::string IRON_SWORD_NAME = "sword_iron";
static const std::string SILVER_SWORD_NAME = "sword_silver";
static const std::string GOLD_SWORD_NAME = "sword_gold";
static const std::string MISTRIL_SWORD_NAME = "sword_mistril";
static const std::string DRAGON_FORGED_SWORD_NAME = "sword_dragon_forged";
static const std::string SCRAP_METAL_SWORD_NAME = "sword_scrap_metal";
static const std::string VERDIGRIS_SWORD_NAME = "sword_verdigris";
static const std::string CRYSTAL_SWORD_NAME = "sword_crystal";
static const std::string TARNISHED_GOLD_SWORD_NAME = "sword_tarnished_gold";
static const std::string CORRUPTED_MISTRIL_SWORD_NAME = "sword_corrupted_mistril";

// Mistpool gear names
static const std::string MISTPOOL_SWORD_NAME = "sword_scrap_metal";
static const std::string MISTPOOL_PICK_AXE_NAME = "pick_axe_worn";
static const std::string MISTPOOL_HELMET_NAME = "scrap_metal_helmet";
static const std::string MISTPOOL_CHESTPIECE_NAME = "scrap_metal_chestpiece";
static const std::string MISTPOOL_PANTS_NAME = "scrap_metal_pants";
static const std::string MISTPOOL_BOOTS_NAME = "scrap_metal_boots";
static const std::string MISTPOOL_GLOVES_NAME = "scrap_metal_ring";

// Cursed armor names
static const std::string CURSED_HELMET_NAME = "cursed_helmet";
static const std::string CURSED_CHESTPIECE_NAME = "cursed_chestpiece";
static const std::string CURSED_PANTS_NAME = "cursed_pants";
static const std::string CURSED_BOOTS_NAME = "cursed_boots";
static const std::string CURSED_GLOVES_NAME = "cursed_gloves";

// Class armor names
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

// Dungeon treasure chest names
static const std::string TREASURE_CHEST_WOOD_NAME = "treasure_chest_wood";
static const std::string TREASURE_CHEST_COPPER_NAME = "treasure_chest_copper";
static const std::string TREASURE_CHEST_SILVER_NAME = "treasure_chest_silver";
static const std::string TREASURE_CHEST_GOLD_NAME = "treasure_chest_gold";

// Lift key names
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

// Dread contract names
static const std::string UPPER_MINES_DREAD_CONTRACT = "upper_mines_dread_contract";
static const std::string TIDE_CAVERNS_DREAD_CONTRACT = "tide_caverns_dread_contract";
static const std::string DEEP_EARTH_DREAD_CONTRACT = "deep_earth_dread_contract";
static const std::string LAVA_CAVES_DREAD_CONTRACT = "lava_caves_dread_contract";
static const std::string RUINS_DREAD_CONTRACT = "ruins_dread_contract";

// Orb names
static const std::string TIDE_CAVERNS_ORB = "tide_caverns_orb";
static const std::string DEEP_EARTH_ORB = "deep_earth_orb";
static const std::string LAVA_CAVES_ORB = "lava_caves_orb";
static const std::string RUINS_ORB = "ruins_orb";

// Soul stone names
static const std::string SOUL_STONE_CLERIC = "soul_stone_cleric";
static const std::string SOUL_STONE_DARK_KNIGHT = "soul_stone_dark_knight";
static const std::string SOUL_STONE_MAGE = "soul_stone_mage";
static const std::string SOUL_STONE_PALADIN = "soul_stone_paladin";
static const std::string SOUL_STONE_ROGUE = "soul_stone_rogue";
static const std::string SOUL_STONE_ORACLE = "soul_stone_oracle";

// Item collections
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
