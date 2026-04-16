#include "Utils.h"

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}



void ResetFade()
{
	fade_initialized = false;
	fade_start_time = 0;
}

void ResetCustomDrawFields()
{
	show_dashes = false;
	show_danger_banner = false;
	ResetFade();
}

bool GameIsPaused()
{
	RValue paused = global_instance->GetMember("__pause_status");
	return paused.ToInt64() > 0;
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return struct_exists.ToBoolean();
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void StructVariableRemove(RValue the_struct, const char* variable_name)
{
	if (StructVariableExists(the_struct, variable_name))
	{
		RValue struct_exists = g_ModuleInterface->CallBuiltin(
			"struct_remove",
			{ the_struct, variable_name }
		);
	}
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return global_variable_exists.ToBoolean();
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists("__YYTK"))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet("__YYTK", __YYTK);
	}
	else
		__YYTK = GlobalVariableGet("__YYTK");
}

void CreateModInfoInGlobalYYTKVariable()
{
	if (!StructVariableExists(__YYTK, MOD_NAME))
	{
		RValue deep_dungeon;
		RValue version = VERSION;
		RValue floor = floor_number;
		g_ModuleInterface->GetRunnerInterface().StructCreate(&deep_dungeon);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "version", &version);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "floor", &floor);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&__YYTK, MOD_NAME, &deep_dungeon);
	}
}

double GetWindowWidth()
{
	RValue window_width = g_ModuleInterface->CallBuiltin("window_get_width", {});
	return window_width.ToDouble();
}

double GetWindowHeight()
{
	RValue window_height = g_ModuleInterface->CallBuiltin("window_get_height", {});
	return window_height.ToDouble();
}

