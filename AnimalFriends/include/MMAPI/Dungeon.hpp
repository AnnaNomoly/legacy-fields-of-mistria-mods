#pragma once

#include "Core.hpp"
#include "StatusEffect.hpp"
#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Dungeon
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_SPAWN_LADDER  = "gml_Script_spawn_ladder@DungeonRunner@DungeonRunner";
		inline constexpr const char* GML_SCRIPT_ENTER_DUNGEON = "gml_Script_enter_dungeon";
	}

	/// Spawns a dungeon ladder at the given room coordinates.
	/// Coordinates are converted from room pixels to the internal ladder grid before being passed to the game.
	/// @param Self The GML instance invoking the spawn (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param x_coord The X position in room pixel coordinates.
	/// @param y_coord The Y position in room pixel coordinates.
	inline void SpawnLadder(YYTK::CInstance* Self, YYTK::CInstance* Other, int64_t x_coord, int64_t y_coord)
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SPAWN_LADDER, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue x = (x_coord * 2) / 16;
		YYTK::RValue y = (y_coord * 2) / 16;
		YYTK::RValue retval;
		YYTK::RValue* arguments[2] = { &x, &y };

		gml_script->m_Functions->m_ScriptFunction(Self, Other, retval, 2, arguments);
	}

	/// Transitions the player into the dungeon at the specified floor level.
	/// @attention Requires MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE to be registered via RegisterScriptContext.
	/// @param dungeon_level The dungeon floor level to enter.
	inline void EnterDungeon(double dungeon_level)
	{
		const auto& refs = MMAPI::Internal::script_reference_map;
		if (!refs.contains(MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_ENTER_DUNGEON, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::CInstance* Self  = refs.at(MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::StatusEffect::Internal::GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE)[1];

		YYTK::RValue level = dungeon_level;
		YYTK::RValue undefined;
		YYTK::RValue result;
		YYTK::RValue* arguments[3] = { &level, &undefined, &undefined };

		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 3, arguments);
	}

	/// Returns true if the current room is a dungeon room.
	inline bool IsDungeonRoom()
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer("gml_Script_is_dungeon_room", reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(nullptr, nullptr, result, 0, nullptr);
		return result.ToBoolean();
	}
}
