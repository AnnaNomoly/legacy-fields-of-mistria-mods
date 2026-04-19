#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::UI;

RValue& GmlScriptOnDrawGuiCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_ON_DRAW_GUI));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (game_is_active && !GameIsPaused())
	{
		// Gloom
		if (active_floor_enchantments.contains(FloorEnchantments::GLOOM))
		{
			g_ModuleInterface->CallBuiltin(
				"draw_set_alpha",
				{ 0.45 } // Set to semi-transparent for overlay
			);

			g_ModuleInterface->CallBuiltin(
				"draw_set_color", {
					8388736 // c_purple
				}
			);

			g_ModuleInterface->CallBuiltin(
				"draw_rectangle",
				{ 0, 0, window_width, window_height, false }
			);

			g_ModuleInterface->CallBuiltin(
				"draw_set_alpha",
				{ 1.0 } // Reset transparency
			);
		}

		// Danger Floor Border
		if (show_dashes)
		{
			DrawDashedBorder(
				20.0f,    // dash length in pixels
				4.0f,     // dash thickness
				80.0f,    // speed pixels per second
				window_width,
				window_height,
				GetCurrentSystemTime()
			);
		}

		// Danger Floor Banner
		if (show_danger_banner)
			FadeInImage(2, 2); // 3, 2

		// Vignette
		if (active_traps.contains(Traps::DISORIENTING))
			DrawVignette();
	}

	return Result;
}