uint64_t GetCurrentSystemTime() {
	return duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int euclidean_modulo(int a, int m)
{
	int r = a % m;
	return (r < 0) ? r + m : r;
}

std::vector<double> linspace(double start, double end, std::size_t num)
{
	std::vector<double> result;
	result.reserve(num);

	if (num == 0)
		return result;

	if (num == 1)
	{
		result.push_back(start);
		return result;
	}

	double step = (end - start) / static_cast<double>(num - 1);
	for (std::size_t i = 0; i < num; ++i)
		result.push_back(start + step * static_cast<double>(i));

	return result;
}

double round_n(double value, int decimals)
{
	double factor = std::pow(10.0, decimals);
	return std::round(value * factor) / factor;
}

std::complex<double> round_complex(const std::complex<double>& z, int decimals)
{
	return {
		round_n(z.real(), decimals),
		round_n(z.imag(), decimals)
	};
}

double GetDistance(int x1, int y1, int x2, int y2)
{
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

std::pair<int, int> GetVector(int x1, int y1, int x2, int y2)
{
	return { x2 - x1, y2 - y1 };
}

int CalculateMeteorDamage(double distance)
{
	const double lethalRadius = 32.0;
	const double maxDistance = 256.0;

	if (Config::config.meteor_trap_scaling_factor == 0)
		return 0;

	if (distance <= lethalRadius)
		return 100; 

	if (distance >= maxDistance)
		return 1;

	double t = (distance - lethalRadius) / (maxDistance - lethalRadius);
	double damage = 100.0f * std::pow(1.0f - t, Config::config.meteor_trap_scaling_factor);

	return max(1, static_cast<int>(damage));
}

bool FacingTrap(int ariX, int ariY, int trapX, int trapY)
{
	static constexpr double PI = 3.14159265358979323846;

	std::pair<int, int> v = GetVector(ariX, ariY, trapX, trapY);
	double rad = ari_facing_dir * PI / 180.0;

	double forwardX = cos(rad);
	double forwardY = -sin(rad);

	float dot = forwardX * v.first + forwardY * v.second;
	bool inFrontHalfPlane = dot >= 0.0f;
	return inFrontHalfPlane;
}

std::pair<int, int> GetCenterOffset(int screen_center_x, int screen_center_y, int image_width, int image_height)
{
	int offset_x = screen_center_x - image_width / 2;
	int offset_y = screen_center_y - image_height / 2;
	return { offset_x, offset_y };
}

std::vector<double> GetCenter(std::vector<double> topLeft, std::vector<double> bottomRight)
{
	double x = (topLeft[0] + bottomRight[0]) / 2.0;
	double y = (topLeft[1] + bottomRight[1]) / 2.0;
	std::vector<double> center = { x, y };
	return center;
}

void DrawRectangle(int color, float x1, float y1, float x2, float y2, bool outline)
{
	g_ModuleInterface->CallBuiltin(
		"draw_set_color", {
		 color
		}
	);

	g_ModuleInterface->CallBuiltin(
		"draw_rectangle", {
			x1, y1, x2, y2, outline
		}
	);
}

void DrawImage(int x, int y, int transparency)
{
	RValue sprite_index = g_ModuleInterface->CallBuiltin(
		"asset_get_index", {
			"aldarian_danger_banner"
		}
	);

	g_ModuleInterface->CallBuiltin(
		"draw_sprite_ext", {
			sprite_index, -1, x, y, 1, 1, 0, 16777215, transparency / 100.0
		}
	);
}

void FadeInImage(double seconds_per_cycle, int repeat_count) {
	// Window corners
	std::vector<double> window_top_left = { 0.0, 0.0 };
	std::vector<double> window_top_right = { window_width, 0.0 };
	std::vector<double> window_bottom_left = { 0.0, window_height };
	std::vector<double> window_bottom_right = { window_width, window_height };

	// Window center
	auto center = GetCenter(window_top_left, window_bottom_right);
	auto centered_offset = GetCenterOffset(center[0], center[1], 400, 120);

	if (!fade_initialized) {
		fade_start_time = GetCurrentSystemTime();
		fade_initialized = true;
	}

	double cycle_ms = seconds_per_cycle * 1000.0;
	uint64_t elapsed = GetCurrentSystemTime() - fade_start_time;

	int current_cycle = elapsed / cycle_ms;

	if (current_cycle >= repeat_count)
		return;

	double cycle_position = static_cast<double>(elapsed % static_cast<int>(cycle_ms));
	double half_cycle = cycle_ms / 2.0;
	int transparency = 0;

	if (cycle_position <= half_cycle) {
		// Fade in: 0 -> 100
		transparency = static_cast<int>((cycle_position / half_cycle) * 100);
	}
	else {
		// Fade out: 100 -> 0
		double fade_out_pos = cycle_position - half_cycle;
		transparency = static_cast<int>(((half_cycle - fade_out_pos) / half_cycle) * 100);
	}

	transparency = std::clamp(transparency, 0, 100);
	DrawImage(centered_offset.first, centered_offset.second, transparency);
}

void DrawDashedBorder(
	float dash_len,
	float dash_thk,
	float speed,          // pixels per second
	float screen_width,
	float screen_height,
	uint64_t current_time_ms
) {
	// Static dash state
	static std::vector<float> dash_positions;  // positions around perimeter
	static bool initialized = false;

	float top_len = screen_width;
	float right_len = screen_height;
	float bottom_len = screen_width;
	float left_len = screen_height;
	float perimeter = 2.0f * (screen_width + screen_height);

	// Desired dash spacing (avg)
	float target_spacing = perimeter / 40.0f;

	if (!initialized) {
		// Compute dash count per edge
		int top_count = static_cast<int>(std::floor(top_len / target_spacing));
		int right_count = static_cast<int>(std::floor(right_len / target_spacing));
		int bottom_count = static_cast<int>(std::floor(bottom_len / target_spacing));
		int left_count = static_cast<int>(std::floor(left_len / target_spacing));

		// Store total dashes spaced along perimeter
		dash_positions.clear();
		for (int i = 0; i < top_count; ++i)
			dash_positions.push_back((top_len / top_count) * i);
		for (int i = 0; i < right_count; ++i)
			dash_positions.push_back(top_len + (right_len / right_count) * i);
		for (int i = 0; i < bottom_count; ++i)
			dash_positions.push_back(top_len + right_len + (bottom_len / bottom_count) * i);
		for (int i = 0; i < left_count; ++i)
			dash_positions.push_back(top_len + right_len + bottom_len + (left_len / left_count) * i);

		initialized = true;
	}

	// Time-based offset
	static uint64_t last_time = current_time_ms;
	float delta_sec = (current_time_ms - last_time) / 1000.0f;
	last_time = current_time_ms;

	static float offset = 0.0f;
	offset += speed * delta_sec;
	if (offset > perimeter) offset -= perimeter;

	// Draw all dashes with animated offset
	for (float base_pos : dash_positions) {
		float pos = base_pos + offset;
		if (pos >= perimeter) pos -= perimeter;

		if (pos < top_len) {
			float x = pos;
			DrawRectangle(255, x, 0.0f, x + dash_len, dash_thk, false);
		}
		else if (pos < top_len + right_len) {
			float y = pos - top_len;
			DrawRectangle(255, screen_width - dash_thk, y, screen_width, y + dash_len, false);
		}
		else if (pos < top_len + right_len + bottom_len) {
			float x = screen_width - (pos - (top_len + right_len));
			DrawRectangle(255, x, screen_height - dash_thk, x + dash_len, screen_height, false);
		}
		else {
			float y = screen_height - (pos - (top_len + right_len + bottom_len));
			DrawRectangle(255, 0.0f, y, dash_thk, y + dash_len, false);
		}
	}
}

void PlaySoundEffect(const char* sound_name, int priority, double gain)
{
	const auto sound_index = g_ModuleInterface->CallBuiltin(
		"asset_get_index",
		{ sound_name }
	);

	g_ModuleInterface->CallBuiltin(
		"audio_play_sound",
		{ sound_index, 100, false, gain }
	);
}

void LoadPerks()
{
	size_t array_length;
	RValue perk_names = global_instance->GetMember("__perk__");
	g_ModuleInterface->GetArraySize(perk_names, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* perk_name;
		g_ModuleInterface->GetArrayEntry(perk_names, i, perk_name);

		perk_name_to_id_map[perk_name->ToString()] = i;
	}
}

bool FairyBuffIsActive()
{
	RValue ari = global_instance->GetMember("__ari");
	RValue status_effects = ari.GetMember("status_effects");
	RValue effects = status_effects.GetMember("effects");
	RValue inner = effects.GetMember("inner");

	if (StructVariableExists(inner, std::to_string(status_effect_name_to_id_map["fairy"]).c_str()))
	{
		RValue fairy_status = inner.GetMember(std::to_string(status_effect_name_to_id_map["fairy"]));
		return fairy_status.m_Kind == VALUE_OBJECT;
	}

	return false;
}

double GetInvulnerabilityHits()
{
	RValue ari = global_instance->GetMember("__ari");
	return ari.GetMember("invulnerable_hits").ToDouble();
}

void SetInvulnerabilityHits(double amount)
{
	RValue ari = *global_instance->GetRefMember("__ari");
	double invulnerability_hits = ari.GetMember("invulnerable_hits").ToDouble();

	if (amount == 0)
		*ari.GetRefMember("invulnerable_hits") = amount;
	else
		*ari.GetRefMember("invulnerable_hits") = invulnerability_hits + amount;
}

void SetFireBreathTime(double value)
{
	// Stop the fire breath spell
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("fire_breath_time") = value;
}

void DisableAllPerks()
{
	std::unordered_set<int> perks_to_disable = {};

	std::vector<std::string> struct_field_names = {};
	auto GetStructFieldNames = [&](IN const char* MemberName, IN OUT RValue* Value) {
		struct_field_names.push_back(MemberName);
		return false;
	};

	RValue dragon_shrine_data = global_instance->GetMember("__dragon_shrine_data");
	RValue inner = dragon_shrine_data.GetMember("inner");

	// Combat Perks
	RValue combat = inner.GetMember("combat");
	g_ModuleInterface->EnumInstanceMembers(combat, GetStructFieldNames);
	for (std::string field_name : struct_field_names)
	{
		if (field_name.contains("tier"))
		{
			size_t array_length;
			RValue tier = combat.GetMember(field_name);
			g_ModuleInterface->GetArraySize(tier, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* entry;
				g_ModuleInterface->GetArrayEntry(tier, i, entry);

				perks_to_disable.insert(entry->GetMember("perk").ToInt64());
			}
		}
	}

	// Mining Perks
	struct_field_names = {};
	RValue mining = inner.GetMember("mining");
	g_ModuleInterface->EnumInstanceMembers(mining, GetStructFieldNames);
	for (std::string field_name : struct_field_names)
	{
		if (field_name.contains("tier"))
		{
			size_t array_length;
			RValue tier = mining.GetMember(field_name);
			g_ModuleInterface->GetArraySize(tier, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* entry;
				g_ModuleInterface->GetArrayEntry(tier, i, entry);

				perks_to_disable.insert(entry->GetMember("perk").ToInt64());
			}
		}
	}

	// Cooking Perks
	perks_to_disable.insert(perk_name_to_id_map["snacktime"]);

	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue __ari_perks_active = *__ari.GetRefMember("perks_active");

	for (int perk : perks_to_disable)
		__ari_perks_active[perk] = false;
}

bool ItemHasBeenAcquired(int item_id)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue items_acquired = *__ari.GetRefMember("items_acquired");
	RValue item_acquired = g_ModuleInterface->CallBuiltin("array_get", { items_acquired, item_id });
	return item_acquired.ToBoolean();
}

void LoadTutorials()
{
	size_t array_length;
	RValue tutorials = global_instance->GetMember("__tutorial__");
	g_ModuleInterface->GetArraySize(tutorials, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* tutorial;
		g_ModuleInterface->GetArrayEntry(tutorials, i, tutorial);

		tutorial_name_to_id_map[tutorial->ToString()] = i;
	}
}

void LoadPlayerStates()
{
	size_t array_length;
	RValue player_states = global_instance->GetMember("__player_state__");
	g_ModuleInterface->GetArraySize(player_states, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* player_state;
		g_ModuleInterface->GetArrayEntry(player_states, i, player_state);

		player_state_to_id_map[player_state->ToString()] = i;
	}
}

void LoadMonsterStates()
{
	// NOTE: Using monster category names from: __monster_category__

	// Mushroom States
	size_t shroom_states_length;
	RValue shroom_states = global_instance->GetMember("__mushroom_state__");
	g_ModuleInterface->GetArraySize(shroom_states, shroom_states_length);
	for (size_t i = 0; i < shroom_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(shroom_states, i, state);

		monster_category_to_state_id_map["shroom"][state->ToString()] = i;
	}

	// Rock Clod States
	size_t rock_clod_states_length;
	RValue rock_clod_states = global_instance->GetMember("__rockclod_state__");
	g_ModuleInterface->GetArraySize(rock_clod_states, rock_clod_states_length);
	for (size_t i = 0; i < rock_clod_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(rock_clod_states, i, state);

		monster_category_to_state_id_map["clod"][state->ToString()] = i;
	}

	// Sapling States
	size_t sapling_states_length;
	RValue sapling_states = global_instance->GetMember("__sapling_state__");
	g_ModuleInterface->GetArraySize(sapling_states, sapling_states_length);
	for (size_t i = 0; i < sapling_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(sapling_states, i, state);

		monster_category_to_state_id_map["sap"][state->ToString()] = i;
	}

	// Enchantern States
	size_t enchantern_states_length;
	RValue enchantern_states = global_instance->GetMember("__enchantern_state__");
	g_ModuleInterface->GetArraySize(enchantern_states, enchantern_states_length);
	for (size_t i = 0; i < enchantern_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(enchantern_states, i, state);

		monster_category_to_state_id_map["enchantern"][state->ToString()] = i;
	}

	// Stalagmite States
	size_t mite_states_length;
	RValue mite_states = global_instance->GetMember("__mite_state__");
	g_ModuleInterface->GetArraySize(mite_states, mite_states_length);
	for (size_t i = 0; i < mite_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(mite_states, i, state);

		monster_category_to_state_id_map["mite"][state->ToString()] = i;
	}

	// Bat States
	size_t bat_states_length;
	RValue bat_states = global_instance->GetMember("__bat_state__");
	g_ModuleInterface->GetArraySize(bat_states, bat_states_length);
	for (size_t i = 0; i < bat_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(bat_states, i, state);

		monster_category_to_state_id_map["bat"][state->ToString()] = i;
	}

	// Mimic States
	size_t mimic_states_length;
	RValue mimic_states = global_instance->GetMember("__mimic_state__");
	g_ModuleInterface->GetArraySize(mimic_states, mimic_states_length);
	for (size_t i = 0; i < mimic_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(mimic_states, i, state);

		monster_category_to_state_id_map["mimic"][state->ToString()] = i;
	}

	// Spirit States
	size_t spirit_states_length;
	RValue spirit_states = global_instance->GetMember("__spirit_state__");
	g_ModuleInterface->GetArraySize(spirit_states, spirit_states_length);
	for (size_t i = 0; i < spirit_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(spirit_states, i, state);

		monster_category_to_state_id_map["spirit"][state->ToString()] = i;
	}

	// Cat States
	size_t cat_states_length;
	RValue cat_states = global_instance->GetMember("__cat_state__");
	g_ModuleInterface->GetArraySize(cat_states, cat_states_length);
	for (size_t i = 0; i < cat_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(cat_states, i, state);

		monster_category_to_state_id_map["cat"][state->ToString()] = i;
	}

	// Rock Stack States
	size_t rock_stack_states_length;
	RValue rock_stack_states = global_instance->GetMember("__rock_stack_state__");
	g_ModuleInterface->GetArraySize(rock_stack_states, rock_stack_states_length);
	for (size_t i = 0; i < rock_stack_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(rock_stack_states, i, state);

		monster_category_to_state_id_map["rock_stack"][state->ToString()] = i;
	}

	// Statue States
	size_t statue_states_length;
	RValue statue_states = global_instance->GetMember("__statue_state__");
	g_ModuleInterface->GetArraySize(statue_states, statue_states_length);
	for (size_t i = 0; i < statue_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(statue_states, i, state);

		monster_category_to_state_id_map["statue"][state->ToString()] = i;
	}

	// Tome States
	size_t tome_states_length;
	RValue tome_states = global_instance->GetMember("__tome_state__");
	g_ModuleInterface->GetArraySize(tome_states, tome_states_length);
	for (size_t i = 0; i < tome_states_length; i++)
	{
		RValue* state;
		g_ModuleInterface->GetArrayEntry(tome_states, i, state);

		monster_category_to_state_id_map["tome"][state->ToString()] = i;
	}

	// TODO: New monsters as added.
}

void LoadBarkData()
{
	size_t array_length;
	RValue bark_data = global_instance->GetMember("__bark_id__");
	g_ModuleInterface->GetArraySize(bark_data, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* bark_name;
		g_ModuleInterface->GetArrayEntry(bark_data, i, bark_name);

		bark_name_to_id_map[bark_name->ToString()] = i;
	}
}

void LoadStatusEffects()
{
	size_t array_length;
	RValue status_effects = global_instance->GetMember("__status_effect_id__");
	g_ModuleInterface->GetArraySize(status_effects, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* status_effect;
		g_ModuleInterface->GetArrayEntry(status_effects, i, status_effect);

		status_effect_name_to_id_map[status_effect->ToString()] = i;
	}
}

void LoadLocations()
{
	size_t array_length;
	RValue locations = global_instance->GetMember("__location_id__");
	g_ModuleInterface->GetArraySize(locations, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* location;
		g_ModuleInterface->GetArrayEntry(locations, i, location);

		location_name_to_id_map[location->ToString()] = i;
	}
}

void LoadInfusions()
{
	size_t array_length;
	RValue infusions = global_instance->GetMember("__infusion__");
	g_ModuleInterface->GetArraySize(infusions, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* infusion;
		g_ModuleInterface->GetArrayEntry(infusions, i, infusion);

		infusion_name_to_id_map[infusion->ToString()] = i;
	}
}

void LoadMonsters()
{
	size_t array_length;
	RValue monster_names = global_instance->GetMember("__monster_id__");
	g_ModuleInterface->GetArraySize(monster_names, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* monster_name;
		g_ModuleInterface->GetArrayEntry(monster_names, i, monster_name);

		monster_name_to_id_map[monster_name->ToString()] = i;
		monster_id_to_name_map[i] = monster_name->ToString();
	}
}

void ModifyMonsterPrototypes()
{
	size_t array_length;
	RValue monster_prototypes = global_instance->GetMember("__monster_prototypes");
	g_ModuleInterface->GetArraySize(monster_prototypes, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* monster_prototype;
		g_ModuleInterface->GetArrayEntry(monster_prototypes, i, monster_prototype);

		if (!StructVariableExists(*monster_prototype, "monster_id") || !StructVariableExists(*monster_prototype, "hp") || !StructVariableExists(*monster_prototype, "damage"))
			continue;

		int monster_id = monster_prototype->GetMember("monster_id").ToInt64();
		if (monster_id == monster_name_to_id_map["barrel"])
			continue;

		double hp = monster_prototype->GetMember("hp").ToDouble();
		hp = std::trunc(hp * Config::config.experimental_monster_base_stat_difficulty_modifier);
		*monster_prototype->GetRefMember("hp") = hp;

		double damage = monster_prototype->GetMember("damage").ToDouble();
		damage = std::trunc(damage * Config::config.experimental_monster_base_stat_difficulty_modifier);
		*monster_prototype->GetRefMember("damage") = damage;

		if (StructVariableExists(*monster_prototype, "projectile_damage"))
		{
			double projectile_damage = monster_prototype->GetMember("projectile_damage").ToDouble();
			projectile_damage = std::trunc(projectile_damage * Config::config.experimental_monster_base_stat_difficulty_modifier);
			*monster_prototype->GetRefMember("projectile_damage") = projectile_damage;
		}
	}
}

void LoadDungeonBiomeCandidateMonsters()
{
	RValue biomes = global_instance->GetMember("__fiddle").GetMember("__inner").GetMember("inner").GetMember("dungeons").GetMember("dungeons").GetMember("biomes");
	int max_floors = global_instance->GetMember("__fiddle").GetMember("__inner").GetMember("inner").GetMember("dungeons").GetMember("misc").GetMember("max_floors").ToInt64() + 1;

	size_t biomes_length;
	g_ModuleInterface->GetArraySize(biomes, biomes_length);

	for (size_t i = 0; i < biomes_length; i++)
	{
		RValue* biome;
		g_ModuleInterface->GetArrayEntry(biomes, i, biome);

		int floor = biome->GetMember("floor").ToInt64();
		std::string biome_name = biome->GetMember("name").ToString();
		RValue enemies = biome->GetMember("votes").GetMember("enemy");

		size_t enemies_length;
		g_ModuleInterface->GetArraySize(enemies, enemies_length);

		for (size_t j = 0; j < enemies_length; j++)
		{
			RValue* enemy;
			g_ModuleInterface->GetArrayEntry(enemies, j, enemy);

			std::string enemy_name = enemy->GetMember("object").ToString();

			if (monster_name_to_id_map.contains(enemy_name))
				dungeon_biome_to_candidate_monsters_map[biome_name].insert(monster_name_to_id_map[enemy_name]);
			else
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to look up enemy name: %s", MOD_NAME, VERSION, enemy_name.c_str());
		}

		if (i < biomes_length - 1)
		{
			RValue* next_biome;
			g_ModuleInterface->GetArrayEntry(biomes, i + 1, next_biome);

			int next_biome_floor = next_biome->GetMember("floor").ToInt64();
			for (size_t j = floor; j < next_biome_floor; j++)
				floor_number_to_biome_name_map[j] = biome_name;
		}
		else
		{
			for (size_t j = floor; j <= max_floors; j++)
				floor_number_to_biome_name_map[j] = biome_name;
		}
	}
}

void LoadObjectIds()
{
	size_t array_length;
	RValue objects = global_instance->GetMember("__object_id__");
	g_ModuleInterface->GetArraySize(objects, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* object;
		g_ModuleInterface->GetArrayEntry(objects, i, object);

		object_id_to_name_map[i] = object->ToString();
	}
}

void LoadItems()
{
	std::unordered_set<std::string> lift_key_names = { 
		UPPER_MINES_KEY_F5_NAME, UPPER_MINES_KEY_F10_NAME, UPPER_MINES_KEY_F15_NAME,
		TIDE_CAVERNS_KEY_F20_NAME, TIDE_CAVERNS_KEY_F25_NAME, TIDE_CAVERNS_KEY_F30_NAME,TIDE_CAVERNS_KEY_F35_NAME,
		DEEP_EARTH_KEY_F40_NAME, DEEP_EARTH_KEY_F45_NAME, DEEP_EARTH_KEY_F50_NAME, DEEP_EARTH_KEY_F55_NAME,
		LAVA_CAVES_KEY_F60_NAME, LAVA_CAVES_KEY_F65_NAME, LAVA_CAVES_KEY_F70_NAME, LAVA_CAVES_KEY_F75_NAME,
		RUINS_KEY_F80_NAME, RUINS_KEY_F85_NAME, RUINS_KEY_F90_NAME, RUINS_KEY_F95_NAME, RUINS_KEY_F100_NAME
	};
	std::unordered_set<std::string> orb_item_names = { TIDE_CAVERNS_ORB, DEEP_EARTH_ORB, LAVA_CAVES_ORB, RUINS_ORB }; // TODO: Add other orbs
	std::vector<std::string> custom_potions = { SUSTAINING_POTION_NAME, HEALTH_SALVE_NAME, STAMINA_SALVE_NAME, MANA_SALVE_NAME }; // TODO: Change to unordered_set
	std::vector<std::string> cursed_armor = { CURSED_HELMET_NAME, CURSED_CHESTPIECE_NAME, CURSED_PANTS_NAME, CURSED_BOOTS_NAME, CURSED_GLOVES_NAME }; // TODO: Change to unordered_set

	size_t array_length;
	RValue item_data = global_instance->GetMember("__item_data");
	g_ModuleInterface->GetArraySize(item_data, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* item;
		g_ModuleInterface->GetArrayEntry(item_data, i, item);

		RValue name_key = item->GetMember("name_key"); // The item's localization key
		if (name_key.m_Kind != VALUE_NULL && name_key.m_Kind != VALUE_UNDEFINED && name_key.m_Kind != VALUE_UNSET)
		{
			int item_id = item->GetMember("item_id").ToInt64();
			std::string item_name = item->GetMember("recipe_key").ToString(); // The internal item name
			item_name_to_id_map[item_name] = item_id;

			// Sigils
			if (item_name_to_sigil_map.contains(item_name))
			{
				deep_dungeon_items.insert(item_id);
				sigil_to_item_id_map[item_name_to_sigil_map.at(item_name)] = item_id;
				item_id_to_sigil_map[item_id] = item_name_to_sigil_map.at(item_name);

				*item->GetRefMember("health_modifier") = 0;
			}

			// Greater Sigils
			if (item_name_to_greater_sigil_map.contains(item_name))
			{
				deep_dungeon_items.insert(item_id);
				greater_sigil_to_item_id_map[item_name_to_greater_sigil_map.at(item_name)] = item_id;
				item_id_to_greater_sigil_map[item_id] = item_name_to_greater_sigil_map.at(item_name);

				*item->GetRefMember("health_modifier") = 0;
			}

			// Lift keys
			if (lift_key_names.contains(item_name))
				lift_key_items.insert(item_id);

			// Orb Items
			if (orb_item_names.contains(item_name))
				orb_items.insert(item_id);

			// Salve Items
			for (std::string custom_potion : custom_potions)
			{
				if (item_name == custom_potion)
				{
					salve_items.insert(item_id);
					deep_dungeon_items.insert(item_id);
					salve_name_to_id_map[item_name] = item_id;
				}
			}

			// All consumable items (except Deep Dungeon items)
			if (Config::config.restrict_items && !deep_dungeon_items.contains(item_id))
			{
				if (name_key.ToString().contains("cooked_dishes"))
					restricted_items.insert(item_id);
				else
				{
					RValue edible = item->GetMember("edible");
					if (edible.m_Kind == VALUE_BOOL && edible.m_Real == 1.0)
						restricted_items.insert(item_id);
				}
			}

			// Armor, Weapons, Tools, etc.
			if (StructVariableExists(*item, "tags"))
			{
				RValue tags = item->GetMember("tags");
				RValue buffer = tags.GetMember("__buffer");

				size_t array_length = 0;
				g_ModuleInterface->GetArraySize(buffer, array_length);
				for (size_t i = 0; i < array_length; i++)
				{
					RValue* array_element;
					g_ModuleInterface->GetArrayEntry(buffer, i, array_element);

					if (Config::config.restrict_armor && array_element->ToString() == "armor")
						*item->GetRefMember("defense") = 0;

					if (array_element->ToString() == "weapon")
					{
						if (item_name == MISTPOOL_SWORD_NAME)
							deep_dungeon_items.insert(item_id);
						else if (Config::config.restrict_weapons)
						{
							*item->GetRefMember("damage") = 0;
							restricted_items.insert(item_id);
						}
					}

					if (Config::config.restrict_tools && array_element->ToString() == "pick_axe")
					{
						if (item_name != MISTPOOL_PICK_AXE_NAME)
							restricted_items.insert(item_id);
					}

					if (Config::config.restrict_items && array_element->ToString() == "bomb")
					{
						*item->GetRefMember("damage") = 0;
						*item->GetRefMember("bomb")->GetRefMember("damage") = 0;
						restricted_items.insert(item_id);
					}
				}
			}

			// All snake oils
			if (Config::config.restrict_items && item_name.contains("snake_oil"))
			{
				/*
				   - [string] __infusion__[1] = 'fire_sword'
				   - [string] __infusion__[4] = 'ice_sword'
				   - [string] __infusion__[16] = 'venom_sword'
				*/
				int default_infusion = item->GetMember("default_infusion").ToInt64();
				if (default_infusion == infusion_name_to_id_map["fire_sword"] || default_infusion == infusion_name_to_id_map["ice_sword"] || default_infusion == infusion_name_to_id_map["venom_sword"])
					restricted_items.insert(item_id);
			}
		}
	}
}

void SetItemHealthModifier(int item_id, double health_modifier)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	g_ModuleInterface->CallBuiltin("struct_set", { item, "health_modifier", health_modifier });
}

