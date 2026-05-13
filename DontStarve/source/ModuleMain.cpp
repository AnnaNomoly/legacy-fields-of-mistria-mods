#include <algorithm>
#include <cmath>
#include <deque>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "Mistbloom";
static const char* const VERSION = "1.1.0";

// Per-save mod data keys
static const char* const HUNGER_VALUE_KEY = "hunger";
static const char* const SANITY_VALUE_KEY = "sanity";
static const char* const FOOD_QUEUE_KEY   = "food_queue";

// Timing constants (seconds)
static const int MAX_FONT_INDEX                = 2;     // font_get_fontname crashes above this as of v0.15.0
static const int SIX_AM_IN_SECONDS             = 21600;
static const int EIGHT_PM_IN_SECONDS           = 72000;
static const int END_OF_DAY_IN_SECONDS         = 93600;
static const int ONE_MINUTE_IN_SECONDS         = 60;
static const int THIRTY_MINUTES_IN_SECONDS     = 1800;
static const int ONE_HUNDRED_FOURTY_FOUR_SECONDS = 144;   // 25 sanity lost per hour
static const int ONE_HOUR_IN_SECONDS           = 3600;

// Mod tuning constants
static const int HUNGER_LOST_PER_TICK          = -1;
static const int SANITY_LOST_PER_TICK          = -1;
static const int HUNGER_HEALTH_LOST_PER_TICK   = -10;
static const int SANITY_HEALTH_LOST_PER_TICK   = -1;
static const int STARTING_HUNGER_VALUE         = 100;
static const int STARTING_SANITY_VALUE         = 100;
static const int CTHUVIAN_SANITY_THRESHOLD     = 30;
static const int SPICE_OF_LIFE_QUEUE_SIZE      = 10;
static const int BLOOD_PACT_HEALTH_REDUCTION   = 50;
static const int ADRENALINE_RUSH_SANITY_RECOVERY = 5;

// Custom asset names
static const std::string SURVIVAL_RATIONS              = "survival_rations";
static const std::string MISTBLOOM_POTION              = "mistbloom_potion";
static const std::string MISTBLOOM_INTRODUCTION_LETTER = "mistbloom_introduction";
static const std::string HUD_FONT_NAME                 = "PixelMplus12";  // Changed after v0.15.0

// Items that bypass the food-queue / spice-of-life system.
static const std::vector<std::string> FOOD_QUEUE_IGNORED_ITEMS = {
	"balors_crate", "confiscated_coffee", "dungeon_fountain_health", "dungeon_fountain_stamina",
	"horse_potion", "lurid_colored_drink", "ryis_lumber", "soup_of_the_day", "soup_of_the_day_gold",
	"stinky_stamina_potion", "unusual_seed", "world_fountain",
	SURVIVAL_RATIONS, MISTBLOOM_POTION,
};

// Locations where Ari loses sanity when outside (at night, in storms, etc.).
static const std::map<MMAPI::Location::Ids, bool> LOCATION_SANITY_LOSS = {
	{ MMAPI::Location::Ids::Aldaria,         true },
	{ MMAPI::Location::Ids::Beach,           true },
	{ MMAPI::Location::Ids::DeepWoods,       true },
	{ MMAPI::Location::Ids::Dungeon,         true },
	{ MMAPI::Location::Ids::EarthSeal,       true },
	{ MMAPI::Location::Ids::EasternRoad,     true },
	{ MMAPI::Location::Ids::Farm,            true },
	{ MMAPI::Location::Ids::FireSeal,        true },
	{ MMAPI::Location::Ids::HaydensFarm,     true },
	{ MMAPI::Location::Ids::Narrows,         true },
	{ MMAPI::Location::Ids::SeridiasChamber, true },
	{ MMAPI::Location::Ids::Summit,          true },
	{ MMAPI::Location::Ids::Town,            true },
	{ MMAPI::Location::Ids::WaterSeal,       true },
	{ MMAPI::Location::Ids::WesternRuins,    true },
};

// ----- State -----

static bool debug_logging = false;

// Persisted (per-save)
static int  ari_hunger_value = STARTING_HUNGER_VALUE;
static int  ari_sanity_value = STARTING_SANITY_VALUE;
static std::deque<std::string> food_queue = {};
static std::string save_prefix;

// Session
static bool game_is_active                    = false;
static bool startup_loaded                    = false;
static bool startup_localized                 = false;
static bool is_inclement_weather              = false;
static bool is_heavy_inclement_weather        = false;
static bool is_hunger_tracked_time_interval   = false;
static bool is_sanity_tracked_time_interval   = false;
static bool snapshot_position                 = false;
static int  time_of_last_hunger_tick          = 0;
static int  time_of_last_sanity_tick          = 0;
static int  time_of_last_priestess_shield_tick = 0;
static bool is_priestess_shield_tracked_time_interval = false;
static bool priestess_shield_active           = false;
static int  hunger_stamina_health_penalty     = 0;
static int  held_item_id                      = -1;
static int  rollback_position_x               = -1;
static int  rollback_position_y               = -1;
static int  survival_rations_item_id          = -1;
static int  mistbloom_potion_item_id          = -1;

// HUD / Window state
static int  font_index   = 0;
static int  hud_y_offset = 100;
static int  window_width = 0;
static int  window_height = 0;

// Lookup tables (populated at game-active)
static std::map<std::string, int> item_name_to_restoration_map;       // edible items (non-cooked)
static std::map<std::string, int> recipe_name_to_stars_map;           // cooked dishes
static std::map<std::string, double> item_name_to_original_stamina_recovery_map;
static std::map<std::string, std::string> localized_item_name_to_internal_name_map;
static std::string last_localized_item_name;                          // captured from AfterGetDisplayName

