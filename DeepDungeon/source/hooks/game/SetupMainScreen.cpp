#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::UI;

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (load_on_start)
	{
		load_on_start = false;
		localize_mod_text = true;
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		window_width = GetWindowWidth();
		window_height = GetWindowHeight();

		Config::Load();
		CreateOrGetGlobalYYTKVariable();
		CreateModInfoInGlobalYYTKVariable();

		LoadPerks();
		LoadSpells();
		LoadSpellIds();
		LoadStatusEffects();
		LoadLocations();
		LoadInfusions();
		LoadObjectIds();
		LoadItems();
		LoadMonsters();
		ModifyMonsterPrototypes();
		LoadDungeonBiomeCandidateMonsters();
		LoadPlayerStates();
		LoadMonsterStates();
		LoadBarkData();
		LoadTutorials();
		LoadStalagmiteAttackData();
		ModifyItems();
	}
	else
		ResetStaticFields(true);

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