void SetItemStaminaModifier(int item_id, double stamina_modifier)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	g_ModuleInterface->CallBuiltin("struct_set", { item, "stamina_modifier", stamina_modifier });
}

void SetItemManaModifier(int item_id, double mana_modifier)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	g_ModuleInterface->CallBuiltin("struct_set", { item, "mana_modifier", mana_modifier });
}

void SetItemShopPrice(int item_id, int store_price)
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	RValue item = g_ModuleInterface->CallBuiltin("array_get", { __item_data, item_id });

	RValue value = *item.GetRefMember("value");
	StructVariableSet(value, "store", store_price);
}

void ModifyItems()
{
	SetItemHealthModifier(item_name_to_id_map[HEALTH_SALVE_NAME], Config::config.health_salve_potency);
	SetItemStaminaModifier(item_name_to_id_map[STAMINA_SALVE_NAME], Config::config.stamina_salve_potency);
	SetItemManaModifier(item_name_to_id_map[MANA_SALVE_NAME], Config::config.mana_salve_potency);

	SetItemShopPrice(item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], Config::config.mistpool_equipment_store_price);
	SetItemShopPrice(item_name_to_id_map[MISTPOOL_SWORD_NAME], Config::config.mistpool_equipment_store_price);
	for (std::string armor_name : MISTPOOL_ARMOR_NAMES)
		SetItemShopPrice(item_name_to_id_map[armor_name], Config::config.mistpool_equipment_store_price);
	for (const auto& pair : salve_name_to_id_map)
		SetItemShopPrice(pair.second, Config::config.salves_store_price);
}

void MarkDungeonTutorialUnseen()
{
	RValue ari = *global_instance->GetRefMember("__ari");
	RValue tutorials_seen = *ari.GetRefMember("tutorials_seen");

	RValue* mines_tutorial;
	g_ModuleInterface->GetArrayEntry(tutorials_seen, tutorial_name_to_id_map["mines"], mines_tutorial);

	*mines_tutorial = false; // TODO: This works, but should only be called once per save file.
}

void ModifyMistpoolWeaponSprites()
{
	// Sprite indexes for the "Scrap Metal Sword" which is used as the Mistpool Sword.
	RValue spr_weapon_sword_scrap_metal_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_s" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_s" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_s" });
	RValue spr_weapon_sword_scrap_metal_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_down_attack_e" });
	RValue spr_ui_item_tool_scrap_metal_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_scrap_metal_sword" });

	if (floor_number < 20)
	{
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t0_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t0_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t0_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t0_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t0_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t0_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t0_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t0_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t0_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t0_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t0_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t0_sword });
	}
	else if (floor_number < 40)
	{
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t1_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t1_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t1_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t1_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t1_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t1_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t1_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t1_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t1_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t1_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t1_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t1_sword });
	}
	else if (floor_number < 60)
	{
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t2_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t2_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t2_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t2_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t2_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t2_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t2_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t2_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t2_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t2_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t2_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t2_sword });
	}
	else
	{
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t3_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t3_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t3_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t3_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t3_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t3_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t3_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t3_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t3_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t3_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t3_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t3_sword });
	}
}

void ModifyMistpoolPickaxeSprites()
{
	// Sprite indexes for the "Worn Pickaxe" which is used as the Mistpool Pickaxe.
	RValue spr_tool_pick_axe_worn_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_e" });
	RValue spr_tool_pick_axe_worn_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_n" });
	RValue spr_tool_pick_axe_worn_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_s" });
	RValue spr_ui_item_tool_rusty_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_rusty_pickaxe" });

	if (floor_number < 20)
	{
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t0_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t0_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t0_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t0_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t0_pickaxe });
	}
	else if (floor_number < 40)
	{
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t1_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t1_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t1_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t1_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t1_pickaxe });
	}
	else if (floor_number < 60)
	{
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t2_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t2_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t2_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t2_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t2_pickaxe });
	}
	else
	{
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t3_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t3_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t3_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t3_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t3_pickaxe });
	}
}

void ModifyBarkSprites()
{
	// TODO: Update as more custom bark sprites are implemented
	RValue spr_ui_bark_icon_no_coin = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_bark_icon_no_coin" });
	RValue spr_ui_bark_icon_no_coin_copy = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_bark_icon_no_coin_copy" });
	RValue spr_ui_bark_icon_inhibiting_trap = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_bark_icon_inhibiting_trap" });

	if (floor_number != 0)
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_bark_icon_no_coin, spr_ui_bark_icon_inhibiting_trap });
	else
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_bark_icon_no_coin, spr_ui_bark_icon_no_coin_copy });
}

void ScaleMistpoolWeapon(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");

	RValue* sword_scrap_metal;
	g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map["sword_scrap_metal"], sword_scrap_metal);

	if (in_dungeon)
	{
		int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
		int damage = (modified_floor_number / 4) + 3;
		*sword_scrap_metal->GetRefMember("damage") = damage;
	}
	else
		*sword_scrap_metal->GetRefMember("damage") = 1;
}

void ScaleMistpoolArmor(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");
	for (std::string mistpool_armor_name : MISTPOOL_ARMOR_NAMES)
	{
		RValue* mistpool_armor_piece;
		g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[mistpool_armor_name], mistpool_armor_piece);

		if (in_dungeon)
		{
			int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
			int defense = modified_floor_number / 20;
			*mistpool_armor_piece->GetRefMember("defense") = defense;
		}
		else
			*mistpool_armor_piece->GetRefMember("defense") = 0;
	}
}

void ScaleMistpoolPickaxe(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");

	RValue* pick_axe_worn;
	g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], pick_axe_worn);

	if (in_dungeon)
	{
		if (floor_number < 20)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 2 : 1;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 1 : 0;
		}
		else if (floor_number < 40)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 3 : 2;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 2 : 1;
		}
		else if (floor_number < 60)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 5 : 3;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 3 : 2;
		}

		else if (floor_number < 80)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 6 : 4;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 4 : 3;
		}

		else if (floor_number < 100)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 8 : 5;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 5 : 4;
		}
	}
	else
	{
		*pick_axe_worn->GetRefMember("damage") = 1;
		*pick_axe_worn->GetRefMember("quality") = 0;
	}
}

void ScaleClassArmor(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");
	for (std::string class_armor_name : CLASS_ARMOR_NAMES)
	{
		RValue* class_armor_piece;
		g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[class_armor_name], class_armor_piece);

		if (in_dungeon)
		{
			int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
			int defense = modified_floor_number / 20;
			*class_armor_piece->GetRefMember("defense") = defense;
		}
		else
			*class_armor_piece->GetRefMember("defense") = 0;
	}
}

int GetRandomSoulStone()
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> random_soul_stone_distribution(0, SOUL_STONE_NAMES.size() - 1);
	return item_name_to_id_map[SOUL_STONE_NAMES[random_soul_stone_distribution(random_generator)]];
}