// Insanity noise overlay
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<int> distribution_1_25(1, 25);
static std::uniform_int_distribution<int> distribution_1_1000(1, 1000);
static std::vector<std::vector<std::vector<int>>> noise_masks;
static const int total_noise_masks = 100;
static int       current_noise_mask = 0;

// Sanity threshold below which the insanity overlay appears.
static const int INSANITY_OVERLAY_THRESHOLD = 70;

// -------- Helpers --------

bool AriIsHungry()                  { return ari_hunger_value <= 0; }
bool AriIsInsane()                  { return ari_sanity_value <= 0; }
bool IsNight(int clock_seconds)     { return clock_seconds >= EIGHT_PM_IN_SECONDS; }

bool AriIsAtSanityLossLocation()
{
	MMAPI::Location::Ids current;
	if (!MMAPI::Location::TryGetCurrentLocation(current)) return false;
	auto it = LOCATION_SANITY_LOSS.find(current);
	return it != LOCATION_SANITY_LOSS.end() && it->second;
}

int ItemIdFor(const std::string& internal_name)
{
	YYTK::RValue id = MMAPI::Item::GetIdFromInternalName(internal_name);
	return MMAPI::Engine::IsNumeric(id) ? static_cast<int>(id.ToInt64()) : -1;
}

std::string ItemInternalName(int item_id)
{
	YYTK::RValue name = MMAPI::Item::GetInternalName(item_id);
	return (name.m_Kind == YYTK::VALUE_STRING) ? name.ToString() : std::string();
}

bool AriHasCosmeticEquipped(const std::string& cosmetic_name)
{
	YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
	YYTK::RValue preset_index = ari.GetMember("preset_index_selected");
	YYTK::RValue presets      = ari.GetMember("presets");
	YYTK::RValue buffer       = presets.GetMember("__buffer");

	YYTK::RValue* preset = nullptr;
	MMAPI::Internal::module_interface->GetArrayEntry(buffer, static_cast<size_t>(preset_index.ToInt64()), preset);
	if (!preset) return false;

	YYTK::RValue assets       = preset->GetMember("assets");
	YYTK::RValue inner_buffer = assets.GetMember("__buffer");
	size_t inner_size = 0;
	MMAPI::Internal::module_interface->GetArraySize(inner_buffer, inner_size);

	for (size_t i = 0; i < inner_size; i++)
	{
		YYTK::RValue* equipped = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(inner_buffer, i, equipped);
		if (!equipped) continue;
		if (equipped->GetMember("name").ToString() == cosmetic_name) return true;
	}
	return false;
}

// Sets a per-item stamina_modifier on the global item-data table. Not in MMAPI's Item helpers.
void SetItemStaminaModifier(int item_id, double stamina_value)
{
	YYTK::RValue item_data = MMAPI::Internal::global_instance->GetMember("__item_data");
	YYTK::RValue item = MMAPI::Internal::module_interface->CallBuiltin("array_get", { item_data, item_id });
	MMAPI::Engine::StructVariableSet(item, "stamina_modifier", stamina_value);
}

void ResetAllItemStaminaModifiers()
{
	for (const auto& [internal_name, original] : item_name_to_original_stamina_recovery_map)
	{
		int item_id = ItemIdFor(internal_name);
		if (item_id >= 0) SetItemStaminaModifier(item_id, original);
	}
}

// -------- Food queue (spice of life) --------

void UpdateFoodQueue(const std::string& item_name)
{
	if (food_queue.size() == SPICE_OF_LIFE_QUEUE_SIZE)
		food_queue.pop_front();
	food_queue.push_back(item_name);

	if (debug_logging)
	{
		std::string s = "[";
		for (size_t i = 0; i < food_queue.size(); i++)
		{
			s += food_queue[i];
			if (i != food_queue.size() - 1) s += ", ";
		}
		s += "]";
		MMAPI::Log::Debug("Food queue updated: %s", s.c_str());
	}
}

int GetFoodQueueOccurrences(const std::string& item_name)
{
	return static_cast<int>(std::count(food_queue.begin(), food_queue.end(), item_name));
}

double GetFoodPenalty(const std::string& item_name, int occurrence_offset)
{
	int occurrences = GetFoodQueueOccurrences(item_name) + occurrence_offset;
	if (occurrences <= 1) return 0.0;
	if (occurrences == 2) return 0.1;
	if (occurrences == 3) return 0.3;
	if (occurrences == 4) return 0.5;
	if (occurrences == 5) return 0.7;
	return 0.9;  // occurrences >= 6
}

// -------- Per-save mod data persistence --------

void WriteModFile()
{
	if (save_prefix.empty()) return;
	MMAPI::ModSave::Write(save_prefix, json{
		{ HUNGER_VALUE_KEY, ari_hunger_value },
		{ SANITY_VALUE_KEY, ari_sanity_value },
		{ FOOD_QUEUE_KEY,   food_queue },
	});
	if (debug_logging)
		MMAPI::Log::Info("Saved hunger=%d sanity=%d", ari_hunger_value, ari_sanity_value);
}

void ReadModFile()
{
	json data = MMAPI::ModSave::Read(save_prefix);
	if (data.empty()) return;
	ari_hunger_value = data.value(HUNGER_VALUE_KEY, STARTING_HUNGER_VALUE);
	ari_sanity_value = data.value(SANITY_VALUE_KEY, STARTING_SANITY_VALUE);
	food_queue       = data.value(FOOD_QUEUE_KEY, std::deque<std::string>{});
	if (debug_logging)
		MMAPI::Log::Info("Loaded hunger=%d sanity=%d", ari_hunger_value, ari_sanity_value);
}

