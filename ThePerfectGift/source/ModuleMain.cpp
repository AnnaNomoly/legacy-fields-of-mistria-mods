#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----

static const char* const MOD_NAME = "ThePerfectGift";
static const char* const VERSION  = "1.4.0";

// ----- Config keys + defaults -----

static const char* const UNLOCK_ALL_GIFT_PREFERENCES_KEY            = "unlock_all_gift_preferences";
static const char* const SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY = "show_gift_preferences_on_item_tooltips";
static const char* const DISABLE_DAILY_TALK_LIMIT_KEY               = "disable_daily_talk_limit";
static const char* const DISABLE_DAILY_GIFT_LIMIT_KEY               = "disable_daily_gift_limit";

static const bool DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES            = false;
static const bool DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS = false;
static const bool DEFAULT_DISABLE_DAILY_TALK_LIMIT               = false;
static const bool DEFAULT_DISABLE_DAILY_GIFT_LIMIT               = false;

// ----- Notification localization keys -----

static const std::string GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY = "mods/ThePerfectGift/gift_preference_detected";
static const std::string GIFT_PREFERENCE_UNLOCKED_LOCALIZATION_KEY = "mods/ThePerfectGift/gift_preference_unlocked";
static const std::string ITEM_PLACEHOLDER_TEXT                     = "<ITEM>";
static const std::string NPC_PLACEHOLDER_TEXT                      = "<NPC>";

// ----- NPC name constants (lowercase, matching the game's internal NPC keys) -----

static const std::string ADELINE   = "adeline";
static const std::string BALOR     = "balor";
static const std::string CALDARUS  = "caldarus";
static const std::string CELINE    = "celine";
static const std::string DARCY     = "darcy";
static const std::string DELL      = "dell";
static const std::string DOZY      = "dozy";
static const std::string EILAND    = "eiland";
static const std::string ELSIE     = "elsie";
static const std::string ERROL     = "errol";
static const std::string HAYDEN    = "hayden";
static const std::string HEMLOCK   = "hemlock";
static const std::string HENRIETTA = "henrietta";
static const std::string HOLT      = "holt";
static const std::string JOSEPHINE = "josephine";
static const std::string JUNIPER   = "juniper";
static const std::string LANDEN    = "landen";
static const std::string LOUIS     = "louis";
static const std::string LUC       = "luc";
static const std::string MAPLE     = "maple";
static const std::string MARCH     = "march";
static const std::string MERRI     = "merri";
static const std::string NORA      = "nora";
static const std::string OLRIC     = "olric";
static const std::string REINA     = "reina";
static const std::string RYIS      = "ryis";
static const std::string SERIDIA   = "seridia";
static const std::string TALIFERRO = "taliferro";
static const std::string TERITHIA  = "terithia";
static const std::string VALEN     = "valen";
static const std::string VERA      = "vera";
static const std::string WHEEDLE   = "wheedle";

// ACTIVE_NPC_LIST is the set the original "unlock all" feature iterates. Stillwell and Zorel are
// intentionally excluded — they aren't in the live game data yet.
static const std::vector<std::string> ACTIVE_NPC_LIST = {
	ADELINE, BALOR, CALDARUS, CELINE, DARCY, DELL, DOZY, EILAND, ELSIE, ERROL,
	HAYDEN, HEMLOCK, HENRIETTA, HOLT, JOSEPHINE, JUNIPER, LANDEN, LOUIS, LUC, MAPLE,
	MARCH, MERRI, NORA, OLRIC, REINA, RYIS, SERIDIA, TALIFERRO, TERITHIA, VALEN, VERA, WHEEDLE
};

