#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;

// ----- Mod metadata -----

static const char* const MOD_NAME = "Elegance";
static const char* const VERSION  = "1.1.0";

// ----- Cosmetic set definitions -----

static const std::string ELEGANT_HAT_ITEM   = "elegant_hat";
static const std::string ELEGANT_DRESS_ITEM = "elegant_dress";
static const std::string ELEGANT_SHOES_ITEM = "elegant_shoes";
static const std::string TEAPOT_HAT_ITEM    = "head_teapot";

// ----- Localization keys -----

// Custom "Elsie won't gossip with you while you're dressed like that" line — substituted in for
// the game's default no-gossip conversation when an inelegant item is equipped.
static const std::string ELEGANCE_WONT_GOSSIP_CONVERSATION_KEY    = "Conversations/Mods/Elegance/wont_gossip";
static const std::string ELEGANCE_GOSSIP_ONCE_MORE_NOTIFICATION   = "Notifications/Mods/Elegance/gossip_once_more";
static const std::string ELSIE_NO_GOSSIP_CONVERSATION_KEY         = "Conversations/gossip/no_gossip";

// ----- State -----

static bool game_is_active               = false;
static bool inelegant_cosmetics_equipped = false;
static int  elegant_cosmetics_equipped   = 0;     // 0..3
// Set by AfterGetSelections when gossip is being suppressed; consumed by BeforePlayConversation
// on the same fire (the game plays the "no_gossip" conversation immediately after an empty
// selections array, so the latch lives for a single get_selections → play_conversation pairing).
static bool wont_gossip                  = false;
// Tracks whether the elegance-3 "gossip again" perk has already been used today. Resets at end
// of day and on return to title.
static bool can_gossip_again             = true;

// ----- Per-tick cosmetic tracking -----

void OnAriTick(YYTK::CInstance* /*self*/)
{
	if (!game_is_active) return;

	int count = 0;
	if (MMAPI::Cosmetic::IsEquipped(ELEGANT_HAT_ITEM))   ++count;
	if (MMAPI::Cosmetic::IsEquipped(ELEGANT_DRESS_ITEM)) ++count;
	if (MMAPI::Cosmetic::IsEquipped(ELEGANT_SHOES_ITEM)) ++count;
	elegant_cosmetics_equipped   = count;
	inelegant_cosmetics_equipped = MMAPI::Cosmetic::IsEquipped(TEAPOT_HAT_ITEM);
}

// ----- Gossip suppression -----

void OnAfterGetGossipSelections(MMAPI::Gossip::AfterGetSelectionsContext& ctx)
{
	if (!game_is_active) return;
	if (!inelegant_cosmetics_equipped) return;

	// Empty the gossip selections so the game falls through to its no-gossip conversation,
	// which we substitute in the BeforePlayConversation hook below.
	ctx.Clear();
	wont_gossip = true;
}

void OnBeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx)
{
	if (!game_is_active || !wont_gossip) return;
	if (ctx.GetKey() != ELSIE_NO_GOSSIP_CONVERSATION_KEY) return;

	wont_gossip = false;
	ctx.SetKey(ELEGANCE_WONT_GOSSIP_CONVERSATION_KEY);
}

// ----- NPC heart bonus -----

void OnBeforeNpcHeartPointsChange(MMAPI::NPC::HeartPointsChangedContext& ctx)
{
	switch (elegant_cosmetics_equipped)
	{
		case 1:
			ctx.SetAmount(ctx.GetAmount() + 1);
			MMAPI::Log::Info("Your elegance earns you 1 additional heart point!");
			break;
		case 2:
			ctx.SetAmount(ctx.GetAmount() + 3);
			MMAPI::Log::Info("Your elegance earns you 3 additional heart points!");
			break;
		case 3:
			ctx.SetAmount(ctx.GetAmount() + 5);
			MMAPI::Log::Info("Your elegance earns you 5 additional heart points!");
			break;
	}
}

// ----- Gossip-again perk -----

void OnAfterGossipMenuClose()
{
	if (elegant_cosmetics_equipped != 3) return;
	if (!can_gossip_again) return;

	MMAPI::Gossip::SetHasGossipedToday(false);
	can_gossip_again = false;
	MMAPI::Log::Info("Your elegance allows you to gossip once more today!");
	MMAPI::Game::CreateNotification(true, ELEGANCE_GOSSIP_ONCE_MORE_NOTIFICATION);
}

// ----- Lifecycle -----

void OnAfterEndDay()
{
	can_gossip_again = true;
}

void OnBeforeSetupMainScreen()
{
	game_is_active               = false;
	inelegant_cosmetics_equipped = false;
	elegant_cosmetics_equipped   = 0;
	wont_gossip                  = false;
	can_gossip_again             = true;
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
	MMAPI::NPC::Enable();
	MMAPI::Text::Enable();
	MMAPI::Gossip::Enable();
	MMAPI::Instance::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::AfterEndDay(OnAfterEndDay);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, OnAriTick);
	MMAPI::Gossip::Hooks::AfterGetSelections(OnAfterGetGossipSelections);
	MMAPI::Gossip::Hooks::AfterClose(OnAfterGossipMenuClose);
	MMAPI::Text::Hooks::BeforePlayConversation(OnBeforePlayConversation);
	MMAPI::NPC::Hooks::BeforeHeartPointsChange(OnBeforeNpcHeartPointsChange);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