// -------- Priestess shield (Blood Pact perk) --------

void ActivatePriestessShield()
{
	YYTK::RValue current = MMAPI::Player::GetInvulnerabilityHits();
	if (MMAPI::Engine::IsNumeric(current) && current.ToInt64() == 0)
	{
		MMAPI::Player::SetInvulnerabilityHits(1);
		MMAPI::StatusEffect::RegisterPersistent(MMAPI::StatusEffect::Ids::GuardiansShield);
		priestess_shield_active = true;
	}
}

void CancelPriestessShield()
{
	YYTK::RValue current = MMAPI::Player::GetInvulnerabilityHits();
	if (MMAPI::Engine::IsNumeric(current) && current.ToInt64() > 0)
		MMAPI::Player::SetInvulnerabilityHits(0);
	MMAPI::StatusEffect::Cancel(MMAPI::StatusEffect::Ids::GuardiansShield);
	priestess_shield_active = false;
}

bool AriHasInvulnerabilityHits()
{
	YYTK::RValue current = MMAPI::Player::GetInvulnerabilityHits();
	return MMAPI::Engine::IsNumeric(current) && current.ToInt64() > 0;
}

// -------- HUD font + scaling (specialized — kept inline) --------

void ResolveHudFontIndex()
{
	for (int i = 0; i <= MAX_FONT_INDEX; i++)
	{
		YYTK::RValue name = MMAPI::Internal::module_interface->CallBuiltin("font_get_fontname", { i });
		if (name.m_Kind == YYTK::VALUE_STRING && std::string(name.ToString()).find(HUD_FONT_NAME) != std::string::npos)
		{
			font_index = i;
			return;
		}
	}
	MMAPI::Log::Warn("Failed to find HUD font index for: %s", HUD_FONT_NAME.c_str());
}

void AdjustHudScaling()
{
	YYTK::RValue settings = MMAPI::Internal::global_instance->GetMember("__settings");
	YYTK::RValue inner    = settings.GetMember("inner");
	YYTK::RValue open_fs  = inner.GetMember("open_fscreen");

	auto set_offset = [&](const std::vector<std::pair<int, int>>& tiers) {
		double expansion = (open_fs.m_Real == 1)
			? inner.GetMember("fscreen_expansion").ToDouble()
			: inner.GetMember("window_expansion").ToDouble();
		for (const auto& [exp, off] : tiers)
			if (static_cast<int>(expansion) <= exp) { hud_y_offset = off; return; }
		hud_y_offset = tiers.empty() ? 100 : tiers.back().second;
	};

	if (open_fs.m_Real == 1)
	{
		// Borderless/fullscreen — bracket by width.
		if      (window_width <= 1680) set_offset({{0,100},{1,25}});
		else if (window_width <= 1760) set_offset({{0,160},{1,100}});
		else if (window_width <= 1920) set_offset({{0,160},{1,100},{2,25}});
		else if (window_width <= 2048) set_offset({{0,160},{1,100},{2,25}});
		else if (window_width <= 2560) set_offset({{0,300},{1,225},{2,160},{3,100}});
		else if (window_width <= 3072) set_offset({{0,365},{1,300},{2,225},{3,160}});
		else if (window_width <= 3200) set_offset({{0,365},{1,300},{2,225},{3,160},{4,100}});
		else if (window_width <= 3840) set_offset({{0,500},{1,425},{2,365},{3,300},{4,225},{5,160}});
	}
	else
	{
		if      (window_width == 2560) set_offset({{0,160},{1,100},{2,25}});
		else if (window_width == 1920) set_offset({{0,160},{1,100},{2,25}});
		else if (window_width <= 1400) set_offset({{0,100},{1,25}});
	}
}

void GenerateNoiseMasks(bool refresh_window_size)
{
	if (refresh_window_size)
	{
		window_width  = static_cast<int>(MMAPI::Engine::GetWindowWidth());
		window_height = static_cast<int>(MMAPI::Engine::GetWindowHeight());
	}

	MMAPI::Log::Info("Generating random noise for insanity overlay (may take several seconds)...");
	noise_masks.clear();
	noise_masks.reserve(total_noise_masks);
	for (int m = 0; m < total_noise_masks; m++)
	{
		std::vector<std::vector<int>> noise;
		for (int i = 0; i < window_width; i++)
			for (int j = 0; j < window_height; j++)
				if (distribution_1_1000(gen) == 1000)
					noise.push_back({ i, j });
		noise_masks.push_back(std::move(noise));
	}
}

void DrawBarSegment(int x1, int y1, int x2, int y2, int color)
{
	MMAPI::Internal::module_interface->CallBuiltin("draw_set_color", { color });
	MMAPI::Internal::module_interface->CallBuiltin("draw_rectangle", { x1, y1, x2, y2, false });
}

void DrawHudBar(int y_offset, int value, int color, const char* icon_sprite_name, const char* border_sprite_name)
{
	YYTK::RValue icon   = MMAPI::Engine::AssetGetIndex(icon_sprite_name);
	YYTK::RValue border = MMAPI::Engine::AssetGetIndex(border_sprite_name);

	MMAPI::Internal::module_interface->CallBuiltin("draw_sprite", { icon, 1, 10, 122 + y_offset });

	int x1 = 50 + 9;
	int y1 = 115 + 5 + y_offset;
	int x2 = x1 + value * 2;
	int y2 = 115 + 40 + y_offset;
	DrawBarSegment(x1, y1, x2, y2, color);
	DrawBarSegment(x2 + 1, y1, x2 + 1 + (100 - value) * 2, y2, 0);

	MMAPI::Internal::module_interface->CallBuiltin("draw_sprite", { border, 1, 50, 115 + y_offset });

	MMAPI::Internal::module_interface->CallBuiltin("draw_set_color", { 16777215 });
	MMAPI::Internal::module_interface->CallBuiltin("draw_set_font",  { font_index });
	MMAPI::Internal::module_interface->CallBuiltin(
		"draw_text_transformed",
		{ 140, 125 + y_offset, YYTK::RValue(std::to_string(value) + "%"), 3, 3, 0 }
	);
}