std::map<Classes, int> CountEquippedClassArmor()
{
	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	std::map<Classes, int> class_armor_equipped = {};
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "prototype"))
			{
				RValue prototype = item.GetMember("prototype");
				if (StructVariableExists(prototype, "recipe_key"))
				{
					RValue recipe_key = prototype.GetMember("recipe_key");
					for (const auto& class_armor : CLASS_NAME_TO_ARMOR_NAMES_MAP)
					{
						if (CLASS_NAME_TO_ARMOR_NAMES_MAP.at(class_armor.first).contains(recipe_key.ToString()))
							class_armor_equipped[class_armor.first]++;
					}
				}
			}
		}
	}

	return class_armor_equipped;
}

std::map<int, int> GetClassArmorInfusions()
{
	std::map<int, int> class_armor_infusions = {};

	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "infusion") && StructVariableExists(item, "prototype"))
			{
				RValue infusion = item.GetMember("infusion");
				RValue prototype = item.GetMember("prototype");
				if (IsNumeric(infusion) && IsObject(prototype) && StructVariableExists(prototype, "recipe_key") && CLASS_ARMOR_NAMES.contains(prototype.GetMember("recipe_key").ToString()))
					class_armor_infusions[infusion.ToInt64()]++;
			}
		}
	}

	return class_armor_infusions;
}

int GetClericAutoRegenPotency()
{
	int cleric_armor_pieces_equipped = CountEquippedClassArmor()[Classes::CLERIC];
	if (cleric_armor_pieces_equipped == 0)
		return 0;
	if (cleric_armor_pieces_equipped < 3)
		return 1;
	if (cleric_armor_pieces_equipped < 5)
		return 2;
	if (cleric_armor_pieces_equipped == 5)
		return 3;
}

double GetDarkKnightDrainPotency()
{
	int dark_knight_pieces_equipped = CountEquippedClassArmor()[Classes::DARK_KNIGHT];
	if (dark_knight_pieces_equipped == 0)
		return 0;
	if (dark_knight_pieces_equipped < 3)
		return 0.03;
	if (dark_knight_pieces_equipped < 5)
		return 0.05;
	if (dark_knight_pieces_equipped == 5)
		return 0.08;
}

double GetPaladinHolyCirclePotency()
{
	int paladin_pieces_equipped = CountEquippedClassArmor()[Classes::PALADIN];
	if (paladin_pieces_equipped == 0)
		return 0;
	if (paladin_pieces_equipped < 3)
		return 0.05;
	if (paladin_pieces_equipped < 5)
		return 0.10;
	if (paladin_pieces_equipped == 5)
		return 0.15;
}

int ScaleTemperanceDamage(int current_health, int max_health, int damage)
{
	if (max_health <= 0)
		return damage;

	current_health = std::clamp(current_health, 0, max_health);

	int health_pct = (current_health * 100) / max_health;

	static const struct {
		int health;
		int multiplier;
	} breakpoints[] = {
		{100,  20},  // -80%
		{95,   40},
		{90,   60},
		{85,   80},
		{80,  100},  // 0%
		{75,  110},
		{70,  120},
		{65,  130},
		{60,  140},
		{55,  150},
		{40,  160},
		{35,  170},
		{30,  180},
		{25,  190},
		{20,  200},
		{15,  210},
		{10,  220},
		{5,   230},
		{0,   240}
	};

	constexpr int count = sizeof(breakpoints) / sizeof(breakpoints[0]);

	for (int i = 0; i < count - 1; ++i)
	{
		int h1 = breakpoints[i].health;
		int m1 = breakpoints[i].multiplier;
		int h2 = breakpoints[i + 1].health;
		int m2 = breakpoints[i + 1].multiplier;

		if (health_pct <= h1 && health_pct >= h2)
		{
			int numerator = (health_pct - h2) * (m1 - m2);
			int denominator = (h1 - h2);
			int multiplier = m2 + numerator / denominator;

			return (damage * multiplier) / 100;
		}
	}

	return (damage * breakpoints[count - 1].multiplier) / 100;
}

ElementalSealEffects GetRandomElementalSealEffect()
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> random_elemental_seal_effect_distribution(0, magic_enum::enum_count<ElementalSealEffects>() - 1);
	return magic_enum::enum_value<ElementalSealEffects>(random_elemental_seal_effect_distribution(random_generator));
}

void LoadSpellIds()
{
	size_t array_length = 0;
	RValue spell_ids = global_instance->GetMember("__spell__");
	g_ModuleInterface->GetArraySize(spell_ids, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spell_ids, i, array_element);

		spell_name_to_id_map[array_element->ToString()] = i;
	}
}

void LoadSpells()
{
	size_t array_length = 0;
	RValue spells = global_instance->GetMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		spell_id_to_default_cost_map[i] = array_element->GetMember("cost").ToInt64();
	}
}

void ModifySpellCosts(bool reset_cost, bool in_dungeon) {
	size_t array_length = 0;
	RValue spells = global_instance->GetMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		int cost = reset_cost ? spell_id_to_default_cost_map[i] : spell_id_to_default_cost_map[i] / 2;
		if (in_dungeon && i == spell_name_to_id_map["growth"])
			cost = reset_cost ? spell_id_to_default_cost_map[i] / 2 : spell_id_to_default_cost_map[i] / 4;
		if (active_greater_sigils.contains(GreaterSigils::CHAIN_SPELL))
			cost = 0;
		if (i == spell_name_to_id_map["full_restore"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			cost = 0;
		if (i == spell_name_to_id_map["growth"] && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			cost = 0;

		*array_element->GetRefMember("cost") = cost;
	}
}

RValue LocalizeString(CInstance* Self, CInstance* Other, std::string localization_key)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(localization_key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void SpawnLadder(CInstance* Self, CInstance* Other, int64_t x_coord, int64_t y_coord)
{
	CScript* gml_Script_spawn_ladder = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_LADDER,
		(PVOID*)&gml_Script_spawn_ladder
	);

	RValue x = (x_coord * 2) / 16;
	RValue y = (y_coord * 2) / 16;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* rvalue_array[2] = { x_ptr, y_ptr };
	RValue retval;
	gml_Script_spawn_ladder->m_Functions->m_ScriptFunction(
		Self,
		Other,
		retval,
		2,
		rvalue_array
	);
}

void SpawnMonster(CInstance* Self, CInstance* Other, int room_x, int room_y, int monster_id)
{
	CScript* gml_script_spawn_monster = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_MONSTER,
		(PVOID*)&gml_script_spawn_monster
	);

	RValue x = room_x;
	RValue y = room_y;
	RValue monster = monster_id;

	RValue result;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* monster_ptr = &monster;
	RValue* arguments[3] = { x_ptr, y_ptr, monster_ptr };

	gml_script_spawn_monster->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		arguments
	);
}

void UnlockRecipe(int item_id, CInstance* Self, CInstance* Other)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue recipe_unlocks = *__ari.GetRefMember("recipe_unlocks");

	if (recipe_unlocks[item_id].m_Real == 0.0)
		recipe_unlocks[item_id] = 1.0; // This value is ultimately what unlocks the recipe.
}

void UnlockLiftKeyRecipe(CInstance* Self, CInstance* Other)
{
	if (!Config::config.disable_dungeon_lift)
		return;

	if (floor_number == 5)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F5_NAME], Self, Other);
	else if (floor_number == 10)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F10_NAME], Self, Other);
	else if (floor_number == 15)
		UnlockRecipe(item_name_to_id_map[UPPER_MINES_KEY_F15_NAME], Self, Other);
	else if (floor_number == 20)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], Self, Other);
	else if (floor_number == 25)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME], Self, Other);
	else if (floor_number == 30)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME], Self, Other);
	else if (floor_number == 35)
		UnlockRecipe(item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME], Self, Other);
	else if (floor_number == 40)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], Self, Other);
	else if (floor_number == 45)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME], Self, Other);
	else if (floor_number == 50)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME], Self, Other);
	else if (floor_number == 55)
		UnlockRecipe(item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME], Self, Other);
	else if (floor_number == 60)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], Self, Other);
	else if (floor_number == 65)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME], Self, Other);
	else if (floor_number == 70)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME], Self, Other);
	else if (floor_number == 75)
		UnlockRecipe(item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME], Self, Other);
	else if (floor_number == 80)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F80_NAME], Self, Other);
	else if (floor_number == 85)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F85_NAME], Self, Other);
	else if (floor_number == 90)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F90_NAME], Self, Other);
	else if (floor_number == 95)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F95_NAME], Self, Other);
	else if (floor_number == 100)
		UnlockRecipe(item_name_to_id_map[RUINS_KEY_F100_NAME], Self, Other);
}

void TeleportAriToRoom(CInstance* Self, CInstance* Other, int location_id, int x_coordinate, int y_coordinate)
{
	CScript* gml_script_ari_teleport_to_room = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TELEPORT_ARI_TO_ROOM,
		(PVOID*)&gml_script_ari_teleport_to_room
	);

	RValue retval;
	RValue location = location_id;
	RValue x = x_coordinate;
	RValue y = y_coordinate;
	RValue* location_ptr = &location;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* argument_array[3] = { location_ptr, x_ptr, y_ptr };
	gml_script_ari_teleport_to_room->m_Functions->m_ScriptFunction(
		Self,
		Other,
		retval,
		3,
		argument_array
	);
}

void CreateNotification(bool ignore_cooldown, std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	uint64_t current_system_time = GetCurrentSystemTime();
	if (ignore_cooldown || current_system_time > notification_name_to_last_display_time_map[notification_localization_str] + 5000)
	{
		CScript* gml_script_create_notification = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			GML_SCRIPT_CREATE_NOTIFICATION,
			(PVOID*)&gml_script_create_notification
		);

		RValue result;
		RValue notification = RValue(notification_localization_str);
		RValue* notification_ptr = &notification;
		gml_script_create_notification->m_Functions->m_ScriptFunction(
			Self,
			Other,
			result,
			1,
			{ &notification_ptr }
		);

		notification_name_to_last_display_time_map[notification_localization_str] = current_system_time;
	}
}

void PlayConversation(std::string conversation_localization_str, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_play_conversation = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_CONVERSATION,
		(PVOID*)&gml_script_play_conversation
	);

	RValue zero = 0;
	RValue conversation = RValue(conversation_localization_str);
	RValue undefined;

	RValue* zero_ptr = &zero;
	RValue* conversation_ptr = &conversation;
	RValue* undefined_ptr = &undefined;

	RValue result;
	RValue* arguments[4] = { zero_ptr, conversation_ptr, undefined_ptr, undefined_ptr };

	gml_script_play_conversation->m_Functions->m_ScriptFunction(
		Self,
		Self,
		result,
		4,
		arguments
	);
}

void CloseTextbox(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_close_textbox = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CLOSE_TEXTBOX,
		(PVOID*)&gml_script_close_textbox
	);

	RValue result;
	gml_script_close_textbox->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

void EmitBark(CInstance* Self, CInstance* Other, RValue bark_id, RValue bark_type)
{
	CScript* gml_script_register_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_BARK_EMITTER_EMIT,
		(PVOID*)&gml_script_register_status_effect
	);

	RValue result;
	RValue* bark_id_ptr = &bark_id;
	RValue* bark_type_ptr = &bark_type;
	RValue* argument_array[2] = { bark_id_ptr, bark_type_ptr };

	gml_script_register_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		2,
		argument_array
	);
}

RValue T2Read(CInstance* Self, CInstance* Other, std::string key)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_T2_READ,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void SceneAudioPlayerStop(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SCENE_AUDIO_PLAYER_STOP,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

void SpawnTutorial(std::string tutorial_name, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_TUTORIAL,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = tutorial_name_to_id_map[tutorial_name];
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);
}

void UpdateToolbarMenu(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_update_toolbar_menu = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_UPDATE_TOOLBAR_MENU,
		(PVOID*)&gml_script_update_toolbar_menu
	);

	RValue result;
	gml_script_update_toolbar_menu->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