// Each entry: {dialogue_key, {npc_name, item_internal_name_1, item_internal_name_2, ...}}.
// When `say@TextboxMenu` plays a line matching the dialogue_key, queue each listed item for unlock
// on the named NPC. Source: game version 0.15.3.
static const std::multimap<std::string, std::vector<std::string>> GIFT_DIALOG_MAP = {
	// Adeline
	{ "Cutscenes/Heart Events/Adeline/adeline_eight_hearts/adeline_eight_hearts/3", { ADELINE, "lemon_pie" }},
	{ "Conversations/Bank/Adeline/Relationship Lines/Relationship/post_8h_lines_romantic/adeline_post_8h_romantic_7/init", { ADELINE, "middlemist" }},
	{ "Conversations/Bank/Adeline/Relationship Lines/Relationship/post_8h_lines_best_friend/adeline_post_8h_best_friend_10/init", { ADELINE, "coffee" }},
	{ "Conversations/Bank/Adeline/Banked Lines/inn_work/inn_work/init", { ADELINE, "wildberry_scone" }},
	{ "Conversations/Bank/Adeline/Banked Lines/loves_paperwork/loves_paperwork/init", { ADELINE, "paper" }},
	{ "Conversations/Bank/Adeline/Banked Lines/wine_cellar/wine_cellar/1", { ADELINE, "red_wine" }},
	{ "Conversations/Bank/Adeline/Banked Lines/wine_cellar/wine_cellar/init", { ADELINE, "coffee", "white_wine" }},
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/lemon_pie/init", { ADELINE, "lemon_pie" }},
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/middlemist/init", { ADELINE, "middlemist" }},
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/paper/init", { ADELINE, "paper" }},
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/perfect_pink_diamond/init", { ADELINE, "perfect_pink_diamond" }},
	{ "Conversations/Bank/Adeline/Market Lines/market_darcy_1/market_darcy_1/init", { ADELINE, "coffee" }},
	{ "Conversations/Bank/Adeline/Market Lines/market_darcy_2/market_darcy_2/init", { ADELINE, "coffee" }},
	{ "Conversations/Bank/Adeline/Market Lines/market_darcy_4/market_darcy_4/init", { ADELINE, "spicy_cheddar_biscuit" }},
	{ "Cutscenes/Story Events/adeline_quest_board/adeline_quest_board/15", { ADELINE, "tulip" }},
	{ "Cutscenes/Story Events/adeline_quest_board/adeline_quest_board/17", { ADELINE, "tulip" }},
	{ "Conversations/Bank/Josephine/Banked Lines/week_one_pt_2/week_two/4", { ADELINE, "coffee" }},
	{ "Conversations/Group Conversations/Adeline_Balor/market/market/2", { ADELINE, "coffee" }},
	{ "Conversations/Bank/Adeline/Banked Lines/office_work/office_work_5/init", { ADELINE, "coffee" }},
	// Balor
	{ "Conversations/Bank/Balor/Relationship Lines/Relationship/post_8h_lines_romantic/balor_post_8h_romantic_3/init", { BALOR, "ore_diamond", "ore_ruby"}},
	{ "Conversations/Bank/Balor/Banked Lines/bath_smelled_nice/bath_smelled_nice/1", { BALOR, "jasmine" }},
	{ "Conversations/Bank/Balor/Gift Lines/gift_lines/perfect_diamond/init", { BALOR, "perfect_diamond" }},
	{ "Conversations/Bank/Balor/Museum Lines/alda_gem_bracelet/alda_gem_bracelet/init", { BALOR, "alda_gem_bracelet" }},
	{ "Conversations/Bank/Balor/Museum Lines/family_crest_pendant/family_crest_pendant/init", { BALOR, "family_crest_pendant" }},
	{ "Conversations/Bank/Balor/Museum Lines/perfect_emerald/perfect_emerald/init", { BALOR, "perfect_emerald" }},
	{ "Conversations/Bank/Balor/Museum Lines/perfect_ruby/perfect_ruby/init", { BALOR, "perfect_ruby" }},
	{ "Conversations/Bank/Balor/Museum Lines/perfect_sapphire/perfect_sapphire/init", { BALOR, "perfect_sapphire" }},
	{ "Conversations/Bank/Balor/Museum Lines/rusted_treasure_chest/rusted_treasure_chest/init", { BALOR, "rusted_treasure_chest" }},
	{ "Conversations/Bank/Balor/Museum Lines/sapphire_betta/sapphire_betta/init", { BALOR, "sapphire_betta" }},
	{ "Conversations/Tutorial Dialogue/misc_quest_lines/gossip_for_elsie_juniper/7", { BALOR, "ore_ruby" }},
	// Caldarus
	{ "Conversations/Bank/Caldarus/Relationship Lines/post_8h_lines_romantic/caldarus_post_8h_romantic_4/init", { CALDARUS, "mont_blanc" }},
	{ "Conversations/Bank/Caldarus/Relationship Lines/post_8h_lines_best_friend/caldarus_post_8h_best_friend_6/1", { CALDARUS, "spirit_mushroom_tea" }},
	{ "Conversations/Bank/Caldarus/Relationship Lines/post_8h_lines_romantic/caldarus_post_8h_romantic_8/1", { CALDARUS, "breath_of_fire" }},
	{ "Conversations/Bank/Caldarus/Statue Lines/general_lines/general_lines_29/1", { CALDARUS, "beet_soup" }},
	{ "Conversations/Bank/Caldarus/Statue Lines/general_lines/general_lines_30/1", { CALDARUS, "mont_blanc" }},
	{ "Conversations/Bank/Caldarus/Banked Lines/items/spirit_mushroom_0/1", { CALDARUS, "spirit_mushroom_tea" }},
	{ "Conversations/Bank/Caldarus/Gift Lines/gift_lines/fried_rice/init", { CALDARUS, "fried_rice" }},
	{ "Conversations/Bank/Caldarus/Gift Lines/gift_lines/mont_blanc/init", { CALDARUS, "mont_blanc" }},
	{ "Conversations/Bank/Caldarus/Gift Lines/gift_lines/sushi_platter/init", { CALDARUS, "sushi_platter" }},
	{ "Conversations/Bank/Caldarus/Relationship Lines/post_8h_lines_romantic/caldarus_post_8h_romantic_4/1", { CALDARUS, "mont_blanc" }},
	{ "Conversations/fetch_quests_follow_ups/request_for_mont_blanc_follow_up_caldarus/init", { CALDARUS, "mont_blanc" }},
	{ "Conversations/fetch_quests_follow_ups/request_for_pumpkin_pie_follow_up_eiland/1", { CALDARUS, "pumpkin_pie" }},
	// Celine
	{ "Conversations/Bank/Celine/Relationship Lines/Relationship/post_8h_lines_best_friend/celine_post_8h_best_friend_7/init", { CELINE, "marigold" }},
	{ "Conversations/Bank/Celine/Relationship Lines/Relationship/post_8h_lines_best_friend/celine_post_8h_best_friend_6/init", { CELINE, "lilac" }},
	{ "Conversations/Bank/Celine/Relationship Lines/Relationship/post_8h_lines_best_friend/celine_post_8h_best_friend_9/init", { CELINE, "poinsettia" }},
	{ "Conversations/Bank/Celine/Date Lines/deep_woods_picnic/deep_woods_picnic_accept_2/3", { CELINE, "temple_flower", "marigold" }},
	{ "Conversations/Bank/Celine/Relationship Lines/Relationship/post_8h_lines_best_friend/celine_post_8h_best_friend_8/init", { CELINE, "heather" }},
	{ "Conversations/Bank/Celine/Banked Lines/daisies/daisies/init", { CELINE, "daisy" }},
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/chrysanthemum/init", { CELINE, "chrysanthemum" }},
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/plum_blossom/init", { CELINE, "plum_blossom" }},
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/rose/init", { CELINE, "rose" }},
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/temple_flower_0/1", { CELINE, "temple_flower" }},
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/temple_flower_1/init", { CELINE, "temple_flower" }},
	{ "Conversations/Bank/Celine/Market Lines/market_darcy_3/market_darcy_3/init", { CELINE, "rose_tea" }},
	{ "Conversations/Group Conversations/Celine_Reina/foraging/foraging/1", { CELINE, "frost_lily" }},
	// Darcy
	{ "Conversations/General Dialogue/birthday_anticipation/darcy_birthday_anticipation_dell/1", { DARCY, "apple" }},
	{ "Conversations/General Dialogue/birthday_anticipation/darcy_birthday_anticipation_luc/1", { DARCY, "apple" }},
	{ "Conversations/General Dialogue/birthday_anticipation/darcy_birthday_anticipation_maple/1", { DARCY, "apple" }},
	// Dell
	{ "Conversations/Bank/Dell/Museum Lines/alda_bronze_sword/alda_bronze_sword/init", { DELL, "alda_bronze_sword" }},
	{ "Conversations/Bank/Dell/Museum Lines/aldarian_sword/aldarian_sword/init", { DELL, "aldarian_sword" }},
	{ "Conversations/Bank/Dell/Museum Lines/caldosian_sword/caldosian_sword/init", { DELL, "caldosian_sword" }},
	{ "Conversations/Bank/Dell/Museum Lines/lightning_dragonfly/lightning_dragonfly/init", { DELL, "lightning_dragonfly" }},
	{ "Conversations/General Dialogue/birthday_anticipation/dell_birthday_anticipation_holt/init", { DELL, "chocolate" }},
	{ "Conversations/General Dialogue/birthday_anticipation/dell_birthday_anticipation_nora/init", { DELL, "chocolate" }},
	{ "Conversations/General Dialogue/birthday_anticipation/dell_birthday_anticipation_celine/init", { DELL, "chocolate" }},
	{ "Conversations/Tutorial Dialogue/misc_quest_lines/gossip_for_elsie_dell/12", { DELL, "chocolate" }},
	{ "Conversations/Tutorial Dialogue/misc_quest_lines/gossip_for_elsie_turn_in/2", { DELL, "chocolate" }},
	// Eiland
	{ "Conversations/Bank/Eiland/Relationship Lines/Relationship/post_8h_lines_romantic/eiland_post_8h_romantic_4/2", { EILAND, "golden_cheesecake" }},
	{ "Conversations/Bank/Eiland/Banked Lines/pumpkin_pie/pumpkin_pie/init", { EILAND, "pumpkin_pie" }},
	{ "Conversations/Bank/Eiland/Market Lines/market_darcy_1/market_darcy_1/init", { EILAND, "roasted_rice_tea" }},
	{ "Conversations/Bank/Eiland/Banked Lines/breakfast/breakfast_2/2", { EILAND, "strawberry_shortcake" }},
	{ "Conversations/fetch_quests_follow_ups/request_for_pumpkin_pie_follow_up_eiland/1", { EILAND, "pumpkin_pie" }},
	// Elsie
	{ "Conversations/Bank/Elsie/Market Lines/market_darcy_1/market_darcy_1/init", { ELSIE, "jasmine_tea" }},
	{ "Conversations/Bank/Elsie/Market Lines/market_darcy_2/market_darcy_2/init", { ELSIE, "jasmine_tea" }},
	{ "Conversations/Bank/Elsie/Market Lines/market_darcy_3/market_darcy_3/init", { ELSIE, "jasmine_tea" }},
	{ "Conversations/Bank/Elsie/Museum Lines/crystal_rose/crystal_rose/init", { ELSIE, "crystal_rose" }},
	{ "Conversations/Bank/Elsie/Museum Lines/feather_pendant/feather_pendant/init", { ELSIE, "alda_feather_pendant" }},
	// Errol
	{ "Conversations/Bank/Errol/Market Lines/market_darcy_1/market_darcy_1/init", { ERROL, "latte" }},
	{ "Conversations/Bank/Errol/Market Lines/market_darcy_3/market_darcy_3/init", { ERROL, "latte" }},
	{ "Conversations/Bank/Errol/Market Lines/market_darcy_4/market_darcy_4/init", { ERROL, "mocha" }},
	// Hayden
	{ "Cutscenes/Heart Events/Hayden/hayden_eight_hearts/hayden_eight_hearts/1", { HAYDEN, "vegetable_quiche" }},
	{ "Conversations/Bank/Hayden/Banked Lines/hot_toddy/hot_toddy/init", { HAYDEN, "cup_of_tea" }},
	{ "Conversations/Bank/Hayden/Market Lines/market_darcy_1/market_darcy_1/init", { HAYDEN, "coffee", "cup_of_tea"}},
	{ "Conversations/Bank/Hayden/Market Lines/market_darcy_2/market_darcy_2/init", { HAYDEN, "coffee" }},
	{ "Conversations/Bank/Hayden/Museum Lines/store_horse/stone_horse/init", { HAYDEN, "stone_horse" }},
	{ "Conversations/Festival Lines/Hayden/shooting_star/shooting_star_romantic_follow_up_A/1", { HAYDEN, "apple_pie" }},
	// Hemlock
	{ "Conversations/Bank/Hemlock/Market Lines/market_wheedle_1/market_wheedle_1/1", { HEMLOCK, "white_wine" }},
	{ "Conversations/Bank/Hemlock/Gift Lines/gift_lines/crayfish_etouffee/init", { HEMLOCK, "crayfish_etouffee" }},
	{ "Conversations/Bank/Josephine/Gift Lines/gift_lines/crayfish_etouffee/init", { HEMLOCK, "crayfish_etouffee" }},
	{ "Conversations/Bank/Hemlock/Market Lines/market_darcy_1/market_darcy_1/init", { HEMLOCK, "coffee", "beer"}},
	{ "Conversations/Bank/Hemlock/Market Lines/market_darcy_2/market_darcy_2/init", { HEMLOCK, "coffee" }},
	{ "Conversations/Bank/Hemlock/Market Lines/market_darcy_4/market_darcy_4/init", { HEMLOCK, "coffee" }},
	{ "Conversations/Bank/Josephine/Museum Lines/caldosian_drinking_horn/caldosian_drinking_horn/init", { HEMLOCK, "caldosian_drinking_horn" }},
	// Henrietta
	{ "Conversations/General Dialogue/season_transition/early_summer_henrietta/init", { HENRIETTA, "corn" }},
	{ "Conversations/Threads/Hayden/henrietta_tales/hayden_henrietta_tales_3/2", { HENRIETTA, "corn" }},
	{ "Conversations/fetch_quests_follow_ups/request_for_ultimate_small_animal_feed_follow_up_henrietta/init", { HENRIETTA, "ultimate_small_animal_feed" }},
	// Holt
	{ "Conversations/Bank/Holt/Museum Lines/narrows_moss/narrows_moss/init", { HOLT, "narrows_moss" }},
	{ "Conversations/General Dialogue/birthday_anticipation/holt_birthday_anticipation_dell/1", { HOLT, "hard_wood" }},
	{ "Conversations/General Dialogue/birthday_anticipation/holt_birthday_anticipation_celine/1", { HOLT, "hard_wood" }},
	// Josephine
	{ "Conversations/Bank/Hemlock/Gift Lines/gift_lines/crayfish_etouffee/init", { JOSEPHINE, "crayfish_etouffee" }},
	{ "Conversations/Bank/Josephine/Gift Lines/gift_lines/crayfish_etouffee/init", { JOSEPHINE, "crayfish_etouffee" }},
	{ "Conversations/Bank/Josephine/Banked Lines/quiche/quiche/init", { JOSEPHINE, "quiche" }},
	{ "Conversations/Bank/Josephine/Market Lines/market_darcy_1/market_darcy_1/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea"}},
	{ "Conversations/Bank/Josephine/Market Lines/market_darcy_2/market_darcy_2/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea" }},
	{ "Conversations/Bank/Josephine/Market Lines/market_darcy_4/market_darcy_4/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea" }},
	{ "Conversations/Bank/Hemlock/Market Lines/market_darcy_2/market_darcy_2/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea" }},
	// Juniper
	{ "Conversations/Bank/Juniper/Banked Lines/breakfast/breakfast/1", { JUNIPER, "latte" }},
	{ "Conversations/Bank/Juniper/Banked Lines/foraging/foraging_3/1", { JUNIPER, "nettle" }},
	{ "Conversations/Bank/Juniper/Banked Lines/foraging/foraging_3/init", { JUNIPER, "nettle" }},
	{ "Conversations/Bank/Juniper/Banked Lines/middlemist_red/middlemist_red/init", { JUNIPER, "middlemist" }},
	{ "Conversations/Bank/Juniper/Banked Lines/new_potion/new_potion/init", { JUNIPER, "newt" }},
	{ "Conversations/Bank/Juniper/Banked Lines/orb_viewing/orb_viewing_2/1", { JUNIPER, "water_chestnut_fritters" }},
	{ "Conversations/Bank/Juniper/Gift Lines/gift_lines/ancient_royal_scepter/init", { JUNIPER, "ancient_royal_scepter" }},
	{ "Conversations/Bank/Juniper/Gift Lines/gift_lines/crystal_rose/init", { JUNIPER, "crystal_rose" }},
	{ "Conversations/Bank/Juniper/Market Lines/market_darcy_1/market_darcy_1/init", { JUNIPER, "latte" }},
	{ "Conversations/Bank/Juniper/Museum Lines/ancient_royal_scepter/ancient_royal_scepter/init", { JUNIPER, "ancient_royal_scepter" }},
	{ "Conversations/Bank/Valen/Market Lines/market_darcy_4/market_darcy_4/init", { JUNIPER, "latte" }},
	{ "Conversations/Tutorial Dialogue/misc_quest_lines/gossip_for_elsie_balor/2", { JUNIPER, "newt" }},
	{ "Conversations/Tutorial Dialogue/misc_quest_lines/gossip_for_elsie_balor/4", { JUNIPER, "newt" }},
	{ "Conversations/Tutorial Dialogue/misc_quest_lines/gossip_for_elsie_turn_in/7", { JUNIPER, "newt" }},
	{ "Conversations/General Dialogue/ari_birthday/ari_birthday_juniper_low_hearts/2", { JUNIPER, "newt" }},
	{ "Conversations/Friday Night at the Inn/Post_FNATI/post_fnati/juniper_post_fnati_2/init", { JUNIPER, "red_wine" }},
	{ "Conversations/Bank/Juniper/Banked Lines/shopping/shopping_8/1", { JUNIPER, "monster_powder" }},
	// Landen
	{ "Conversations/Bank/Landen/Banked Lines/inn_special/inn_special/init", { LANDEN, "vegetable_pot_pie" }},
	{ "Cutscenes/Heart Events/Ryis/ryis_six_hearts/ryis_six_hearts/17", { LANDEN, "coconut_cream_pie" }},
	{ "Cutscenes/Story Events/Town Repair/upgrade_the_carpenters_shop/upgrade_the_carpenters_shop_pt_2_follow_up_landen/1", { LANDEN, "vegetable_pot_pie" }},
	// Luc
	{ "Conversations/fetch_quests_follow_ups/request_for_cheese_follow_up_hemlock/init", { LUC, "cheese" }},
	{ "Conversations/Bank/Luc/Gift Lines/gift_lines/grilled_cheese/init", { LUC, "grilled_cheese" }},
	{ "Conversations/Bank/Luc/Market Lines/market_darcy_1/market_darcy_1/init", { LUC, "hot_cocoa" }},
	{ "Conversations/Bank/Luc/Museum Lines/amber_trapped_insect/amber_trapped_insect/init", { LUC, "amber_trapped_insect" }},
	{ "Conversations/Bank/Luc/Museum Lines/cave_shrimp/cave_shrimp/init", { LUC, "cave_shrimp" }},
	{ "Conversations/Bank/Luc/Museum Lines/copper_nugget_beetle/copper_nugget_beetle/init", { LUC, "copper_beetle" }},
	{ "Conversations/Bank/Luc/Museum Lines/fairy_bee/fairy_bee/init", { LUC, "fairy_bee" }},
	{ "Conversations/Bank/Luc/Museum Lines/puddle_spider/puddle_spider/init", { LUC, "puddle_spider" }},
	{ "Conversations/Bank/Luc/Museum Lines/sea_scarab/sea_scarab/init", { LUC, "sea_scarab" }},
	{ "Conversations/Bank/Luc/Museum Lines/snowball_beetle/snowball_beetle/init", { LUC, "snowball_beetle" }},
	{ "Conversations/Bank/Luc/Museum Lines/strobe_firefly/strobe_firefly/init", { LUC, "strobe_firefly" }},
	{ "Conversations/Bank/Reina/Banked Lines/luc_and_maple_cheese/luc_and_maple_cheese/init", { LUC, "cheese" }},
	{ "Conversations/Bank/Luc/Museum Lines/bumblebee/bumblebee/init", { LUC, "bumblebee" }},
	// Louis
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/crystal/init", { LOUIS, "crystal" }},
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/lilac/init", { LOUIS, "lilac" }},
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/red_wine/init", { LOUIS, "red_wine" }},
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/white_wine/init", { LOUIS, "white_wine" }},
	// Maple
	{ "Conversations/Bank/Maple/Gift Lines/gift_lines/berries_and_cream/init", { MAPLE, "berries_and_cream" }},
	{ "Conversations/Bank/Maple/Market Lines/market_darcy_2/market_darcy_2/init", { MAPLE, "hot_cocoa" }},
	{ "Conversations/Bank/Maple/Market Lines/market_darcy_4/market_darcy_4/init", { MAPLE, "hot_cocoa" }},
	{ "Conversations/Bank/Maple/Museum Lines/stone_shell/stone_shell/init", { MAPLE, "stone_shell" }},
	{ "Conversations/Bank/Reina/Banked Lines/luc_and_maple_cheese/luc_and_maple_cheese/init", { MAPLE, "cheese" }},
	{ "Conversations/General Dialogue/birthday_anticipation/maple_self_birthday_anticipation/init", { MAPLE, "lost_crown_of_aldaria" }},
	{ "Conversations/Friday Night at the Inn/Post_FNATI/post_fnati/maple_post_fnati_1/2", { MAPLE, "hot_cocoa" }},
	// March
	{ "Conversations/Bank/March/Banked Lines/beer/beer/init", { MARCH, "beer" }},
	{ "Conversations/Bank/March/Banked Lines/chocolate/chocolate/init", { MARCH, "chocolate" }},
	{ "Conversations/Bank/March/Banked Lines/cold_beer_long_day/cold_beer_long_day/init", { MARCH, "beer" }},
	{ "Conversations/Bank/March/Gift Lines/gift_lines/gold_ingot/init", { MARCH, "gold_ingot" }},
	{ "Conversations/Bank/March/Gift Lines/gift_lines/mocha/init", { MARCH, "mocha" }},
	{ "Conversations/Bank/March/Gift Lines/gift_lines/red_snapper_sushi/init", { MARCH, "red_snapper_sushi" }},
	{ "Conversations/Bank/March/Market Lines/market_darcy_1/market_darcy_1/init", { MARCH, "hot_cocoa" }},
	{ "Conversations/Bank/March/Market Lines/market_darcy_3/market_darcy_3/init", { MARCH, "hot_cocoa" }},
	{ "Conversations/Bank/March/Museum Lines/meteorite/meteorite/init", { MARCH, "meteorite" }},
	{ "Conversations/Bank/March/Museum Lines/perfect_copper_ore/perfect_copper_ore/init", { MARCH, "perfect_copper_ore" }},
	{ "Conversations/Bank/March/Museum Lines/perfect_iron_ore/perfect_iron_ore/init", { MARCH, "perfect_iron_ore" }},
	{ "Conversations/Bank/March/Museum Lines/perfect_silver_ore/perfect_silver_ore/init", { MARCH, "perfect_silver_ore" }},
	{ "Conversations/Festival Lines/March/animal_festival/animal_festival_0/init", { MARCH, "hot_cocoa" }},
	{ "Conversations/Group Conversations/Elsie_March_Olric_Ryis/breakfast/breakfast_3/1", { MARCH, "coffee" }},
	// Merri
	{ "Conversations/Bank/Merri/Gift Lines/gift_lines/glass/init", { MERRI, "glass" }},
	{ "Conversations/Bank/Merri/Gift Lines/gift_lines/hard_wood/init", { MERRI, "hard_wood" }},
	{ "Conversations/Bank/Merri/Gift Lines/gift_lines/latte/init", { MERRI, "latte" }},
	// Nora
	{ "Conversations/Bank/Nora/Gift Lines/gift_lines/ancient_gold_coin/init", { NORA, "ancient_gold_coin" }},
	{ "Conversations/Bank/Nora/Gift Lines/gift_lines/coffee/init", { NORA, "coffee" }},
	{ "Conversations/Bank/Nora/Market Lines/market_darcy_4/market_darcy_4/init", { NORA, "latte" }},
	{ "Conversations/Bank/Nora/Museum Lines/ancient_gold_coin/ancient_gold_coin/init", { NORA, "ancient_gold_coin" }},
	{ "Conversations/General Dialogue/birthday_anticipation/nora_birthday_anticipation_dell/1", { NORA, "baked_potato" }},
	{ "Conversations/General Dialogue/birthday_anticipation/nora_birthday_anticipation_celine/1", { NORA, "baked_potato" }},
	// Olric
	{ "Conversations/Bank/March/Banked Lines/hardboiled_egg/hardboiled_egg/init", { OLRIC, "hard_boiled_egg" }},
	{ "Conversations/Bank/Olric/Gift Lines/gift_lines/hard_boiled_egg/init", { OLRIC, "hard_boiled_egg" }},
	{ "Conversations/Group Conversations/Elsie_March_Olric_Ryis/breakfast/breakfast_3/3", { OLRIC, "hard_boiled_egg" }},
	{ "Conversations/Bank/March/Banked Lines/olric_stone/olric_stone/init", { OLRIC, "ore_stone" }},
	{ "Conversations/Bank/Olric/Market Lines/market_wheedle_1/market_wheedle_1/init", { OLRIC, "ore_stone" }},
	// Reina
	{ "Conversations/Bank/Reina/Relationship Lines/Relationship/post_8h_lines_romantic/reina_post_8h_romantic_2/1", { REINA, "cauliflower_curry" }},
	{ "Conversations/Bank/Reina/Relationship Lines/Relationship/post_8h_lines_romantic/reina_post_8h_romantic_8/1", { REINA, "turnip_and_potato_gratin" }},
	{ "Conversations/Bank/Reina/Banked Lines/garlic/garlic/init", { REINA, "garlic" }},
	{ "Conversations/Bank/Reina/Banked Lines/garlic/garlic_2/init", { REINA, "garlic" }},
	{ "Conversations/Bank/Reina/Banked Lines/general_store_shopping/general_store_shopping_3/init", { REINA, "garlic" }},
	{ "Conversations/Bank/Reina/Gift Lines/gift_lines/daffodil/init", { REINA, "daffodil" }},
	{ "Conversations/Bank/Reina/Market Lines/market_darcy_1/market_darcy_1/init", { REINA, "coffee", "iced_coffee"}},
	{ "Conversations/Bank/Reina/Market Lines/market_darcy_4/market_darcy_4/init", { REINA, "coffee" }},
	{ "Cutscenes/Heart Events/Reina/reina_six_hearts/reina_six_hearts/37", { REINA, "grilled_cheese" }},
	{ "Cutscenes/Heart Events/Reina/reina_two_hearts/reina_two_hearts/1", { REINA, "wildberry_pie" }},
	{ "Conversations/General Dialogue/ari_birthday/ari_birthday_reina_romantic/1", { REINA, "wildberry_pie" }},
	{ "Conversations/Bank/Reina/Banked Lines/seridia_follow_up/reina_0_follow_up_reina/1", { REINA, "crystal_berry_pie" }},
	// Ryis
	{ "Conversations/Bank/Ryis/Relationship Lines/Relationship/post_8h_lines_romantic/ryis_post_8h_romantic_10/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Ryis/Relationship Lines/Relationship/post_8h_lines_best_friend/ryis_post_8h_best_friend_12/4", { RYIS, "golden_horse_hair" }},
	{ "Conversations/Bank/Ryis/Relationship Lines/Relationship/post_8h_lines_best_friend/ryis_post_8h_best_friend_2/1", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Ryis/Banked Lines/bath_smells_like_lavender/bath_smells_like_lavender/init", { RYIS, "lilac" }},
	{ "Conversations/Bank/Ryis/Gift Lines/gift_lines/hard_wood/1", { RYIS, "hard_wood" }},
	{ "Conversations/Bank/Ryis/Market Lines/market_darcy_1/market_darcy_1/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Ryis/Market Lines/market_darcy_2/market_darcy_2/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Ryis/Market Lines/market_darcy_3/market_darcy_3/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Landen/Banked Lines/veggie_sub/veggie_sub/init", { RYIS, "veggie_sub_sandwich" }},
	{ "Conversations/Bank/Ryis/Banked Lines/shopping_for_landen/shopping_for_landen_2/init", { RYIS, "bread" }},
	{ "Conversations/Group Conversations/Elsie_March_Olric_Ryis/breakfast/breakfast_3/2", { RYIS, "bread" }},
	// Seridia
	{ "Conversations/Group Conversations/Juniper_Seridia/disciple/disciple_5/init", { SERIDIA, "night_queen" }},
	{ "Conversations/fetch_quests_follow_ups/request_for_monster_mash_follow_up_seridia/init", { SERIDIA, "monster_mash" }},
	// Taliferro
	{ "Conversations/Bank/Taliferro/Banked Lines/challenge_completed_lines/incredibly_hot_pot_terithia/init", { TALIFERRO, "incredibly_hot_pot" }},
	{ "Conversations/Bank/Taliferro/Banked Lines/challenge_completed_lines/veggie_sub_sandwich_holt/init", { TALIFERRO, "veggie_sub_sandwich" }},
	// Terithia
	{ "Conversations/Bank/Terithia/Gift Lines/gift_lines/fish_stew/init", { TERITHIA, "fish_stew" }},
	{ "Conversations/Bank/Valen/Banked Lines/terithia_fish_jerky/terithia_fish_jerky/init", { TERITHIA, "canned_sardines" }},
	{ "Conversations/General Dialogue/birthday_anticipation/terithia_birthday_anticipation_errol/1", { TERITHIA, "dried_squid" }},
	{ "Conversations/General Dialogue/birthday_anticipation/terithia_birthday_anticipation_landen/1", { TERITHIA, "dried_squid" }},
	// Valen
	{ "Conversations/Bank/Valen/Banked Lines/rainy_stock_up/rainy_stock_up/init", { VALEN, "honey" }},
	{ "Conversations/Bank/Valen/Banked Lines/winter_honey/winter_honey/init", { VALEN, "honey" }},
	{ "Conversations/Bank/Valen/Market Lines/market_darcy_1/market_darcy_1/init", { VALEN, "coffee" }},
	{ "Conversations/Bank/Valen/Market Lines/market_darcy_2/market_darcy_2/init", { VALEN, "coffee" }},
	{ "Conversations/Bank/Valen/Market Lines/market_darcy_3/market_darcy_3/init", { VALEN, "coffee" }},
	{ "Conversations/Bank/Valen/Banked Lines/breakfast/breakfast_4/init", { VALEN, "coffee" }},
	{ "Conversations/General Dialogue/ari_birthday/ari_birthday_valen_low_hearts/1", { VALEN, "coffee" }},
	{ "Conversations/Festival Lines/Valen/shooting_star/shooting_star_romantic_follow_up_A/1", { VALEN, "beet_soup" }},
	{ "Conversations/General Dialogue/birthday_anticipation/valen_birthday_anticipation_hayden/1", { VALEN, "vegetable_soup" }},
	{ "Conversations/Group Conversations/Adeline_Valen/salmon_benefits/salmon_benefits/init", { VALEN, "pan_fried_salmon" }},
	{ "Conversations/Group Conversations/Celine_Valen/deep_woods_nettle/deep_woods_nettle/1", { VALEN, "nettle" }},
	{ "Conversations/Bank/Reina/Banked Lines/general_store_shopping/general_store_shopping_3/init", { VALEN, "garlic" }},
	{ "Conversations/General Dialogue/ari_birthday/ari_birthday_valen_high_hearts/1", { VALEN, "green_tea" }},
	// Vera
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/coconut_milk/init", { VERA, "coconut_milk" }},
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/cranberry_juice/init", { VERA, "cranberry_juice" }},
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/gazpacho/init", { VERA, "gazpacho" }},
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/orange_juice/init", { VERA, "orange_juice" }},
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/pomegranate/init", { VERA, "pomegranate" }},
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/summer_salad/init", { VERA, "summer_salad" }},
	// MOD: Talkative Celine
	{ "conversations/talkative_celine/frost lily/init", { CELINE, "frost_lily" }},
	{ "conversations/talkative_celine/plum blossom/init", { ADELINE, "plum_blossom" }},
	{ "conversations/talkative_celine/plum blossom/init", { CELINE, "plum_blossom" }},
	{ "conversations/talkative_celine/jasmine/init", { CELINE, "jasmine" }},
	{ "conversations/talkative_celine/jasmine/init", { JOSEPHINE, "jasmine" }},
	{ "conversations/talkative_celine/poinsettia/init", { CELINE, "poinsettia" }},
	{ "conversations/talkative_celine/poinsettia/init", { JUNIPER, "poinsettia" }},
	{ "conversations/talkative_celine/crocus/init", { CELINE, "crocus" }},
	{ "conversations/talkative_celine/snapdragon/init", { CELINE, "snapdragon" }},
	{ "conversations/talkative_celine/middlemist/init", { CELINE, "middlemist" }},
	{ "conversations/talkative_celine/snowdrop anemone/1", { CELINE, "snowdrop_anemone" }},
	{ "conversations/talkative_celine/lilac/init", { CELINE, "lilac" }},
	{ "conversations/talkative_celine/daffodil/init", { CELINE, "daffodil" }},
	{ "conversations/talkative_celine/daffodil/1", { REINA, "daffodil" }},
	{ "conversations/talkative_celine/dandelion/init", { CELINE, "dandelion" }},
	{ "conversations/talkative_celine/dandelion/init", { VALEN, "dandelion" }},
	{ "conversations/talkative_celine/tulip/init", { CELINE, "tulip" }},
	{ "conversations/talkative_celine/spring galette/init", { ADELINE, "spring_galette" }},
	{ "conversations/talkative_celine/rose tea/init", { CELINE, "rose_tea" }},
	{ "conversations/talkative_celine/spring salad/init", { CELINE, "spring_salad" }},
	{ "conversations/talkative_celine/hydrangea/init", { CELINE, "hydrangea" }},
	{ "conversations/talkative_celine/rose/init", { CELINE, "rose" }},
	{ "conversations/talkative_celine/catmint/init", { CELINE, "catmint" }},
	{ "conversations/talkative_celine/cosmos/init", { CELINE, "cosmos" }},
	{ "conversations/talkative_celine/cosmos/1", { ELSIE, "cosmos" }},
	{ "conversations/talkative_celine/marigold/init", { CELINE, "marigold" }},
	{ "conversations/talkative_celine/daisy/init", { CELINE, "daisy" }},
	{ "conversations/talkative_celine/daisy/init", { MAPLE, "daisy" }},
	{ "conversations/talkative_celine/sunflower/init", { CELINE, "sunflower" }},
	{ "conversations/talkative_celine/sunflower/init", { NORA, "toasted_sunflower_seeds" }},
	{ "conversations/talkative_celine/iris/init", { CELINE, "iris" }},
	{ "conversations/talkative_celine/iris/init", { ELSIE, "iris" }},
	{ "conversations/talkative_celine/celosia/init", { CELINE, "celosia" }},
	{ "conversations/talkative_celine/celosia/init", { LOUIS, "celosia" }},
	{ "conversations/talkative_celine/chrysanthemum/init", { CELINE, "chrysanthemum" }},
	{ "conversations/talkative_celine/heather/init", { CELINE, "heather" }},
	{ "conversations/talkative_celine/viola/init", { CELINE, "viola" }},
	{ "conversations/talkative_celine/crystal rose/init", { CELINE, "crystal_rose" }},
};