void DrawInsanityOverlay()
{
	if (noise_masks.empty()) return;

	// Semi-transparent dark-grey wash over the whole screen. Alpha ramps from 0.3 at the
	// threshold (sanity=70) up to 0.7 capped (anywhere below sanity=30).
	double overlay_alpha = static_cast<double>(100 - ari_sanity_value) / 100.0;
	if (overlay_alpha > 0.7) overlay_alpha = 0.7;

	MMAPI::Internal::module_interface->CallBuiltin("draw_set_alpha", { overlay_alpha });
	MMAPI::Internal::module_interface->CallBuiltin("draw_set_color", { 4210752 });
	MMAPI::Internal::module_interface->CallBuiltin("draw_rectangle", { 0, 0, window_width, window_height, false });

	// Black noise rectangles. Density scales with sanity: at sanity≤10 every point in the mask is
	// drawn; at sanity≤70 only every 64th point. Tiers below come straight from the original.
	MMAPI::Internal::module_interface->CallBuiltin("draw_set_alpha", { 1.0 });
	MMAPI::Internal::module_interface->CallBuiltin("draw_set_color", { 0 });

	int stride = 1;
	if      (ari_sanity_value <= 10) stride = 1;
	else if (ari_sanity_value <= 20) stride = 2;
	else if (ari_sanity_value <= 30) stride = 4;
	else if (ari_sanity_value <= 40) stride = 8;
	else if (ari_sanity_value <= 50) stride = 16;
	else if (ari_sanity_value <= 60) stride = 32;
	else                              stride = 64;  // sanity <= 70

	const auto& mask = noise_masks[current_noise_mask];
	for (size_t i = 0; i < mask.size(); i += stride)
	{
		int x = mask[i][0];
		int y = mask[i][1];
		MMAPI::Internal::module_interface->CallBuiltin("draw_rectangle", { x - 2, y - 2, x + 2, y + 2, false });
	}

	// Cycle to the next pre-generated mask for the next frame.
	current_noise_mask = (current_noise_mask + 1) % total_noise_masks;
}

// -------- Hooks --------

void LoadReferenceData()
{
	// One-time data load from the game tables. Matches the original mod's setup_main_screen
	// first-time-run block — done at title-screen setup so the player doesn't see the
	// noise-mask generation hitch when gameplay starts.
	survival_rations_item_id = ItemIdFor(SURVIVAL_RATIONS);
	mistbloom_potion_item_id = ItemIdFor(MISTBLOOM_POTION);

	YYTK::RValue item_data = MMAPI::Internal::global_instance->GetMember("__item_data");
	size_t count = 0;
	MMAPI::Internal::module_interface->GetArraySize(item_data, count);
	for (size_t i = 0; i < count; i++)
	{
		YYTK::RValue* entry = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(item_data, i, entry);
		if (!entry) continue;

		YYTK::RValue name_key = entry->GetMember("name_key");
		if (name_key.m_Kind == YYTK::VALUE_UNDEFINED || name_key.m_Kind == YYTK::VALUE_UNSET || name_key.m_Kind == YYTK::VALUE_NULL)
			continue;

		std::string recipe_key = entry->GetMember("recipe_key").ToString();
		YYTK::RValue stamina_modifier = entry->GetMember("stamina_modifier");

		if (name_key.ToString().contains("cooked_dishes"))
		{
			// Only treat the entry as a cooked dish if it actually has a recipe struct (the
			// original mod guards on this to avoid mis-tagging dish-categorized items that
			// the game ships without a recipe).
			YYTK::RValue recipe = entry->GetMember("recipe");
			bool has_recipe = recipe.m_Kind != YYTK::VALUE_UNDEFINED
			               && recipe.m_Kind != YYTK::VALUE_UNSET
			               && recipe.m_Kind != YYTK::VALUE_NULL;
			if (has_recipe)
			{
				YYTK::RValue stars = entry->GetMember("stars");
				int star_count = MMAPI::Engine::IsNumeric(stars) ? static_cast<int>(stars.ToDouble()) : 1;
				recipe_name_to_stars_map[recipe_key] = star_count;
			}
			if (stamina_modifier.m_Kind == YYTK::VALUE_REAL)
				item_name_to_original_stamina_recovery_map[recipe_key] = stamina_modifier.ToDouble();
		}
		else if (entry->GetMember("edible").ToBoolean())
		{
			item_name_to_restoration_map[recipe_key] = static_cast<int>(stamina_modifier.ToDouble());
			if (stamina_modifier.m_Kind == YYTK::VALUE_REAL)
				item_name_to_original_stamina_recovery_map[recipe_key] = stamina_modifier.ToDouble();
		}
	}

	ResolveHudFontIndex();
	GenerateNoiseMasks(true);
}

