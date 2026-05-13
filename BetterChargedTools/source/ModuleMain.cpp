#include <map>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "BetterChargedTools";
static const char* const VERSION  = "1.1.0";

// ----- Config keys -----

static const char* const KEY_REDUCE_STAMINA  = "reduce_charged_stamina_usage";
static const char* const KEY_REMOVE_PENALTY  = "remove_charged_damage_penalty";

// ----- Tuning -----

// Refund 1 stamina every Nth tool-action consumption (or every Nth modify_stamina call for the
// watering can). N=3 yields a ~33% stamina reduction.
static constexpr int STAMINA_REFUND_PERIOD = 3;

// ----- Config -----

struct Config
{
	bool reduce_charged_stamina_usage  = true;
	bool remove_charged_damage_penalty = true;
};

// ----- State -----

static bool   startup_loaded = false;
static Config config;

// Per-tool counter of stamina-consuming events within the current use_item. Reset on every
// BeforeUseItem fire so the refund cadence applies within a single charged-tool swing/sweep.
//   - Non-watering-can tools: incremented in AfterToolAction when stamina was actually consumed
//     during the action; every Nth increment triggers a +1 stamina refund.
//   - Watering can: incremented in BeforeToolAction (pre-action); BeforeStaminaChange consults
//     the count and zeros the cost on every Nth modify_stamina fire, decrementing afterward.
// Two patterns because the watering can fires modify_stamina multiple times mid-action while the
// other tools fire it once after — the watering can needs *cancellation* timed against that flow,
// the others can get the same effect via a *refund* after the fact.
static std::map<MMAPI::Player::Tool, int> tool_action_count;

// Latched in AfterStaminaChange when the post-Before amount was negative. Read in AfterToolAction
// to gate the refund — we don't want to refund if no real cost was applied this fire.
static bool stamina_consumed_this_action = false;

// ----- Config -----

static void LoadConfig()
{
	fs::path path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json doc = MMAPI::Config::Load(path);

	config.reduce_charged_stamina_usage  = MMAPI::Config::GetValue<bool>(doc, KEY_REDUCE_STAMINA, true);
	config.remove_charged_damage_penalty = MMAPI::Config::GetValue<bool>(doc, KEY_REMOVE_PENALTY, true);

	nlohmann::json roundtrip;
	roundtrip[KEY_REDUCE_STAMINA] = config.reduce_charged_stamina_usage;
	roundtrip[KEY_REMOVE_PENALTY] = config.remove_charged_damage_penalty;
	MMAPI::Config::Save(path, roundtrip);

	MMAPI::Log::Info("reduce_charged_stamina_usage=%s, remove_charged_damage_penalty=%s",
		config.reduce_charged_stamina_usage  ? "true" : "false",
		config.remove_charged_damage_penalty ? "true" : "false");
}

// ----- Hooks -----

void OnBeforeUseItem(MMAPI::Item::UseItemContext& /*ctx*/)
{
	tool_action_count.clear();
}

void OnBeforeToolAction(MMAPI::Player::ToolActionContext& ctx)
{
	stamina_consumed_this_action = false;

	// Watering can: pre-increment so BeforeStaminaChange can decide whether to cancel the cost
	// during the FSM action's modify_stamina fires.
	if (ctx.GetTool() == MMAPI::Player::Tool::WateringCan)
		tool_action_count[ctx.GetTool()] += 1;
}

void OnAfterToolAction(MMAPI::Player::ToolActionContext& ctx)
{
	// Watering can is handled in BeforeStaminaChange — skip the after-action refund path.
	if (ctx.GetTool() == MMAPI::Player::Tool::WateringCan) return;
	if (!config.reduce_charged_stamina_usage) return;
	if (!stamina_consumed_this_action) return;

	tool_action_count[ctx.GetTool()] += 1;
	if (tool_action_count[ctx.GetTool()] % STAMINA_REFUND_PERIOD == 0)
		MMAPI::Player::ModifyStamina(1);

	stamina_consumed_this_action = false;
}

void OnBeforeStaminaChange(MMAPI::Player::BeforeStaminaChangeContext& ctx)
{
	if (!config.reduce_charged_stamina_usage) return;
	if (ctx.GetAmount() >= 0.0)                return;

	auto& count = tool_action_count[MMAPI::Player::Tool::WateringCan];
	if (count <= 0) return;

	if (count % STAMINA_REFUND_PERIOD == 0)
		ctx.SetAmount(0.0);
	count -= 1;
}

void OnAfterStaminaChange(MMAPI::Player::AfterStaminaChangeContext& ctx)
{
	if (ctx.GetAmount() < 0.0)
		stamina_consumed_this_action = true;
}

void OnBeforeNodeInteraction(MMAPI::Player::NodeInteractionContext& ctx)
{
	if (config.remove_charged_damage_penalty && ctx.GetDamageModifier() < 0.0)
		ctx.SetDamageModifier(0.0);
}

void OnBeforeSetupMainScreen()
{
	stamina_consumed_this_action = false;
	tool_action_count.clear();

	if (startup_loaded) return;
	LoadConfig();
	startup_loaded = true;
}

// ----- Init -----

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
	MMAPI::Player::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Item::Hooks::BeforeUseItem(OnBeforeUseItem);
	MMAPI::Player::Hooks::BeforeToolAction(OnBeforeToolAction);
	MMAPI::Player::Hooks::AfterToolAction(OnAfterToolAction);
	MMAPI::Player::Hooks::BeforeStaminaChange(OnBeforeStaminaChange);
	MMAPI::Player::Hooks::AfterStaminaChange(OnAfterStaminaChange);
	MMAPI::Player::Hooks::BeforePickNode(OnBeforeNodeInteraction);
	MMAPI::Player::Hooks::BeforeChopNode(OnBeforeNodeInteraction);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
