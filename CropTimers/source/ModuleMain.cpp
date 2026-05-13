#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----

static const char* const MOD_NAME = "CropTimers";
static const char* const VERSION  = "1.2.0";

// ----- Config keys + defaults -----

static const char* const TIMERS_START_VISIBLE_KEY  = "timers_start_visible";
static const char* const ONLY_SHOW_TIMER_ZERO_KEY  = "only_show_timer_zero";
static const char* const RED_TIMER_ZERO_KEY        = "red_timer_zero";
static const char* const ACTIVATION_BUTTON_KEY     = "activation_button";

static const bool        DEFAULT_TIMERS_START_VISIBLE = true;
static const bool        DEFAULT_ONLY_SHOW_TIMER_ZERO = false;
static const bool        DEFAULT_RED_TIMER_ZERO       = true;
static const std::string DEFAULT_ACTIVATION_BUTTON    = "T";

// Crop renderers' prototype.category_id == 3. (The original mod loaded the full
// __object_category__ map but never actually consulted it — the comparison was always against
// the hardcoded constant. Dropped the map, kept the constant.)
static constexpr int CROP_CATEGORY_ID = 3;

// ----- Config -----

struct CropTimersConfig
{
	bool        timers_start_visible = DEFAULT_TIMERS_START_VISIBLE;
	bool        only_show_timer_zero = DEFAULT_ONLY_SHOW_TIMER_ZERO;
	bool        red_timer_zero       = DEFAULT_RED_TIMER_ZERO;
	std::string activation_button    = DEFAULT_ACTIVATION_BUTTON;
};

void to_json(json& j, const CropTimersConfig& c)
{
	j = json{
		{ TIMERS_START_VISIBLE_KEY, c.timers_start_visible },
		{ ONLY_SHOW_TIMER_ZERO_KEY, c.only_show_timer_zero },
		{ RED_TIMER_ZERO_KEY,       c.red_timer_zero       },
		{ ACTIVATION_BUTTON_KEY,    c.activation_button    },
	};
}

void from_json(const json& j, CropTimersConfig& c)
{
	c.timers_start_visible = MMAPI::Config::GetValue<bool>(j, TIMERS_START_VISIBLE_KEY, DEFAULT_TIMERS_START_VISIBLE);
	c.only_show_timer_zero = MMAPI::Config::GetValue<bool>(j, ONLY_SHOW_TIMER_ZERO_KEY, DEFAULT_ONLY_SHOW_TIMER_ZERO);
	c.red_timer_zero       = MMAPI::Config::GetValue<bool>(j, RED_TIMER_ZERO_KEY,       DEFAULT_RED_TIMER_ZERO);
	c.activation_button    = MMAPI::Config::GetValue<std::string>(j, ACTIVATION_BUTTON_KEY, DEFAULT_ACTIVATION_BUTTON);

	if (!MMAPI::Input::TryParseKeybind(c.activation_button))
	{
		MMAPI::Log::Error("Invalid \"%s\" value (%s) — not one of the supported keys", ACTIVATION_BUTTON_KEY, c.activation_button.c_str());
		c.activation_button = DEFAULT_ACTIVATION_BUTTON;
	}
}

// ----- State -----

static CropTimersConfig config{};
static bool startup_loaded         = false;
static bool timers_visible         = true;
static bool processing_user_input  = false;
static std::optional<MMAPI::Input::Keybind> activation_keybind;

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
				MMAPI::Log::Error("No readable values in configuration file: %s!", path.string().c_str());
			config = CropTimersConfig{};
		}
		else
		{
			config = j.get<CropTimersConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = CropTimersConfig{};
	}
}

bool IsCropOverlayActiveLocation()
{
	return MMAPI::Location::IsCurrentLocation(MMAPI::Location::Ids::Farm)
	    || MMAPI::Location::IsCurrentLocation(MMAPI::Location::Ids::SmallGreenhouse)
	    || MMAPI::Location::IsCurrentLocation(MMAPI::Location::Ids::LargeGreenhouse);
}

bool ShouldOverlayTimers()
{
	return timers_visible && IsCropOverlayActiveLocation();
}

