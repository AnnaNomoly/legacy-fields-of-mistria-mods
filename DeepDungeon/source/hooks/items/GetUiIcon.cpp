#include "../../utils/Utils.h"

RValue& GmlScriptGetUiIconCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_ITEM_UI_ICON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Self != nullptr && !crafting_menu_open)
	{
		RValue self = Self->ToRValue();
		if (StructVariableExists(self, "item_id"))
		{
			int item_id = self.GetMember("item_id").ToInt64();

			bool modify_icon = false;
			if (deep_dungeon_items.contains(item_id))
				modify_icon = true;
			else if (item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME] || item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME] || item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME] || item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME] || item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME])
				modify_icon = true;
			else if (item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME])
				modify_icon = true;

			if (modify_icon)
				Result = GetDynamicItemSprite(item_id);
		}
	}

	return Result;
}
