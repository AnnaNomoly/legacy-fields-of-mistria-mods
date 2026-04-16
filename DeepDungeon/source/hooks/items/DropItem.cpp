#include "../../utils/Utils.h"

RValue& GmlScriptDropItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
		script_name_to_reference_map[GML_SCRIPT_DROP_ITEM] = { Self, Other };

	if (ari_current_gm_room.contains("rm_mines"))
	{
		bool chance_to_spawn_glowstone = false;

		if (Arguments[0]->m_Kind == VALUE_ARRAY)
		{
			size_t array_length;
			g_ModuleInterface->GetArraySize(*Arguments[0], array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* array_element;
				g_ModuleInterface->GetArrayEntry(*Arguments[0], i, array_element);

				if (StructVariableExists(*array_element, "item_id"))
				{
					int item_id = array_element->GetMember("item_id").ToInt64();
					if (item_id == item_name_to_id_map["ore_stone"] && ItemHasBeenAcquired(item_id))
						chance_to_spawn_glowstone = true;
				}
			}
		}
		else if (Arguments[0]->m_Kind == VALUE_INT64 && Arguments[0]->ToInt64() == item_name_to_id_map["ore_stone"] && ItemHasBeenAcquired(Arguments[0]->ToInt64()))
			chance_to_spawn_glowstone = true;

		// TODO: Should there be some RNG for dropping glowstone?
		if (chance_to_spawn_glowstone)
		{
			if (floor_number < 20) // Upper Mines
				DropItem(item_name_to_id_map["glow_stone_tiny"], ari_x, ari_y, Self, Other);
			else if (floor_number < 40) // Tide Caverns
				DropItem(item_name_to_id_map["glow_stone_small"], ari_x, ari_y, Self, Other);
			else if (floor_number < 60) // Deep Earth
				DropItem(item_name_to_id_map["glow_stone_medium"], ari_x, ari_y, Self, Other);
			else if (floor_number < 80) // Lava Caves
				DropItem(item_name_to_id_map["glow_stone_large"], ari_x, ari_y, Self, Other);
			else if (floor_number < 100) // Ruins
				DropItem(item_name_to_id_map["glow_stone_giant"], ari_x, ari_y, Self, Other);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DROP_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
