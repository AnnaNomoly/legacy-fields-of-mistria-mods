#include <cmath>
#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "ShipIt";
static const char* const VERSION  = "1.2.0";

// ----- Config -----

static const char* const CONFIG_KEY_SELL_TRASHED_ITEMS = "sell_trashed_items";
static const bool        DEFAULT_SELL_TRASHED_ITEMS    = false;

static bool sell_trashed_items = DEFAULT_SELL_TRASHED_ITEMS;

// ----- Game constants -----

// Custom interactable placed by the mod's content pack; substring-matched against object names.
static const std::string CUSTOM_OBJECT_NAME_FRAGMENT      = "balor_crystal_ball";
// Conversation key the mod redirects play_conversation to when the crystal ball is used.
static const std::string SELL_SHIPPING_BIN_CONVERSATION   = "Conversations/Mods/Sell Items/sell_shipping_bin_items";
// Dialogue key fired by the conversation's "yes, sell" branch — substring-matched in play_text.
static const std::string SELL_SHIPPING_BIN_DIALOGUE_BRANCH = "Conversations/Mods/Sell Items/sell_shipping_bin_items/1";
// Notification key whose resolved string contains a `<VALUE>` placeholder we substitute with the
// gold gained on the most recent sell or trash batch.
static const std::string ITEMS_SOLD_NOTIFICATION          = "Notifications/Mods/Sell Items/items_sold";
static const std::string VALUE_PLACEHOLDER                = "<VALUE>";
// Asset name of the sound effect that plays around any trash action — used as a heuristic to
// distinguish "trash" pops/drains from ordinary inventory pops.
static const std::string TRASH_ITEM_SOUND_NAME            = "snd_UITrashItem";
static const std::string ARCHAEOLOGY_TAG                  = "archaeology";

// ----- State -----

static bool         startup_loaded             = false;
static bool         game_is_active             = false;
// Latched from the BeforeInteract hook when the player interacts with the crystal ball; consumed
// by the very next BeforePlayConversation fire to redirect to our custom conversation.
static bool         custom_object_used         = false;
// Pending gold to apply on the next obj_ari tick. The mod defers application out of the
// inventory hook into the ari context (matches the original mod's pattern — calling modify_gold
// from inside the inventory menu's call stack may not be safe re: reentrancy).
static int          gold_to_gain               = 0;
// Most recent gold value to display in an items_sold notification. Read by the AfterLocalizedString
// hook when the placeholder is being resolved.
static int          gold_earned                = 0;
// Resolved on first setup_main_screen and reused for AudioIsPlaying gating in the trash hooks.
static YYTK::RValue trash_item_sound_asset;

// ----- Config -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	sell_trashed_items = MMAPI::Config::GetValue<bool>(config, CONFIG_KEY_SELL_TRASHED_ITEMS, DEFAULT_SELL_TRASHED_ITEMS);

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_SELL_TRASHED_ITEMS] = sell_trashed_items;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config: %s=%s", CONFIG_KEY_SELL_TRASHED_ITEMS, sell_trashed_items ? "true" : "false");
}

// ----- Pricing -----

// Returns the gold value the player should receive for trashing this item, applying the
// Back-in-Vogue and Back-in-Vogue II archaeology perks (each multiplies the base value by 1.25
// when the item carries the archaeology tag — they stack compounding for 1.5625x with both).
static int GoldValueForTrash(int item_id)
{
	if (item_id < 0) return 0;

	YYTK::RValue bin_rv = MMAPI::Item::GetBinValue(item_id);
	if (!MMAPI::Engine::IsNumeric(bin_rv)) return 0;

	double value = static_cast<double>(bin_rv.ToInt64());
	if (value <= 0.0) return 0;

	if (MMAPI::Item::HasTag(item_id, ARCHAEOLOGY_TAG))
	{
		if (MMAPI::Perk::IsActive(MMAPI::Perk::Ids::BackInVogue))     value *= 1.25;
		if (MMAPI::Perk::IsActive(MMAPI::Perk::Ids::BackInVogueTwo))  value *= 1.25;
	}

	return static_cast<int>(std::ceil(value));
}

// Updates the running pending-gold total and refreshes the notification value, then fires the
// items_sold notification. Mirrors the original mod's "cumulative across pending trashes" UX:
// rapid trashes that share an obj_ari tick gap all show the running total.
static void CreditTrashedItem(int item_id)
{
	int gold = GoldValueForTrash(item_id);
	if (gold <= 0) return;

	gold_to_gain += gold;
	gold_earned = gold_to_gain;
}

// ----- Hooks -----

