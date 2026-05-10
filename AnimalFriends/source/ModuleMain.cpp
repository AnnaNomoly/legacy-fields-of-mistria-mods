#include <filesystem>
#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const VERSION = "1.4.4";
static const char* const FRIENDSHIP_MULTIPLIER_KEY = "friendship_multiplier";
static const char* const AUTO_PET_KEY = "auto_pet";
static const char* const AUTO_FEED_KEY = "auto_feed";
static const char* const PREVENT_FRIENDSHIP_LOSS_KEY = "prevent_friendship_loss";
static const char* const AUTO_BELL_IN_KEY = "auto_bell_in";
static const char* const AUTO_BELL_OUT_KEY = "auto_bell_out";
static const char* const ANIMAL_WAKE_UP_TIME_KEY = "animal_wake_up_time";
static const char* const ANIMAL_BED_TIME_KEY = "animal_bed_time";
static const char* const MUTE_AUTO_BELL_SOUNDS_KEY = "mute_auto_bell_sounds";
static const char* const SPAWN_EXTRA_BEADS_DAILY_KEY = "spawn_extra_beads_daily";
static const char* const EXTRA_BEADS_DAILY_MULTIPLIER_KEY = "extra_beads_daily_multiplier";
static const char* const GAIN_RANCHING_XP_KEY = "gain_ranching_xp";

static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";

static const int DEFAULT_FRIENDSHIP_MULTIPLIER = 5;
static const bool DEFAULT_PREVENT_FRIENDSHIP_LOSS = true;
static const bool DEFAULT_AUTO_PET = false;
static const bool DEFAULT_AUTO_FEED = false;
static const bool DEFAULT_AUTO_BELL_IN = false;
static const bool DEFAULT_AUTO_BELL_OUT = false;
static const bool DEFAULT_MUTE_AUTO_BELL_SOUNDS = false;
static const bool DEFAULT_SPAWN_EXTRA_BEADS_DAILY = false;
static const int DEFAULT_EXTRA_BEADS_DAILY_MULTIPLIER = 1;
static const bool DEFAULT_GAIN_RANCHING_XP = true;

static const int EIGHT_AM_IN_SECONDS = 28800;
static const int SIX_PM_IN_SECONDS = 64800;
static const int MAX_ANIMAL_HEART_POINTS = 1800;
static const int BASE_HEART_POINTS_PER_ACTION = 5;

struct AnimalFriendsConfig
{
	int friendship_multiplier = DEFAULT_FRIENDSHIP_MULTIPLIER;
	bool prevent_friendship_loss = DEFAULT_PREVENT_FRIENDSHIP_LOSS;
	bool auto_pet = DEFAULT_AUTO_PET;
	bool auto_feed = DEFAULT_AUTO_FEED;
	bool auto_bell_in = DEFAULT_AUTO_BELL_IN;
	bool auto_bell_out = DEFAULT_AUTO_BELL_OUT;
	int animal_wake_up_time = EIGHT_AM_IN_SECONDS;
	int animal_bed_time = SIX_PM_IN_SECONDS;
	bool mute_auto_bell_sounds = DEFAULT_MUTE_AUTO_BELL_SOUNDS;
	bool spawn_extra_beads_daily = DEFAULT_SPAWN_EXTRA_BEADS_DAILY;
	int extra_beads_daily_multiplier = DEFAULT_EXTRA_BEADS_DAILY_MULTIPLIER;
	bool gain_ranching_xp = DEFAULT_GAIN_RANCHING_XP;
};

void to_json(json& json_object, const AnimalFriendsConfig& config)
{
	json_object = json{
		{ FRIENDSHIP_MULTIPLIER_KEY, config.friendship_multiplier },
		{ PREVENT_FRIENDSHIP_LOSS_KEY, config.prevent_friendship_loss },
		{ AUTO_PET_KEY, config.auto_pet },
		{ AUTO_FEED_KEY, config.auto_feed },
		{ AUTO_BELL_IN_KEY, config.auto_bell_in },
		{ AUTO_BELL_OUT_KEY, config.auto_bell_out },
		{ ANIMAL_WAKE_UP_TIME_KEY, config.animal_wake_up_time },
		{ ANIMAL_BED_TIME_KEY, config.animal_bed_time },
		{ MUTE_AUTO_BELL_SOUNDS_KEY, config.mute_auto_bell_sounds },
		{ SPAWN_EXTRA_BEADS_DAILY_KEY, config.spawn_extra_beads_daily },
		{ EXTRA_BEADS_DAILY_MULTIPLIER_KEY, config.extra_beads_daily_multiplier },
		{ GAIN_RANCHING_XP_KEY, config.gain_ranching_xp }
	};
}

