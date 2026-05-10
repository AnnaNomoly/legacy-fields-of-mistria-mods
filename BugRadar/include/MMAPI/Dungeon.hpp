#pragma once

#include "Core.hpp"
#include "Instance.hpp"

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

	/// Context passed to AfterDungeonRoomStart callbacks. Exposes the DungeonRunner instance
	/// so callbacks can invoke DungeonRunner-bound scripts (SpawnLadder, SpawnMonster, etc.) without
	/// borrowing a captured Self/Other from elsewhere.
	struct DungeonRoomStartContext
	{
		YYTK::CInstance* m_dungeon_runner = nullptr;
		YYTK::CInstance* m_other          = nullptr;

		/// The live DungeonRunner instance (the script's Self at hook time).
		YYTK::CInstance* GetDungeonRunner() const { return m_dungeon_runner; }

		/// Alias for `GetDungeonRunner()`; the script's Self at hook time.
		YYTK::CInstance* GetSelf() const { return m_dungeon_runner; }

		/// The script's Other at hook time.
		YYTK::CInstance* GetOther() const { return m_other; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_SPAWN_LADDER          = "gml_Script_spawn_ladder@DungeonRunner@DungeonRunner";
		inline constexpr const char* GML_SCRIPT_ENTER_DUNGEON         = "gml_Script_enter_dungeon";
		inline constexpr const char* GML_SCRIPT_ON_DUNGEON_ROOM_START = "gml_Script_on_room_start@DungeonRunner@DungeonRunner";

		using BeforeSpawnLadderCallback     = void(*)(MMAPI::Dungeon::SpawnLadderContext&);
		using AfterDungeonRoomStartCallback = void(*)(MMAPI::Dungeon::DungeonRoomStartContext&);

		inline BeforeSpawnLadderCallback     before_spawn_ladder_callback      = nullptr;
		inline AfterDungeonRoomStartCallback after_dungeon_room_start_callback = nullptr;

		// Live DungeonRunner Self/Other, latched from the on_room_start hook. Used by
		// TryGetDungeonRunnerContext for callers outside any hook frame.
		inline YYTK::CInstance* dungeon_runner_self  = nullptr;
		inline YYTK::CInstance* dungeon_runner_other = nullptr;

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by Dungeon::Enable().
		inline void ClearDungeonRunnerOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			dungeon_runner_self  = nullptr;
			dungeon_runner_other = nullptr;
		}

		inline YYTK::RValue& GmlScriptSpawnLadderCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_spawn_ladder_callback)
			{
				MMAPI::Dungeon::SpawnLadderContext context;
				before_spawn_ladder_callback(context);

				if (context.m_cancelled)
					return Result;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_SPAWN_LADDER)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

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

			// Latch the live DungeonRunner so TryGetDungeonRunnerContext works from any code path.
			dungeon_runner_self  = Self;
			dungeon_runner_other = Other;

			if (after_dungeon_room_start_callback)
			{
				MMAPI::Dungeon::DungeonRoomStartContext context{ Self, Other };
				after_dungeon_room_start_callback(context);
			}

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

		/// Resolves the DungeonRunner's GML calling context, latched from the most recent on_room_start hook.
		/// Cleared automatically when the game returns to the title menu.
		/// @return True if a DungeonRunner has been observed this session, false otherwise.
		inline bool TryGetDungeonRunnerContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			if (!dungeon_runner_self)
				return false;
			Self  = dungeon_runner_self;
			Other = dungeon_runner_other;
			return true;
		}
	}

	/// Activates Dungeon utility functions that directly call game scripts.
	/// Installs the on_room_start hook so the live DungeonRunner is latched for TryGetDungeonRunnerContext
	/// (the captured pointer is cleared on return-to-title via the setup_main_screen pub/sub).
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		Aurie::AurieStatus status = MMAPI::Instance::Enable();
		if (!Aurie::AurieSuccess(status))
			return status;

		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearDungeonRunnerOnReturnToTitle);

		return MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_ON_DUNGEON_ROOM_START,    reinterpret_cast<PVOID>(Internal::GmlScriptAfterDungeonRoomStartCallback) },
		});
	}

	/// Spawns a dungeon ladder at the given room coordinates.
	/// Coordinates are converted from room pixels to the internal ladder grid before being passed to the game.
	/// @param Self The GML instance invoking the spawn (passed through to the script call). Typically the live DungeonRunner.
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

	/// Transitions the player into the dungeon at the specified floor level.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param dungeon_level The dungeon floor level to enter.
	inline void EnterDungeon(double dungeon_level)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_ENTER_DUNGEON, reinterpret_cast<PVOID*>(&gml_script));

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

		/// Registers a callback that runs after the game initializes a new dungeon room.
		/// Use `ctx.GetDungeonRunner()` to invoke DungeonRunner-bound scripts (SpawnLadder, SpawnMonster, etc.) without
		/// having to fetch the DungeonRunner Self/Other from elsewhere.
		/// @param callback A function called with a DungeonRoomStartContext after the game's dungeon room start script runs.
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
