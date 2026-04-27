#include <map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "ThePerfectGift";
static const char* const VERSION = "1.3.0";
static const char* const UNLOCK_ALL_GIFT_PREFERENCES_KEY = "unlock_all_gift_preferences";
static const char* const SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY = "show_gift_preferences_on_item_tooltips";
static const char* const DISABLE_DAILY_TALK_LIMIT_KEY = "disable_daily_talk_limit";
static const char* const DISABLE_DAILY_GIFT_LIMIT_KEY = "disable_daily_gift_limit";
static const char* const GML_SCRIPT_TRY_ITEM_ID_TO_STRING = "gml_Script_try_item_id_to_string";
static const char* const GML_SCRIPT_CREATE_NOTIFICATION = "gml_Script_create_notification";
static const char* const GML_SCRIPT_GET_LOCALIZER = "gml_Script_get@Localizer@Localizer";
static const char* const GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_NAME = "gml_Script_get_display_name@anon@2420@LiveItem@LiveItem";
static const char* const GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_DESCRIPTION = "gml_Script_get_display_description@anon@3696@LiveItem@LiveItem";
static const char* const GML_SCRIPT_LOCALIZER_GET = "gml_Script_get@Localizer@Localizer";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_TEXTBOX_SAY = "gml_Script_say@TextboxMenu@TextboxMenu";
static const char* const GML_SCRIPT_CRAFTING_MENU_CLOSE = "gml_Script_on_close@CraftingMenu@CraftingMenu";
static const char* const GML_SCRIPT_CRAFTING_MENU_INITIALIZE = "gml_Script_initialize@CraftingMenu@CraftingMenu";
static const std::string GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY = "mods/ThePerfectGift/gift_preference_detected";
static const std::string GIFT_PREFERENCE_UNLOCKED_LOCALIZATION_KEY = "mods/ThePerfectGift/gift_preference_unlocked";
static const std::string ITEM_PLACEHOLDER_TEXT = "<ITEM>";
static const std::string NPC_PLACEHOLDER_TEXT = "<NPC>";
static const std::string ADELINE = "adeline";
static const std::string BALOR = "balor";
static const std::string CALDARUS = "caldarus";
static const std::string CELINE = "celine";
static const std::string DARCY = "darcy";
static const std::string DELL = "dell";
static const std::string DOZY = "dozy";
static const std::string EILAND = "eiland";
static const std::string ELSIE = "elsie";
static const std::string ERROL = "errol";
static const std::string HAYDEN = "hayden";
static const std::string HEMLOCK = "hemlock";
static const std::string HENRIETTA = "henrietta";
static const std::string HOLT = "holt";
static const std::string JOSEPHINE = "josephine";
static const std::string JUNIPER = "juniper";
static const std::string LANDEN = "landen";
static const std::string LOUIS = "louis";
static const std::string LUC = "luc";
static const std::string MAPLE = "maple";
static const std::string MARCH = "march";
static const std::string MERRI = "merri";
static const std::string NORA = "nora";
static const std::string OLRIC = "olric";
static const std::string REINA = "reina";
static const std::string RYIS = "ryis";
static const std::string SERIDIA = "seridia";
//static const std::string STILLWELL = "stillwell";
static const std::string TALIFERRO = "taliferro";
static const std::string TERITHIA = "terithia";
static const std::string VALEN = "valen";
static const std::string VERA = "vera";
static const std::string WHEEDLE = "wheedle";
//static const std::string ZOREL = "zorel";
static const bool DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES = false;
static const bool DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS = false;
static const bool DEFAULT_DISABLE_DAILY_TALK_LIMIT = false;
static const bool DEFAULT_DISABLE_DAILY_GIFT_LIMIT = false;
static const std::vector<std::string> ACTIVE_NPC_LIST = { // As of 0.15.1
	ADELINE, BALOR, CALDARUS, CELINE, DARCY, DELL, DOZY, EILAND, ELSIE, ERROL,
	HAYDEN, HEMLOCK, HENRIETTA, HOLT, JOSEPHINE, JUNIPER, LANDEN, LOUIS, LUC, MAPLE,
	MARCH, MERRI, NORA, OLRIC, REINA, RYIS, SERIDIA, TALIFERRO, TERITHIA, VALEN, VERA, WHEEDLE
};
static const std::multimap<std::string, std::vector<std::string>> GIFT_DIALOG_MAP = { // As of 0.15.3
	// Adeline
	{ "Cutscenes/Heart Events/Adeline/adeline_eight_hearts/adeline_eight_hearts/3", { ADELINE, "lemon_pie" }},
	{ "Conversations/Bank/Adeline/Relationship Lines/Relationship/post_8h_lines_romantic/adeline_post_8h_romantic_7/init", { ADELINE, "middlemist" }},
	{ "Conversations/Bank/Adeline/Relationship Lines/Relationship/post_8h_lines_best_friend/adeline_post_8h_best_friend_10/init", { ADELINE, "coffee" }},
	{ "Conversations/Bank/Adeline/Banked Lines/inn_work/inn_work/init", { ADELINE, "wildberry_scone" }},
	{ "Conversations/Bank/Adeline/Banked Lines/loves_paperwork/loves_paperwork/init", { ADELINE, "paper" }},
	{ "Conversations/Bank/Adeline/Banked Lines/wine_cellar/wine_cellar/1", { ADELINE, "red_wine" }},
	{ "Conversations/Bank/Adeline/Banked Lines/wine_cellar/wine_cellar/init", { ADELINE, "coffee", "white_wine" }},
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/lemon_pie/init", { ADELINE, "lemon_pie" }}, // Gift Line
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/middlemist/init", { ADELINE, "middlemist" }}, // Gift Line
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/paper/init", { ADELINE, "paper" }}, // Gift Line
	{ "Conversations/Bank/Adeline/Gift Lines/gift_lines/perfect_pink_diamond/init", { ADELINE, "perfect_pink_diamond" }}, // Gift Line
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
	{ "Conversations/Bank/Balor/Gift Lines/gift_lines/perfect_diamond/init", { BALOR, "perfect_diamond" }}, // Gift Line
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
	{ "Conversations/Bank/Caldarus/Gift Lines/gift_lines/fried_rice/init", { CALDARUS, "fried_rice" }}, // Gift Line
	{ "Conversations/Bank/Caldarus/Gift Lines/gift_lines/mont_blanc/init", { CALDARUS, "mont_blanc" }}, // Gift Line
	{ "Conversations/Bank/Caldarus/Gift Lines/gift_lines/sushi_platter/init", { CALDARUS, "sushi_platter" }}, // Gift Line
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
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/chrysanthemum/init", { CELINE, "chrysanthemum" }}, // Gift Line
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/plum_blossom/init", { CELINE, "plum_blossom" }}, // Gift Line
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/rose/init", { CELINE, "rose" }}, // Gift Line
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/temple_flower_0/1", { CELINE, "temple_flower" }}, // Gift Line
	{ "Conversations/Bank/Celine/Gift Lines/gift_lines/temple_flower_1/init", { CELINE, "temple_flower" }}, // Gift Line
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
	// Dozy
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
	{ "Conversations/Bank/Hemlock/Gift Lines/gift_lines/crayfish_etouffee/init", { HEMLOCK, "crayfish_etouffee" }}, // Gift Line
	{ "Conversations/Bank/Josephine/Gift Lines/gift_lines/crayfish_etouffee/init", { HEMLOCK, "crayfish_etouffee" }}, // Gift Line
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
	{ "Conversations/Bank/Hemlock/Gift Lines/gift_lines/crayfish_etouffee/init", { JOSEPHINE, "crayfish_etouffee" }}, // Gift Line
	{ "Conversations/Bank/Josephine/Gift Lines/gift_lines/crayfish_etouffee/init", { JOSEPHINE, "crayfish_etouffee" }}, // Gift Line
	{ "Conversations/Bank/Josephine/Banked Lines/quiche/quiche/init", { JOSEPHINE, "quiche" }},
	{ "Conversations/Bank/Josephine/Market Lines/market_darcy_1/market_darcy_1/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea"}},
	{ "Conversations/Bank/Josephine/Market Lines/market_darcy_2/market_darcy_2/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea" }},
	{ "Conversations/Bank/Josephine/Market Lines/market_darcy_4/market_darcy_4/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea" }},
	{ "Conversations/Bank/Hemlock/Market Lines/market_darcy_2/market_darcy_2/init", { JOSEPHINE, "cup_of_tea", "green_tea", "jasmine_tea", "lavender_tea", "roasted_rice_tea", "rose_tea", "tea" }}, // Indicates Josephine likes tea, but without the highlighting hint.
	// Juniper
	{ "Conversations/Bank/Juniper/Banked Lines/breakfast/breakfast/1", { JUNIPER, "latte" }},
	{ "Conversations/Bank/Juniper/Banked Lines/foraging/foraging_3/1", { JUNIPER, "nettle" }},
	{ "Conversations/Bank/Juniper/Banked Lines/foraging/foraging_3/init", { JUNIPER, "nettle" }},
	{ "Conversations/Bank/Juniper/Banked Lines/middlemist_red/middlemist_red/init", { JUNIPER, "middlemist" }},
	{ "Conversations/Bank/Juniper/Banked Lines/new_potion/new_potion/init", { JUNIPER, "newt" }},
	{ "Conversations/Bank/Juniper/Banked Lines/orb_viewing/orb_viewing_2/1", { JUNIPER, "water_chestnut_fritters" }},
	{ "Conversations/Bank/Juniper/Gift Lines/gift_lines/ancient_royal_scepter/init", { JUNIPER, "ancient_royal_scepter" }}, // Gift Line
	{ "Conversations/Bank/Juniper/Gift Lines/gift_lines/crystal_rose/init", { JUNIPER, "crystal_rose" }}, // Gift Line
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
	{ "Conversations/Bank/Luc/Gift Lines/gift_lines/grilled_cheese/init", { LUC, "grilled_cheese" }}, // Gift Line
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
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/crystal/init", { LOUIS, "crystal" }}, // Gift Line
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/lilac/init", { LOUIS, "lilac" }}, // Gift Line
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/red_wine/init", { LOUIS, "red_wine" }}, // Gift Line
	{ "Conversations/Bank/Louis/Gift Lines/gift_lines/white_wine/init", { LOUIS, "white_wine" }}, // Gift Line
	// Maple
	{ "Conversations/Bank/Maple/Gift Lines/gift_lines/berries_and_cream/init", { MAPLE, "berries_and_cream" }}, // Gift Line
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
	{ "Conversations/Bank/March/Gift Lines/gift_lines/gold_ingot/init", { MARCH, "gold_ingot" }}, // Gift Line
	{ "Conversations/Bank/March/Gift Lines/gift_lines/mocha/init", { MARCH, "mocha" }}, // Gift Line
	{ "Conversations/Bank/March/Gift Lines/gift_lines/red_snapper_sushi/init", { MARCH, "red_snapper_sushi" }}, // Gift Line
	{ "Conversations/Bank/March/Market Lines/market_darcy_1/market_darcy_1/init", { MARCH, "hot_cocoa" }},
	{ "Conversations/Bank/March/Market Lines/market_darcy_3/market_darcy_3/init", { MARCH, "hot_cocoa" }},
	{ "Conversations/Bank/March/Museum Lines/meteorite/meteorite/init", { MARCH, "meteorite" }},
	{ "Conversations/Bank/March/Museum Lines/perfect_copper_ore/perfect_copper_ore/init", { MARCH, "perfect_copper_ore" }},
	{ "Conversations/Bank/March/Museum Lines/perfect_iron_ore/perfect_iron_ore/init", { MARCH, "perfect_iron_ore" }},
	{ "Conversations/Bank/March/Museum Lines/perfect_silver_ore/perfect_silver_ore/init", { MARCH, "perfect_silver_ore" }},
	{ "Conversations/Festival Lines/March/animal_festival/animal_festival_0/init", { MARCH, "hot_cocoa" }},
	{ "Conversations/Group Conversations/Elsie_March_Olric_Ryis/breakfast/breakfast_3/1", { MARCH, "coffee" }},
	// Merri
	{ "Conversations/Bank/Merri/Gift Lines/gift_lines/glass/init", { MERRI, "glass" }}, // Gift Line
	{ "Conversations/Bank/Merri/Gift Lines/gift_lines/hard_wood/init", { MERRI, "hard_wood" }}, // Gift Line
	{ "Conversations/Bank/Merri/Gift Lines/gift_lines/latte/init", { MERRI, "latte" }}, // Gift Line
	// Nora
	{ "Conversations/Bank/Nora/Gift Lines/gift_lines/ancient_gold_coin/init", { NORA, "ancient_gold_coin" }}, // Gift Line
	{ "Conversations/Bank/Nora/Gift Lines/gift_lines/coffee/init", { NORA, "coffee" }}, // Gift Line
	{ "Conversations/Bank/Nora/Market Lines/market_darcy_4/market_darcy_4/init", { NORA, "latte" }},
	{ "Conversations/Bank/Nora/Museum Lines/ancient_gold_coin/ancient_gold_coin/init", { NORA, "ancient_gold_coin" }},
	{ "Conversations/General Dialogue/birthday_anticipation/nora_birthday_anticipation_dell/1", { NORA, "baked_potato" }},
	{ "Conversations/General Dialogue/birthday_anticipation/nora_birthday_anticipation_celine/1", { NORA, "baked_potato" }},
	// Olric
	{ "Conversations/Bank/March/Banked Lines/hardboiled_egg/hardboiled_egg/init", { OLRIC, "hard_boiled_egg" }},
	{ "Conversations/Bank/Olric/Gift Lines/gift_lines/hard_boiled_egg/init", { OLRIC, "hard_boiled_egg" }}, // Gift Line
	{ "Conversations/Group Conversations/Elsie_March_Olric_Ryis/breakfast/breakfast_3/3", { OLRIC, "hard_boiled_egg" }},
	{ "Conversations/Bank/March/Banked Lines/olric_stone/olric_stone/init", { OLRIC, "ore_stone" }},
	{ "Conversations/Bank/Olric/Market Lines/market_wheedle_1/market_wheedle_1/init", { OLRIC, "ore_stone" }},
	// Reina
	{ "Conversations/Bank/Reina/Relationship Lines/Relationship/post_8h_lines_romantic/reina_post_8h_romantic_2/1", { REINA, "cauliflower_curry" }},
	{ "Conversations/Bank/Reina/Relationship Lines/Relationship/post_8h_lines_romantic/reina_post_8h_romantic_8/1", { REINA, "turnip_and_potato_gratin" }},
	{ "Conversations/Bank/Reina/Banked Lines/garlic/garlic/init", { REINA, "garlic" }},
	{ "Conversations/Bank/Reina/Banked Lines/garlic/garlic_2/init", { REINA, "garlic" }},
	{ "Conversations/Bank/Reina/Banked Lines/general_store_shopping/general_store_shopping_3/init", { REINA, "garlic" }},
	{ "Conversations/Bank/Reina/Gift Lines/gift_lines/daffodil/init", { REINA, "daffodil" }}, // Gift Line
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
	{ "Conversations/Bank/Ryis/Gift Lines/gift_lines/hard_wood/1", { RYIS, "hard_wood" }}, // Gift Line
	{ "Conversations/Bank/Ryis/Market Lines/market_darcy_1/market_darcy_1/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Ryis/Market Lines/market_darcy_2/market_darcy_2/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Ryis/Market Lines/market_darcy_3/market_darcy_3/init", { RYIS, "iced_coffee" }},
	{ "Conversations/Bank/Landen/Banked Lines/veggie_sub/veggie_sub/init", { RYIS, "veggie_sub_sandwich" }},
	{ "Conversations/Bank/Ryis/Banked Lines/shopping_for_landen/shopping_for_landen_2/init", { RYIS, "bread" }},
	{ "Conversations/Group Conversations/Elsie_March_Olric_Ryis/breakfast/breakfast_3/2", { RYIS, "bread" }},
	// Seridia
	{ "Conversations/Group Conversations/Juniper_Seridia/disciple/disciple_5/init", { SERIDIA, "night_queen" }},
	{ "Conversations/fetch_quests_follow_ups/request_for_monster_mash_follow_up_seridia/init", { SERIDIA, "monster_mash" }},
	// Stillwell
	// Taliferro
	{ "Conversations/Bank/Taliferro/Banked Lines/challenge_completed_lines/incredibly_hot_pot_terithia/init", { TALIFERRO, "incredibly_hot_pot" }},
	{ "Conversations/Bank/Taliferro/Banked Lines/challenge_completed_lines/veggie_sub_sandwich_holt/init", { TALIFERRO, "veggie_sub_sandwich" }},
	// Terithia
	{ "Conversations/Bank/Terithia/Gift Lines/gift_lines/fish_stew/init", { TERITHIA, "fish_stew" }}, // Gift Line
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
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/coconut_milk/init", { VERA, "coconut_milk" }}, // Gift Line
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/cranberry_juice/init", { VERA, "cranberry_juice" }}, // Gift Line
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/gazpacho/init", { VERA, "gazpacho" }}, // Gift Line
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/orange_juice/init", { VERA, "orange_juice" }}, // Gift Line
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/pomegranate/init", { VERA, "pomegranate" }}, // Gift Line
	{ "Conversations/Bank/Vera/Gift Lines/gift_lines/summer_salad/init", { VERA, "summer_salad" }}, // Gift Line
	// Wheedle
	// Zorel
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
	{ "conversations/talkative_celine/spring galette/init", { ADELINE, "spring_galette" }}, // Verify this mentions Adeline
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

