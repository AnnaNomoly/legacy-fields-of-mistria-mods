#include "../../utils/Utils.h"

RValue& GmlScriptFindNpcBlipNoiseCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_FIND_NPC_BLIP_NOISE));
	original(Self, Other, Result, ArgumentCount, Arguments);

	if (AriCurrentGmRoomIsDungeonFloor())
	{
		std::string audio_asset_name = Result.ToString();
		if (audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipSeridiaHuman" || audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipPriestess")
			Result = "SoundEffects/NPCs/Vocal/TextBlipHeadPriestess";
	}

	return Result;
}