// ----- Config -----

struct ThePerfectGiftConfig
{
	bool unlock_all_gift_preferences            = DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES;
	bool show_gift_preferences_on_item_tooltips = DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS;
	bool disable_daily_talk_limit               = DEFAULT_DISABLE_DAILY_TALK_LIMIT;
	bool disable_daily_gift_limit               = DEFAULT_DISABLE_DAILY_GIFT_LIMIT;
};

void to_json(json& j, const ThePerfectGiftConfig& c)
{
	j = json{
		{ UNLOCK_ALL_GIFT_PREFERENCES_KEY,            c.unlock_all_gift_preferences            },
		{ SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, c.show_gift_preferences_on_item_tooltips },
		{ DISABLE_DAILY_TALK_LIMIT_KEY,               c.disable_daily_talk_limit               },
		{ DISABLE_DAILY_GIFT_LIMIT_KEY,               c.disable_daily_gift_limit               },
	};
}

void from_json(const json& j, ThePerfectGiftConfig& c)
{
	c.unlock_all_gift_preferences            = MMAPI::Config::GetValue<bool>(j, UNLOCK_ALL_GIFT_PREFERENCES_KEY,            DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES);
	c.show_gift_preferences_on_item_tooltips = MMAPI::Config::GetValue<bool>(j, SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS);
	c.disable_daily_talk_limit               = MMAPI::Config::GetValue<bool>(j, DISABLE_DAILY_TALK_LIMIT_KEY,               DEFAULT_DISABLE_DAILY_TALK_LIMIT);
	c.disable_daily_gift_limit               = MMAPI::Config::GetValue<bool>(j, DISABLE_DAILY_GIFT_LIMIT_KEY,               DEFAULT_DISABLE_DAILY_GIFT_LIMIT);
}