static YYTKInterface* g_ModuleInterface = nullptr;
static bool load_on_start = true;
static bool localize_items = true;
static bool crafting_menu_open = false;
static bool unlock_all_gift_preferences = DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES;
static bool show_gift_preferences_on_item_tooltips = DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS;
static bool disable_daily_talk_limit = DEFAULT_DISABLE_DAILY_TALK_LIMIT;
static bool disable_daily_gift_limit = DEFAULT_DISABLE_DAILY_GIFT_LIMIT;
static std::map<std::string, std::vector<std::string>> gifts_to_unlock = {};
static std::map<std::string, INT64> item_name_to_id_map = {};
static std::map<INT64, std::string> item_id_to_name_map = {};
static std::map<std::string, std::string> internal_item_name_to_localized_item_name_map = {};
static std::map<std::string, std::string> localized_item_name_to_internal_item_name_map = {};
static std::string localized_item_name = "";
static std::string gift_preference_npc_name = "";
static std::string gift_preference_internal_item_name = "";
static std::map<int, std::string> npc_id_to_name_map = {};
static std::map<std::string, std::vector<int>> npc_name_to_liked_gifts_map = {};
static std::map<std::string, std::vector<int>> npc_name_to_loved_gifts_map = {};

bool GameIsPaused()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	RValue paused = *global_instance->GetRefMember("__pause_status");
	return paused.m_i64 > 0;
}