RValue DeserializeLiveItem(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_deserialize_live_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DESERIALIZE_LIVE_ITEM,
		(PVOID*)&gml_script_deserialize_live_item
	);

	std::map<std::string, RValue> rvalue_map = {
		{ "cosmetic", RValue() }, // UNDEFINED
		{ "item_id", RValue("sword_scrap_metal")}, // STRING
		{ "infusion", RValue() }, // UNDEFINED
		{ "animal_cosmetic", RValue() }, // UNDEFINED
		{ "date_photo", RValue() }, // UNDEFINED
		{ "inner_item", RValue() }, // UNDEFINED
		{ "gold_to_gain", RValue() }, // UNDEFINED
		{ "auto_use", false }, // BOOL
		{ "pet_cosmetic_set_name", RValue() } // UNDEFINED
	};

	RValue result;
	RValue input = rvalue_map;
	RValue* input_ptr = &input;
	gml_script_deserialize_live_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void DropItem(int item_id, double x_coord, double y_coord, CInstance* Self, CInstance* Other)
{
	RValue item = DeserializeLiveItem(Self, Other);
	*item.GetRefMember("prototype") = item_id_to_prototype_map[item_id];
	*item.GetRefMember("item_id") = item_id;

	CScript* gml_script_drop_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DROP_ITEM,
		(PVOID*)&gml_script_drop_item
	);

	RValue x = x_coord;
	RValue y = y_coord;
	RValue undefined;

	RValue* item_ptr = &item;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* undefined_ptr = &undefined;

	RValue result;
	RValue* arguments[4] = { item_ptr, x_ptr, y_ptr, undefined_ptr };

	gml_script_drop_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		4,
		arguments
	);
}

void DropLiftKey()
{
	if(floor_number >= 95)
		DropItem(item_name_to_id_map[RUINS_KEY_F95_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 90)
		DropItem(item_name_to_id_map[RUINS_KEY_F90_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 85)
		DropItem(item_name_to_id_map[RUINS_KEY_F85_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 80)
		DropItem(item_name_to_id_map[RUINS_KEY_F80_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 75)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 70)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 65)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 60)
		DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 55)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 50)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 45)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 40)
		DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 35)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 30)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 25)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 20)
		DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 15)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F15_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 10)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F10_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
	else if (floor_number >= 5)
		DropItem(item_name_to_id_map[UPPER_MINES_KEY_F5_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
}

void EnterDungeon(double dungeon_level, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_enter_dungeon = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ENTER_DUNGEON,
		(PVOID*)&gml_script_enter_dungeon
	);

	RValue result;
	RValue level = dungeon_level;
	RValue undefined;
	RValue* level_ptr = &level;
	RValue* undefined_ptr = &undefined;
	RValue* arguments[3] = { level_ptr, undefined_ptr, undefined_ptr };

	gml_script_enter_dungeon->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		arguments
	);
}

RValue InventoryCountItem(int item_id, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_inventory_count_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INVENTORY_COUNT_ITEM,
		(PVOID*)&gml_script_inventory_count_item
	);

	RValue result;
	RValue item = item_id;
	RValue* item_ptr = &item;

	gml_script_inventory_count_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &item_ptr }
	);

	return result;
}

void InventoryRemoveItem(int item_id, int quantity, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_inventory_remove_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INVENTORY_REMOVE_ITEM,
		(PVOID*)&gml_script_inventory_remove_item
	);

	RValue result;
	RValue item = item_id;
	RValue amount = quantity;
	RValue* item_ptr = &item;
	RValue* amount_ptr = &amount;
	RValue* arguments[2] = { item_ptr, amount_ptr };

	gml_script_inventory_remove_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		2,
		arguments
	);
}

bool AriCurrentGmRoomIsDungeonFloor()
{
	if (boss_battle != BossBattle::NONE)
		return true;
	return ari_current_gm_room.contains("rm_mines") && ari_current_gm_room != "rm_mines_entry" && !ari_current_gm_room.contains("seal");
}

void SetFloorNumber()
{
	if (boss_battle != BossBattle::NONE)
	{
		if (ari_current_gm_room == "rm_mines_tide_ritual_chamber")
			floor_number = 20;
		else if (ari_current_gm_room == "rm_mines_deep_ritual_chamber")
			floor_number = 40;
		else if (ari_current_gm_room == "rm_mines_lava_ritual_chamber")
			floor_number = 60;
		else if (ari_current_gm_room == "rm_mines_ruins_ritual_chamber")
			floor_number = 80;
	}
	else if (ari_current_gm_room.contains("treasure") || ari_current_gm_room.contains("milestone"))
		return; // Update 0.15.0 changed treasure rooms to "be considered side rooms rather than level-progressing rooms"
	else if (ari_current_gm_room.contains("ritual"))
		return; // Update 0.15.0 changed ritual rooms to "be considered side rooms rather than level-progressing rooms"
	else if (ari_current_gm_room == "rm_mines_upper_floor1")
		floor_number = 1;
	else if (ari_current_gm_room == "rm_mines_upper_elevator5")
		floor_number = 5;
	else if (ari_current_gm_room == "rm_mines_upper_elevator10")
		floor_number = 10;
	else if (ari_current_gm_room == "rm_mines_upper_elevator15")
		floor_number = 15;
	else if (ari_current_gm_room == "rm_water_seal")
		floor_number = 20;
	else if (ari_current_gm_room == "rm_mines_tide_floor21")
		floor_number = 21;
	else if (ari_current_gm_room == "rm_mines_tide_elevator25")
		floor_number = 25;
	else if (ari_current_gm_room == "rm_mines_tide_elevator30")
		floor_number = 30;
	else if (ari_current_gm_room == "rm_mines_tide_elevator35")
		floor_number = 35;
	else if (ari_current_gm_room == "rm_earth_seal")
		floor_number = 40;
	else if (ari_current_gm_room == "rm_mines_deep_41")
		floor_number = 41;
	else if (ari_current_gm_room == "rm_mines_deep_45")
		floor_number = 45;
	else if (ari_current_gm_room == "rm_mines_deep_50")
		floor_number = 50;
	else if (ari_current_gm_room == "rm_mines_deep_55")
		floor_number = 55;
	else if (ari_current_gm_room == "rm_fire_seal")
		floor_number = 60;
	else if (ari_current_gm_room == "rm_mines_lava_61")
		floor_number = 61;
	else if (ari_current_gm_room == "rm_mines_lava_65")
		floor_number = 65;
	else if (ari_current_gm_room == "rm_mines_lava_70")
		floor_number = 70;
	else if (ari_current_gm_room == "rm_mines_lava_75")
		floor_number = 75;
	else if (ari_current_gm_room == "rm_ruins_seal" || ari_current_gm_room == "rm_void_seal")
		floor_number = 80;
	else if (ari_current_gm_room == "rm_mines_ruins_85")
		floor_number = 85;
	else if (ari_current_gm_room == "rm_priestess_quarters")
		floor_number = 90;
	else if (ari_current_gm_room == "rm_mines_ruins_95")
		floor_number = 95;
	else if (ari_current_gm_room == "rm_seridias_chamber")
		floor_number = 100;
	else
		floor_number++;
}