// ----- State -----

static ThePerfectGiftConfig config{};
static bool startup_loaded     = false;
static bool crafting_menu_open = false;

// Cached item lookup, built once at title-screen setup (Item::ForEachItem + Item::GetInternalName).
static std::map<std::string, int> item_name_to_id_cache;

// Gifts queued for unlocking on next NPC interaction. Keyed by lowercase NPC internal name.
static std::map<std::string, std::vector<std::string>> gifts_to_unlock;

// Notification placeholder substitution context. Set immediately before CreateNotification,
// consumed by the AfterLocalizedString hook when it sees the notification key.
static std::string gift_preference_npc_name;
static std::string gift_preference_internal_item_name;

// ----- Helpers -----

void LoadOrCreateConfigFile()
{
	try
	{
		auto path = MMAPI::Config::GetConfigPath(MOD_NAME);
		bool existed = std::filesystem::exists(path);
		json j = MMAPI::Config::Load(path);

		if (!existed)
			MMAPI::Log::Warn("Configuration file was not found. Creating file: %s", path.string().c_str());

		if (j.empty())
		{
			if (existed)
				MMAPI::Log::Error("No readable values in configuration file: %s!", path.string().c_str());
			config = ThePerfectGiftConfig{};
		}
		else
		{
			config = j.get<ThePerfectGiftConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = ThePerfectGiftConfig{};
	}
}

void BuildItemNameCache()
{
	item_name_to_id_cache.clear();
	MMAPI::Item::ForEachItem([](int id) {
		YYTK::RValue name = MMAPI::Item::GetInternalName(id);
		if (name.m_Kind == YYTK::VALUE_STRING)
			item_name_to_id_cache[name.ToString()] = id;
	});
}

// Capitalizes the first character of `s` (in-place) — used for "<NPC>" placeholder substitution.
std::string CapitalizeFirst(std::string s)
{
	if (!s.empty())
		s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
	return s;
}

void DisplayNotification(CInstance* self, const std::string& localization_key, const std::string& npc_name, const std::string& item_internal_name)
{
	// Stash the placeholder values where AfterLocalizedString can find them, then trigger the
	// notification — the localizer hook will fire synchronously and substitute <ITEM> / <NPC>.
	gift_preference_npc_name           = npc_name;
	gift_preference_internal_item_name = item_internal_name;
	MMAPI::Game::CreateNotification(false, localization_key);
	gift_preference_npc_name.clear();
	gift_preference_internal_item_name.clear();
}

void UnlockGiftsForNpc(CInstance* npc, const std::string& npc_name, bool silent)
{
	auto it = gifts_to_unlock.find(npc_name);
	if (it == gifts_to_unlock.end() || it->second.empty()) return;

	for (const std::string& item_internal_name : it->second)
	{
		auto cache_it = item_name_to_id_cache.find(item_internal_name);
		if (cache_it == item_name_to_id_cache.end()) continue;
		int item_id = cache_it->second;

		if (MMAPI::NPC::KnowsGiftPreference(npc, item_id))
			continue;

		MMAPI::NPC::LearnGiftPreference(npc, item_id);

		if (!silent)
			DisplayNotification(npc, GIFT_PREFERENCE_UNLOCKED_LOCALIZATION_KEY, npc_name, item_internal_name);
	}

	if (!config.unlock_all_gift_preferences)
		it->second.clear();
}

void QueueAllGiftPreferences()
{
	for (const std::string& npc_name : ACTIVE_NPC_LIST)
	{
		auto npc_id = MMAPI::NPC::TryFromInternalName(npc_name);
		if (!npc_id) continue;

		YYTK::RValue liked = MMAPI::NPC::GetLikedGifts(*npc_id);
		YYTK::RValue loved = MMAPI::NPC::GetLovedGifts(*npc_id);

		auto append_buffer = [&npc_name](YYTK::RValue buf) {
			if (buf.m_Kind == YYTK::VALUE_UNDEFINED) return;
			size_t count = 0;
			MMAPI::Internal::module_interface->GetArraySize(buf, count);
			for (size_t i = 0; i < count; i++)
			{
				YYTK::RValue* entry = nullptr;
				MMAPI::Internal::module_interface->GetArrayEntry(buf, i, entry);
				if (!entry || !MMAPI::Engine::IsNumeric(*entry)) continue;
				YYTK::RValue name = MMAPI::Item::GetInternalName(static_cast<int>(entry->ToInt64()));
				if (name.m_Kind == YYTK::VALUE_STRING)
					gifts_to_unlock[npc_name].push_back(name.ToString());
			}
		};

		append_buffer(liked);
		append_buffer(loved);
	}
}

// Parses a custom-mod dialogue key of the form:
//   conversations/mods/<modname>/gift_hint_<n>/<npc>/<item>/[gift_hint_<n>/<npc>/<item>/]...<conversation>
// Each (gift_hint_*, npc, item) triple queues an item for unlock on the named NPC.
void ParseCustomModDialogue(CInstance* self, const std::string& dialogue_key)
{
	std::vector<std::string> tokens;
	std::stringstream ss(dialogue_key);
	std::string token;
	while (std::getline(ss, token, '/'))
		tokens.push_back(token);

	if (tokens.size() < 7 || tokens[0] != "conversations" || tokens[1] != "mods")
	{
		MMAPI::Log::Error("Invalid custom mod dialogue key: %s", dialogue_key.c_str());
		return;
	}

	const size_t triple_token_count = tokens.size() - 4;
	if (triple_token_count % 3 != 0)
	{
		MMAPI::Log::Error("Invalid custom mod dialogue key: %s", dialogue_key.c_str());
		return;
	}

	// Each triple is (gift_hint_<n>, npc, item).
	for (size_t i = 3; i < tokens.size() - 1; i += 3)
	{
		if (tokens[i].rfind("gift_hint_", 0) != 0)
		{
			MMAPI::Log::Error("Invalid custom mod dialogue key: %s", dialogue_key.c_str());
			return;
		}

		const std::string& npc_name  = tokens[i + 1];
		const std::string& item_name = tokens[i + 2];

		auto& list = gifts_to_unlock[npc_name];
		if (std::find(list.begin(), list.end(), item_name) == list.end())
			list.push_back(item_name);

		DisplayNotification(self, GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY, npc_name, item_name);
	}
}

// Sets `me.gift_flag = true` / `me.talk_flag = true` on the live NPC instance to bypass the daily
// per-NPC interaction limit. The flags are normally cleared at end-of-day.
void BypassDailyLimits(CInstance* self)
{
	if (!self) return;
	YYTK::RValue self_rv = self->ToRValue();
	if (!MMAPI::Engine::StructVariableExists(self_rv, "me")) return;
	YYTK::RValue me = self_rv.GetMember("me");

	if (config.disable_daily_gift_limit && MMAPI::Engine::StructVariableExists(me, "gift_flag"))
		MMAPI::Engine::StructVariableSet(me, "gift_flag", true);

	if (config.disable_daily_talk_limit && MMAPI::Engine::StructVariableExists(me, "talk_flag"))
		MMAPI::Engine::StructVariableSet(me, "talk_flag", true);
}

// Derives the NPC's internal name from its object name (e.g. "obj_adeline" → "adeline"). Returns
// empty if the object name doesn't have the expected prefix or is malformed.
std::string NpcNameFromObject(CInstance* self)
{
	if (!self || !self->m_Object || !self->m_Object->m_Name) return {};
	std::string_view name = self->m_Object->m_Name;
	constexpr std::string_view prefix = "obj_";
	if (name.size() <= prefix.size() || name.substr(0, prefix.size()) != prefix) return {};
	return std::string(name.substr(prefix.size()));
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	gifts_to_unlock.clear();
	crafting_menu_open = false;

	if (!startup_loaded)
	{
		LoadOrCreateConfigFile();
		BuildItemNameCache();
		startup_loaded = true;
	}

	if (config.unlock_all_gift_preferences)
		QueueAllGiftPreferences();
}

void OnAfterCraftingMenuOpen()  { crafting_menu_open = true;  }
void OnAfterCraftingMenuClose() { crafting_menu_open = false; }

void OnNpcTick(CInstance* self)
{
	BypassDailyLimits(self);

	if (gifts_to_unlock.empty()) return;

	std::string npc_name = NpcNameFromObject(self);
	if (npc_name.empty()) return;
	if (!gifts_to_unlock.contains(npc_name)) return;

	UnlockGiftsForNpc(self, npc_name, config.unlock_all_gift_preferences);
}

void OnAfterGetDisplayDescription(MMAPI::Item::GetDisplayDescriptionContext& ctx)
{
	if (!config.show_gift_preferences_on_item_tooltips) return;
	if (crafting_menu_open) return;
	if (ctx.GetItemId() < 0) return;

	int item_id = ctx.GetItemId();

	// Collect "Liked By" / "Loved By" NPC lists by scanning each NPC's gift buffers.
	std::vector<std::string> liked_by;
	std::vector<std::string> loved_by;
	MMAPI::NPC::ForEachId([&](MMAPI::NPC::Ids npc) {
		std::string name = CapitalizeFirst(MMAPI::NPC::GetInternalName(npc));
		if (name.empty()) return;
		if (MMAPI::NPC::LikesGift(npc, item_id)) liked_by.push_back(name);
		if (MMAPI::NPC::LovesGift(npc, item_id)) loved_by.push_back(name);
	});

	auto join = [](const std::vector<std::string>& names) {
		std::string out;
		for (size_t i = 0; i < names.size(); i++)
		{
			out += names[i];
			if (i + 1 < names.size()) out += ", ";
		}
		return out;
	};

	std::string description(ctx.GetResolved());
	// Apply in reverse order so the final result is "Loved By: ...\n\nLiked By: ...\n\n<description>"
	// — matches the original mod's prepend ordering.
	if (!liked_by.empty()) description = "Liked By: " + join(liked_by) + "\n\n" + description;
	if (!loved_by.empty()) description = "Loved By: " + join(loved_by) + "\n\n" + description;
	ctx.SetResolved(std::move(description));
}

void OnAfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& ctx)
{
	std::string_view key = ctx.GetKey();
	if (key != GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY && key != GIFT_PREFERENCE_UNLOCKED_LOCALIZATION_KEY) return;
	if (gift_preference_npc_name.empty() && gift_preference_internal_item_name.empty()) return;

	std::string result(ctx.GetResolved());

	// Replace <ITEM> with the localized item name (e.g. "Lemon Pie").
	if (auto pos = result.find(ITEM_PLACEHOLDER_TEXT); pos != std::string::npos)
	{
		std::string localized_item;
		auto cache_it = item_name_to_id_cache.find(gift_preference_internal_item_name);
		if (cache_it != item_name_to_id_cache.end())
		{
			YYTK::RValue rv = MMAPI::Item::GetLocalizedName(cache_it->second);
			if (rv.m_Kind == YYTK::VALUE_STRING)
				localized_item = rv.ToString();
		}
		result.replace(pos, ITEM_PLACEHOLDER_TEXT.length(), localized_item.empty() ? gift_preference_internal_item_name : localized_item);
	}

	// Replace <NPC> with the capitalized NPC name (e.g. "adeline" → "Adeline").
	if (auto pos = result.find(NPC_PLACEHOLDER_TEXT); pos != std::string::npos)
		result.replace(pos, NPC_PLACEHOLDER_TEXT.length(), CapitalizeFirst(gift_preference_npc_name));

	ctx.SetResolved(std::move(result));
}

