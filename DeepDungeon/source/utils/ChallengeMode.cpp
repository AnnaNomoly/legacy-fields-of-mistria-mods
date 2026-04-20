#include "../utils/Utils.h"

using namespace State::Player;

static fs::path GetChallengeModeFilePath()
{
	std::string filename = save_prefix + ".bin";
	return fs::current_path() / "mod_data" / "DeepDungeon" / filename;
}

void WriteChallengeModeFile()
{
	try
	{
		fs::create_directories(GetChallengeModeFilePath().parent_path());
		cista::write<cista::mode::NONE>(GetChallengeModeFilePath(), challenge_mode_progress);
	}
	catch (const std::exception& e)
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error writing challenge mode file: %s", MOD_NAME, MOD_VERSION, e.what());
	}
}

bool ReadChallengeModeFile()
{
	if (!fs::exists(GetChallengeModeFilePath()))
		return false;

	try
	{
		auto wrapped = cista::read<ChallengeModeProgress, cista::mode::NONE>(GetChallengeModeFilePath());
		challenge_mode_progress = *wrapped;
		return true;
	}
	catch (const std::exception& e)
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error reading challenge mode file: %s", MOD_NAME, MOD_VERSION, e.what());
		return false;
	}
}
