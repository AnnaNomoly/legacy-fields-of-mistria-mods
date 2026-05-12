#pragma once

#include "Core.hpp"
#include "Dungeon.hpp"
#include "Engine.hpp"
#include "Hook.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Monster
{
	/// Source: globalInstance.__monster_id__
	enum class Ids : int
	{
		Barrel            = 0,
		Bat               = 1,
		BatBlue           = 2,
		Cat               = 3,
		CatVoid           = 4,
		Copperclod        = 5,
		Enchantern        = 6,
		EnchanternBlue    = 7,
		Goldclod          = 8,
		GriffinStatue     = 9,
		Ironclod          = 10,
		Mimic             = 11,
		Mistrilclod       = 12,
		Mushroom          = 13,
		MushroomBlue      = 14,
		MushroomGreen     = 15,
		MushroomPurple    = 16,
		RockStack         = 17,
		Rockclod          = 18,
		RockclodBlue      = 19,
		RockclodGreen     = 20,
		RockclodPurple    = 21,
		RockclodRed       = 22,
		Sapling           = 23,
		SaplingBlue       = 24,
		SaplingCool       = 25,
		SaplingOrange     = 26,
		SaplingOrangeMini = 27,
		SaplingPink       = 28,
		SaplingPurple     = 29,
		Silverclod        = 30,
		Spirit            = 31,
		SpiritPurple      = 32,
		Stalagmite        = 33,
		StalagmiteGreen   = 34,
		StalagmitePurple  = 35,
		Tome              = 36
	};

	/// Total number of enumerators in Ids. Iterating [0, IdCount) covers every Ids value.
	inline constexpr int IdCount = 37;

	/// Invokes fn with every Ids value, in ascending order.
	template <typename Fn>
	inline void ForEachId(Fn fn)
	{
		for (int i = 0; i < IdCount; ++i)
			fn(static_cast<Ids>(i));
	}

	/// Source: globalInstance.__monster_category__
	/// Logical groupings used by the game's per-category FSM state arrays
	/// (e.g. Category::Mite corresponds to globalInstance.__mite_state__).
	enum class Categories : int
	{
		Shroom     = 0,
		Clod       = 1,
		Sap        = 2,
		Enchantern = 3,
		Mite       = 4,
		Bat        = 5,
		Mimic      = 6,
		Spirit     = 7,
		Cat        = 8,
		Barrel     = 9,
		RockStack  = 10,
		Statue     = 11,
		Tome       = 12
	};

	/// Per-category FSM state enums. Each enum's integer value matches the index of the corresponding
	/// state name in `globalInstance.__<category>_state__`, which is also what the game stores in
	/// `monster_instance.fsm.state.state_id`. Use MMAPI::Monster::IsInState(monster, StateEnum::X) to
	/// compare against a specific state.
	///
	/// Monster::Enable() verifies these enums against the live game arrays and logs a warning on any
	/// mismatch — if the game patches a state list, the warning surfaces the divergence so the enum
	/// can be re-dumped from globalInstance.
	namespace States
	{
		/// Source: globalInstance.__shroom_state__
		enum class Shroom : int
		{
			Idle, Acknowledgment, Walk, WindupSlide, Windup, Attack, Tired,
			Shell, Wiggle, WiggleExit, Dying, Explode
		};

		/// Source: globalInstance.__rockclod_state__
		enum class Clod : int
		{
			Idle, Acknowledgment, Walk, Windup, Attack, Tired, Hurt, Dying, Flying
		};

		/// Source: globalInstance.__sapling_state__
		enum class Sap : int
		{
			Idle, Acknowledgment, Walk, Windup, Attack, Tired, Hurt, Dying, Splitting
		};

		/// Source: globalInstance.__enchantern_state__
		enum class Enchantern : int
		{
			Idle, Acknowledgment, FlickerOn, Charge, Flee, GoHome, Hurt, Dying
		};

		/// Source: globalInstance.__mite_state__
		enum class Mite : int
		{
			Idle, Walk, Windup, Attack, Tired, Flee, Hurt, Dying
		};

		/// Source: globalInstance.__bat_state__
		enum class Bat : int
		{
			Idle, Acknowledgment, Walk, Windup, Attack, Hurt, Dying, Flee
		};

		/// Source: globalInstance.__mimic_state__
		enum class Mimic : int
		{
			Idle, Attack, Hurt, Gobble, Dying, Fade
		};

		/// Source: globalInstance.__spirit_state__
		enum class Spirit : int
		{
			Idle, Teleport, Windup, Attack, Tired, Hurt, Dying, Acknowledgment, Recovery
		};

		/// Source: globalInstance.__cat_state__
		enum class Cat : int
		{
			Idle, Acknowledgment, Walk, Windup, Attack, Tired, Petrified, Hurt, Dying
		};

		/// Source: globalInstance.__barrel_state__
		enum class Barrel : int
		{
			Idle, Priming, Swelling
		};

		/// Source: globalInstance.__rock_stack_state__
		enum class RockStack : int
		{
			Idle, Acknowledgment, Walk, Windup, Hurt, Launching, Catching, Dying, Hopping
		};

		/// Source: globalInstance.__statue_state__
		enum class Statue : int
		{
			Acknowledgment, Idle, Chase, Tumbling, Dying
		};

		/// Source: globalInstance.__tome_state__
		enum class Tome : int
		{
			Acknowledgment, Stunned, Idle, Windup, StunAttack, Flying, Hurt, Dying, GentleStun
		};
	}

	struct SpawnMonsterContext
	{
		int m_monster_id = 0;
		bool m_cancelled = false;

		/// Returns the monster type being spawned.
		MMAPI::Monster::Ids GetMonster() const { return static_cast<MMAPI::Monster::Ids>(m_monster_id); }

		/// Replaces the monster type to spawn.
		void SetMonster(MMAPI::Monster::Ids monster) { m_monster_id = static_cast<int>(monster); }

		/// Prevents the game's spawn_monster script from running.
		void Cancel() { m_cancelled = true; }
	};

	namespace Internal
	{
		inline bool enabled = false;

		inline constexpr const char* GML_SCRIPT_SPAWN_MONSTER = "gml_Script_spawn_monster";

		using BeforeMonsterSpawnCallback = void(*)(MMAPI::Monster::SpawnMonsterContext&);

		inline BeforeMonsterSpawnCallback before_monster_spawn_callback = nullptr;

		inline YYTK::RValue& GmlScriptSpawnMonsterCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_monster_spawn_callback && Arguments && ArgumentCount >= 3 && Arguments[2])
			{
				MMAPI::Monster::SpawnMonsterContext context{ static_cast<int>(Arguments[2]->ToInt64()) };
				before_monster_spawn_callback(context);

				if (context.m_cancelled)
					return Result;

				*Arguments[2] = context.m_monster_id;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_SPAWN_MONSTER)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}
	}

	/// Activates Monster utility functions. Cascades to MMAPI::Dungeon::Enable so SpawnMonster can resolve
	/// the live DungeonRunner via TryGetDungeonRunnerContext. Eagerly installs the spawn_monster script hook
	/// used by Hooks::BeforeMonsterSpawn.
	/// @return Status::Success if the hooks are installed (or already were); otherwise a failure status.
	inline MMAPI::Status Enable()
	{
		if (Internal::enabled)
			return MMAPI::Status::Success;

		MMAPI::Log::Debug("MMAPI::Monster::Enable() called");

		MMAPI::Status status = MMAPI::Dungeon::Enable();
		if (!MMAPI::IsSuccess(status))
			return status;

		status = MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_SPAWN_MONSTER,
			reinterpret_cast<PVOID>(Internal::GmlScriptSpawnMonsterCallback)
		);
		if (!MMAPI::IsSuccess(status))
			return status;

		Internal::enabled = true;
		return MMAPI::Status::Success;
	}

	/// Returns the FSM state id from the given monster instance, read from `monster.fsm.state.state_id`.
	/// @return The state id, or -1 if monster is null or doesn't expose the expected fsm/state members.
	inline int GetStateId(YYTK::CInstance* monster)
	{
		MMAPI_REQUIRE_ENABLED("Monster", -1);

		if (!monster)
			return -1;

		YYTK::RValue monster_rv = monster->ToRValue();
		if (!MMAPI::Engine::StructVariableExists(monster_rv, "fsm"))
			return -1;

		YYTK::RValue fsm = monster_rv.GetMember("fsm");
		if (!MMAPI::Engine::StructVariableExists(fsm, "state"))
			return -1;

		YYTK::RValue state = fsm.GetMember("state");
		if (!MMAPI::Engine::StructVariableExists(state, "state_id"))
			return -1;

		return static_cast<int>(state.GetMember("state_id").ToInt64());
	}

	/// Returns true if the monster's current FSM state id matches the given category state.
	/// The caller is responsible for pairing the monster with the right state enum
	/// (e.g. comparing a stalagmite against MMAPI::Monster::States::Mite::Attack).
	/// @tparam StateEnum One of the enums declared in MMAPI::Monster::States.
	template <typename StateEnum>
	inline bool IsInState(YYTK::CInstance* monster, StateEnum state)
	{
		return GetStateId(monster) == static_cast<int>(state);
	}

	namespace Hooks
	{
		/// Registers a callback that runs before the game spawns a monster.
		/// Use ctx.SetMonster() to change which monster spawns, or ctx.Cancel() to prevent the spawn entirely.
		/// @param callback A function called with a mutable spawn context before the game processes it.
		/// @return Status::Success if the hook was installed; Status::AlreadyRegistered if a callback is already registered; otherwise a failure status.
		inline MMAPI::Status BeforeMonsterSpawn(Internal::BeforeMonsterSpawnCallback callback)
		{
			MMAPI::Status status = MMAPI::Monster::Enable();
			if (!MMAPI::IsSuccess(status))
				return status;

			return MMAPI::Internal::RegisterHook(
				"Monster::BeforeMonsterSpawn",
				Internal::before_monster_spawn_callback,
				callback
			);
		}
	}

	/// Spawns a monster at the given room coordinates on the current dungeon floor.
	/// @attention Requires MMAPI::Monster::Enable() to have been called.
	/// @param room_x The X position in room coordinates to spawn the monster at.
	/// @param room_y The Y position in room coordinates to spawn the monster at.
	/// @param monster The monster type to spawn.
	/// @return True if the script was invoked, false if the required context is unavailable.
	inline bool SpawnMonster(int room_x, int room_y, MMAPI::Monster::Ids monster)
	{
		MMAPI_REQUIRE_ENABLED("Monster", false);

		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Dungeon::Internal::TryGetDungeonRunnerContext(Self, Other))
			return false;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SPAWN_MONSTER, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue x = room_x;
		YYTK::RValue y = room_y;
		YYTK::RValue monster_id = static_cast<int>(monster);
		YYTK::RValue result;
		YYTK::RValue* arguments[3] = { &x, &y, &monster_id };

		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 3, arguments);
		return true;
	}
}