int RValueAsInt(RValue value)
{
	if (value.m_Kind == VALUE_REAL)
		return static_cast<int>(value.m_Real);
	if (value.m_Kind == VALUE_INT64)
		return static_cast<int>(value.m_i64);
	if (value.m_Kind == VALUE_INT32)
		return static_cast<int>(value.m_i32);
}

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_REAL && value.m_Real == 1)
		return true;
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return RValueAsBool(struct_exists);
}

RValue StructVariableGet(RValue the_struct, const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, variable_name }
	);
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

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
		{ UNLOCK_ALL_GIFT_PREFERENCES_KEY, use_defaults ? DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES : unlock_all_gift_preferences },
		{ SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, use_defaults ? DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS : show_gift_preferences_on_item_tooltips },
		{ DISABLE_DAILY_TALK_LIMIT_KEY, use_defaults ? DEFAULT_DISABLE_DAILY_TALK_LIMIT : disable_daily_talk_limit },
		{ DISABLE_DAILY_GIFT_LIMIT_KEY, use_defaults ? DEFAULT_DISABLE_DAILY_GIFT_LIMIT : disable_daily_gift_limit }
	};
	return config_json;
}

void LogDefaultConfigValues()
{
	unlock_all_gift_preferences = DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES;
	show_gift_preferences_on_item_tooltips = DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS;
	disable_daily_talk_limit = DEFAULT_DISABLE_DAILY_TALK_LIMIT;
	disable_daily_gift_limit = DEFAULT_DISABLE_DAILY_GIFT_LIMIT;
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, UNLOCK_ALL_GIFT_PREFERENCES_KEY, DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_DAILY_TALK_LIMIT_KEY, DEFAULT_DISABLE_DAILY_TALK_LIMIT ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_DAILY_GIFT_LIMIT_KEY, DEFAULT_DISABLE_DAILY_GIFT_LIMIT ? "true" : "false");
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

		// Try to find the mod_data/ThePerfectGift directory.
		std::string the_perfect_gift_folder = mod_data_folder + "\\ThePerfectGift";
		if (!std::filesystem::exists(the_perfect_gift_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"ThePerfectGift\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, the_perfect_gift_folder.c_str());
			std::filesystem::create_directory(the_perfect_gift_folder);
		}

		// Try to find the mod_data/ThePerfectGift/ThePerfectGift.json config file.
		bool update_config_file = false;
		std::string config_file = the_perfect_gift_folder + "\\" + "ThePerfectGift.json";
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
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Add your desired values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
					LogDefaultConfigValues();
				}
				else
				{
					// Try loading the unlock_all_gift_preferences value.
					if (json_object.contains(UNLOCK_ALL_GIFT_PREFERENCES_KEY))
					{
						unlock_all_gift_preferences = json_object[UNLOCK_ALL_GIFT_PREFERENCES_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, UNLOCK_ALL_GIFT_PREFERENCES_KEY, unlock_all_gift_preferences ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, UNLOCK_ALL_GIFT_PREFERENCES_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, UNLOCK_ALL_GIFT_PREFERENCES_KEY, DEFAULT_UNLOCK_ALL_GIFT_PREFERENCES ? "true" : "false");
					}

					// Try loading the show_gift_preferences_on_item_tooltips value.
					if (json_object.contains(SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY))
					{
						show_gift_preferences_on_item_tooltips = json_object[SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, show_gift_preferences_on_item_tooltips ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS_KEY, DEFAULT_SHOW_GIFT_PREFERENCES_ON_ITEM_TOOLTIPS ? "true" : "false");
					}

					// Try loading the disable_daily_talk_limit value.
					if (json_object.contains(DISABLE_DAILY_TALK_LIMIT_KEY))
					{
						disable_daily_talk_limit = json_object[DISABLE_DAILY_TALK_LIMIT_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_DAILY_TALK_LIMIT_KEY, disable_daily_talk_limit ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, DISABLE_DAILY_TALK_LIMIT_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_DAILY_TALK_LIMIT_KEY, DEFAULT_DISABLE_DAILY_TALK_LIMIT ? "true" : "false");
					}

					// Try loading the disable_daily_gift_limit value.
					if (json_object.contains(DISABLE_DAILY_GIFT_LIMIT_KEY))
					{
						disable_daily_gift_limit = json_object[DISABLE_DAILY_GIFT_LIMIT_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_DAILY_GIFT_LIMIT_KEY, disable_daily_gift_limit ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, DISABLE_DAILY_GIFT_LIMIT_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, DISABLE_DAILY_GIFT_LIMIT_KEY, DEFAULT_DISABLE_DAILY_GIFT_LIMIT ? "true" : "false");
					}
				}

				update_config_file = true;
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
				LogDefaultConfigValues();
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"ThePerfectGift.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());
			json default_json = CreateConfigJson(true);

			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_json << std::endl;
			out_stream.close();

			LogDefaultConfigValues();
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
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);
		LogDefaultConfigValues();
	}
}