void OnSetupMainScreen()
{
	if (!startup_loaded)
	{
		// First title-screen visit of the session — populate the reference tables before the
		// player picks a save. Matches the original mod's behavior; pre-pays the noise-mask
		// generation cost during the title screen rather than at first weather fire.
		LoadReferenceData();
		startup_loaded = true;
		return;
	}

	// Return-to-title reset (subsequent visits).
	ari_hunger_value = STARTING_HUNGER_VALUE;
	ari_sanity_value = STARTING_SANITY_VALUE;
	food_queue.clear();
	hunger_stamina_health_penalty = 0;
	save_prefix.clear();
	game_is_active = false;
	held_item_id = -1;
	is_heavy_inclement_weather = false;
	is_hunger_tracked_time_interval = false;
	is_inclement_weather = false;
	is_priestess_shield_tracked_time_interval = false;
	is_sanity_tracked_time_interval = false;
	last_localized_item_name.clear();
	priestess_shield_active = false;
	rollback_position_x = -1;
	rollback_position_y = -1;
	snapshot_position = false;
	time_of_last_hunger_tick = 0;
	time_of_last_priestess_shield_tick = 0;
	time_of_last_sanity_tick = 0;
	ResetAllItemStaminaModifiers();
}

void OnGameActive()
{
	game_is_active = true;

	// Idempotent one-shot setup: ensure Ari has the SURVIVAL_RATIONS recipe and the intro mail.
	int survival_id = ItemIdFor(SURVIVAL_RATIONS);
	if (survival_id >= 0) MMAPI::Recipe::Unlock(survival_id, false);
	if (!MMAPI::Mail::Exists(MISTBLOOM_INTRODUCTION_LETTER))
		MMAPI::Mail::SendMail(MISTBLOOM_INTRODUCTION_LETTER);
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	if (save_prefix.empty())
		save_prefix = ctx.GetSavePrefix();
	WriteModFile();
}

void OnAfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	save_prefix = ctx.GetSavePrefix();
	ReadModFile();
}

void OnAfterClockUpdate(MMAPI::Calendar::ClockUpdateContext& /*ctx*/)
{
	if (!game_is_active) return;

	int now = MMAPI::Game::GetCurrentTimeInSeconds();

	// Latch first observed time as the baseline so day-start doesn't trigger an immediate tick.
	if (time_of_last_hunger_tick == 0) time_of_last_hunger_tick = now;
	if (time_of_last_sanity_tick == 0) time_of_last_sanity_tick = now;

	// At end of day the clock wraps; reset baselines to 6 AM.
	if (time_of_last_hunger_tick == END_OF_DAY_IN_SECONDS) time_of_last_hunger_tick = SIX_AM_IN_SECONDS;
	if (time_of_last_sanity_tick == END_OF_DAY_IN_SECONDS) time_of_last_sanity_tick = SIX_AM_IN_SECONDS;

	bool ari_is_in_dungeon = MMAPI::Dungeon::IsDungeonRoom();

	// Hunger tick every 30 minutes of game time, skipping the first 30 minutes of the day.
	if (!is_hunger_tracked_time_interval
	    && now >= SIX_AM_IN_SECONDS + THIRTY_MINUTES_IN_SECONDS
	    && now - time_of_last_hunger_tick >= THIRTY_MINUTES_IN_SECONDS)
	{
		is_hunger_tracked_time_interval = true;
		time_of_last_hunger_tick = now;
		ari_hunger_value = (std::max)(0, ari_hunger_value + HUNGER_LOST_PER_TICK);
	}

	// Sanity tick every 144s (25 sanity/hr).
	if (!is_sanity_tracked_time_interval
	    && now - time_of_last_sanity_tick >= ONE_HUNDRED_FOURTY_FOUR_SECONDS)
	{
		is_sanity_tracked_time_interval = true;
		time_of_last_sanity_tick = now;

		bool sanity_lost = false;
		if (ari_is_in_dungeon)
		{
			ari_sanity_value = (std::max)(0, ari_sanity_value + SANITY_LOST_PER_TICK);
			sanity_lost = true;
		}
		else
		{
			if (IsNight(now) && AriIsAtSanityLossLocation())
			{
				ari_sanity_value = (std::max)(0, ari_sanity_value + SANITY_LOST_PER_TICK);
				sanity_lost = true;
			}
			if ((is_inclement_weather || is_heavy_inclement_weather) && AriIsAtSanityLossLocation())
			{
				bool drop = false;
				if (is_heavy_inclement_weather)
					drop = true;
				else  // light inclement
				{
					if (!MMAPI::Perk::IsActive(MMAPI::Perk::Ids::Refreshing))
						drop = true;
					else if (!AriHasCosmeticEquipped("head_rain_hat"))
						drop = true;
				}
				if (drop)
				{
					ari_sanity_value = (std::max)(0, ari_sanity_value + SANITY_LOST_PER_TICK);
					sanity_lost = true;
				}
			}
		}

		// Recover sanity if no loss tick fired.
		if (!sanity_lost)
			ari_sanity_value = (std::min)(STARTING_SANITY_VALUE, ari_sanity_value - SANITY_LOST_PER_TICK);
	}

	// Snapshot Ari's position every minute (used to roll her back when insanity teleports).
	if (now % ONE_MINUTE_IN_SECONDS == 0)
		snapshot_position = true;

	// Priestess Shield (Blood Pact perk) ticks once per hour while in the dungeon.
	if (!ari_is_in_dungeon)
	{
		time_of_last_priestess_shield_tick = now;
		is_priestess_shield_tracked_time_interval = false;
	}
	else if (!is_priestess_shield_tracked_time_interval
	         && now - time_of_last_priestess_shield_tick >= ONE_HOUR_IN_SECONDS)
	{
		is_priestess_shield_tracked_time_interval = true;
		time_of_last_priestess_shield_tick = now;
	}
}

