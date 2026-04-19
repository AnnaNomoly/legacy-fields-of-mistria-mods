#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

RValue& GmlScriptSpawnMonsterCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!active_traps.contains(Traps::LURING))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());

		// Sigil of Silence — first monster always silenced; each subsequent has a shrinking chance.
		if (active_sigils.contains(Sigils::SILENCE))
		{
			const int divisor = (ari_current_gm_room == "rm_mines_ruins_arena3") ? 5 : 3;
			int activation_threshold = 100;
			for (int i = 0; i < sigil_of_silence_count; i++)
				activation_threshold /= divisor;

			sigil_of_silence_count++;

			std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
			if (activation_threshold == 100 || zero_to_ninety_nine_distribution(random_generator) < activation_threshold)
				return Result;
		}

		// Sigil of Alteration — replaces all spawns with the designated monster.
		if (active_sigils.contains(Sigils::ALTERATION) && sigil_of_alteration_monster_id != -1)
			*Arguments[2] = sigil_of_alteration_monster_id;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SPAWN_MONSTER));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), Arguments[2]->ToInt64()) == initial_floor_monsters.end())
		initial_floor_monsters.push_back(Arguments[2]->ToInt64());

	return Result;
}