void LoadNpcData()
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	// Load NPC IDs
	size_t npc_ids_length;
	RValue npc_ids = *global_instance->GetRefMember("__npc_id__");
	g_ModuleInterface->GetArraySize(npc_ids, npc_ids_length);
	for (size_t i = 0; i < npc_ids_length; i++)
	{
		RValue* npc_name = nullptr;
		g_ModuleInterface->GetArrayEntry(npc_ids, i, npc_name);
		npc_id_to_name_map[i] = npc_name->ToString();
	}

	// Load NPC Gift Preferences
	size_t npc_prototypes_length;
	RValue npc_prototypes = *global_instance->GetRefMember("__npc_prototypes");
	g_ModuleInterface->GetArraySize(npc_prototypes, npc_prototypes_length);
	for (size_t i = 0; i < npc_prototypes_length; i++)
	{
		RValue* npc_prototype = nullptr;
		g_ModuleInterface->GetArrayEntry(npc_prototypes, i, npc_prototype);

		RValue liked_gifts = *npc_prototype->GetRefMember("liked_gifts")->GetRefMember("__buffer");
		RValue loved_gifts = *npc_prototype->GetRefMember("loved_gifts")->GetRefMember("__buffer");

		// Liked Gifts
		size_t liked_gifts_length;
		g_ModuleInterface->GetArraySize(liked_gifts, liked_gifts_length);
		for (size_t j = 0; j < liked_gifts_length; j++)
		{
			RValue* item_id;
			g_ModuleInterface->GetArrayEntry(liked_gifts, j, item_id);
			npc_name_to_liked_gifts_map[npc_id_to_name_map[i]].push_back(RValueAsInt(*item_id));
		}

		// Loved Gifts
		size_t loved_gifts_length;
		g_ModuleInterface->GetArraySize(loved_gifts, loved_gifts_length);
		for (size_t j = 0; j < loved_gifts_length; j++)
		{
			RValue* item_id;
			g_ModuleInterface->GetArrayEntry(loved_gifts, j, item_id);
			npc_name_to_loved_gifts_map[npc_id_to_name_map[i]].push_back(RValueAsInt(*item_id));
		}
	}
}