void OnBeforeHealthChange(MMAPI::Player::BeforeHealthChangeContext& ctx)
{
	double amount = ctx.GetAmount();

	// Health gained from an item (food/potion): also restore hunger/sanity per item kind.
	if (amount >= 0)
	{
		std::string held = ItemInternalName(held_item_id);
		if (held.empty()) return;

		if (held == SURVIVAL_RATIONS)
			ari_hunger_value = (std::min)(100, ari_hunger_value + 35);
		else if (held == MISTBLOOM_POTION)
			ari_sanity_value = (std::min)(100, ari_sanity_value + 50);
		else
		{
			int hunger_modifier = 0;
			if (recipe_name_to_stars_map.count(held) > 0)
				hunger_modifier = 2 * (recipe_name_to_stars_map[held] * 10);
			else if (item_name_to_restoration_map.count(held) > 0)
				hunger_modifier = 2 * item_name_to_restoration_map[held];

			if (hunger_modifier > 0)
				ari_hunger_value = (std::min)(100, ari_hunger_value + hunger_modifier);
		}
	}
	// Damage in the dungeon also bleeds sanity.
	else if (amount < 0 && MMAPI::Dungeon::IsDungeonRoom())
	{
		ari_sanity_value = (std::max)(0, ari_sanity_value + static_cast<int>(amount));
	}
}

void OnBeforeStaminaChange(MMAPI::Player::BeforeStaminaChangeContext& ctx)
{
	double amount = ctx.GetAmount();

	if (amount >= 0)
	{
		// Stamina recovery from consuming an item.
		std::string held = ItemInternalName(held_item_id);
		bool is_recipe_or_edible = !held.empty()
			&& (recipe_name_to_stars_map.count(held) > 0 || item_name_to_restoration_map.count(held) > 0);

		if (!is_recipe_or_edible)
		{
			// Non-food stamina recovery: treat as hunger gain at 2x.
			ari_hunger_value = (std::min)(100, ari_hunger_value + static_cast<int>(2 * amount));
		}
		else
		{
			// Food: spice-of-life penalty applied to stamina (unless on the ignore list).
			if (std::find(FOOD_QUEUE_IGNORED_ITEMS.begin(), FOOD_QUEUE_IGNORED_ITEMS.end(), held) == FOOD_QUEUE_IGNORED_ITEMS.end())
			{
				UpdateFoodQueue(held);
				double penalty = GetFoodPenalty(held, 0);
				double adjusted = std::trunc((1.0 - penalty) * amount);
				if (adjusted == 0) adjusted = 1.0;
				ctx.SetAmount(adjusted);
			}
		}
	}
	else
	{
		// Stamina cost: apply weather penalty multipliers, then drain hunger.
		double cost = amount;
		if (!MMAPI::Dungeon::IsDungeonRoom() && AriIsAtSanityLossLocation())
		{
			bool nice_swing = MMAPI::Perk::IsActive(MMAPI::Perk::Ids::NiceSwing);
			if (is_heavy_inclement_weather) cost *= nice_swing ? 2 : 3;
			else if (is_inclement_weather)  cost *= nice_swing ? 1 : 2;
		}
		ctx.SetAmount(cost);

		// If stamina cost would push hunger below zero, drain health instead.
		int new_hunger = ari_hunger_value + static_cast<int>(cost);
		if (new_hunger < 0)
		{
			ari_hunger_value = 0;
			hunger_stamina_health_penalty += new_hunger;
		}
		else
		{
			ari_hunger_value = new_hunger;
		}
	}
}

void OnAfterDrawGui()
{
	if (!game_is_active || MMAPI::Game::IsPaused()) return;

	int sanity_bar_offset = 50;
	DrawHudBar(hud_y_offset,                       ari_hunger_value, 4235519, "spr_ui_hud_hunger_bar_icon", "spr_ui_hud_hunger_bar");
	DrawHudBar(hud_y_offset + sanity_bar_offset,   ari_sanity_value, 8388736, "spr_ui_hud_sanity_bar_icon", "spr_ui_hud_hunger_bar");

	if (ari_sanity_value <= INSANITY_OVERLAY_THRESHOLD)
		DrawInsanityOverlay();
}

void OnAfterHeldItem(MMAPI::Player::HeldItemContext& ctx)
{
	int id = ctx.GetItemId();
	if (id != held_item_id) held_item_id = id;
}

void OnBeforeUseItem(MMAPI::Item::UseItemContext& ctx)
{
	int item_id = ctx.GetItemId();
	if (item_id < 0) return;

	// When a food item starts being used, restore its original stamina modifier — the AfterGetDisplayDescription
	// hook may have just lowered it to apply the spice-of-life penalty. The use_item script uses the
	// stamina_modifier at the moment of use, so we reset to the original first.
	std::string name = ItemInternalName(item_id);
	if (!name.empty()
	    && (recipe_name_to_stars_map.count(name) > 0 || item_name_to_restoration_map.count(name) > 0)
	    && item_name_to_original_stamina_recovery_map.count(name) > 0)
	{
		SetItemStaminaModifier(item_id, item_name_to_original_stamina_recovery_map[name]);
	}
}

void OnAfterRoomStart(MMAPI::Weather::AfterRoomStartContext& /*ctx*/)
{
	snapshot_position = true;
}

void OnAfterGetWeather(MMAPI::Weather::AfterGetWeatherContext& ctx)
{
	MMAPI::Weather::Ids w = ctx.GetWeather();
	is_heavy_inclement_weather = (w == MMAPI::Weather::Ids::HeavyInclement);
	is_inclement_weather       = (w == MMAPI::Weather::Ids::Inclement) || is_heavy_inclement_weather;
}

void OnBeforePlayText(MMAPI::Text::PlayTextContext& ctx)
{
	// Cthuvian speak — when Ari is severely insane, replace the textbox key with one of 25 random
	// Cthuvian conversation entries.
	if (ari_sanity_value <= CTHUVIAN_SANITY_THRESHOLD)
	{
		int r = distribution_1_25(gen);
		ctx.SetKey("Conversations/Mods/Mistbloom/Cthuvian/" + std::to_string(r));
	}
}

