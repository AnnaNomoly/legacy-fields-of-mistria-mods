#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::UI;

// MMAPI Game::Hooks::BeforeSetupMainScreen callback.
// Runs after MMAPI's internal latcher resets (instance_reference_map clear + module on_setup
// handlers) and before the original setup_main_screen GML script.
void BeforeSetupMainScreen()
{
	if (load_on_start)
	{
		load_on_start = false;
		localize_mod_text = true;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		// Mirror DD's lazily-obtained global into MMAPI in case it was null at ModuleInitialize.
		MMAPI::Internal::global_instance = global_instance;
		window_width = GetWindowWidth();
		window_height = GetWindowHeight();

		Config::Load();
		CreateOrGetGlobalYYTKVariable();
		CreateModInfoInGlobalYYTKVariable();

		LoadSpells();
		LoadObjectIds();
		LoadItems();
		ModifyMonsterPrototypes();
		LoadDungeonBiomeCandidateMonsters();
		LoadStalagmiteAttackData();
		ModifyItems();
	}
	else
		ResetStaticFields(true);
}
