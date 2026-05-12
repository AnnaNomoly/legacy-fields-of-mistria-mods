#pragma once

#include "Core.hpp"
#include "Game.hpp"
#include "Hook.hpp"
#include "Instance.hpp"
#include "Location.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Dungeon
{
	struct SpawnLadderContext
	{
		bool m_cancelled = false;

		void Cancel() { m_cancelled = true; }
	};

	struct DungeonRoomStartContext;
	inline int GetFloorNumber();

	/// Context passed to AfterDungeonRoomStart callbacks. Exposes the DungeonRunner instance
	/// so callbacks can invoke DungeonRunner-bound scripts (SpawnLadder, SpawnMonster, etc.) without
	/// borrowing a captured Self/Other from elsewhere.
	///
	/// Each dungeon floor is a distinct GameMaker room, so this context doubles as MMAPI's
	/// "floor started" signal — call `ctx.GetFloorNumber()` to read the freshly-updated floor.
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

		/// The current dungeon floor number, 1-indexed. See MMAPI::Dungeon::GetFloorNumber.
		int GetFloorNumber() const { return MMAPI::Dungeon::GetFloorNumber(); }
	};

	namespace Internal
	{
		inline bool enabled = false;

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

		// Stateful floor number tracking. Updated on each room transition by OnGoToRoomUpdateFloor,
		// which subscribes to Location's go_to_room pub/sub so the value is set BEFORE the new room's
		// on_room_start@DungeonRunner fires (DD does this for the same reason). Reset on return-to-title.
		// 1-indexed (1 = first floor); 0 means the player is not in a dungeon-related room.
		inline int current_floor_number = 0;

		// Mirrors DeepDungeon's SetFloorNumber room-name table. Side rooms (treasure/milestone/
		// ritual chambers) preserve the previous floor; explicit elevator/seal rooms snap to a
		// known floor; everything else increments by one (the "next floor" case).
		inline void UpdateFloorNumberForRoom(const std::string& room)
		{
			if (room.find("treasure")  != std::string::npos ||
			    room.find("milestone") != std::string::npos ||
			    room.find("ritual")    != std::string::npos)
				return;

			if      (room == "rm_mines_upper_floor1")     current_floor_number = 1;
			else if (room == "rm_mines_upper_elevator5")  current_floor_number = 5;
			else if (room == "rm_mines_upper_elevator10") current_floor_number = 10;
			else if (room == "rm_mines_upper_elevator15") current_floor_number = 15;
			else if (room == "rm_water_seal")             current_floor_number = 20;
			else if (room == "rm_mines_tide_floor21")     current_floor_number = 21;
			else if (room == "rm_mines_tide_elevator25")  current_floor_number = 25;
			else if (room == "rm_mines_tide_elevator30")  current_floor_number = 30;
			else if (room == "rm_mines_tide_elevator35")  current_floor_number = 35;
			else if (room == "rm_earth_seal")             current_floor_number = 40;
			else if (room == "rm_mines_deep_41")          current_floor_number = 41;
			else if (room == "rm_mines_deep_45")          current_floor_number = 45;
			else if (room == "rm_mines_deep_50")          current_floor_number = 50;
			else if (room == "rm_mines_deep_55")          current_floor_number = 55;
			else if (room == "rm_fire_seal")              current_floor_number = 60;
			else if (room == "rm_mines_lava_61")          current_floor_number = 61;
			else if (room == "rm_mines_lava_65")          current_floor_number = 65;
			else if (room == "rm_mines_lava_70")          current_floor_number = 70;
			else if (room == "rm_mines_lava_75")          current_floor_number = 75;
			else if (room == "rm_ruins_seal" || room == "rm_void_seal") current_floor_number = 80;
			else if (room == "rm_mines_ruins_85")         current_floor_number = 85;
			else if (room == "rm_priestess_quarters")     current_floor_number = 90;
			else if (room == "rm_mines_ruins_95")         current_floor_number = 95;
			else if (room == "rm_seridias_chamber")       current_floor_number = 100;
			else                                          current_floor_number++;
		}

		// Subscribed to Location's go_to_room pub/sub. Mirrors DD's logic: dungeon-related
		// rooms (mines, seal, priestess quarters, seridias chamber, minus the mines entry)
		// route to UpdateFloorNumberForRoom; everything else resets the floor to 0.
		inline void OnGoToRoomUpdateFloor(const std::string& room)
		{
			bool is_dungeon_related =
				(room.find("rm_mines") != std::string::npos
				 || room.find("seal") != std::string::npos
				 || room == "rm_priestess_quarters"
				 || room == "rm_seridias_chamber")
				&& room != "rm_mines_entry";

			if (is_dungeon_related)
				UpdateFloorNumberForRoom(room);
			else
				current_floor_number = 0;
		}

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by Dungeon::Enable().
		inline void ClearDungeonRunnerOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			dungeon_runner_self  = nullptr;
			dungeon_runner_other = nullptr;
			current_floor_number = 0;
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
	/// Cascades to MMAPI::Location::Enable and subscribes to its go_to_room pub/sub so the floor counter
	/// stays current — DD relies on floor_number being set before on_room_start@DungeonRunner runs.
	/// @return Status::Success if the hooks are installed (or already were); otherwise a failure status.
	inline MMAPI::Status Enable()
	{
		if (Internal::enabled)
			return MMAPI::Status::Success;

		MMAPI::Log::Debug("MMAPI::Dungeon::Enable() called");

		MMAPI::Status status = MMAPI::Instance::Enable();
		if (!MMAPI::IsSuccess(status))
			return status;

		status = MMAPI::Location::Enable();
		if (!MMAPI::IsSuccess(status))
			return status;

		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearDungeonRunnerOnReturnToTitle);
		MMAPI::Location::Internal::RegisterOnGoToRoomHandler(Internal::OnGoToRoomUpdateFloor);

		status = MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_ON_DUNGEON_ROOM_START,    reinterpret_cast<PVOID>(Internal::GmlScriptAfterDungeonRoomStartCallback) },
			{ Internal::GML_SCRIPT_SPAWN_LADDER,             reinterpret_cast<PVOID>(Internal::GmlScriptSpawnLadderCallback) },
		});
		if (!MMAPI::IsSuccess(status))
			return status;

		Internal::enabled = true;
		return MMAPI::Status::Success;
	}

	/// Spawns a dungeon ladder at the given room coordinates.
	/// Coordinates are converted from room pixels to the internal ladder grid before being passed to the game.
	/// @attention Requires MMAPI::Dungeon::Enable() to have been called.
	/// @param x_coord The X position in the dungeon room.
	/// @param y_coord The Y position in the dungeon room.
	/// @return True if the script was invoked, false if the required context is unavailable.
	inline bool SpawnLadder(int64_t x_coord, int64_t y_coord)
	{
		MMAPI_REQUIRE_ENABLED("Dungeon", false);

		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetDungeonRunnerContext(Self, Other))
			return false;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SPAWN_LADDER, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue x = (x_coord * 2) / 16;
		YYTK::RValue y = (y_coord * 2) / 16;
		YYTK::RValue retval;
		YYTK::RValue* arguments[2] = { &x, &y };

		gml_script->m_Functions->m_ScriptFunction(Self, Other, retval, 2, arguments);
		return true;
	}

	/// Transitions the player into the dungeon at the specified floor level.
	/// @attention Requires MMAPI::Dungeon::Enable() to have been called.
	/// @param dungeon_level The dungeon floor level to enter.
	inline void EnterDungeon(double dungeon_level)
	{
		MMAPI_REQUIRE_ENABLED_VOID("Dungeon");

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

	/// Returns true if the current room is an active dungeon floor (excludes the dungeon entry and seal rooms).
	/// Stricter than IsDungeonRoom: matches the rooms where dungeon-run gameplay actually happens.
	inline bool IsCurrentRoomDungeonFloor()
	{
		std::string room = MMAPI::Game::GetCurrentRoomName();
		return room.find("rm_mines") != std::string::npos
		    && room != "rm_mines_entry"
		    && room.find("seal") == std::string::npos;
	}

	/// Returns the player's current dungeon floor number, 1-indexed (1 = first floor).
	/// Returns 0 when the player is not in a dungeon-related room (overworld, indoor buildings, etc.).
	/// Includes seal rooms — `rm_water_seal` returns 20, `rm_earth_seal` returns 40, etc.
	/// Tracked via the goto_gm_room hook (set before on_room_start runs); reset on return-to-title.
	///
	/// Note that MMAPI::Dungeon::EnterDungeon takes a 0-indexed floor argument,
	/// so callers passing GetFloorNumber() to EnterDungeon should subtract 1.
	inline int GetFloorNumber()
	{
		return Internal::current_floor_number;
	}

	namespace Hooks
	{
		/// Registers a callback that runs before the game spawns a dungeon ladder.
		/// Call ctx.Cancel() to prevent the ladder from spawning.
		/// @param callback A function called with a mutable spawn ladder context before the game processes it.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status BeforeSpawnLadder(Internal::BeforeSpawnLadderCallback callback)
		{
			MMAPI::Status status = MMAPI::Dungeon::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Dungeon::BeforeSpawnLadder",
				Internal::before_spawn_ladder_callback,
				callback
			);
		}

		/// Registers a callback that runs after the game initializes a new dungeon room.
		/// Each dungeon floor is a distinct GameMaker room, so this also fires once per floor — call `ctx.GetFloorNumber()`
		/// for the freshly-updated floor count, or branch on the room name if you need finer detail.
		/// Use `ctx.GetDungeonRunner()` to invoke DungeonRunner-bound scripts (SpawnLadder, SpawnMonster, etc.) without
		/// having to fetch the DungeonRunner Self/Other from elsewhere.
		/// @param callback A function called with a DungeonRoomStartContext after the game's dungeon room start script runs.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status AfterDungeonRoomStart(Internal::AfterDungeonRoomStartCallback callback)
		{
			MMAPI::Status status = MMAPI::Dungeon::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Dungeon::AfterDungeonRoomStart",
				Internal::after_dungeon_room_start_callback,
				callback
			);
		}
	}
}
