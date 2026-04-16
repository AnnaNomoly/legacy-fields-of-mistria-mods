#include "../../utils/Utils.h"

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
		std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

		// Sigil of Silence
		if (active_sigils.contains(Sigils::SILENCE))
		{
			int chance_to_activate = zero_to_ninety_nine_distribution(random_generator);
			int activation_threshold = 100;
			for (int i = 0; i < sigil_of_silence_count; i++)
			{
				if(ari_current_gm_room == "rm_mines_ruins_arena3")
					activation_threshold /= 5;
				else
					activation_threshold /= 3;
			}

			bool activate = false;
			if (activation_threshold == 100)
				activate = true;
			else if (chance_to_activate < activation_threshold)
				activate = true;

			sigil_of_silence_count++;

			if (activate)
				return Result;
		}

		// Sigil of Alteration
		if (active_sigils.contains(Sigils::ALTERATION))
		{
			if (sigil_of_alteration_monster_id != -1)
				*Arguments[2] = sigil_of_alteration_monster_id;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SPAWN_MONSTER));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (std::find(initial_floor_monsters.begin(), initial_floor_monsters.end(), Arguments[2]->ToInt64()) == initial_floor_monsters.end())
		initial_floor_monsters.push_back(Arguments[2]->ToInt64());

	return Result;
}