// Returns true if the sprite name belongs to a stage/seed sprite for a crop/flower/forage —
// the set we apply timer overlays to. Identifies sprites like "spr_crop_wheat_stage3",
// "spr_flower_marigold_seed", "spr_forage_mushroom_stage1", etc.
bool IsCropStageSprite(std::string_view sprite_name)
{
	bool is_crop_family = sprite_name.find("spr_crop")   != std::string_view::npos
	                   || sprite_name.find("spr_flower") != std::string_view::npos
	                   || sprite_name.find("spr_forage") != std::string_view::npos;
	if (!is_crop_family) return false;

	return sprite_name.find("seed")  != std::string_view::npos
	    || sprite_name.find("stage") != std::string_view::npos;
}

// Computes the days-remaining timer for a crop node. Returns std::nullopt if the node isn't a
// timer-eligible crop or if it should be skipped under the current config (e.g. only_show_timer_zero
// with a non-zero remainder).
std::optional<int> ComputeCropDaysRemaining(YYTK::RValue node)
{
	if (!MMAPI::Engine::StructVariableExists(node, "prototype")) return std::nullopt;
	YYTK::RValue prototype = node.GetMember("prototype");
	if (prototype.m_Kind != YYTK::VALUE_OBJECT) return std::nullopt;

	YYTK::RValue category_id = prototype.GetMember("category_id");
	if (!MMAPI::Engine::IsNumeric(category_id)) return std::nullopt;
	if (category_id.ToInt64() != CROP_CATEGORY_ID) return std::nullopt;

	int day_count = static_cast<int>(node.GetMember("day_count").ToInt64());
	bool regrow_cycle = node.GetMember("regrow_cycle").ToBoolean();

	int total_days;
	if (!regrow_cycle)
	{
		bool stage_zero_is_seed = prototype.GetMember("stage_zero_is_seed").ToBoolean();
		YYTK::RValue day_to_stage = prototype.GetMember("day_to_stage");
		YYTK::RValue buffer = day_to_stage.GetMember("__buffer");
		size_t array_length = 0;
		MMAPI::Internal::module_interface->GetArraySize(buffer, array_length);
		total_days = static_cast<int>(array_length) - 1;
		if (!stage_zero_is_seed) total_days -= 1;
	}
	else
	{
		YYTK::RValue post_harvest = prototype.GetMember("post_harvest_day_to_stage");
		YYTK::RValue buffer = post_harvest.GetMember("__buffer");
		size_t array_length = 0;
		MMAPI::Internal::module_interface->GetArraySize(buffer, array_length);
		total_days = static_cast<int>(array_length) - 1;
	}

	int days_remaining = total_days - day_count;
	if (days_remaining < 0) days_remaining = 0;

	if (config.only_show_timer_zero && days_remaining != 0)
		return std::nullopt;

	return days_remaining;
}

// Resolves the timer-variant sprite asset for `base_sprite_name` (e.g. "spr_crop_wheat_stage3") at
// the given days-remaining (e.g. 5 → "spr_crop_wheat_stage3_timer_5"; 0 → "..._timer_0_red" when
// red_timer_zero is enabled). Returns a VALUE_UNDEFINED RValue if no asset matches.
YYTK::RValue ResolveTimerSprite(std::string_view base_sprite_name, int days_remaining)
{
	std::string name = std::string(base_sprite_name) + "_timer_" + std::to_string(days_remaining);
	if (days_remaining == 0 && config.red_timer_zero)
		name += "_red";

	return MMAPI::Engine::AssetGetIndex(name);
}

// Strips a "_timer_N[_red]" suffix from `sprite_name`. Returns the bare sprite name (e.g.
// "spr_crop_wheat_stage3_timer_5_red" → "spr_crop_wheat_stage3"). If `sprite_name` doesn't contain
// "_timer_", returns it unchanged.
std::string StripTimerSuffix(std::string sprite_name)
{
	size_t pos = sprite_name.find("_timer_");
	if (pos != std::string::npos)
		sprite_name.erase(pos);
	return sprite_name;
}

void TryApplyTimerSpriteToInstance(CInstance* inst)
{
	if (!inst) return;

	YYTK::RValue sprite_index = MMAPI::Engine::InstanceVariableGet(inst, "sprite_index");
	YYTK::RValue sprite_name_rv = MMAPI::Internal::module_interface->CallBuiltin("sprite_get_name", { sprite_index });
	if (sprite_name_rv.m_Kind != YYTK::VALUE_STRING) return;

	std::string sprite_name = sprite_name_rv.ToString();
	if (!IsCropStageSprite(sprite_name)) return;

	YYTK::RValue node = MMAPI::Object::GetNode(inst);
	if (node.m_Kind == YYTK::VALUE_UNDEFINED) return;

	auto days_remaining = ComputeCropDaysRemaining(node);
	if (!days_remaining) return;

	YYTK::RValue replacement = ResolveTimerSprite(sprite_name, *days_remaining);
	if (replacement.m_Kind == YYTK::VALUE_REF)
		MMAPI::Engine::InstanceVariableSet(inst, "sprite_index", replacement);
}

