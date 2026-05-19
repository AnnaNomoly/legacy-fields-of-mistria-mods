#include "Utils.h"

using namespace State::UI;

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

void DrawVignette()
{
	const float vignette_size = 0.35f; //0.25f; // fraction of screen width/height the gradient extends inward
	const float vignette_alpha = 1.0; //0.85f; // opacity at the screen edges (0.0 = invisible, 1.0 = fully black)

	const float w       = window_width;
	const float h       = window_height;
	const float depth_x = w * vignette_size;
	const float depth_y = h * vignette_size;
	const int   black   = 0;

	g_ModuleInterface->CallBuiltin("draw_set_blend_mode", { 1 }); // bm_normal

	// Top edge
	g_ModuleInterface->CallBuiltin("draw_primitive_begin", { 5 }); // pr_trianglestrip
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { 0,           0, black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w,           0, black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { depth_x, depth_y, black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w - depth_x, depth_y, black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_primitive_end", {});

	// Bottom edge
	g_ModuleInterface->CallBuiltin("draw_primitive_begin", { 5 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { depth_x,     h - depth_y, black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w - depth_x, h - depth_y, black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { 0,           h,           black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w,           h,           black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_primitive_end", {});

	// Left edge
	g_ModuleInterface->CallBuiltin("draw_primitive_begin", { 5 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { 0,       0,           black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { depth_x, depth_y,     black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { 0,       h,           black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { depth_x, h - depth_y, black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_primitive_end", {});

	// Right edge
	g_ModuleInterface->CallBuiltin("draw_primitive_begin", { 5 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w - depth_x, depth_y,     black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w,           0,           black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w - depth_x, h - depth_y, black, 0.0 });
	g_ModuleInterface->CallBuiltin("draw_vertex_color", { w,           h,           black, vignette_alpha });
	g_ModuleInterface->CallBuiltin("draw_primitive_end", {});

	g_ModuleInterface->CallBuiltin("draw_set_blend_mode", { 0 }); // restore bm_normal
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