void from_json(const json& json_object, AnimalFriendsConfig& config)
{
	config.friendship_multiplier = MMAPI::Config::GetValue<int>(json_object, FRIENDSHIP_MULTIPLIER_KEY, DEFAULT_FRIENDSHIP_MULTIPLIER, 1, 100);
	config.prevent_friendship_loss = MMAPI::Config::GetValue<bool>(json_object, PREVENT_FRIENDSHIP_LOSS_KEY, DEFAULT_PREVENT_FRIENDSHIP_LOSS);
	config.auto_pet = MMAPI::Config::GetValue<bool>(json_object, AUTO_PET_KEY, DEFAULT_AUTO_PET);
	config.auto_feed = MMAPI::Config::GetValue<bool>(json_object, AUTO_FEED_KEY, DEFAULT_AUTO_FEED);
	config.auto_bell_in = MMAPI::Config::GetValue<bool>(json_object, AUTO_BELL_IN_KEY, DEFAULT_AUTO_BELL_IN);
	config.auto_bell_out = MMAPI::Config::GetValue<bool>(json_object, AUTO_BELL_OUT_KEY, DEFAULT_AUTO_BELL_OUT);
	config.animal_wake_up_time = MMAPI::Config::GetValue<int>(json_object, ANIMAL_WAKE_UP_TIME_KEY, EIGHT_AM_IN_SECONDS, 21600, 86400);
	config.animal_bed_time = MMAPI::Config::GetValue<int>(json_object, ANIMAL_BED_TIME_KEY, SIX_PM_IN_SECONDS, 21600, 86400);
	config.mute_auto_bell_sounds = MMAPI::Config::GetValue<bool>(json_object, MUTE_AUTO_BELL_SOUNDS_KEY, DEFAULT_MUTE_AUTO_BELL_SOUNDS);
	config.spawn_extra_beads_daily = MMAPI::Config::GetValue<bool>(json_object, SPAWN_EXTRA_BEADS_DAILY_KEY, DEFAULT_SPAWN_EXTRA_BEADS_DAILY);
	config.extra_beads_daily_multiplier = MMAPI::Config::GetValue<int>(json_object, EXTRA_BEADS_DAILY_MULTIPLIER_KEY, DEFAULT_EXTRA_BEADS_DAILY_MULTIPLIER, 1, 10);
	config.gain_ranching_xp = MMAPI::Config::GetValue<bool>(json_object, GAIN_RANCHING_XP_KEY, DEFAULT_GAIN_RANCHING_XP);
}

static YYTKInterface* g_ModuleInterface = nullptr;
static bool load_on_start = true;
static AnimalFriendsConfig config = {};
static bool once_per_day = true;
static int num_player_animals = 0;
static bool extra_beads_daily_received = false;

void handle_eptr(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Error: %s", VERSION, e.what());
	}
}

void LogDefaultConfigValues()
{
	config = AnimalFriendsConfig{};

	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %d!", VERSION, FRIENDSHIP_MULTIPLIER_KEY, config.friendship_multiplier);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, PREVENT_FRIENDSHIP_LOSS_KEY, config.prevent_friendship_loss ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, AUTO_PET_KEY, config.auto_pet ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, AUTO_FEED_KEY, config.auto_feed ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, AUTO_BELL_IN_KEY, config.auto_bell_in ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, AUTO_BELL_OUT_KEY, config.auto_bell_out ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %d!", VERSION, ANIMAL_WAKE_UP_TIME_KEY, config.animal_wake_up_time);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %d!", VERSION, ANIMAL_BED_TIME_KEY, config.animal_bed_time);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, MUTE_AUTO_BELL_SOUNDS_KEY, config.mute_auto_bell_sounds ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, SPAWN_EXTRA_BEADS_DAILY_KEY, config.spawn_extra_beads_daily ? "true" : "false");
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %d!", VERSION, EXTRA_BEADS_DAILY_MULTIPLIER_KEY, config.extra_beads_daily_multiplier);
	g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Using DEFAULT \"%s\" value: %s!", VERSION, GAIN_RANCHING_XP_KEY, config.gain_ranching_xp ? "true" : "false");
}

void LoadOrCreateConfigFile()
{
	std::exception_ptr eptr;
	try
	{
		std::filesystem::path config_file = MMAPI::Config::GetConfigPath("AnimalFriends");
		bool config_file_exists = std::filesystem::exists(config_file);
		json json_object = MMAPI::Config::Load(config_file);

		if (!config_file_exists)
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Configuration file was not found. Creating file: %s", VERSION, config_file.string().c_str());
		}

		if (json_object.empty())
		{
			if (config_file_exists)
			{
				g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - No readable values found in mod configuration file: %s!", VERSION, config_file.string().c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[AnimalFriends %s] - Defaults will be used and written back to the configuration file.", VERSION);
			}

			LogDefaultConfigValues();
			MMAPI::Config::Save(config_file, config);
			return;
		}

		config = json_object.get<AnimalFriendsConfig>();
		if (config.animal_wake_up_time >= config.animal_bed_time)
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Invalid animal schedule in mod configuration file: wake up time must be before bed time.", VERSION);
			g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - Using DEFAULT animal schedule values.", VERSION);
			config.animal_wake_up_time = EIGHT_AM_IN_SECONDS;
			config.animal_bed_time = SIX_PM_IN_SECONDS;
		}

		MMAPI::Config::Save(config_file, config);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - Loaded configuration file: %s", VERSION, config_file.string().c_str());
	}
	catch (...)
	{
		eptr = std::current_exception();
	}

	handle_eptr(eptr);
}

