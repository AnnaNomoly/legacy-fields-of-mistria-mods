#include <filesystem>
#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "AnimalFriends";
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
		MMAPI::Log::Error("Error: %s", e.what());
	}
}

void LogDefaultConfigValues()
{
	config = AnimalFriendsConfig{};

	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %d!", FRIENDSHIP_MULTIPLIER_KEY, config.friendship_multiplier);
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", PREVENT_FRIENDSHIP_LOSS_KEY, config.prevent_friendship_loss ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", AUTO_PET_KEY, config.auto_pet ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", AUTO_FEED_KEY, config.auto_feed ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", AUTO_BELL_IN_KEY, config.auto_bell_in ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", AUTO_BELL_OUT_KEY, config.auto_bell_out ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %d!", ANIMAL_WAKE_UP_TIME_KEY, config.animal_wake_up_time);
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %d!", ANIMAL_BED_TIME_KEY, config.animal_bed_time);
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", MUTE_AUTO_BELL_SOUNDS_KEY, config.mute_auto_bell_sounds ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", SPAWN_EXTRA_BEADS_DAILY_KEY, config.spawn_extra_beads_daily ? "true" : "false");
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %d!", EXTRA_BEADS_DAILY_MULTIPLIER_KEY, config.extra_beads_daily_multiplier);
	MMAPI::Log::Warn("Using DEFAULT \"%s\" value: %s!", GAIN_RANCHING_XP_KEY, config.gain_ranching_xp ? "true" : "false");
}

void LoadOrCreateConfigFile()
{
	std::exception_ptr eptr;
	try
	{
		std::filesystem::path config_file = MMAPI::Config::GetConfigPath(MOD_NAME);
		bool config_file_exists = std::filesystem::exists(config_file);
		json json_object = MMAPI::Config::Load(config_file);

		if (!config_file_exists)
		{
			MMAPI::Log::Warn("Configuration file was not found. Creating file: %s", config_file.string().c_str());
		}

		if (json_object.empty())
		{
			if (config_file_exists)
			{
				MMAPI::Log::Error("No readable values found in mod configuration file: %s!", config_file.string().c_str());
				MMAPI::Log::Warn("Defaults will be used and written back to the configuration file.");
			}

			LogDefaultConfigValues();
			MMAPI::Config::Save(config_file, config);
			return;
		}

		config = json_object.get<AnimalFriendsConfig>();
		if (config.animal_wake_up_time >= config.animal_bed_time)
		{
			MMAPI::Log::Error("Invalid animal schedule in mod configuration file: wake up time must be before bed time.");
			MMAPI::Log::Info("Using DEFAULT animal schedule values.");
			config.animal_wake_up_time = EIGHT_AM_IN_SECONDS;
			config.animal_bed_time = SIX_PM_IN_SECONDS;
		}

		MMAPI::Config::Save(config_file, config);
		MMAPI::Log::Info("Loaded configuration file: %s", config_file.string().c_str());
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
		MMAPI::Log::Info("AUTO FEEDER fed an animal, and boosted it's heart points from %d to %d!", original, updated);
	}
	else
	{
		MMAPI::Log::Info("AUTO FEEDER fed an animal, but it's already at MAX heart points!");
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
		MMAPI::Log::Info("AUTO PETTER pet an animal, and boosted it's heart points from %d to %d!", original, updated);
	}
	else
	{
		MMAPI::Log::Info("AUTO PETTER pet an animal, but it's already at MAX heart points!");
	}
}

void HandleAri(CInstance* self)
{
	if (!once_per_day || !(config.auto_pet || config.auto_feed))
		return;

	num_player_animals = 0;
	int ranching_xp_gained = 0;

	MMAPI::Animal::ForEachAnimal([&](RValue& animal)
	{
		++num_player_animals;
		if (config.auto_feed) AutoFeedAnimal(animal, ranching_xp_gained);
		if (config.auto_pet)  AutoPetAnimal(animal, ranching_xp_gained);
	});

	if (ranching_xp_gained > 0)
	{
		MMAPI::Skill::GainExperience(MMAPI::Skill::Ids::Ranching, ranching_xp_gained);
		MMAPI::Log::Info("Ari gained %d ranching experience from the AUTO PETTER and AUTO FEEDER!", ranching_xp_gained);
	}

	once_per_day = false;
}

void HandleFarmBell(CInstance* self)
{
	int time = MMAPI::Calendar::GetCurrentTimeInSeconds();
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

void OnAnimalHeartPointsChanged(MMAPI::Animal::HeartPointsChangedContext& ctx)
{
	double amount = ctx.GetAmount();

	if (config.prevent_friendship_loss && amount < 0.0)
	{
		amount = 0.0;
		MMAPI::Log::Info("Prevented an animal's heart points from being reduced!");
	}

	int original_amount = static_cast<int>(amount);
	int modified_amount = static_cast<int>(std::round(amount * config.friendship_multiplier));
	ctx.SetAmount(static_cast<double>(modified_amount));
	if (modified_amount > 0)
		MMAPI::Log::Info("Boosted the heart points gained for animal from %d to %d!", original_amount, modified_amount);
}

void OnNewDay()
{
	ResetStaticFields(false);
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

void OnSetupMainScreen()
{
	ResetStaticFields(true);
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
	MMAPI::Animal::Enable();
	MMAPI::Skill::Enable();
	MMAPI::Weather::Enable();
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, HandleAri);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::FarmBell, HandleFarmBell);
	MMAPI::Animal::Hooks::BeforeHeartPointsChange(OnAnimalHeartPointsChanged);
	MMAPI::Animal::Hooks::AfterPutDown(OnAnimalPutDown);
	MMAPI::Animal::Hooks::AfterPet(OnAnimalPet);
	MMAPI::Game::Hooks::BeforeNewDay(OnNewDay);
	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	LoadOrCreateConfigFile();

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