// TODO: Update this to use the item info in global instance
void LoadItemData(CInstance* Self, CInstance* Other)
{
	for (int64_t i = 0; i < 5000; i++)
	{
		CScript* gml_script_try_item_id_to_string = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			GML_SCRIPT_TRY_ITEM_ID_TO_STRING,
			(PVOID*)&gml_script_try_item_id_to_string
		);

		RValue* item_id = new RValue(i);
		RValue item_name;
		gml_script_try_item_id_to_string->m_Functions->m_ScriptFunction(
			Self,
			Other,
			item_name,
			1,
			{ &item_id }
		);
		delete item_id;

		if (item_name.m_Kind != VALUE_NULL && item_name.m_Kind != VALUE_UNSET && item_name.m_Kind != VALUE_UNDEFINED)
		{
			std::string item_name_str = item_name.ToString();
			if (item_name_to_id_map.count(item_name_str) <= 0)
				item_name_to_id_map[item_name_str] = i;
			if (item_id_to_name_map.count(i) <= 0)
				item_id_to_name_map[i] = item_name_str;
		}
	}
	if (item_name_to_id_map.size() > 0 && item_id_to_name_map.size() > 0)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Successfully loaded item data!", MOD_NAME, VERSION);
	}
	else {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to load data for items!", MOD_NAME, VERSION);
	}
}

void DisplayNotification(CInstance* Self, CInstance* Other, std::string localization_key)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CREATE_NOTIFICATION,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = RValue(localization_key);
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);

	gift_preference_npc_name = "";
	gift_preference_internal_item_name = "";
}

void UnlockGifts(CInstance* npc, std::string npc_name, bool silent)
{
	if (!gifts_to_unlock[npc_name].empty())
	{
		RValue me_exists = g_ModuleInterface->CallBuiltin("struct_exists", { npc, "me" });
		if (me_exists.m_Kind == VALUE_BOOL && me_exists.m_Real == 1)
		{
			RValue me = g_ModuleInterface->CallBuiltin("struct_get", { npc, "me" });

			RValue known_gift_preferences_exists = g_ModuleInterface->CallBuiltin("struct_exists", { me, "known_gift_preferences" });
			if (known_gift_preferences_exists.m_Kind == VALUE_BOOL && known_gift_preferences_exists.m_Real == 1)
			{
				RValue known_gift_preferences = g_ModuleInterface->CallBuiltin("struct_get", { me, "known_gift_preferences" });				
				RValue inner_exists = g_ModuleInterface->CallBuiltin("struct_exists", { known_gift_preferences, "inner" });
				if (inner_exists.m_Kind == VALUE_BOOL && inner_exists.m_Real == 1)
				{
					RValue inner = g_ModuleInterface->CallBuiltin("struct_get", { known_gift_preferences, "inner" });
					for (int i = 0; i < gifts_to_unlock[npc_name].size(); i++)
					{
						// Check if this preference has already been learned.
						RValue already_set = g_ModuleInterface->CallBuiltin("struct_exists", { inner, item_name_to_id_map[gifts_to_unlock[npc_name][i]] });
						if (already_set.m_Kind == VALUE_BOOL && already_set.m_Real == 0)
						{
							// Set the gift preference.
							RValue set = 0.0;
							g_ModuleInterface->CallBuiltin("struct_set", { inner, item_name_to_id_map[gifts_to_unlock[npc_name][i]], set });

							if (!silent)
							{
								// Display the notification.
								gift_preference_npc_name = npc_name;
								gift_preference_internal_item_name = gifts_to_unlock[npc_name][i];
								DisplayNotification(npc, npc, GIFT_PREFERENCE_UNLOCKED_LOCALIZATION_KEY);
							}
						}
					}

					if(!unlock_all_gift_preferences)
						gifts_to_unlock[npc_name].clear();
				}
			}
		}
	}
}

