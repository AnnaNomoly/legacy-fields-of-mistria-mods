#include "../../utils/Utils.h"

using namespace State::UI;
using namespace State::Maps;

// MMAPI Item::Hooks::AfterGetUiIcon callback.
void AfterGetUiIcon(MMAPI::Item::GetUiIconContext& ctx)
{
	if (crafting_menu_open)
		return;

	int item_id = ctx.GetItemId();
	if (item_id < 0)
		return;

	bool modify_icon = deep_dungeon_items.contains(item_id)
		|| default_sword_items.contains(item_id)
		|| item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME]
		|| item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME]
		|| item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME]
		|| item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME]
		|| item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME]
		|| item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME];

	if (modify_icon)
		ctx.SetSpriteAsset(GetDynamicItemSprite(item_id));
}
