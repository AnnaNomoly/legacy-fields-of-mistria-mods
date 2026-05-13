#include <random>
#include <string>
#include <string_view>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Variant -----

static const char* const FULL_RESTORE_VARIANT_NAME = "Full Restore";
static const char* const GROWTH_SPELL_VARIANT_NAME = "Growth Spell";
static const char* const SUMMON_RAIN_VARIANT_NAME  = "Summon Rain";
static const char* const FIRE_BREATH_VARIANT_NAME  = "Fire Breath";

// CHANGE PER VARIANT BUILD!
static const char* const MOD_NAME    = "QuakeSpell";
static const char* const MOD_VARIANT = FULL_RESTORE_VARIANT_NAME;
static const char* const VERSION     = "1.3.0";

static MMAPI::Spell::Ids GetVariantSpell()
{
	std::string_view v = MOD_VARIANT;
	if (v == FULL_RESTORE_VARIANT_NAME) return MMAPI::Spell::Ids::FullRestore;
	if (v == GROWTH_SPELL_VARIANT_NAME) return MMAPI::Spell::Ids::Growth;
	if (v == SUMMON_RAIN_VARIANT_NAME)  return MMAPI::Spell::Ids::SummonRain;
	if (v == FIRE_BREATH_VARIANT_NAME)  return MMAPI::Spell::Ids::FireBreath;
	return MMAPI::Spell::Ids::FullRestore;
}

// ----- Config -----

static const char* const SPELL_COST_KEY        = "spell_cost";
static const char* const DAMAGE_ARI_KEY        = "damage_ari";
static const char* const IGNORE_MIMICS_KEY     = "ignore_mimics";
static const char* const SOUND_EFFECTS_KEY     = "sound_effects";
static const char* const SCREEN_FLASH_KEY      = "screen_flash";
static const char* const AFTERBURN_KEY         = "afterburn";
static const char* const ONE_SHOT_MONSTERS_KEY = "one_shot_monsters";
static const char* const MINIMUM_MAGNITUDE_KEY = "minimum_magnitude";
static const char* const MAXIMUM_MAGNITUDE_KEY = "maximum_magnitude";

static const int  DEFAULT_SPELL_COST_VALUE        = 2;
static const bool DEFAULT_DAMAGE_ARI_VALUE        = true;
static const bool DEFAULT_IGNORE_MIMICS_VALUE     = false;
static const bool DEFAULT_SOUND_EFFECTS_VALUE     = true;
static const bool DEFAULT_SCREEN_FLASH_VALUE      = true;
static const bool DEFAULT_AFTERBURN_VALUE         = true;
static const bool DEFAULT_ONE_SHOT_MONSTERS_VALUE = true;
static const int  DEFAULT_MINIMUM_MAGNITUDE_VALUE = 5;
static const int  DEFAULT_MAXIMUM_MAGNITUDE_VALUE = 9;

struct QuakeSpellConfig
{
	int  spell_cost        = DEFAULT_SPELL_COST_VALUE;
	bool damage_ari        = DEFAULT_DAMAGE_ARI_VALUE;
	bool ignore_mimics     = DEFAULT_IGNORE_MIMICS_VALUE;
	bool sound_effects     = DEFAULT_SOUND_EFFECTS_VALUE;
	bool screen_flash      = DEFAULT_SCREEN_FLASH_VALUE;
	bool afterburn         = DEFAULT_AFTERBURN_VALUE;
	bool one_shot_monsters = DEFAULT_ONE_SHOT_MONSTERS_VALUE;
	int  minimum_magnitude = DEFAULT_MINIMUM_MAGNITUDE_VALUE;
	int  maximum_magnitude = DEFAULT_MAXIMUM_MAGNITUDE_VALUE;
};

void to_json(json& j, const QuakeSpellConfig& c)
{
	j = json{
		{ SPELL_COST_KEY,        c.spell_cost        },
		{ DAMAGE_ARI_KEY,        c.damage_ari        },
		{ IGNORE_MIMICS_KEY,     c.ignore_mimics     },
		{ SOUND_EFFECTS_KEY,     c.sound_effects     },
		{ SCREEN_FLASH_KEY,      c.screen_flash      },
		{ AFTERBURN_KEY,         c.afterburn         },
		{ ONE_SHOT_MONSTERS_KEY, c.one_shot_monsters },
		{ MINIMUM_MAGNITUDE_KEY, c.minimum_magnitude },
		{ MAXIMUM_MAGNITUDE_KEY, c.maximum_magnitude },
	};
}