RValue GetDynamicItemSprite(int item_id)
{
	if (item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
	{
		if (active_traps.contains(Traps::INHIBITING) || !AriCurrentGmRoomIsDungeonFloor())
		{
			if (floor_number < 20)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword_disabled" });
			else if (floor_number < 40)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword_disabled" });
			else if (floor_number < 60)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword_disabled" });
		}
		else
		{
			if (floor_number < 20)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword" });
			else if (floor_number < 40)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword" });
			else if (floor_number < 60)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword" });
		}
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_pickaxe" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_pickaxe" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_pickaxe" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_pickaxe" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_4" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::ALTERATION])
	{
		if (active_sigils.contains(Sigils::ALTERATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_alteration_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_alteration" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::CONCEALMENT])
	{
		if (active_sigils.contains(Sigils::CONCEALMENT) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_concealment_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_concealment" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::FORTIFICATION])
	{
		if (active_sigils.contains(Sigils::FORTIFICATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortification_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortification" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::FORTUNE])
	{
		if (active_sigils.contains(Sigils::FORTUNE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortune_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortune" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::PROTECTION])
	{
		if (active_sigils.contains(Sigils::PROTECTION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || GetInvulnerabilityHits() > 0 || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_protection_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_protection" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::RAGE])
	{
		if (active_sigils.contains(Sigils::RAGE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_rage_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_rage" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::REDEMPTION])
	{
		if (active_sigils.contains(Sigils::REDEMPTION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || FairyBuffIsActive() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_redemption_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_redemption" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SAFETY])
	{
		if (active_sigils.contains(Sigils::SAFETY) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_safety_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_safety" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SERENITY])
	{
		if (active_sigils.contains(Sigils::SERENITY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_serenity_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_serenity" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SILENCE])
	{
		if (active_sigils.contains(Sigils::SILENCE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_silence_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_silence" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::STRENGTH])
	{
		if (active_sigils.contains(Sigils::STRENGTH) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_strength_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_strength" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::TEMPTATION])
	{
		if (active_sigils.contains(Sigils::TEMPTATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0 || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_temptation_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_temptation" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SIGHT])
	{
		if (active_sigils.contains(Sigils::SIGHT) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_sight_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_sight" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::INTUITION])
	{
		if (active_sigils.contains(Sigils::INTUITION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_intuition_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_intuition" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_benediction_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_benediction" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_astral_flow_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_astral_flow" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_chain_spell_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_chain_spell" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_spirit_surge_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_spirit_surge" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_meikyo_shisui_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_meikyo_shisui" });
	}
	else if (item_id == salve_name_to_id_map[SUSTAINING_POTION_NAME])
	{
		if (active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_potion_sustain_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_potion_sustain" });
	}
	else if (item_id == salve_name_to_id_map[HEALTH_SALVE_NAME])
	{
		if (salves_used[HEALTH_SALVE_NAME] >= Config::config.health_salve_limit || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_health_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_health" });
	}
	else if (item_id == salve_name_to_id_map[STAMINA_SALVE_NAME])
	{
		if (salves_used[STAMINA_SALVE_NAME] >= Config::config.stamina_salve_limit || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_stamina_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_stamina" });
	}
	else if (item_id == salve_name_to_id_map[MANA_SALVE_NAME])
	{
		if (salves_used[MANA_SALVE_NAME] >= Config::config.mana_salve_limit || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_mana_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_mana" });
	}
}

RValue GetDynamicUiSprite(std::string sprite_name)
{
	// OnDungeonRoomStart Conversation Sprite Overrides
	if (!journal_menu_open) // floor_start_time == current_time_in_seconds
	{
		// Priestess Portrait Replacement
		if (sprite_name.contains("spr_portrait_seridia") && !sprite_name.contains("flashback"))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_portrait_seridia_flashback_priestess_sad" });
		// Heart Insert Icon Conversation Replacement
		else if (sprite_name == "spr_ui_dialogue_namebar_heartinsert")
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_dialogue_namebar_circleinsert" });
		// Heart & Circle Icon Conversation Replacement
		else if (sprite_name.contains("spr_ui_dialogue_heart_") || sprite_name.contains("spr_ui_dialogue_circle_"))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_dialogue_circle_purple" });
	}

	// Full Restore (Spell Icon)
	if (sprite_name == "spr_ui_journal_magic_restore_spell_icon_main")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_spell_icon_main" });
		}
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_spell_icon_main" });
			}
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_spell_icon_main" });
			}
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_spell_icon_main" });
			}
		}
		// Predict (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_spell_icon_main" });
		}
		// Full Restore Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_restore_spell_icon_disabled" });
	}
	// Summon Rain (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_rain_spell_icon_main")
	{
		// Flood (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_spell_icon_main" });
		}
		// Summon Rain Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_rain_spell_icon_disabled" });
	}
	// Growth (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_growth_spell_icon_main")
	{
		// Quake (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_spell_icon_main" });
		}
		// Condemn (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_spell_icon_main" });
		}
		// Growth Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_growth_spell_icon_disabled" });
	}
	// Fire Breath (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_fire_spell_icon_main")
	{
		// Fire Breath Disabled
		if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_fire_spell_icon_disabled" });
	}
	// Sacred Light (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_sacred_light_spell_icon_main")
	{
		// Sacred Light Disabled
		if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_sacred_light_spell_icon_disabled" });
	}
	// Dungeon Backplate
	else if (sprite_name == "spr_ui_dungeon_backplate")
	{
		// Floor Enchantments & Offerings
		if (!active_floor_enchantments.empty() || !active_offerings.empty())
		{
			std::string sprite_name = "backplate";
			std::string group_one_enchantment_str = "";
			std::string group_two_enchantment_str = "";
			std::string group_three_enchantment_str = "";
			std::string offering_str = "";

			for (FloorEnchantments floor_enchantment : active_floor_enchantments)
			{
				auto group_one_enchantment = std::find(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.begin(), GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_one_enchantment != GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.end())
					group_one_enchantment_str += magic_enum::enum_name(floor_enchantment);

				auto group_two_enchantment = std::find(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.begin(), GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_two_enchantment != GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.end())
					group_two_enchantment_str += magic_enum::enum_name(floor_enchantment);

				auto group_three_enchantment = std::find(GROUP_THREE_FLOOR_ENCHANTMENTS.begin(), GROUP_THREE_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_three_enchantment != GROUP_THREE_FLOOR_ENCHANTMENTS.end())
					group_three_enchantment_str += magic_enum::enum_name(floor_enchantment);
			}

			for (Offerings offering : active_offerings)
				offering_str += magic_enum::enum_name(offering);

			if (!group_one_enchantment_str.empty())
				sprite_name += "_" + group_one_enchantment_str;
			if (!group_two_enchantment_str.empty())
				sprite_name += "_" + group_two_enchantment_str;
			if (!group_three_enchantment_str.empty())
				sprite_name += "_" + group_three_enchantment_str;
			if (!offering_str.empty())
				sprite_name += "_" + offering_str;

			std::transform(sprite_name.begin(), sprite_name.end(), sprite_name.begin(), [](unsigned char c) { return std::tolower(c); });
			return g_ModuleInterface->CallBuiltin("asset_get_index", { RValue(sprite_name) });
		}
		// Empty
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { RValue("backplate_empty") });
	}
	// Full Restore (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_restore_card_icon")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_card_icon" });
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_card_icon" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_card_icon" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_card_icon" });
		}
		// Predict (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_card_icon" });
	}
	// Summon Rain (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_rain_card_icon")
	{
		// Flood (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_card_icon" });
	}
	// Growth (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_growth_card_icon")
	{
		// Quake (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_card_icon" });
		// Condemn (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_card_icon" });
	}
	// Full Restore (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_restore")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_siphon_life" });
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enfire" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enblizzard" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enpoison" });
		}
		// Predict (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_predict" });
	}
	// Summon Rain (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_rain")
	{
		// Flood (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_flood" });
	}
	// Growth (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_growth")
	{
		// Quake (Mage Set Bonus)
		if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_quake" });
		// Condemn (Oracle Set Bonus)
		else if (CountEquippedClassArmor()[Classes::ORACLE] >= 5)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_condemn" });
	}
	// Spell Card Backplate
	else if (sprite_name == "spr_ui_journal_magic_card_backplate")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (CountEquippedClassArmor()[Classes::DARK_KNIGHT] >= 3)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_sipon_life_card_backplate" });
		// Flood (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 2)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_flood_card_backplate" });
		// Elemental Seal (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 3 && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enfire_card_backplate" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enblizzard_card_backplate" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enpoison_card_backplate" });
		}
		// Quake (Mage Set Bonus)
		else if (CountEquippedClassArmor()[Classes::MAGE] >= 4)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_quake_card_backplate" });
	}
	return RValue();
}

std::unordered_set<FloorEnchantments> RandomFloorEnchantments(bool is_first_floor, DungeonBiomes dungeon_biome)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

	std::unordered_set<FloorEnchantments> random_floor_enchantments = {};

	if (is_first_floor)
	{
		const std::vector<FloorEnchantments> FIRST_FLOOR_POSSIBLE_ENCHANTMENTS = {
			FloorEnchantments::RESTORATION, FloorEnchantments::SECOND_WIND, FloorEnchantments::HASTE
		};

		std::uniform_int_distribution<size_t> first_floor_distribution(0, FIRST_FLOOR_POSSIBLE_ENCHANTMENTS.size() - 1);
		return { FIRST_FLOOR_POSSIBLE_ENCHANTMENTS[first_floor_distribution(random_generator)] };
	}

	if (dungeon_biome == DungeonBiomes::UPPER)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 70% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 70)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 45% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 45)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 20% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 20)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 50) // 50% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 50% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 50% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 50)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 25% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 25)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::TIDE_CAVERNS)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 85% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 60% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 60)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 35% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 35)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 60) // 60% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 40% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 40% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 40)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 15% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 15)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 60) // 60% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 40% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::DEEP_EARTH)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 85% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 40% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 40)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 70) // 70% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 30% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 45% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 45)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 65% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 65)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 20% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 20)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 70) // 70% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 30% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::LAVA_CAVES)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 80% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 80)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 95% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 95)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 45% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 45)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 80) // 80% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 20% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 60% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 60)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 75% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 75)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 25% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 25)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 80) // 80% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 20% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	if (dungeon_biome == DungeonBiomes::RUINS)
	{
		// Predict (Oracle Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] == 1 && CountEquippedClassArmor()[Classes::ORACLE] >= 5)
		{
			// 85% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 85)
			{
				std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
			}

			// 95% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 95)
			{
				std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
				random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
			}

			// 50% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 50)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 90) // 90% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 10% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
		// Default
		else
		{
			// 65% chance for Group 1
			int group_one_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_one_chance < 65)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_one_distribution(0, GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS[group_one_distribution(random_generator)]);
				}
			}

			// 75% chance for Group 2
			int group_two_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_two_chance < 75)
			{
				if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
				else
				{
					std::uniform_int_distribution<size_t> group_two_distribution(0, GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.size() - 1);
					random_floor_enchantments.insert(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS[group_two_distribution(random_generator)]);
				}
			}

			// 30% chance for Group 3
			int group_three_chance = zero_to_ninety_nine_distribution(random_generator);
			if (group_three_chance < 30)
			{
				int gloom_chance = zero_to_ninety_nine_distribution(random_generator);
				if (gloom_chance < 90) // 90% chance for Gloom
					random_floor_enchantments.insert(FloorEnchantments::GLOOM);
				else // 10% chance for Fey
					random_floor_enchantments.insert(FloorEnchantments::FEY);
			}
		}
	}

	return random_floor_enchantments;
}

void GenerateFloorTraps()
{
	if (TRAP_SPAWN_POINTS.contains(ari_current_gm_room))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

		if (spawn_points.empty())
			return;

		int min_traps = 2;
		int biome_adjusted_max_traps = (floor_number / 20) + 2;

		std::uniform_int_distribution<size_t> traps_for_room_distribution(min_traps, biome_adjusted_max_traps);
		int random_trap_count = traps_for_room_distribution(random_generator);

		// Disarm Trap (Rogue Set Bonus)
		if (CountEquippedClassArmor()[Classes::ROGUE] >= 4)
			random_trap_count -= 2;

		// Peril
		if (active_offerings.contains(Offerings::PERIL))
			random_trap_count += 2;

		int count = 0;
		for (int i = 0; i < random_trap_count; i++)
		{
			std::uniform_int_distribution<size_t> random_number_of_traps_for_current_room(0, spawn_points.size() - 1);
			int random_index = random_number_of_traps_for_current_room(random_generator);
			floor_trap_positions.insert(spawn_points[random_index]);
			spawn_points.erase(spawn_points.begin() + random_index);
		}
	}
}

void GenerateTreasureSpot(CInstance* Self, CInstance* Other)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_seven_distribution(0, 7);

	int biome_adjusted_max_traps_with_peril = (floor_number / 20) + 4;
	int floors_to_descend = zero_to_seven_distribution(random_generator);

	if (floor_trap_positions.size() == biome_adjusted_max_traps_with_peril)
		floors_to_descend++;

	treasure_spot.floors_to_descend = floors_to_descend;
	treasure_spot.state = TreasureSpot::WAITING_TO_SPAWN;

	if (treasure_spot.floors_to_descend > 0)
		CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
}

std::vector<int> GenerateRandomMonstersIdsForCurrentFloor(int monsters_to_spawn, const int monster_id_to_exclude)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::vector<int> candidate_monsters(dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].begin(), dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].end());

	if (monster_id_to_exclude != -1)
		candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_id_to_exclude), candidate_monsters.end());
	
	// Don't spawn griffin statues for luring traps.
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["griffin_statue"]), candidate_monsters.end());

	std::vector<int> random_monsters = {};
	for (int i = 0; i < monsters_to_spawn; i++)
	{
		std::uniform_int_distribution<size_t> random_monster_distribution(0, candidate_monsters.size() - 1);
		random_monsters.push_back(candidate_monsters[random_monster_distribution(random_generator)]);
	}

	return random_monsters;
}

int SelectRandomMonsterForAlteration()
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::vector<int> candidate_monsters(dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].begin(), dungeon_biome_to_candidate_monsters_map[floor_number_to_biome_name_map[floor_number]].end());

	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["stalagmite"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["stalagmite_green"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["stalagmite_purple"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["spirit"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["spirit_purple"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["mimic"]), candidate_monsters.end());
	candidate_monsters.erase(std::remove(candidate_monsters.begin(), candidate_monsters.end(), monster_name_to_id_map["griffin_statue"]), candidate_monsters.end());

	std::uniform_int_distribution<size_t> random_monster_distribution(0, candidate_monsters.size() - 1);
	return candidate_monsters[random_monster_distribution(random_generator)];
}

void SpawnDreadBeast(CInstance* Self, CInstance* Other)
{
	if (TRAP_SPAWN_POINTS.contains(ari_current_gm_room))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

		if (spawn_points.empty())
			return;

		std::uniform_int_distribution<size_t> trap_spawn_points_distribution(0, spawn_points.size() - 1);
		int random_index = trap_spawn_points_distribution(random_generator);
		std::pair<int, int> spawn_point = spawn_points[random_index];

		// TODO: Update this as dread beast logic is implemented
		std::vector<std::string> possible_dread_beast_monsters = {};
		if (floor_number < 20)
			possible_dread_beast_monsters = { "rockclod", "sapling", "sapling_cool", "mushroom", "enchantern" };
		else if (floor_number < 40)
			possible_dread_beast_monsters = { "rockclod_blue", "sapling_blue", "mushroom_green", "enchantern_blue", "stalagmite", "bat" };
		else if (floor_number < 60)
			possible_dread_beast_monsters = { "rockclod_green", "sapling_purple", "mushroom_blue", "stalagmite_green", "bat_blue" };
		else if (floor_number < 80)
			possible_dread_beast_monsters = { "rockclod_red", "sapling_orange", "mushroom_purple", "stalagmite_purple", "spirit", "cat" };
		else
			possible_dread_beast_monsters = { "sapling_pink", "spirit_purple", "cat_void", "rock_stack", "tome" }; // TODO: "rockclod_purple" if/when implemented

		std::uniform_int_distribution<size_t> random_dread_beast_distribution(0, possible_dread_beast_monsters.size() - 1);
		random_index = random_dread_beast_distribution(random_generator);
		int monster_id = monster_name_to_id_map[possible_dread_beast_monsters[random_index]];

		dread_beast_monster_id = monster_id;
		SpawnMonster(Self, Other, spawn_point.first, spawn_point.second, monster_id);
	}
}

void SelectDreadBeast(CInstance* Self, CInstance* Other)
{
	// TODO: Update this as Dread Beasts are implemented by removing the prune statements below.
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["barrel"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["copperclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["goldclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["ironclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["mimic"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["mistrilclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["rock_stack_lava"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["sapling_orange_mini"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["silverclod"]), initial_floor_monsters.end());
	initial_floor_monsters.erase(std::remove(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["rockclod_purple"]), initial_floor_monsters.end()); // TODO: don't remove if/when implemented (MONSTER NOT IMPLEMENTED)

	if (initial_floor_monsters.size() > 0)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<int> initial_floor_monster_distribution(0, initial_floor_monsters.size() - 1);

		dread_beast_monster_id = initial_floor_monsters[initial_floor_monster_distribution(random_generator)];
	}
	else
		SpawnDreadBeast(Self, Other);
}

RValue GetUnifiedTime(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_unified_time = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_UNIFIED_TIME,
		(PVOID*)&gml_script_get_unified_time
	);

	RValue result;
	gml_script_get_unified_time->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void CancelStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id)
{
	CScript* gml_script_cancel_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CANCEL_STATUS_EFFECT,
		(PVOID*)&gml_script_cancel_status_effect
	);

	RValue result;
	RValue* status_effect_id_ptr = &status_effect_id;

	gml_script_cancel_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &status_effect_id_ptr }
	);
}

void CancelAllStatusEffects()
{
	std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

	// Remove all status effects
	for (int i = 0; i <= status_effect_name_to_id_map.size(); i++)
		CancelStatusEffect(refs[0], refs[1], i);
}

void RegisterStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id, RValue amount, RValue start, RValue finish)
{
	CScript* gml_script_register_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_REGISTER_STATUS_EFFECT,
		(PVOID*)&gml_script_register_status_effect
	);

	RValue result;
	RValue* status_effect_id_ptr = &status_effect_id;
	RValue* amount_ptr = &amount;
	RValue* start_ptr = &start;
	RValue* finish_ptr = &finish;
	RValue* argument_array[4] = { status_effect_id_ptr, amount_ptr, start_ptr, finish_ptr };

	gml_script_register_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		4,
		argument_array
	);
}

RValue GetHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_HEALTH,
		(PVOID*)&gml_script_get_health
	);

	RValue result;
	gml_script_get_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void SetHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_set_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SET_HEALTH,
		(PVOID*)&gml_script_set_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_set_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

void ModifyHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_HEALTH,
		(PVOID*)&gml_script_modify_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_modify_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

RValue GetMaxHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MAX_HEALTH,
		(PVOID*)&gml_script_get_max_health
	);

	RValue result;
	gml_script_get_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void SetMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("base_health") = value;

	RValue current_health = GetHealth(Self, Other);
	if (current_health.ToInt64() > value)
		SetHealth(Self, Other, value);
}

int ModifyMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	int max_health = __ari.GetMember("base_health").ToInt64() + value;
	*__ari.GetRefMember("base_health") = max_health;

	return max_health;
}

void VitalsMenuSetHealth(CInstance* Self, CInstance* Other, int current_health_value, int max_health_value)
{
	CScript* gml_script_vitals_menu_set_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_health
	);

	RValue result;
	RValue current_health = current_health_value;
	RValue max_health = max_health_value;
	RValue* current_health_ptr = &current_health;
	RValue* max_health_ptr = &max_health;
	RValue arg2 = false;
	RValue* arg2_ptr = &arg2;
	RValue* argument_array[3] = { current_health_ptr, max_health_ptr, arg2_ptr };

	gml_script_vitals_menu_set_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		argument_array
	);
}

void VitalsMenuSetMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_vitals_menu_set_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_max_health
	);

	RValue result;
	RValue max_health = value;
	RValue* max_health_ptr = &max_health;

	gml_script_vitals_menu_set_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &max_health_ptr }
	);
}

RValue GetStamina(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_STAMINA,
		(PVOID*)&gml_script_get_stamina
	);

	RValue result;
	gml_script_get_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void ModifyStamina(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	RValue result;
	RValue stamina_modifier = value;
	RValue* stamina_modifier_ptr = &stamina_modifier;

	gml_script_modify_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &stamina_modifier_ptr }
	);
}

RValue GetMana(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MANA,
		(PVOID*)&gml_script_get_mana
	);

	RValue result;
	gml_script_get_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void ModifyMana(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_MANA,
		(PVOID*)&gml_script_modify_mana
	);

	RValue result;
	RValue mana_modifier = value;
	RValue* mana_modifier_ptr = &mana_modifier;

	gml_script_modify_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &mana_modifier_ptr }
	);
}

void CastSpell(CInstance* Self, CInstance* Other, int spell_id)
{
	CScript* gml_script_cast_spell = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CAST_SPELL,
		(PVOID*)&gml_script_cast_spell
	);

	RValue result;
	RValue health_modifier = spell_id;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_cast_spell->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

void RevealFloorTraps()
{
	if (active_sigils.contains(Sigils::SIGHT) && revealed_floor_traps.empty())
	{
		for (auto floor_trap = floor_trap_positions.begin(); floor_trap != floor_trap_positions.end(); floor_trap++) {
			RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
			if (instance_layer_exists)
			{
				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

				RevealedFloorTrap revealed_floor_trap = RevealedFloorTrap(floor_trap->first, floor_trap->second, true, instance);
				revealed_floor_traps.push_back(revealed_floor_trap);
			}
		}
	}

	for (RevealedFloorTrap revealed_floor_trap : revealed_floor_traps)
	{
		if (revealed_floor_trap.is_active && !floor_trap_positions.contains({ revealed_floor_trap.x, revealed_floor_trap.y }))
		{
			revealed_floor_trap.is_active = false;

			RValue revealed_floor_trap_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { revealed_floor_trap.instance });
			if (revealed_floor_trap_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { revealed_floor_trap.instance });
		}
	}
}

void ApplyFloorTraps(CInstance* Self, CInstance* Other)
{
	// Prune traps that have fully applied.
	if (active_traps.contains(Traps::CONFUSING) && active_traps_to_value_map[Traps::CONFUSING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect ended at: %d", MOD_NAME, VERSION, current_time_in_seconds);
		active_traps.erase(Traps::CONFUSING);
		active_traps_to_value_map.erase(Traps::CONFUSING);
	}
	if (active_traps.contains(Traps::DISORIENTING) && active_traps_to_value_map[Traps::DISORIENTING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect ended at: %d", MOD_NAME, VERSION, current_time_in_seconds);
		active_traps.erase(Traps::DISORIENTING);
		active_traps_to_value_map.erase(Traps::DISORIENTING);
	}
	if (active_traps.contains(Traps::INHIBITING) && active_traps_to_value_map[Traps::INHIBITING] < current_time_in_seconds)
	{
		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect ended at: %d", MOD_NAME, VERSION, current_time_in_seconds);
		active_traps.erase(Traps::INHIBITING);
		active_traps_to_value_map.erase(Traps::INHIBITING);

		if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
			UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
	}

	// Process traps in range.
	for (auto floor_trap = floor_trap_positions.begin(); floor_trap != floor_trap_positions.end();) {
		double distance = GetDistance(ari_x, ari_y, floor_trap->first, floor_trap->second);
		if (distance <= 16)
		{
			static thread_local pcg32 random_generator([] {
				std::random_device rd;
				return pcg32(
					(static_cast<uint64_t>(rd()) << 32) | rd(),
					(static_cast<uint64_t>(rd()) << 32) | rd()
				);
			}());
			std::uniform_int_distribution<size_t> random_trap_distribution(0, magic_enum::enum_count<Traps>() - 1);
			std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

			Traps trap = magic_enum::enum_value<Traps>(random_trap_distribution(random_generator));

			// Hallowed Ground (Paladin Set Bonus)
			bool malfunction = zero_to_ninety_nine_distribution(random_generator) < 50 ? true : false;
			if (CountEquippedClassArmor()[Classes::PALADIN] == 5 && malfunction)
			{
				PlaySoundEffect("snd_bark_heart603", 100, 1);
				CreateNotification(true, MALFUNCTION_TRAP_NOTIFICATION_KEY, Self, Other);
			}
			else
			{
				g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Trap Triggered: %s", MOD_NAME, VERSION, magic_enum::enum_name(trap).data());
				active_traps.insert({ trap, { floor_trap->first, floor_trap->second } });

				if (trap == Traps::CONFUSING)
				{
					PlaySoundEffect("snd_bark_o_o", 100, 1);
					CreateNotification(true, CONFUSING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["annoyed"], 0);

					if (!active_traps_to_value_map.contains(Traps::CONFUSING))
					{
						active_traps_to_value_map[Traps::CONFUSING] = current_time_in_seconds + Config::config.confusing_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect started at: %d", MOD_NAME, VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::CONFUSING] += Config::config.confusing_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Confusing Trap effect extended by: %d", MOD_NAME, VERSION, Config::config.confusing_trap_duration_seconds);
					}
				}
				else if (trap == Traps::DISORIENTING)
				{
					PlaySoundEffect("snd_interactable_scan", 100, 1);
					CreateNotification(true, DISORIENTING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["annoyed"], 0);

					if (!active_traps_to_value_map.contains(Traps::DISORIENTING))
					{
						active_traps_to_value_map[Traps::DISORIENTING] = current_time_in_seconds + Config::config.disorienting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect started at: %d", MOD_NAME, VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::DISORIENTING] += Config::config.disorienting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Disorienting Trap effect extended by: %d", MOD_NAME, VERSION, Config::config.disorienting_trap_duration_seconds);
					}
				}
				else if (trap == Traps::EXPLODING)
				{
					PlaySoundEffect("snd_Explosion_CaveReverb", 100, 0.35);
					CreateNotification(true, EXPLODING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["angry"], 0);
				}
				else if (trap == Traps::INHIBITING)
				{
					PlaySoundEffect("snd_bark_surprised", 100, 1);
					CreateNotification(true, INHIBITING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["no_coin"], 0);

					if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
						UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);

					if (!active_traps_to_value_map.contains(Traps::INHIBITING))
					{
						active_traps_to_value_map[Traps::INHIBITING] = current_time_in_seconds + Config::config.inhibiting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect started at: %d", MOD_NAME, VERSION, current_time_in_seconds);
					}
					else
					{
						active_traps_to_value_map[Traps::INHIBITING] += Config::config.inhibiting_trap_duration_seconds;
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Inhibiting Trap effect extended by: %d", MOD_NAME, VERSION, Config::config.inhibiting_trap_duration_seconds);
					}
				}
				else if (trap == Traps::LURING)
				{
					std::uniform_int_distribution<int> random_position_offset_distribution(-12, 12);

					PlaySoundEffect("snd_ScrollRaise", 100, 1);
					CreateNotification(true, LURING_TRAP_NOTIFICATION_KEY, Self, Other);
					EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

					// TODO: Restrict monster spawns as necessary (stalagmite_pink? TBD)
					std::vector<int> random_monsters;
					if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite"]);
					else if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite_green"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite_green"]);
					else if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), monster_name_to_id_map["stalagmite_purple"]) != initial_floor_monsters.end())
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count, monster_name_to_id_map["stalagmite_purple"]);
					else
						random_monsters = GenerateRandomMonstersIdsForCurrentFloor(Config::config.luring_trap_monster_spawn_count);

					for (int i = 0; i < random_monsters.size(); i++)
						SpawnMonster(Self, Other, floor_trap->first + random_position_offset_distribution(random_generator), floor_trap->second + random_position_offset_distribution(random_generator), random_monsters[i]);

					active_traps.erase(Traps::LURING);
				}
				else if (trap == Traps::METEOR)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_VoidPortalSpawn", 100, 0.7);
						CreateNotification(true, METEOR_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });
						
						CustomAOE meteor = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 600, current_time_in_seconds, true, instance, CustomAOETypes::METEOR);
						meteor_aoes.push_back(meteor);
					}

					active_traps.erase(Traps::METEOR);
				}
				else if (trap == Traps::GAZE)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_VoidMassAppear", 100, 0.7);
						CreateNotification(true, GAZE_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });
						
						CustomAOE gaze = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, 600, current_time_in_seconds, true, instance, CustomAOETypes::GAZE);
						gaze_aoes.push_back(gaze);
					}

					active_traps.erase(Traps::GAZE);
				}
				else if (trap == Traps::_VOID)
				{
					RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
					if (instance_layer_exists)
					{
						PlaySoundEffect("snd_MagicVoidLightSpell", 100, 0.3);
						CreateNotification(true, VOID_TRAP_NOTIFICATION_KEY, Self, Other);
						EmitBark(script_name_to_reference_map[GML_SCRIPT_BARK_EMITTER][0], script_name_to_reference_map["obj_ari"][1], bark_name_to_id_map["exclamation_mark"], 0);

						RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
						RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { floor_trap->first, floor_trap->second, RValue("Instances"), obj_assetobject });

						CustomAOE void_aoe = CustomAOE(floor_trap->first, floor_trap->second, current_time_in_seconds, Config::config.void_trap_duration_seconds, current_time_in_seconds, true, instance, CustomAOETypes::_VOID);
						void_aoes.push_back(void_aoe);
					}

					active_traps.erase(Traps::_VOID);
				}
			}

			floor_trap = floor_trap_positions.erase(floor_trap);
		}
		else
			++floor_trap;
	}
}