void AutomaticallyUnlockAllGifts()
{
	for (std::string npc_name : ACTIVE_NPC_LIST)
	{
		for (int item_id : npc_name_to_liked_gifts_map[npc_name])
			gifts_to_unlock[npc_name].push_back(item_id_to_name_map[item_id]);
		for (int item_id : npc_name_to_loved_gifts_map[npc_name])
			gifts_to_unlock[npc_name].push_back(item_id_to_name_map[item_id]);
	}
}

void ParseCustomModDialogue(CInstance* Self, CInstance* Other, std::string dialogue_key)
{
	std::string token;
	std::istringstream iss(dialogue_key);
	std::vector<std::string> tokens;

	// Split the input string by '/'
	while (std::getline(iss, token, '/')) {
		tokens.push_back(token);
	}

	if (tokens.size() < 7 || tokens[0] != "conversations" || tokens[1] != "mods") {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid custom mod dialogue key detected: %s", MOD_NAME, VERSION, dialogue_key.c_str());
		return;
	}

	//std::string mod_name = tokens[2];

	// Gift hint groups are found in the tokens from index 3 up to tokens.size() - 2.
	size_t gift_hint_token_count = tokens.size() - 4; // Exclude the 3 initial tokens and the last conversation token
	if (gift_hint_token_count % 3 != 0) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid custom mod dialogue key detected: %s", MOD_NAME, VERSION, dialogue_key.c_str());
		return;
	}

	// Iterate over each group of gift hint tokens.
	for (size_t i = 3; i < tokens.size() - 1; i += 3) {
		// Validate that the first token of the group starts with "gift_hint_"
		if (tokens[i].find("gift_hint_") != 0) {
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Invalid custom mod dialogue key detected: %s", MOD_NAME, VERSION, dialogue_key.c_str());
			return;
		}

		std::string npc_name = tokens[i + 1];
		if (gifts_to_unlock.count(npc_name) <= 0)
			gifts_to_unlock[npc_name] = {};

		std::string item_name = tokens[i + 2];
		if (std::find(gifts_to_unlock[npc_name].begin(), gifts_to_unlock[npc_name].end(), item_name) == gifts_to_unlock[npc_name].end())
			gifts_to_unlock[npc_name].push_back(item_name);

		// Display the notification.
		gift_preference_npc_name = npc_name;
		gift_preference_internal_item_name = item_name;
		DisplayNotification(Self, Other, GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY);
	}
}

RValue GetLocalizedString(CInstance* Self, CInstance* Other, std::string localization_key)
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

void DisableDailyGiftLimit(CInstance* self)
{
	if (StructVariableExists(self, "me"))
	{
		RValue me = *self->GetRefMember("me");
		if (StructVariableExists(me, "gift_flag"))
			StructVariableSet(me, "gift_flag", true);
	}
}