void ResetStaticFields(bool returnedToTitleScreen)
{
	if (returnedToTitleScreen)
	{
		num_player_animals = 0;
	}

	once_per_day = true;
	extra_beads_daily_received = false;
}

void AutoFeedAnimal(RValue animal, int& xp_out)
{
	if (MMAPI::Animal::HasEaten(animal))
		return;

	if (config.gain_ranching_xp)
		xp_out += MMAPI::Animal::GetXpValueOrDefault(MMAPI::Animal::XpValues::Feed, 2);

	MMAPI::Animal::SetHasEaten(animal, true);

	int original = MMAPI::Animal::GetHeartPoints(animal);
	if (original < MAX_ANIMAL_HEART_POINTS)
	{
		int updated = min(original + (BASE_HEART_POINTS_PER_ACTION * config.friendship_multiplier), MAX_ANIMAL_HEART_POINTS);
		MMAPI::Animal::SetHeartPoints(animal, updated);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - AUTO FEEDER fed an animal, and boosted it's heart points from %d to %d!", VERSION, original, updated);
	}
	else
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - AUTO FEEDER fed an animal, but it's already at MAX heart points!", VERSION);
	}
}

void AutoPetAnimal(RValue animal, int& xp_out)
{
	if (config.gain_ranching_xp)
		xp_out += MMAPI::Animal::GetXpValueOrDefault(MMAPI::Animal::XpValues::Pet, 2);

	if (MMAPI::Animal::HasBeenPet(animal))
		return;

	MMAPI::Animal::SetHasBeenPet(animal, true);

	int original = MMAPI::Animal::GetHeartPoints(animal);
	if (original < MAX_ANIMAL_HEART_POINTS)
	{
		int updated = min(original + (BASE_HEART_POINTS_PER_ACTION * config.friendship_multiplier), MAX_ANIMAL_HEART_POINTS);
		MMAPI::Animal::SetHeartPoints(animal, updated);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - AUTO PETTER pet an animal, and boosted it's heart points from %d to %d!", VERSION, original, updated);
	}
	else
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - AUTO PETTER pet an animal, but it's already at MAX heart points!", VERSION);
	}
}

void HandleAri(CInstance* self)
{
	MMAPI::RegisterInstanceContext(
		MMAPI::Instance::Internal::INSTANCE_OBJ_ARI,
		MMAPI::Internal::global_instance->GetRefMember("__ari")->ToInstance(),
		self
	);

	if (!once_per_day || !(config.auto_pet || config.auto_feed))
		return;

	RValue all_animals = MMAPI::Animal::GetAllAnimals();
	if (all_animals.m_Kind != VALUE_OBJECT)
	{
		once_per_day = false;
		return;
	}

	RValue buffer = *all_animals.GetRefMember("__buffer");
	if (buffer.m_Kind != VALUE_ARRAY)
	{
		once_per_day = false;
		return;
	}

	size_t size = 0;
	g_ModuleInterface->GetArraySize(buffer, size);
	num_player_animals = static_cast<int>(size);
	int ranching_xp_gained = 0;

	for (size_t i = 0; i < size; i++)
	{
		RValue entry = buffer[i];
		if (config.auto_feed) AutoFeedAnimal(entry, ranching_xp_gained);
		if (config.auto_pet)  AutoPetAnimal(entry, ranching_xp_gained);
	}

	if (ranching_xp_gained > 0)
	{
		MMAPI::Skill::GainExperience(MMAPI::Skill::Ids::Ranching, ranching_xp_gained);
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - Ari gained %d ranching experience from the AUTO PETTER and AUTO FEEDER!", VERSION, ranching_xp_gained);
	}

	once_per_day = false;
}

