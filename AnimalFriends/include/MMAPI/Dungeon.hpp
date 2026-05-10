#pragma once

#include "Core.hpp"
#include "StatusEffect.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Dungeon
{
	struct SpawnLadderContext
	{
		bool m_cancelled = false;

		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	struct GoToRoomContext
	{
		std::string m_room_name;

		std::string_view GetRoomName() const { return m_room_name; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_SPAWN_LADDER          = "gml_Script_spawn_ladder@DungeonRunner@DungeonRunner";
		inline constexpr const char* GML_SCRIPT_ENTER_DUNGEON         = "gml_Script_enter_dungeon";
		inline constexpr const char* GML_SCRIPT_GO_TO_ROOM            = "gml_Script_goto_gm_room";
		inline constexpr const char* GML_SCRIPT_ON_DUNGEON_ROOM_START = "gml_Script_on_room_start@DungeonRunner@DungeonRunner";

		using BeforeSpawnLadderCallback      = void(*)(MMAPI::Dungeon::SpawnLadderContext&);
		using AfterGoToRoomCallback         = void(*)(MMAPI::Dungeon::GoToRoomContext&);
		using AfterDungeonRoomStartCallback = void(*)();

		inline BeforeSpawnLadderCallback      before_spawn_ladder_callback       = nullptr;
		inline AfterGoToRoomCallback         after_go_to_room_callback         = nullptr;
		inline AfterDungeonRoomStartCallback after_dungeon_room_start_callback = nullptr;

		inline YYTK::RValue& GmlScriptSpawnLadderCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			MMAPI::Dungeon::SpawnLadderContext context;
			before_spawn_ladder_callback(context);

			if (context.m_cancelled)
				return Result;

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_SPAWN_LADDER)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline YYTK::RValue& GmlScriptGoToRoomCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_GO_TO_ROOM)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			YYTK::RValue gm_room = Result.GetMember("gm_room");
			YYTK::RValue room_name_rv = MMAPI::Internal::module_interface->CallBuiltin("room_get_name", { gm_room });

			MMAPI::Dungeon::GoToRoomContext context;
			if (room_name_rv.m_Kind != YYTK::VALUE_UNDEFINED)
				context.m_room_name = room_name_rv.ToString();

			after_go_to_room_callback(context);

			return Result;
		}

		inline YYTK::RValue& GmlScriptAfterDungeonRoomStartCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_ON_DUNGEON_ROOM_START)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			after_dungeon_room_start_callback();

			return Result;
		}

		inline Aurie::AurieStatus RegisterSpawnLadderHook(BeforeSpawnLadderCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_SPAWN_LADDER,
				reinterpret_cast<PVOID>(GmlScriptSpawnLadderCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_spawn_ladder_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterGoToRoomHook(AfterGoToRoomCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_GO_TO_ROOM,
				reinterpret_cast<PVOID>(GmlScriptGoToRoomCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_go_to_room_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterDungeonRoomStartHook(AfterDungeonRoomStartCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_ON_DUNGEON_ROOM_START,
				reinterpret_cast<PVOID>(GmlScriptAfterDungeonRoomStartCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_dungeon_room_start_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Spawns a dungeon ladder at the given room coordinates.
	/// Coordinates are converted from room pixels to the internal ladder grid before being passed to the game.
	/// @param Self The GML instance invoking the spawn (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param x_coord The X position in the dungeon room.
	/// @param y_coord The Y position in the dungeon room.
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

	/// Activates Dungeon utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::StatusEffect::Enable();
	}

	/// Transitions the player into the dungeon at the specified floor level.
	/// @attention Requires MMAPI::Dungeon::Enable() to have been called.
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

	namespace Hooks
	{
		/// Registers a callback that runs before the game spawns a dungeon ladder.
		/// Call ctx.Cancel() to prevent the ladder from spawning.
		/// @param callback A function called with a mutable spawn ladder context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeSpawnLadder(Internal::BeforeSpawnLadderCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_spawn_ladder_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterSpawnLadderHook(callback);
		}

		/// Registers a callback that runs after the game transitions to a new room.
		/// Use ctx.GetRoomName() to read the name of the room that was entered.
		/// @param callback A function called with the room transition context after the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterGoToRoom(Internal::AfterGoToRoomCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_go_to_room_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterGoToRoomHook(callback);
		}

		/// Registers a callback that runs after the game initializes a new dungeon room.
		/// @param callback A function called after the game's dungeon room start script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterDungeonRoomStart(Internal::AfterDungeonRoomStartCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_dungeon_room_start_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterDungeonRoomStartHook(callback);
		}
	}
}
