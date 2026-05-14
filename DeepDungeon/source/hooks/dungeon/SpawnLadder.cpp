#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;

// MMAPI Dungeon::Hooks::BeforeSpawnLadder callback.
// Suppresses the ladder during seal rooms and boss-battle ritual chambers.
void BeforeSpawnLadder(MMAPI::Dungeon::SpawnLadderContext& ctx)
{
	if (ari_current_gm_room.contains("seal"))
		ctx.Cancel();
	else if (boss_battle != BossBattle::NONE && ari_current_gm_room.contains("ritual_chamber"))
		ctx.Cancel();
}