void RevertTimerSpriteOnInstance(CInstance* inst)
{
	if (!inst) return;

	YYTK::RValue sprite_index = MMAPI::Engine::InstanceVariableGet(inst, "sprite_index");
	YYTK::RValue sprite_name_rv = MMAPI::Internal::module_interface->CallBuiltin("sprite_get_name", { sprite_index });
	if (sprite_name_rv.m_Kind != YYTK::VALUE_STRING) return;

	std::string sprite_name = sprite_name_rv.ToString();
	if (!IsCropStageSprite(sprite_name)) return;
	if (sprite_name.find("_timer_") == std::string::npos) return;

	std::string base_name = StripTimerSuffix(sprite_name);
	YYTK::RValue base_sprite = MMAPI::Engine::AssetGetIndex(base_name);
	if (base_sprite.m_Kind == YYTK::VALUE_REF)
		MMAPI::Engine::InstanceVariableSet(inst, "sprite_index", base_sprite);
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	processing_user_input = false;

	if (!startup_loaded)
	{
		LoadOrCreateConfigFile();
		activation_keybind = MMAPI::Input::TryParseKeybind(config.activation_button);
		timers_visible = config.timers_start_visible;
		startup_loaded = true;
	}
}

// Handles the hotkey press: toggles visibility and immediately applies/reverts all crop sprites in
// the current room. During normal play, the per-instance BeforeNodeRendererSetSprite hook is what
// keeps the overlay applied as crops change stages — this draw-gui pass only fires on the toggle
// edge to retroactively apply/revert sprites that were set before the toggle.
void OnAfterDrawGui()
{
	if (!MMAPI::Engine::WindowHasFocus() || processing_user_input)
		return;

	if (!activation_keybind || !MMAPI::Input::IsKeybindPressed(*activation_keybind))
		return;

	processing_user_input = true;
	timers_visible = !timers_visible;

	if (ShouldOverlayTimers())
	{
		MMAPI::Room::ForEachActiveInstance([](CInstance* inst) {
			TryApplyTimerSpriteToInstance(inst);
		});
	}
	else
	{
		MMAPI::Room::ForEachActiveInstance([](CInstance* inst) {
			RevertTimerSpriteOnInstance(inst);
		});
	}

	processing_user_input = false;
}

// Fires when a crop renderer changes its sprite (e.g. growth-stage advance at day rollover).
// We substitute the about-to-be-set sprite with the timer-variant if timers are visible and the
// node is an eligible crop.
void OnBeforeNodeRendererSetSprite(MMAPI::Object::NodeRendererSetSpriteContext& ctx)
{
	if (!ShouldOverlayTimers()) return;

	YYTK::RValue sprite_asset = ctx.GetSpriteAsset();
	if (sprite_asset.m_Kind != YYTK::VALUE_REF) return;

	YYTK::RValue asset_type = MMAPI::Internal::module_interface->CallBuiltin("asset_get_type", { sprite_asset });
	if (asset_type.ToInt64() != static_cast<int64_t>(MMAPI::Engine::AssetType::Sprite)) return;

	YYTK::RValue sprite_name_rv = MMAPI::Internal::module_interface->CallBuiltin("sprite_get_name", { sprite_asset });
	if (sprite_name_rv.m_Kind != YYTK::VALUE_STRING) return;

	std::string sprite_name = sprite_name_rv.ToString();
	if (!IsCropStageSprite(sprite_name)) return;

	YYTK::RValue node = MMAPI::Object::GetNode(ctx.GetSelf());
	if (node.m_Kind == YYTK::VALUE_UNDEFINED) return;

	auto days_remaining = ComputeCropDaysRemaining(node);
	if (!days_remaining) return;

	YYTK::RValue replacement = ResolveTimerSprite(sprite_name, *days_remaining);
	if (replacement.m_Kind == YYTK::VALUE_REF)
		ctx.SetSpriteAsset(replacement);
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
	MMAPI::Input::Enable();
	MMAPI::Location::Enable();
	MMAPI::Object::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Display::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Object::Hooks::BeforeNodeRendererSetSprite(OnBeforeNodeRendererSetSprite);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