void ProcessCustomAOEs()
{
	for (CustomAOE& meteor : meteor_aoes)
	{
		if (meteor.is_active && current_time_in_seconds >= meteor.spawned_time + meteor.duration)
		{
			meteor.is_active = false;
			PlaySoundEffect("snd_EarthquakeImpact", 1, 1);
			PlaySoundEffect("snd_AriLowHealthWarning", 1, 1);

			double distance = GetDistance(ari_x, ari_y, meteor.x, meteor.y);
			double modifier = CalculateMeteorDamage(distance) / 100.0;

			double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
			ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);

			RValue meteor_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { meteor.instance });
			if (meteor_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { meteor.instance });
		}
	}

	for (CustomAOE& gaze : gaze_aoes)
	{
		if (gaze.is_active && current_time_in_seconds >= gaze.spawned_time + gaze.duration)
		{
			gaze.is_active = false;
			PlaySoundEffect("snd_CosmicImpact", 1, 0.30);

			bool facing_trap = FacingTrap(ari_x, ari_y, gaze.x, gaze.y);
			if (facing_trap)
			{
				double modifier = Config::config.gaze_trap_max_health_damage_percent / 100.0;
				double adjusted_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToDouble() * modifier;
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1 * adjusted_health);
				PlaySoundEffect("snd_AriLowHealthWarning", 1, 1);
			}

			RValue gaze_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { gaze.instance });
			if (gaze_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { gaze.instance });
		}
	}

	for (CustomAOE& void_aoe : void_aoes)
	{
		if (void_aoe.is_active && current_time_in_seconds >= void_aoe.last_application + 15) // TODO: Make tick rate configurable
		{
			void_aoe.last_application = current_time_in_seconds;

			double distance = GetDistance(ari_x, ari_y, void_aoe.x, void_aoe.y);
			if (distance > 96 && distance <= 298)
				ModifyHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], -1);
		}

		if (void_aoe.is_active && current_time_in_seconds >= void_aoe.spawned_time + void_aoe.duration)
		{
			void_aoe.is_active = false;

			RValue void_trap_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { void_aoe.instance });
			if (void_trap_instance_exists.ToBoolean())
				g_ModuleInterface->CallBuiltin("instance_destroy", { void_aoe.instance });
		}
	}
}

void ProcessTreasureSpot(CInstance* Self, CInstance* Other)
{
	if (treasure_spot.state == TreasureSpot::WAITING_TO_SPAWN && treasure_spot.floors_to_descend == 0)
	{
		int biome_adjusted_max_traps_with_peril = (floor_number / 20) + 4;
		if (floor_trap_positions.size() < biome_adjusted_max_traps_with_peril)
		{
			// Find a position for the treasure spot on the current floor.
			std::vector<std::pair<int, int>> spawn_points = TRAP_SPAWN_POINTS.at(ari_current_gm_room);

			std::pair<int, int> treasure_spot_coordinates;
			for (std::pair<int, int> spawn_point : spawn_points)
			{
				if (floor_trap_positions.contains(spawn_point))
					continue;

				treasure_spot_coordinates = spawn_point;
				break;
			}

			RValue instance_layer_exists = g_ModuleInterface->CallBuiltin("layer_exists", { "Instances" });
			if (instance_layer_exists)
			{
				PlaySoundEffect("snd_TreasureChestSpawn", 100, 0.7);
				CreateNotification(true, TREASURE_SPOT_SPAWNED_NOTIFICATION_KEY, Self, Other);

				RValue obj_assetobject = g_ModuleInterface->CallBuiltin("asset_get_index", { "obj_assetobject" });
				RValue instance = g_ModuleInterface->CallBuiltin("instance_create_layer", { treasure_spot_coordinates.first, treasure_spot_coordinates.second, RValue("Instances"), obj_assetobject });

				treasure_spot.x = treasure_spot_coordinates.first;
				treasure_spot.y = treasure_spot_coordinates.second;
				treasure_spot.is_active = true;
				treasure_spot.instance = instance;
				treasure_spot.state = TreasureSpot::SPAWNED;
			}
			else
			{
				treasure_spot.floors_to_descend++;
				CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
			}
		}
		else
		{
			treasure_spot.floors_to_descend++;
			CreateNotification(true, TREASURE_SPOT_NOT_PRESENT_NOTIFICATION_KEY, Self, Other);
		}
		
	}

	if (treasure_spot.state == TreasureSpot::SPAWNED)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> random_greater_sigil_distribution(0, magic_enum::enum_count<GreaterSigils>() - 1);

		double distance = GetDistance(ari_x, ari_y, treasure_spot.x, treasure_spot.y);
		if (distance <= 8)
		{
			treasure_spot.state = TreasureSpot::FOUND;
			CreateNotification(true, TREASURE_SPOT_FOUND_NOTIFICATION_KEY, Self, Other);

			//GreaterSigils random_greater_sigil = magic_enum::enum_value<GreaterSigils>(random_greater_sigil_distribution(random_generator));
			//DropItem(greater_sigil_to_item_id_map[random_greater_sigil], ari_x, ari_y, Self, Other);
			
			if (floor_number < 20)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE], ari_x, ari_y, Self, Other);
			else if (floor_number < 40)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI], ari_x, ari_y, Self, Other);
			else if (floor_number < 60)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION], ari_x, ari_y, Self, Other);
			else if (floor_number < 80)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL], ari_x, ari_y, Self, Other);
			else if (floor_number < 100)
				DropItem(greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW], ari_x, ari_y, Self, Other);
			
			if (treasure_spot.is_active)
			{
				treasure_spot.is_active = false;

				RValue treasure_spot_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { treasure_spot.instance });
				if (treasure_spot_instance_exists.ToBoolean())
					g_ModuleInterface->CallBuiltin("instance_destroy", { treasure_spot.instance });
			}
		}
	}

}

void ProcessSpiritConcealment()
{
	// TODO: Update as more spirits get added
	for (CInstance* monster : current_floor_monsters)
	{
		if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points"))
		{
			RValue monster_id = monster->GetMember("monster_id");
			double hit_points = monster->GetMember("hit_points").ToDouble();
			if (IsNumeric(monster_id) && (monster_id.ToInt64() == monster_name_to_id_map["spirit_purple"]) && std::isfinite(hit_points) && hit_points > 0)
			{
				if (active_sigils.contains(Sigils::CONCEALMENT) && !StructVariableExists(monster, "__deep_dungeon__deactivated"))
				{
					StructVariableSet(monster, "__deep_dungeon__deactivated", true);
					g_ModuleInterface->CallBuiltin("instance_deactivate_object", { monster });
				}

				else if (!active_sigils.contains(Sigils::CONCEALMENT) && StructVariableExists(monster, "__deep_dungeon__deactivated"))
				{
					StructVariableRemove(monster, "__deep_dungeon__deactivated");
					g_ModuleInterface->CallBuiltin("instance_activate_object", { monster });
				}
			}
		}
	}
}

void ApplyOfferingPenalties(CInstance* Self, CInstance* Other)
{
	if (ari_resource_to_penalty_map[AriResources::HEALTH])
		ModifyHealth(Self, Other, Config::config.offering_health_requirement * -1);
	if (ari_resource_to_penalty_map[AriResources::STAMINA])
		ModifyStamina(Self, Other, Config::config.offering_stamina_requirement * -1);
	if (ari_resource_to_penalty_map[AriResources::MANA])
		ModifyMana(Self, Other, Config::config.offering_mana_requirement * -1);

	ari_resource_to_penalty_map.clear();
}

void TrackAriResources(CInstance* Self, CInstance* Other)
{
	ari_resource_to_value_map[AriResources::HEALTH] = GetHealth(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::MAX_HEALTH] = GetMaxHealth(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::STAMINA] = GetStamina(Self, Other).ToInt64();
	ari_resource_to_value_map[AriResources::MANA] = GetMana(Self, Other).ToInt64();
}

void GenerateTreasureChestLoot(std::string object_name, CInstance* Self, CInstance* Other)
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
	std::uniform_int_distribution<size_t> random_sigil_distribution(0, magic_enum::enum_count<Sigils>() - 1);

	// Sigils
	std::vector<int> sigil_roll_success_thresholds = {};
	if (object_name == TREASURE_CHEST_WOOD_NAME)
		sigil_roll_success_thresholds = { 50, 25, 0, 0 };
	else if (object_name == TREASURE_CHEST_COPPER_NAME)
		sigil_roll_success_thresholds = { 75, 50, 10, 0 };
	else if (object_name == TREASURE_CHEST_SILVER_NAME)
		sigil_roll_success_thresholds = { 100, 50, 25, 0 };
	else if (object_name == TREASURE_CHEST_GOLD_NAME)
		sigil_roll_success_thresholds = { 100, 100, 25, 10 };

	// Treasure Hunter (Rogue Set Bonus)
	if (CountEquippedClassArmor()[Classes::ROGUE] == 5)
		sigil_roll_success_thresholds.push_back(100);

	std::unordered_set<Sigils> sigils_spawned = {};
	for (size_t i = 0; i < sigil_roll_success_thresholds.size(); i++)
	{
		int roll_for_drop = zero_to_ninety_nine_distribution(random_generator);
		if (roll_for_drop < sigil_roll_success_thresholds[i])
		{
			Sigils random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));
			while (sigils_spawned.contains(random_sigil))
				random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));

			sigils_spawned.insert(random_sigil);
			DropItem(sigil_to_item_id_map[random_sigil], ari_x, ari_y, Self, Other);
		}
	}

	// Cursed Armor
	int cursed_armor_roll_success_threshold = 0;
	if (object_name == TREASURE_CHEST_WOOD_NAME)
		cursed_armor_roll_success_threshold = 1 * Config::config.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_COPPER_NAME)
		cursed_armor_roll_success_threshold = 2 * Config::config.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_SILVER_NAME)
		cursed_armor_roll_success_threshold = 3 * Config::config.cursed_armor_drop_chance_modifier;
	else if (object_name == TREASURE_CHEST_GOLD_NAME)
		cursed_armor_roll_success_threshold = 4 * Config::config.cursed_armor_drop_chance_modifier;

	int roll_for_drop = zero_to_ninety_nine_distribution(random_generator);
	if (roll_for_drop < cursed_armor_roll_success_threshold)
	{
		if (floor_number < 20) // Upper Mines
			DropItem(item_name_to_id_map[CURSED_CHESTPIECE_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 40) // Tide Caverns
			DropItem(item_name_to_id_map[CURSED_HELMET_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 60) // Deep Earth
			DropItem(item_name_to_id_map[CURSED_GLOVES_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 80) // Lava Caves
			DropItem(item_name_to_id_map[CURSED_PANTS_NAME], ari_x, ari_y, Self, Other);
		else if (floor_number < 100) // Ruins
			DropItem(item_name_to_id_map[CURSED_BOOTS_NAME], ari_x, ari_y, Self, Other);
	}
}

void ResetStaticFields(bool returned_to_title_screen)
{
	if (returned_to_title_screen)
	{
		game_is_active = false;
		unlock_recipes = true;
		is_restoration_tracked_interval = false;
		is_second_wind_tracked_interval = false;
		is_fumigate_tracked_interval = false;
		is_deep_wounds_tracked_interval = false;
		ari_x = -1;
		ari_y = -1;
		ari_facing_dir = -1;
		floor_number = 0;
		floor_start_time = 0;
		current_time_in_seconds = -1;
		time_of_last_restoration_tick = -1;
		time_of_last_second_wind_tick = -1;
		time_of_last_fumigate_tick = -1;
		time_of_last_deep_wounds_tick = -1;
		time_of_last_outbreak_tick = -1;
		held_item_id = -1;
		ari_current_location = "";
		ari_current_gm_room = "";
		script_name_to_reference_map.clear();
	}

	crafting_menu_open = false;
	journal_menu_open = false;
	drop_biome_reward = false;
	biome_reward_disabled = false;
	dread_beast_configured = false;
	sigil_item_used = false;
	greater_sigil_item_used = false;
	salve_item_used = false;
	lift_key_used = false;
	orb_item_used = false;
	heart_crystal_used = false;
	inner_fire_cast = false;
	reckoning_applied = false;
	fairy_buff_applied = false;
	stoneskin_applied = false;
	offering_chance_occurred = false;
	obj_dragonshrine_focused = false;
	obj_dungeon_elevator_focused = false;
	obj_dungeon_ladder_down_focused = false;
	frailty_hit_counter = 0;
	grudge_counter = 0;
	deep_wounds_damage_pool = 0;
	stoneskin_shield_amount = 0;
	spirit_link_combined_health_pool = 0;
	sigil_of_silence_count = 0;
	sigil_of_alteration_monster_id = 0;
	dread_beast_monster_id = -1;
	dread_beasts_configured = 0;
	boss_monsters_configured = 0;
	salves_used.clear();
	active_sigils.clear();
	active_greater_sigils.clear();
	queued_offerings.clear();
	active_offerings.clear();
	active_floor_enchantments.clear();
	active_traps.clear();
	active_traps_to_value_map.clear();
	floor_trap_positions.clear();
	current_floor_monsters.clear();
	ResetCustomDrawFields();
	ModifySpellCosts(true, false); // TODO: Make sure this works. Go into the dungeon, Growth should be reduced to 4, exit to title, load back in, growth should be 8 (since you won't load into the dungeon).
}

