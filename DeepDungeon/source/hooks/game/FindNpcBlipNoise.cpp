#include "../../utils/Utils.h"

// MMAPI NPC::Hooks::AfterFindBlipNoise callback.
// Registered in Init.cpp via MMAPI::NPC::Hooks::AfterFindBlipNoise(AfterFindBlipNoise).
void AfterFindBlipNoise(MMAPI::NPC::FindBlipNoiseContext& ctx)
{
	if (!AriCurrentGmRoomIsDungeonFloor())
		return;

	std::string_view audio_asset_name = ctx.GetAudioAssetName();
	if (audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipSeridiaHuman"
		|| audio_asset_name == "SoundEffects/NPCs/Vocal/TextBlipPriestess")
	{
		ctx.SetAudioAssetName("SoundEffects/NPCs/Vocal/TextBlipHeadPriestess");
	}
}
