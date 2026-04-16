#include "../../utils/Utils.h"

RValue& GmlScriptSceneAudioPlayerPlayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY))
		script_name_to_reference_map[GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY] = { Self, Other };

	if (game_is_active && Config::config.randomize_dungeon_music && AriCurrentGmRoomIsDungeonFloor() && floor_number != 91)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> game_music_distribution(0, MUSIC_INTERNAL_NAMES.size() - 1);
		*Arguments[0] = RValue(MUSIC_INTERNAL_NAMES.at(game_music_distribution(random_generator)));
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}
