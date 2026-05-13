#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "MuteAnySound";
static const char* const VERSION  = "1.1.0";

// ----- Config -----

static const char* const CONFIG_KEY_MUTED_SOUNDS = "muted_sounds";

// ----- State -----

static bool startup_loaded = false;

// Cached asset indices for the muted sound names. Resolved once on first title-screen setup so the
// per-frame mute check skips the asset_get_index lookup.
static std::vector<YYTK::RValue> muted_sound_indices;

// ----- Config -----

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	std::vector<std::string> muted_sounds;
	if (config.contains(CONFIG_KEY_MUTED_SOUNDS) && config[CONFIG_KEY_MUTED_SOUNDS].is_array())
		muted_sounds = config[CONFIG_KEY_MUTED_SOUNDS].get<std::vector<std::string>>();

	muted_sound_indices.clear();
	muted_sound_indices.reserve(muted_sounds.size());
	for (const std::string& sound_name : muted_sounds)
	{
		YYTK::RValue index = MMAPI::Engine::AssetGetIndex(sound_name);
		if (index.m_Kind == YYTK::VALUE_UNDEFINED || index.m_Kind == YYTK::VALUE_UNSET || index.m_Kind == YYTK::VALUE_NULL)
		{
			MMAPI::Log::Warn("Sound \"%s\" did not resolve to a valid asset; skipped.", sound_name.c_str());
			continue;
		}
		muted_sound_indices.push_back(index);
	}

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_MUTED_SOUNDS] = muted_sounds;
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Muting %zu sounds.", muted_sound_indices.size());
}

// ----- Hooks -----

void OnAfterDrawGui()
{
	for (YYTK::RValue& sound_index : muted_sound_indices)
		if (MMAPI::Engine::AudioIsPlaying(sound_index))
			MMAPI::Engine::StopSoundEffect(sound_index);
}

void OnBeforeSetupMainScreen()
{
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
	MMAPI::Display::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Display::Hooks::AfterDrawGui(OnAfterDrawGui);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