void from_json(const json& j, QuakeSpellConfig& c)
{
	c.spell_cost        = MMAPI::Config::GetValue<int> (j, SPELL_COST_KEY,        DEFAULT_SPELL_COST_VALUE, 0, 12);
	c.damage_ari        = MMAPI::Config::GetValue<bool>(j, DAMAGE_ARI_KEY,        DEFAULT_DAMAGE_ARI_VALUE);
	c.ignore_mimics     = MMAPI::Config::GetValue<bool>(j, IGNORE_MIMICS_KEY,     DEFAULT_IGNORE_MIMICS_VALUE);
	c.sound_effects     = MMAPI::Config::GetValue<bool>(j, SOUND_EFFECTS_KEY,     DEFAULT_SOUND_EFFECTS_VALUE);
	c.screen_flash      = MMAPI::Config::GetValue<bool>(j, SCREEN_FLASH_KEY,      DEFAULT_SCREEN_FLASH_VALUE);
	c.afterburn         = MMAPI::Config::GetValue<bool>(j, AFTERBURN_KEY,         DEFAULT_AFTERBURN_VALUE);
	c.one_shot_monsters = MMAPI::Config::GetValue<bool>(j, ONE_SHOT_MONSTERS_KEY, DEFAULT_ONE_SHOT_MONSTERS_VALUE);
	c.minimum_magnitude = MMAPI::Config::GetValue<int> (j, MINIMUM_MAGNITUDE_KEY, DEFAULT_MINIMUM_MAGNITUDE_VALUE, 1, 9);
	c.maximum_magnitude = MMAPI::Config::GetValue<int> (j, MAXIMUM_MAGNITUDE_KEY, DEFAULT_MAXIMUM_MAGNITUDE_VALUE, 1, 9);

	if (c.minimum_magnitude > c.maximum_magnitude)
	{
		c.minimum_magnitude = DEFAULT_MINIMUM_MAGNITUDE_VALUE;
		c.maximum_magnitude = DEFAULT_MAXIMUM_MAGNITUDE_VALUE;
	}
}

// ----- State -----

static QuakeSpellConfig config{};
static bool startup_loaded = false;

static std::mt19937 rng(std::random_device{}());

// Transient per-cast state — cleared via ResetTransientState (setup_main_screen / end_of_day / room transitions).
static int      quake_magnitude          = 0;
static bool     quake_spell_active       = false;
static bool     modify_mana_pending      = false;
static bool     modify_health_pending    = false;
static bool     afterburn_pending        = false;
static bool     afterburn_active         = false;
static bool     screen_flash_pending     = false;
static bool     screen_flash_initialized = false;
static uint64_t screen_flash_start_time  = 0;
static bool     rumble_sound_pending     = false;
static bool     rumble_sound_initialized = false;
static uint64_t rumble_sound_start_time  = 0;

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
				MMAPI::Log::Error("No readable values found in configuration file: %s!", path.string().c_str());
			config = QuakeSpellConfig{};
		}
		else
		{
			config = j.get<QuakeSpellConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = QuakeSpellConfig{};
	}
}

// Toggles persistent fire breath. Two separate fields gate the ability:
//   1. `__ari.fire_breath_time` — the countdown the game checks to allow breathing fire.
//   2. The FlameBreath status effect's `finish` — Ari's UI buff indicator.
// Both must be set together (to infinity or 0) for the buff to persist visibly and functionally.
void AfterburnToggle(bool enable)
{
	YYTK::RValue ari = *MMAPI::Internal::global_instance->GetRefMember("__ari");
	double value = enable ? static_cast<double>(MMAPI::StatusEffect::InfiniteDuration) : 0.0;

	*ari.GetRefMember("fire_breath_time") = value;

	YYTK::RValue status_effects = *ari.GetRefMember("status_effects");
	YYTK::RValue effects        = *status_effects.GetRefMember("effects");
	YYTK::RValue inner          = *effects.GetRefMember("inner");

	std::string flame_breath_key = std::to_string(static_cast<int>(MMAPI::StatusEffect::Ids::FlameBreath));
	if (!MMAPI::Engine::StructVariableExists(inner, flame_breath_key.c_str()))
		return;

	YYTK::RValue flame_breath = *inner.GetRefMember(flame_breath_key.c_str());
	*flame_breath.GetRefMember("finish") = value;
}