void OnBeforePlayText(MMAPI::Text::PlayTextContext& ctx)
{
	if (config.unlock_all_gift_preferences) return;  // Everything's already queued; no per-line work.

	std::string_view key = ctx.GetKey();

	// Custom mod gift hint format: "conversations/mods/<modname>/gift_hint_<n>/<npc>/<item>/..."
	std::string lower_key(key);
	std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	if (lower_key.find("conversations/mods") != std::string::npos
	    && lower_key.find("gift_hint_") != std::string::npos)
	{
		ParseCustomModDialogue(nullptr, std::string(key));
		return;
	}

	// Built-in dialogue map lookup (multimap — one key can have multiple gift hints).
	auto range = GIFT_DIALOG_MAP.equal_range(std::string(key));
	if (range.first == range.second) return;

	for (auto it = range.first; it != range.second; ++it)
	{
		const std::vector<std::string>& entry = it->second;
		if (entry.empty()) continue;

		const std::string& npc_name = entry[0];
		for (size_t i = 1; i < entry.size(); i++)
		{
			const std::string& item_name = entry[i];
			auto& list = gifts_to_unlock[npc_name];
			list.push_back(item_name);  // Original allowed duplicates here — preserved.
			DisplayNotification(nullptr, GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY, npc_name, item_name);
		}
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)module_interface);
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Game::Enable();
	MMAPI::Item::Enable();
	MMAPI::NPC::Enable();
	MMAPI::Text::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterCraftingMenuOpen(OnAfterCraftingMenuOpen);
	MMAPI::Game::Hooks::AfterCraftingMenuClose(OnAfterCraftingMenuClose);
	MMAPI::Item::Hooks::AfterGetDisplayDescription(OnAfterGetDisplayDescription);
	MMAPI::Text::Hooks::AfterLocalizedString(OnAfterLocalizedString);
	MMAPI::Text::Hooks::BeforePlayText(OnBeforePlayText);

	// Register the same OnNpcTick callback for every NPC's obj_<name>. Each call uses a unique
	// object name (the map key) so the registrations don't collide on the AlreadyRegistered check.
	using O = MMAPI::Instance::Objects;
	constexpr O npc_objects[] = {
		O::Adeline, O::Balor, O::Caldarus, O::Celine, O::Darcy, O::Dell, O::Dozy, O::Eiland,
		O::Elsie, O::Errol, O::Hayden, O::Hemlock, O::Henrietta, O::Holt, O::Josephine,
		O::Juniper, O::Landen, O::Louis, O::Luc, O::Maple, O::March, O::Merri, O::Nora,
		O::Olric, O::Reina, O::Ryis, O::Seridia, O::Taliferro, O::Terithia, O::Valen, O::Vera, O::Wheedle,
	};
	for (auto o : npc_objects)
		MMAPI::Instance::Hooks::OnObjectCall(o, OnNpcTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