void OnAfterEndDay()
{
	// Daily reset of session flags + restore sanity to full at the start of the new day. Matches
	// the original mod's ResetStaticFields(false) plus the post-end_day sanity reset.
	// Note: `hunger_stamina_health_penalty` is intentionally NOT reset here — it's a pending
	// debt consumed on the next obj_ari tick, and the original only zeroes it at title return.
	held_item_id = -1;
	is_heavy_inclement_weather = false;
	is_hunger_tracked_time_interval = false;
	is_inclement_weather = false;
	is_priestess_shield_tracked_time_interval = false;
	is_sanity_tracked_time_interval = false;
	last_localized_item_name.clear();
	priestess_shield_active = false;
	rollback_position_x = -1;
	rollback_position_y = -1;
	snapshot_position = false;
	time_of_last_hunger_tick = 0;
	time_of_last_priestess_shield_tick = 0;
	time_of_last_sanity_tick = 0;
	ari_sanity_value = STARTING_SANITY_VALUE;
}

void OnAfterShouldDie(MMAPI::Player::AfterShouldDieContext& ctx)
{
	if (ctx.GetWillDie())
	{
		ari_hunger_value = STARTING_HUNGER_VALUE;
		ari_sanity_value = STARTING_SANITY_VALUE;
	}
}

void OnAfterGetDisplayName(MMAPI::Item::GetDisplayNameContext& ctx)
{
	last_localized_item_name = ctx.GetResolved();
}

void OnAfterGetDisplayDescription(MMAPI::Item::GetDisplayDescriptionContext& ctx)
{
	if (last_localized_item_name.empty()) return;
	auto it = localized_item_name_to_internal_name_map.find(last_localized_item_name);
	if (it == localized_item_name_to_internal_name_map.end()) return;

	const std::string& internal_name = it->second;

	// Prepend "Recently Eaten: N" if applicable.
	int occurrences = GetFoodQueueOccurrences(internal_name);
	if (occurrences > 0)
	{
		std::string resolved(ctx.GetResolved());
		ctx.SetResolved("Recently Eaten: " + std::to_string(occurrences) + "\n\n" + resolved);
	}

	// Apply the spice-of-life stamina-recovery penalty to the item's modifier so the inventory
	// preview reflects how much stamina the next use will actually restore.
	auto orig_it = item_name_to_original_stamina_recovery_map.find(internal_name);
	if (orig_it != item_name_to_original_stamina_recovery_map.end())
	{
		int item_id = ItemIdFor(internal_name);
		if (item_id >= 0)
		{
			double penalty = GetFoodPenalty(internal_name, 1);
			double adjusted = std::trunc((1.0 - penalty) * orig_it->second);
			if (adjusted == 0) adjusted = 1.0;
			SetItemStaminaModifier(item_id, adjusted);
		}
	}
}

void OnBeforeLocalizedString(MMAPI::Text::LocalizedStringContext& /*ctx*/)
{
	// Lazy first-fire: build the localized-name → internal-name reverse map. Localization isn't
	// guaranteed ready at AfterGameActive time, so we defer this to the first localizer call.
	if (startup_localized) return;
	startup_localized = true;

	YYTK::RValue item_data = MMAPI::Internal::global_instance->GetMember("__item_data");
	size_t count = 0;
	MMAPI::Internal::module_interface->GetArraySize(item_data, count);
	for (size_t i = 0; i < count; i++)
	{
		YYTK::RValue* entry = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(item_data, i, entry);
		if (!entry) continue;

		YYTK::RValue name_key = entry->GetMember("name_key");
		if (name_key.m_Kind != YYTK::VALUE_STRING) continue;

		std::string name_key_str = name_key.ToString();
		std::string recipe_key   = entry->GetMember("recipe_key").ToString();

		bool is_cooked = name_key_str.find("cooked_dishes") != std::string::npos;
		bool is_edible = entry->GetMember("edible").ToBoolean();
		if (is_cooked || is_edible)
		{
			YYTK::RValue localized = MMAPI::Text::GetLocalizedString(name_key_str);
			if (localized.m_Kind == YYTK::VALUE_STRING)
				localized_item_name_to_internal_name_map[localized.ToString()] = recipe_key;
		}
	}
}

void OnAfterDisplayResize(MMAPI::Display::DisplayResizeContext& ctx)
{
	bool regenerate = false;
	int new_w = static_cast<int>(ctx.m_window_width);
	int new_h = static_cast<int>(ctx.m_window_height);
	if (window_width != 0 && window_width != new_w)  { regenerate = true; window_width  = new_w; }
	if (window_height != 0 && window_height != new_h) { regenerate = true; window_height = new_h; }
	if (regenerate) GenerateNoiseMasks(false);
	AdjustHudScaling();
}

