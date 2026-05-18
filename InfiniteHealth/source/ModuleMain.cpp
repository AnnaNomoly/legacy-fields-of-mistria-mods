#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

#include <string>

using namespace Aurie;
using namespace YYTK;

static const char* const MOD_NAME = "InfiniteHealth";
static const char* const VERSION = "1.1.0";

static YYTKInterface* g_ModuleInterface = nullptr;

static void BeforeHealthChange(MMAPI::Player::BeforeHealthChangeContext& ctx)
{
	if (ctx.GetAmount() < 0)
		ctx.SetAmount(0);
}

static void OnBeforeRunDate(MMAPI::Dates::BeforeRunDateContext& ctx)
{
	if (ctx.HasNpc())
		MMAPI::Log::Info("[hook] BeforeRunDate: date_id=%d, npc_id=%d", ctx.GetDateId(), ctx.GetNpcId());
	else
		MMAPI::Log::Info("[hook] BeforeRunDate: date_id=%d (no npc arg)", ctx.GetDateId());
}

static void OnAfterRunDate(MMAPI::Dates::AfterRunDateContext& ctx)
{
	if (ctx.HasNpc())
		MMAPI::Log::Info("[hook] AfterRunDate: date_id=%d, npc_id=%d", ctx.GetDateId(), ctx.GetNpcId());
	else
		MMAPI::Log::Info("[hook] AfterRunDate: date_id=%d (no npc arg)", ctx.GetDateId());
}

static bool g_reina_busy = false;

static void OnBeforeActivityChange(MMAPI::NPC::BeforeActivityChangeContext& ctx)
{
	// Demo callback: log every Reina activity change at Info level (in addition to the
	// diag log on every NPC). The override map will already have substituted to "reading"
	// if g_reina_busy is set; this callback could change the decision further.
	if (ctx.GetNpc() == MMAPI::NPC::Ids::Reina)
	{
		MMAPI::Log::Info("[hook] BeforeActivityChange(Reina): activity='%s' decision=%d substitute='%s'",
			std::string(ctx.GetActivityName()).c_str(),
			static_cast<int>(ctx.GetDecision()),
			std::string(ctx.GetSubstituteName()).c_str());
	}
}

static void OnBeforeQuestStart(MMAPI::Quest::BeforeQuestStartContext& ctx)
{
	MMAPI::Log::Info("[hook] BeforeQuestStart: %s", std::string(ctx.GetQuestName()).c_str());
}

static void OnAfterQuestComplete(MMAPI::Quest::AfterQuestCompleteContext& ctx)
{
	MMAPI::Log::Info("[hook] AfterQuestComplete: %s", std::string(ctx.GetQuestName()).c_str());
}

static void OnBeginStep()
{
	if (!MMAPI::Input::IsKeybindPressed(MMAPI::Input::Keybind(MMAPI::Input::KeyboardKeys::F8)))
		return;

	auto active = MMAPI::Quest::GetActiveQuests();
	MMAPI::Log::Info("[F8] Quest::GetActiveQuests: %llu active quest(s)",
		static_cast<unsigned long long>(active.size()));
	for (const auto& name : active)
	{
		MMAPI::Log::Info("[F8]   %s (stage=%d)",
			name.c_str(),
			MMAPI::Quest::GetCurrentStage(name));
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(g_ModuleInterface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Log::SetSinks(MMAPI::Log::Sinks::Console | MMAPI::Log::Sinks::File);

	MMAPI::Player::Enable();
	MMAPI::Player::Hooks::BeforeHealthChange(BeforeHealthChange);

	MMAPI::Input::Enable();
	MMAPI::Anchor::Enable();
	MMAPI::Anchor::Hooks::BeforeBeginStep(OnBeginStep);

	MMAPI::NPC::Enable();
	MMAPI::NPC::Hooks::BeforeActivityChange(OnBeforeActivityChange);

	MMAPI::T2::Enable();
	MMAPI::Schedule::Enable();

	MMAPI::Quest::Enable();
	MMAPI::Quest::Hooks::BeforeQuestStart(OnBeforeQuestStart);
	MMAPI::Quest::Hooks::AfterQuestComplete(OnAfterQuestComplete);

	MMAPI::Log::Info("Plugin started! Press F8 to dump active quests; accept/complete quests to test hooks.");
	return AURIE_SUCCESS;
}