void OnAriTick(YYTK::CInstance* /*self*/)
{
	if (!game_is_active) return;

	if (gold_to_gain > 0)
	{
		MMAPI::Player::ModifyGold(gold_to_gain);
		MMAPI::Log::Info("Gained %d Tesserae from trashing items!", gold_to_gain);
		gold_to_gain = 0;
	}
}

void OnBeforeInteract(MMAPI::Instance::InteractContext& ctx)
{
	int object_id = ctx.GetObjectId();
	if (object_id < 0) return;

	if (MMAPI::Object::InternalNameContains(object_id, CUSTOM_OBJECT_NAME_FRAGMENT))
		custom_object_used = true;
}

void OnBeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx)
{
	if (!custom_object_used) return;
	custom_object_used = false;
	ctx.SetKey(SELL_SHIPPING_BIN_CONVERSATION);
}

void OnBeforePlayText(MMAPI::Text::PlayTextContext& ctx)
{
	if (std::string(ctx.GetKey()).find(SELL_SHIPPING_BIN_DIALOGUE_BRANCH) == std::string::npos)
		return;

	YYTK::RValue result = MMAPI::Game::SellShippingBinItems();
	if (result.m_Kind != YYTK::VALUE_OBJECT) return;
	if (!MMAPI::Engine::StructVariableExists(result, "gold_earned")) return;

	gold_earned = static_cast<int>(result.GetMember("gold_earned").ToInt64());
	MMAPI::Log::Info("Gained %d Tesserae from shipping items! Renown will be added at the end-of-day screen.", gold_earned);
	MMAPI::Game::CreateNotification(true, ITEMS_SOLD_NOTIFICATION);
}

void OnAfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& ctx)
{
	if (std::string(ctx.GetKey()).find(ITEMS_SOLD_NOTIFICATION) == std::string::npos)
		return;

	std::string resolved(ctx.GetResolved());
	size_t idx = resolved.find(VALUE_PLACEHOLDER);
	if (idx == std::string::npos) return;

	resolved.replace(idx, VALUE_PLACEHOLDER.length(), std::to_string(gold_earned));
	ctx.SetResolved(std::move(resolved));
}

void OnAfterSlotPop(MMAPI::Inventory::AfterSlotPopContext& ctx)
{
	if (!game_is_active || !sell_trashed_items) return;
	if (!MMAPI::Engine::AudioIsPlaying(trash_item_sound_asset)) return;

	int item_id = ctx.GetItemId();
	if (item_id < 0) return;

	CreditTrashedItem(item_id);
	MMAPI::Game::CreateNotification(true, ITEMS_SOLD_NOTIFICATION);
}

void OnAfterSlotDrain(MMAPI::Inventory::AfterSlotDrainContext& ctx)
{
	if (!game_is_active || !sell_trashed_items) return;
	if (!MMAPI::Engine::AudioIsPlaying(trash_item_sound_asset)) return;

	bool any_credited = false;
	ctx.ForEachItem([&](YYTK::RValue item)
	{
		if (item.m_Kind != YYTK::VALUE_OBJECT) return;
		if (!MMAPI::Engine::StructVariableExists(item, "item_id")) return;
		int item_id = static_cast<int>(item.GetMember("item_id").ToInt64());
		int before = gold_to_gain;
		CreditTrashedItem(item_id);
		if (gold_to_gain > before) any_credited = true;
	});

	if (any_credited)
		MMAPI::Game::CreateNotification(true, ITEMS_SOLD_NOTIFICATION);
}

void OnBeforeSetupMainScreen()
{
	game_is_active     = false;
	custom_object_used = false;
	gold_to_gain       = 0;
	gold_earned        = 0;

	if (!startup_loaded)
	{
		LoadConfig();
		trash_item_sound_asset = MMAPI::Engine::AssetGetIndex(TRASH_ITEM_SOUND_NAME);
		startup_loaded = true;
	}
}

void OnGameActive()
{
	game_is_active = true;
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
	MMAPI::Instance::Enable();
	MMAPI::Player::Enable();
	MMAPI::Perk::Enable();
	MMAPI::Item::Enable();
	MMAPI::Text::Enable();
	MMAPI::Inventory::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, OnAriTick);
	MMAPI::Instance::Hooks::BeforeInteract(OnBeforeInteract);
	MMAPI::Text::Hooks::BeforePlayConversation(OnBeforePlayConversation);
	MMAPI::Text::Hooks::BeforePlayText(OnBeforePlayText);
	MMAPI::Text::Hooks::AfterLocalizedString(OnAfterLocalizedString);
	MMAPI::Inventory::Hooks::AfterSlotPop(OnAfterSlotPop);
	MMAPI::Inventory::Hooks::AfterSlotDrain(OnAfterSlotDrain);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