void ResetTransientState()
{
	if (afterburn_active)
		AfterburnToggle(false);

	quake_magnitude          = 0;
	quake_spell_active       = false;
	modify_mana_pending      = false;
	modify_health_pending    = false;
	afterburn_pending        = false;
	afterburn_active         = false;
	screen_flash_pending     = false;
	screen_flash_initialized = false;
	screen_flash_start_time  = 0;
	rumble_sound_pending     = false;
	rumble_sound_initialized = false;
	rumble_sound_start_time  = 0;
}

void DamageMonsterInstance(CInstance* self)
{
	if (!self) return;

	YYTK::RValue self_rv = self->ToRValue();
	// Per-quake idempotency tag so we don't damage the same instance twice during the same cast.
	if (MMAPI::Engine::StructVariableExists(self_rv, "__quake_spell__processed"))
		return;
	if (!MMAPI::Engine::StructVariableExists(self_rv, "hit_points"))
		return;

	YYTK::RValue hp = self_rv.GetMember("hit_points");
	if (hp.m_Kind == YYTK::VALUE_UNSET || hp.m_Kind == YYTK::VALUE_UNDEFINED)
		return;

	if (config.one_shot_monsters)
	{
		*self_rv.GetRefMember("hit_points") = 0.0;
	}
	else
	{
		double remaining = hp.ToDouble() - quake_magnitude * 10.0;
		*self_rv.GetRefMember("hit_points") = (std::max)(0.0, remaining);
	}

	MMAPI::Engine::StructVariableSet(self_rv, "__quake_spell__processed", true);
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	if (!startup_loaded)
	{
		LoadOrCreateConfigFile();
		// The variant spell's normal mana cost is overwritten so the cast_spell flow deducts the
		// configured amount even when no afterburn substitution is in play.
		MMAPI::Spell::SetCost(GetVariantSpell(), config.spell_cost);
		startup_loaded = true;
	}

	ResetTransientState();
}

void OnAfterEndDay()
{
	ResetTransientState();
}

void OnBeforeGoToRoom(MMAPI::Location::BeforeGoToRoomContext& /*ctx*/)
{
	// Mirrors the original fade_out trigger: clear transient quake state on room transitions so the
	// spell can be cast again in the next room and the flash/rumble timers don't bleed across.
	ResetTransientState();
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& /*ctx*/)
{
	if (afterburn_active)
	{
		afterburn_active = false;
		AfterburnToggle(false);
	}
}

void OnAfterCanCastSpell(MMAPI::Spell::CanCastSpellContext& ctx)
{
	if (ctx.GetSpell() != GetVariantSpell()) return;

	bool in_dungeon = MMAPI::Location::IsCurrentLocation(MMAPI::Location::Ids::Dungeon);
	YYTK::RValue mana_rv = MMAPI::Player::GetMana();
	int current_mana = MMAPI::Engine::IsNumeric(mana_rv) ? static_cast<int>(mana_rv.ToInt64()) : 0;

	ctx.SetResult(in_dungeon && !quake_spell_active && current_mana >= config.spell_cost);
}

void OnBeforeSpellCast(MMAPI::Spell::BeforeSpellCastContext& ctx)
{
	if (ctx.GetSpell() != GetVariantSpell()) return;

	// Drop HP of every ladder_candidate breakable in the room to 0.
	MMAPI::Room::ForEachObjectNode([](CInstance* /*instance*/, YYTK::RValue node) {
		if (MMAPI::Object::IsLadderCandidate(node))
			MMAPI::Object::SetNodeHitpoints(node, 0);
	});

	std::uniform_int_distribution<int> dist(config.minimum_magnitude, config.maximum_magnitude);
	quake_magnitude = dist(rng);

	modify_mana_pending = true;
	quake_spell_active  = true;
	if (config.damage_ari)    modify_health_pending = true;
	if (config.screen_flash)  screen_flash_pending  = true;
	if (config.sound_effects) rumble_sound_pending  = true;

	MMAPI::Log::Info("Quake spell cast! Magnitude: %d", quake_magnitude);

	if (config.afterburn)
	{
		afterburn_pending = true;
		// Run fire_breath's cast logic so the FlameBreath status effect gets registered with normal
		// finish time; AfterSpellCast then patches both fire_breath_time and finish to infinity.
		ctx.SetSpell(MMAPI::Spell::Ids::FireBreath);
	}
	else
	{
		// No afterburn — the spell's intrinsic cast effect is fully replaced by the quake.
		ctx.Cancel();
	}
}

