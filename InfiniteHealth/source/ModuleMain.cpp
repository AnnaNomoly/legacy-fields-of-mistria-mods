#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;

static const char* const MOD_NAME = "InfiniteHealth";
static const char* const VERSION  = "1.1.0";

static YYTKInterface* g_ModuleInterface = nullptr;

static void BeforeHealthChange(MMAPI::Player::BeforeHealthChangeContext& ctx)
{
	if (ctx.GetAmount() < 0)
		ctx.SetAmount(0);
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)g_ModuleInterface);
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(g_ModuleInterface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Player::Enable();
	MMAPI::Player::Hooks::BeforeHealthChange(BeforeHealthChange);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