void OnAriTick(CInstance* self)
{
	bool ari_is_in_dungeon = MMAPI::Dungeon::IsDungeonRoom();

	// Hunger tick consequence: take damage when hunger is empty.
	if (is_hunger_tracked_time_interval)
	{
		if (AriIsHungry())
			MMAPI::Player::ModifyHealth(HUNGER_HEALTH_LOST_PER_TICK);
		is_hunger_tracked_time_interval = false;
	}

	// Sanity tick consequence: take damage when sanity is empty.
	if (is_sanity_tracked_time_interval)
	{
		if (AriIsInsane())
			MMAPI::Player::ModifyHealth(SANITY_HEALTH_LOST_PER_TICK);
		is_sanity_tracked_time_interval = false;
	}

	// If the stamina drain overflowed hunger, charge the remainder to health.
	if (hunger_stamina_health_penalty < 0)
	{
		MMAPI::Player::ModifyHealth(hunger_stamina_health_penalty);
		hunger_stamina_health_penalty = 0;
	}

	// Snapshot Ari's position once per "snapshot due" flag (driven by the clock + on_room_start).
	if (snapshot_position && !MMAPI::Game::IsPaused())
	{
		YYTK::RValue x; MMAPI::Internal::module_interface->GetBuiltin("x", self, NULL_INDEX, x);
		YYTK::RValue y; MMAPI::Internal::module_interface->GetBuiltin("y", self, NULL_INDEX, y);
		rollback_position_x = static_cast<int>(x.ToDouble());
		rollback_position_y = static_cast<int>(y.ToDouble());
		snapshot_position = false;
	}

	// Insanity teleport: while insane, occasionally roll Ari back to her last snapshotted position.
	if (!MMAPI::Game::IsPaused() && AriIsInsane() && distribution_1_1000(gen) == 1000)
	{
		YYTK::RValue x = rollback_position_x;
		YYTK::RValue y = rollback_position_y;
		MMAPI::Internal::module_interface->SetBuiltin("x", self, NULL_INDEX, x);
		MMAPI::Internal::module_interface->SetBuiltin("y", self, NULL_INDEX, y);
	}

	// Blood Pact: clamp HP to (max - 50) while the perk is active.
	if (MMAPI::Perk::IsActive(MMAPI::Perk::Ids::GuardiansShield))
	{
		YYTK::RValue max_hp = MMAPI::Player::GetMaxHealth();
		YYTK::RValue cur_hp = MMAPI::Player::GetHealth();
		if (MMAPI::Engine::IsNumeric(max_hp) && MMAPI::Engine::IsNumeric(cur_hp))
		{
			double effective_max = max_hp.ToDouble() - BLOOD_PACT_HEALTH_REDUCTION;
			if (cur_hp.ToDouble() > effective_max)
				MMAPI::Player::SetHealth(static_cast<int>(effective_max));
		}

		// Periodic shield re-arm in the dungeon.
		if (is_priestess_shield_tracked_time_interval)
		{
			ActivatePriestessShield();
			is_priestess_shield_tracked_time_interval = false;
		}

		// Cancel shield outside the dungeon (or once hits have been consumed).
		if (priestess_shield_active && (!AriHasInvulnerabilityHits() || !ari_is_in_dungeon))
			CancelPriestessShield();
	}
	else
	{
		if (priestess_shield_active || AriHasInvulnerabilityHits())
			CancelPriestessShield();
		is_priestess_shield_tracked_time_interval = false;
	}
}

void OnMonsterTick(CInstance* self)
{
	if (!MMAPI::Perk::IsActive(MMAPI::Perk::Ids::GenerousInDefeat)) return;

	YYTK::RValue self_rv = self->ToRValue();
	if (MMAPI::Engine::StructVariableExists(self_rv, "__dont_starve__processed_monster_death")
	    && self_rv.GetMember("__dont_starve__processed_monster_death").ToBoolean())
		return;

	if (!MMAPI::Engine::StructVariableExists(self_rv, "hit_points")) return;
	YYTK::RValue hp = self_rv.GetMember("hit_points");
	if (hp.m_Kind != YYTK::VALUE_REAL || hp.ToDouble() > 0) return;

	MMAPI::Engine::StructVariableSet(self_rv, "__dont_starve__processed_monster_death", true);
	ari_sanity_value = (std::min)(STARTING_SANITY_VALUE, ari_sanity_value + ADRENALINE_RUSH_SANITY_RECOVERY);
}


EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)(module_interface));
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Calendar::Enable();
	MMAPI::Display::Enable();
	MMAPI::Dungeon::Enable();
	MMAPI::Item::Enable();
	MMAPI::Location::Enable();
	MMAPI::Player::Enable();
	MMAPI::Recipe::Enable();
	MMAPI::StatusEffect::Enable();
	MMAPI::Text::Enable();
	MMAPI::Weather::Enable();
	MMAPI::Game::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Game::Hooks::AfterLoadGame(OnAfterLoadGame);
	MMAPI::Game::Hooks::AfterEndDay(OnAfterEndDay);
	MMAPI::Game::Hooks::AfterDrawGui(OnAfterDrawGui);
	MMAPI::Calendar::Hooks::AfterClockUpdate(OnAfterClockUpdate);
	MMAPI::Player::Hooks::BeforeHealthChange(OnBeforeHealthChange);
	MMAPI::Player::Hooks::BeforeStaminaChange(OnBeforeStaminaChange);
	MMAPI::Player::Hooks::AfterHeldItem(OnAfterHeldItem);
	MMAPI::Player::Hooks::AfterShouldDie(OnAfterShouldDie);
	MMAPI::Item::Hooks::BeforeUseItem(OnBeforeUseItem);
	MMAPI::Item::Hooks::AfterGetDisplayName(OnAfterGetDisplayName);
	MMAPI::Item::Hooks::AfterGetDisplayDescription(OnAfterGetDisplayDescription);
	MMAPI::Text::Hooks::BeforeLocalizedString(OnBeforeLocalizedString);
	MMAPI::Text::Hooks::BeforePlayText(OnBeforePlayText);
	MMAPI::Weather::Hooks::AfterRoomStart(OnAfterRoomStart);
	MMAPI::Weather::Hooks::AfterGetWeather(OnAfterGetWeather);
	MMAPI::Display::Hooks::AfterDisplayResize(OnAfterDisplayResize);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari,     OnAriTick);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Monster, OnMonsterTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
