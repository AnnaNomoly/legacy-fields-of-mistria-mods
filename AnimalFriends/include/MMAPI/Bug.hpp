#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Bug
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_SPAWN_BUG  = "gml_Script_spawn_bug";
		inline constexpr const char* GML_SCRIPT_CREATE_BUG = "gml_Script_setup@gml_Object_obj_bug_Create_0";
	}

	/// Spawns a bug at the given room coordinates.
	/// @param Self The GML instance invoking the spawn (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param x_coord The X coordinate at which to spawn the bug.
	/// @param y_coord The Y coordinate at which to spawn the bug.
	/// @return The spawned bug as an RValue.
	inline YYTK::RValue Spawn(YYTK::CInstance* Self, YYTK::CInstance* Other, int x_coord, int y_coord)
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SPAWN_BUG, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		YYTK::RValue x = x_coord;
		YYTK::RValue y = y_coord;
		YYTK::RValue undefined;
		YYTK::RValue* args[3] = { &x, &y, &undefined };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 3, args);
		return result;
	}
}

namespace MMAPI::Bug::Experimental
{
	namespace Internal
	{
		inline bool override_next_bug_item_id = false;
		inline int next_bug_item_id = -1;

		inline void ClearPendingSpawn()
		{
			override_next_bug_item_id = false;
			next_bug_item_id = -1;
		}
	}

	/// Spawns a bug and overrides the next bug setup item ID.
	/// @attention Requires the mod to call HandleCreateBugSetup from a hook on MMAPI::Bug::Internal::GML_SCRIPT_CREATE_BUG.
	/// @param Self The GML instance invoking the spawn (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param x_coord The X coordinate at which to spawn the bug.
	/// @param y_coord The Y coordinate at which to spawn the bug.
	/// @param bug_item_id The item ID to apply to the bug during its setup call.
	/// @return The spawned bug as an RValue.
	inline YYTK::RValue SpawnCustom(YYTK::CInstance* Self, YYTK::CInstance* Other, int x_coord, int y_coord, int bug_item_id)
	{
		Internal::override_next_bug_item_id = true;
		Internal::next_bug_item_id = bug_item_id;

		YYTK::RValue result = MMAPI::Bug::Spawn(Self, Other, x_coord, y_coord);
		if (result.m_Kind == YYTK::VALUE_UNDEFINED)
			Internal::ClearPendingSpawn();

		return result;
	}

	/// Applies the pending custom bug item ID to a create-bug setup hook argument list.
	/// @param Arguments The Arguments array from a hook callback on MMAPI::Bug::Internal::GML_SCRIPT_CREATE_BUG.
	inline void HandleCreateBugSetup(YYTK::RValue** Arguments)
	{
		if (!Internal::override_next_bug_item_id)
			return;

		Arguments[0]->m_i64 = Internal::next_bug_item_id;
		Internal::ClearPendingSpawn();
	}

	/// Clears any pending custom bug item ID override.
	inline void ClearPendingSpawn()
	{
		Internal::ClearPendingSpawn();
	}
}