void OnAfterSpellCast(MMAPI::Spell::AfterSpellCastContext& ctx)
{
	if (!afterburn_pending) return;
	if (ctx.GetSpell() != MMAPI::Spell::Ids::FireBreath) return;

	afterburn_pending = false;
	afterburn_active  = true;
	AfterburnToggle(true);
}

void OnBeforeManaChange(MMAPI::Player::BeforeManaChangeContext& ctx)
{
	if (!modify_mana_pending) return;
	modify_mana_pending = false;
	ctx.SetAmount(-static_cast<double>(config.spell_cost));
}

void OnAriTick(CInstance* /*self*/)
{
	if (!modify_health_pending || quake_magnitude <= 0) return;
	modify_health_pending = false;

	int dmg = quake_magnitude * 10;
	MMAPI::Player::ModifyHealth(-dmg);
	MMAPI::Log::Info("Ari took %d damage from the quake!", dmg);
}

void OnMonsterTick(CInstance* self)
{
	if (!quake_spell_active) return;
	DamageMonsterInstance(self);
}

void OnMonsterMimicTick(CInstance* self)
{
	if (!quake_spell_active) return;
	if (config.ignore_mimics) return;
	DamageMonsterInstance(self);
}

void OnAfterDrawGui()
{
	uint64_t now = MMAPI::Internal::GetCurrentSystemTime();

	if (screen_flash_pending)
	{
		if (!screen_flash_initialized)
		{
			screen_flash_start_time  = now;
			screen_flash_initialized = true;
			MMAPI::Engine::PlaySoundEffect("snd_AriLowHealthWarning", 100, 1.0);
		}

		if (now < screen_flash_start_time + 50)
		{
			MMAPI::Internal::module_interface->CallBuiltin("draw_set_color", { 255 });
			double w = MMAPI::Engine::GetWindowWidth();
			double h = MMAPI::Engine::GetWindowHeight();
			MMAPI::Internal::module_interface->CallBuiltin("draw_rectangle", { 0, 0, w, h, false });
		}
		else
		{
			screen_flash_pending     = false;
			screen_flash_initialized = false;
			screen_flash_start_time  = 0;
		}
	}

	if (rumble_sound_pending)
	{
		if (!rumble_sound_initialized)
		{
			rumble_sound_start_time  = now;
			rumble_sound_initialized = true;
		}

		if (now < rumble_sound_start_time + 1000)
		{
			if      (now % 10 == 0) MMAPI::Engine::PlaySoundEffect("snd_EarthbreakerRockBreak1", 50, 1.0);
			else if (now %  7 == 0) MMAPI::Engine::PlaySoundEffect("snd_EarthbreakerRockBreak2", 50, 1.0);
			else if (now %  5 == 0) MMAPI::Engine::PlaySoundEffect("snd_EarthbreakerRockBreak3", 50, 1.0);
		}
		else
		{
			rumble_sound_pending     = false;
			rumble_sound_initialized = false;
			rumble_sound_start_time  = 0;
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

	module_interface->Print(CM_LIGHTAQUA, "[%s (%s) %s] - Plugin starting...", MOD_NAME, MOD_VARIANT, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Game::Enable();
	MMAPI::Location::Enable();
	MMAPI::Player::Enable();
	MMAPI::Spell::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterEndDay(OnAfterEndDay);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Location::Hooks::BeforeGoToRoom(OnBeforeGoToRoom);
	MMAPI::Spell::Hooks::AfterCanCastSpell(OnAfterCanCastSpell);
	MMAPI::Spell::Hooks::BeforeSpellCast(OnBeforeSpellCast);
	MMAPI::Spell::Hooks::AfterSpellCast(OnAfterSpellCast);
	MMAPI::Player::Hooks::BeforeManaChange(OnBeforeManaChange);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari,          OnAriTick);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Monster,      OnMonsterTick);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::MonsterMimic, OnMonsterMimicTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
