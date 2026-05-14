#include "Utils.h"

#include <MMAPI/MMAPI.hpp>

using namespace State::Floor;
using namespace State::UI;
using namespace State::Maps;

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, MOD_VERSION, e.what());
	}
}

bool IsNumeric(RValue value)
{
	return MMAPI::Engine::IsNumeric(value);
}

bool IsObject(RValue value)
{
	return MMAPI::Engine::IsObject(value);
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	return MMAPI::Engine::StructVariableExists(the_struct, variable_name);
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return MMAPI::Engine::StructVariableSet(the_struct, variable_name, value);
}

void StructVariableRemove(RValue the_struct, const char* variable_name)
{
	MMAPI::Engine::StructVariableRemove(the_struct, variable_name);
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return global_variable_exists.ToBoolean();
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists("__YYTK"))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet("__YYTK", __YYTK);
	}
	else
		__YYTK = GlobalVariableGet("__YYTK");
}

void CreateModInfoInGlobalYYTKVariable()
{
	if (!StructVariableExists(__YYTK, MOD_NAME))
	{
		RValue deep_dungeon;
		RValue version = MOD_VERSION;
		RValue floor = floor_number;
		g_ModuleInterface->GetRunnerInterface().StructCreate(&deep_dungeon);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "version", &version);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "floor", &floor);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&__YYTK, MOD_NAME, &deep_dungeon);
	}
}

double GetWindowWidth()
{
	return MMAPI::Engine::GetWindowWidth();
}

double GetWindowHeight()
{
	return MMAPI::Engine::GetWindowHeight();
}

void PlaySoundEffect(const char* sound_name, int /*priority*/, double gain)
{
	// DD historically hard-coded priority=100; preserve that pre-migration behavior.
	MMAPI::Engine::PlaySoundEffect(sound_name, 100, gain);
}

void SetMaxHealth(int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("base_health") = value;

	if (MMAPI::Player::GetHealth().ToInt64() > value)
		MMAPI::Player::SetHealth(value);
}

int ModifyMaxHealth(int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	int max_health = __ari.GetMember("base_health").ToInt64() + value;
	*__ari.GetRefMember("base_health") = max_health;

	return max_health;
}

void UnlockRecipe(int item_id)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue recipe_unlocks = *__ari.GetRefMember("recipe_unlocks");

	if (recipe_unlocks[item_id].m_Real == 0.0)
		recipe_unlocks[item_id] = 1.0;
}
