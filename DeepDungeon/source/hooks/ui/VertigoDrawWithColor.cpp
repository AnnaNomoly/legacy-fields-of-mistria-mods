#include "../../utils/Utils.h"

RValue& GmlScriptVertigoDrawWithColorCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active && !crafting_menu_open && AriCurrentGmRoomIsDungeonFloor()) // TODO: Make sure the AriCurrentGmRoomIsDungeonFloor() condition is working correctly.
	{
		RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { *Arguments[0] });
		if (type.ToInt64() == 1) // asset_sprite
		{
			RValue name = g_ModuleInterface->CallBuiltin("sprite_get_name", { *Arguments[0] });
			std::string name_str = name.ToString();

			RValue dynamic_sprite = GetDynamicUiSprite(name_str);
			if (dynamic_sprite.m_Kind == VALUE_REF)
				*Arguments[0] = dynamic_sprite;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
