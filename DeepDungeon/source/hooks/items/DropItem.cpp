#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Maps;

// MMAPI Item::Hooks::BeforeDropItem callback.
void BeforeDropItem(MMAPI::Item::DropItemContext& ctx)
{
	if (is_challenge_mode || !ari_current_gm_room.contains("rm_mines"))
		return;

	const int ore_stone_id = item_name_to_id_map["ore_stone"];
	if (!ctx.IsItem(ore_stone_id) || !ItemHasBeenAcquired(ore_stone_id))
		return;

	if (floor_number < 20) // Upper Mines
		ctx.Drop(item_name_to_id_map["glow_stone_tiny"], ari_x, ari_y);
	else if (floor_number < 40) // Tide Caverns
		ctx.Drop(item_name_to_id_map["glow_stone_small"], ari_x, ari_y);
	else if (floor_number < 60) // Deep Earth
		ctx.Drop(item_name_to_id_map["glow_stone_medium"], ari_x, ari_y);
	else if (floor_number < 80) // Lava Caves
		ctx.Drop(item_name_to_id_map["glow_stone_large"], ari_x, ari_y);
	else if (floor_number < 100) // Ruins
		ctx.Drop(item_name_to_id_map["glow_stone_giant"], ari_x, ari_y);
}
