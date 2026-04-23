#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::UI;
using namespace State::Maps;

RValue& GmlScriptTakePressCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_TAKE_PRESS));
	original(Self, Other, Result, ArgumentCount, Arguments);

	const bool in_game = game_is_active && !GameIsPaused();
	const bool is_interact = Arguments[0]->ToInt64() == 6 && Result.ToBoolean();

	// Chance for an Offering event when using a ladder on a dungeon floor.
	if (in_game && is_interact && obj_dungeon_ladder_down_focused && !offering_chance_occurred && (floor_number < 19 || floor_number % 10 != 9))
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

		int roll = zero_to_ninety_nine_distribution(random_generator);
		if (active_sigils.contains(Sigils::TEMPTATION) || roll < Config::config.offering_event_chance)
		{
			const std::vector<AriResources> POSSIBLE_OFFERING_RESOURCES = {
				AriResources::HEALTH, AriResources::STAMINA, AriResources::MANA
			};
			std::uniform_int_distribution<size_t> random_offering_resource_distribution(0, POSSIBLE_OFFERING_RESOURCES.size() - 1);

			AriResources resource = POSSIBLE_OFFERING_RESOURCES[random_offering_resource_distribution(random_generator)];
			if (resource == AriResources::HEALTH && ari_resource_to_value_map[AriResources::HEALTH] > Config::config.offering_health_requirement)
				PlayConversation("Conversations/Mods/Deep Dungeon/offering/health", Self, Other);
			else if (resource == AriResources::STAMINA && ari_resource_to_value_map[AriResources::STAMINA] > Config::config.offering_stamina_requirement)
				PlayConversation("Conversations/Mods/Deep Dungeon/offering/stamina", Self, Other);
			else if (resource == AriResources::MANA && ari_resource_to_value_map[AriResources::MANA] > Config::config.offering_mana_requirement)
				PlayConversation("Conversations/Mods/Deep Dungeon/offering/mana", Self, Other);

			Result = false;
		}

		offering_chance_occurred = true;
	}
	// Disable the elevator if config option set.
	else if (in_game && is_interact && Config::config.disable_dungeon_lift && (ari_current_gm_room.contains("rm_mines") || ari_current_gm_room.contains("seal")) && obj_dungeon_elevator_focused)
	{
		PlayConversation(ELEVATOR_LOCKED_CONVERSATION_KEY, Self, Other);
		Result = false;
	}
	else if (in_game && is_interact && ari_current_gm_room == "rm_farm" && obj_dragonshrine_focused && T2Read(script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1], "caldarus_is_human").ToBoolean())
	{
		PlayConversation("Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods", Self, Other);
		Result = false;
	}

	return Result;
}
