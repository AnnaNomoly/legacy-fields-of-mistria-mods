#include "../../utils/Utils.h"

using namespace State::Maps;

// MMAPI Item::Hooks::BeforeGetTreasure callback.
// DD generates supplementary loot for dungeon treasure chests via GenerateTreasureChestLoot.
void BeforeGetTreasure(MMAPI::Item::GetTreasureContext& ctx)
{
	if (ctx.GetObjectId() < 0)
		return;

	const std::string object_name{ ctx.GetObjectName() };
	if (object_name.empty())
		return;

	if (DUNGEON_TREASURE_CHEST_NAMES.contains(object_name))
		GenerateTreasureChestLoot(object_name, nullptr, nullptr);
}