void HandleFarmBell(CInstance* self)
{
	int time = MMAPI::Game::GetCurrentTimeInSeconds();
	bool is_sunny = MMAPI::Weather::IsSunny();

	if (config.auto_bell_in)
	{
		if ((time >= config.animal_bed_time || !is_sunny) &&
			!MMAPI::Engine::StructVariableExists(self, "__animal_friends__bell_in"))
		{
			MMAPI::Animal::RingBellIn(self);
			if (config.mute_auto_bell_sounds)
				MMAPI::Engine::StopSoundEffect("snd_Bell_Bring_Inside");
			MMAPI::Engine::StructVariableSet(self, "__animal_friends__bell_in", true);
		}
	}

	if (config.auto_bell_out)
	{
		if (time >= config.animal_wake_up_time && time < config.animal_bed_time && is_sunny &&
			!MMAPI::Engine::StructVariableExists(self, "__animal_friends__bell_out"))
		{
			MMAPI::Animal::RingBellOut(self);
			if (config.mute_auto_bell_sounds)
				MMAPI::Engine::StopSoundEffect("snd_Bell_Bring_Outside");
			MMAPI::Engine::StructVariableSet(self, "__animal_friends__bell_out", true);
		}
	}
}

void ObjectCallback(IN FWCodeEvent& CodeEvent)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self || !self->m_Object || MMAPI::Game::IsPaused())
		return;

	if (MMAPI::Instance::NameContains(self, "obj_ari"))
		HandleAri(self);
	else if (MMAPI::Instance::NameContains(self, "obj_farm_bell"))
		HandleFarmBell(self);
}

void OnAnimalHeartPointsChanged(MMAPI::Animal::HeartPointsChangedContext& ctx)
{
	double amount = ctx.GetAmount();

	if (config.prevent_friendship_loss && amount < 0.0)
	{
		amount = 0.0;
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - Prevented an animal's heart points from being reduced!", VERSION);
	}

	int original_amount = static_cast<int>(amount);
	int modified_amount = static_cast<int>(std::round(amount * config.friendship_multiplier));
	ctx.SetAmount(static_cast<double>(modified_amount));
	if (modified_amount > 0)
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - Boosted the heart points gained for animal from %d to %d!", VERSION, original_amount, modified_amount);
}

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ResetStaticFields(true);
	MMAPI::ClearScriptContexts();

	if (load_on_start)
	{
		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		MMAPI::Initialize(g_ModuleInterface, global_instance, g_ArSelfModule);
		MMAPI::Animal::Hooks::OnHeartPointsChanged(OnAnimalHeartPointsChanged);
		MMAPI::Animal::Hooks::OnPutDown(OnAnimalPutDown);
		MMAPI::Animal::Hooks::OnPet(OnAnimalPet);
		MMAPI::Game::Hooks::OnNewDay(OnNewDay);
		LoadOrCreateConfigFile();
		load_on_start = false;
	}

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

void OnNewDay()
{
	ResetStaticFields(false);
}

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_WEATHER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	MMAPI::RegisterScriptContext(MMAPI::Weather::Internal::GML_SCRIPT_GET_WEATHER, Self, Other);

	return Result;
}

void OnAnimalPutDown(CInstance* animal)
{
	if (config.spawn_extra_beads_daily && !extra_beads_daily_received)
	{
		extra_beads_daily_received = true;
		MMAPI::Animal::SpawnShinyBeads(animal, num_player_animals * config.extra_beads_daily_multiplier);
	}
}

void OnAnimalPet(CInstance* animal)
{
	if (config.spawn_extra_beads_daily && !extra_beads_daily_received)
	{
		extra_beads_daily_received = true;
		MMAPI::Animal::SpawnShinyBeads(animal, num_player_animals * config.extra_beads_daily_multiplier);
	}
}

AurieStatus RegisterHook(const char* script_name, PVOID callback)
{
	CScript* script = nullptr;
	AurieStatus status = g_ModuleInterface->GetNamedRoutinePointer(script_name, reinterpret_cast<PVOID*>(&script));
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Failed to get script (%s)!", VERSION, script_name);
		return status;
	}

	status = MmCreateHook(
		g_ArSelfModule,
		script_name,
		script->m_Functions->m_ScriptFunction,
		callback,
		nullptr
	);

	if (!AurieSuccess(status))
		g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Failed to hook script (%s)!", VERSION, script_name);

	return status;
}

static const struct HookEntry { const char* script_name; PVOID callback; } HOOK_TABLE[] = {
	{ GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(GmlScriptSetupMainScreenCallback) },
	{ GML_SCRIPT_GET_WEATHER,       reinterpret_cast<PVOID>(GmlScriptGetWeatherCallback)      },
};

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[AnimalFriends %s] - Plugin starting...", VERSION);

	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Failed to hook EVENT_OBJECT_CALL!", VERSION);
		g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Exiting due to failure on start!", VERSION);
		return status;
	}

	for (const auto& hook : HOOK_TABLE)
	{
		status = RegisterHook(hook.script_name, hook.callback);
		if (!AurieSuccess(status))
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[AnimalFriends %s] - Exiting due to failure on start!", VERSION);
			return status;
		}
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[AnimalFriends %s] - Plugin started!", VERSION);
	return AURIE_SUCCESS;
}