void DisableDailyTalkLimit(CInstance* self)
{
	if (StructVariableExists(self, "me"))
	{
		RValue me = *self->GetRefMember("me");
		if (StructVariableExists(me, "talk_flag"))
			StructVariableSet(me, "talk_flag", true);
	}
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

	if (disable_daily_gift_limit)
		DisableDailyGiftLimit(self);

	if (disable_daily_talk_limit)
		DisableDailyTalkLimit(self);

	if (gifts_to_unlock.size() > 0 && !GameIsPaused())
	{
		if (gifts_to_unlock.contains(ADELINE) && strstr(self->m_Object->m_Name, "obj_adeline"))
			UnlockGifts(self, ADELINE, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(BALOR) && strstr(self->m_Object->m_Name, "obj_balor"))
			UnlockGifts(self, BALOR, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(CALDARUS) && strstr(self->m_Object->m_Name, "obj_caldarus"))
			UnlockGifts(self, CALDARUS, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(CELINE) && strstr(self->m_Object->m_Name, "obj_celine"))
			UnlockGifts(self, CELINE, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(DARCY) && strstr(self->m_Object->m_Name, "obj_darcy"))
			UnlockGifts(self, DARCY, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(DELL) && strstr(self->m_Object->m_Name, "obj_dell"))
			UnlockGifts(self, DELL, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(DOZY) && strstr(self->m_Object->m_Name, "obj_dozy"))
			UnlockGifts(self, DOZY, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(EILAND) && strstr(self->m_Object->m_Name, "obj_eiland"))
			UnlockGifts(self, EILAND, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(ELSIE) && strstr(self->m_Object->m_Name, "obj_elsie"))
			UnlockGifts(self, ELSIE, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(ERROL) && strstr(self->m_Object->m_Name, "obj_errol"))
			UnlockGifts(self, ERROL, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(HAYDEN) && strstr(self->m_Object->m_Name, "obj_hayden"))
			UnlockGifts(self, HAYDEN, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(HEMLOCK) && strstr(self->m_Object->m_Name, "obj_hemlock"))
			UnlockGifts(self, HEMLOCK, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(HENRIETTA) && strstr(self->m_Object->m_Name, "obj_henrietta"))
			UnlockGifts(self, HENRIETTA, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(HOLT) && strstr(self->m_Object->m_Name, "obj_holt"))
			UnlockGifts(self, HOLT, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(JOSEPHINE) && strstr(self->m_Object->m_Name, "obj_josephine"))
			UnlockGifts(self, JOSEPHINE, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(JUNIPER) && strstr(self->m_Object->m_Name, "obj_juniper"))
			UnlockGifts(self, JUNIPER, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(LANDEN) && strstr(self->m_Object->m_Name, "obj_landen"))
			UnlockGifts(self, LANDEN, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(LOUIS) && strstr(self->m_Object->m_Name, "obj_louis"))
			UnlockGifts(self, LOUIS, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(LUC) && strstr(self->m_Object->m_Name, "obj_luc"))
			UnlockGifts(self, LUC, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(MAPLE) && strstr(self->m_Object->m_Name, "obj_maple"))
			UnlockGifts(self, MAPLE, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(MARCH) && strstr(self->m_Object->m_Name, "obj_march"))
			UnlockGifts(self, MARCH, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(MERRI) && strstr(self->m_Object->m_Name, "obj_merri"))
			UnlockGifts(self, MERRI, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(NORA) && strstr(self->m_Object->m_Name, "obj_nora"))
			UnlockGifts(self, NORA, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(OLRIC) && strstr(self->m_Object->m_Name, "obj_olric"))
			UnlockGifts(self, OLRIC, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(REINA) && strstr(self->m_Object->m_Name, "obj_reina"))
			UnlockGifts(self, REINA, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(RYIS) && strstr(self->m_Object->m_Name, "obj_ryis"))
			UnlockGifts(self, RYIS, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(SERIDIA) && strstr(self->m_Object->m_Name, "obj_seridia"))
			UnlockGifts(self, SERIDIA, unlock_all_gift_preferences);
		//if (gifts_to_unlock.contains(STILLWELL) && strstr(self->m_Object->m_Name, "obj_stillwell"))
		//	UnlockGifts(self, STILLWELL, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(TALIFERRO) && strstr(self->m_Object->m_Name, "obj_taliferro"))
			UnlockGifts(self, TALIFERRO, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(TERITHIA) && strstr(self->m_Object->m_Name, "obj_terithia"))
			UnlockGifts(self, TERITHIA, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(VALEN) && strstr(self->m_Object->m_Name, "obj_valen"))
			UnlockGifts(self, VALEN, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(VERA) && strstr(self->m_Object->m_Name, "obj_vera"))
			UnlockGifts(self, VERA, unlock_all_gift_preferences);
		if (gifts_to_unlock.contains(WHEEDLE) && strstr(self->m_Object->m_Name, "obj_wheedle"))
			UnlockGifts(self, WHEEDLE, unlock_all_gift_preferences);
		//if (gifts_to_unlock.contains(ZOREL) && strstr(self->m_Object->m_Name, "obj_zorel"))
		//	UnlockGifts(self, ZOREL, unlock_all_gift_preferences);
	}
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

RValue& GmlScriptGetDisplayNameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_NAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	localized_item_name = Result.ToString();
	return Result;
}

RValue& GmlScriptGetDisplayDescriptionCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_DESCRIPTION));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (show_gift_preferences_on_item_tooltips && !crafting_menu_open)
	{
		if (localized_item_name.size() > 0)
		{
			if (localized_item_name_to_internal_item_name_map.count(localized_item_name) > 0)
			{
				int item_id = item_name_to_id_map[localized_item_name_to_internal_item_name_map[localized_item_name]];

				std::vector<std::string> npcs_who_like_the_gift = {};
				for (const auto& map_entry : npc_name_to_liked_gifts_map)
				{
					const auto it = std::find(map_entry.second.begin(), map_entry.second.end(), item_id);
					if (it != map_entry.second.end())
						npcs_who_like_the_gift.push_back(map_entry.first);
				}

				std::string liked_by_string = "";
				if (npcs_who_like_the_gift.size() > 0)
				{
					liked_by_string += "Liked By: ";
					for (int i = 0; i < npcs_who_like_the_gift.size(); i++)
					{
						liked_by_string += npcs_who_like_the_gift[i];
						if (i < npcs_who_like_the_gift.size() - 1)
							liked_by_string += ", ";
					}
					liked_by_string += "\n\n";
				}

				std::vector<std::string> npcs_who_love_the_gift = {};
				for (const auto& map_entry : npc_name_to_loved_gifts_map)
				{
					const auto it = std::find(map_entry.second.begin(), map_entry.second.end(), item_id);
					if (it != map_entry.second.end())
						npcs_who_love_the_gift.push_back(map_entry.first);
				}

				std::string loved_by_string = "";
				if (npcs_who_love_the_gift.size() > 0)
				{
					loved_by_string += "Loved By: ";
					for (int i = 0; i < npcs_who_love_the_gift.size(); i++)
					{
						loved_by_string += npcs_who_love_the_gift[i];
						if (i < npcs_who_love_the_gift.size() - 1)
							loved_by_string += ", ";
					}
					loved_by_string += "\n\n";
				}

				if (liked_by_string.size() > 0)
					Result = RValue(liked_by_string + Result.ToString());
				if (loved_by_string.size() > 0)
					Result = RValue(loved_by_string + Result.ToString());
			}
		}
	}

	return Result;
}

RValue& GmlScriptTextboxTranslateCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Load localized item names.
	if (localize_items)
	{
		localize_items = false;

		// Load items.
		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);

		RValue __item_data = *global_instance->GetRefMember("__item_data");

		size_t array_length;
		g_ModuleInterface->GetArraySize(__item_data, array_length);

		for (size_t i = 0; i < array_length; i++)
		{
			RValue* array_element;
			g_ModuleInterface->GetArrayEntry(__item_data, i, array_element);

			RValue name_key = *array_element->GetRefMember("name_key");
			if (name_key.m_Kind != VALUE_NULL && name_key.m_Kind != VALUE_UNDEFINED && name_key.m_Kind != VALUE_UNSET)
			{
				RValue recipe_key = *array_element->GetRefMember("recipe_key");
				RValue localized_name = GetLocalizedString(Self, Other, name_key.ToString());
				localized_item_name_to_internal_item_name_map[localized_name.ToString()] = recipe_key.ToString();
				internal_item_name_to_localized_item_name_map[recipe_key.ToString()] = localized_name.ToString();
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_LOCALIZER_GET));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (ArgumentCount == 1 && Arguments[0]->m_Kind == VALUE_STRING)
	{
		std::string localization_key = Arguments[0]->ToString();
		if (localization_key.compare(GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY) == 0 || localization_key.compare(GIFT_PREFERENCE_UNLOCKED_LOCALIZATION_KEY) == 0)
		{
			std::string result_str = Result.ToString();

			// Replace the <ITEM> placeholder text.
			size_t item_placeholder_index = result_str.find(ITEM_PLACEHOLDER_TEXT);
			if (item_placeholder_index != std::string::npos) {
				result_str.replace(item_placeholder_index, ITEM_PLACEHOLDER_TEXT.length(), internal_item_name_to_localized_item_name_map[gift_preference_internal_item_name]);
			}

			// Replace the <NPC> placeholder text.
			size_t npc_placeholder_index = result_str.find(NPC_PLACEHOLDER_TEXT);
			if (npc_placeholder_index != std::string::npos) {
				std::string npc_name = gift_preference_npc_name;
				npc_name[0] = std::toupper(npc_name[0]);
				result_str.replace(npc_placeholder_index, NPC_PLACEHOLDER_TEXT.length(), npc_name);
			}

			Result = RValue(result_str);
		}
	}
	
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
	gifts_to_unlock = {}; // { { EILAND, {"pumpkin_pie", "apple_pie"}} };
	localized_item_name = "";
	gift_preference_npc_name = "";
	gift_preference_internal_item_name = "";
	crafting_menu_open = false;

	if (load_on_start)
	{
		CreateOrLoadConfigFile();
		LoadNpcData();
		LoadItemData(Self, Other);

		load_on_start = false;
	}

	if (unlock_all_gift_preferences)
		AutomaticallyUnlockAllGifts();

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

RValue& GmlScriptTranslateCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!unlock_all_gift_preferences)
	{
		std::string dialog_string = Arguments[0]->ToString();
		std::string dialog_string_lowercase = Arguments[0]->ToString();
		std::transform(dialog_string_lowercase.begin(), dialog_string_lowercase.end(), dialog_string_lowercase.begin(), [](unsigned char c) { return std::tolower(c); });
		if (dialog_string_lowercase.find("conversations/mods") != std::string::npos && dialog_string_lowercase.find("gift_hint_") != std::string::npos)
		{
			ParseCustomModDialogue(Self, Other, dialog_string);
		}
		else if (GIFT_DIALOG_MAP.count(dialog_string) > 0)
		{
			auto range = GIFT_DIALOG_MAP.equal_range(dialog_string);
			for (auto it = range.first; it != range.second; ++it)
			{
				std::vector<std::string> gifts = it->second; // { REINA, "coffee", "iced_coffee"}}
				std::string npc_name = gifts[0];

				if (gifts_to_unlock.count(npc_name) <= 0)
					gifts_to_unlock[npc_name] = {};

				for (int i = 1; i < gifts.size(); i++)
				{
					gifts_to_unlock[npc_name].push_back(gifts[i]);

					// Display the notification.
					gift_preference_npc_name = npc_name;
					gift_preference_internal_item_name = gifts[i];
					DisplayNotification(Self, Other, GIFT_PREFERENCE_DETECTED_LOCALIZATION_KEY);
				}
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TEXTBOX_SAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
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

void CreateHookGmlScriptGetDisplayName(AurieStatus& status)
{
	CScript* gml_script_get_display_name = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_NAME,
		(PVOID*)&gml_script_get_display_name
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_NAME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_NAME,
		gml_script_get_display_name->m_Functions->m_ScriptFunction,
		GmlScriptGetDisplayNameCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_NAME);
	}
}

void CreateHookGmlScriptGetDisplayDescription(AurieStatus& status)
{
	CScript* gml_script_get_display_description = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_DESCRIPTION,
		(PVOID*)&gml_script_get_display_description
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_DESCRIPTION);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_DESCRIPTION,
		gml_script_get_display_description->m_Functions->m_ScriptFunction,
		GmlScriptGetDisplayDescriptionCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LIVE_ITEM_GET_DISPLAY_DESCRIPTION);
	}
}

void CreateHookGmlScriptTextboxTranslate(AurieStatus& status)
{
	CScript* gml_script_textbox_translate = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_textbox_translate
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_LOCALIZER,
		gml_script_textbox_translate->m_Functions->m_ScriptFunction,
		GmlScriptTextboxTranslateCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_LOCALIZER);
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

void CreateHookGmlScriptTranslate(AurieStatus& status)
{
	CScript* gml_script_translate = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TEXTBOX_SAY,
		(PVOID*)&gml_script_translate
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TEXTBOX_SAY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_TEXTBOX_SAY,
		gml_script_translate->m_Functions->m_ScriptFunction,
		GmlScriptTranslateCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_TEXTBOX_SAY);
	}
}

void CreateHookEventObject(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook EVENT_OBJECT_CALL!", MOD_NAME, VERSION);
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;
	
	status = ObGetInterface(
		"YYTK_Main", 
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateHookEventObject(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptTranslate(status);
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

	CreateHookGmlScriptTextboxTranslate(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetDisplayDescription(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetDisplayName(status);
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

	CreateHookGmlScriptCraftingMenuInitialize(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}
