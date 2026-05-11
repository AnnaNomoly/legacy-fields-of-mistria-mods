#pragma once

#include "Core.hpp"
#include "Dungeon.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Monster
{
	/// Source: __monster_id.json.__monster_id__
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

		/// Returns true if any callback has cancelled this spawn.
		bool IsCancelled() const { return m_cancelled; }
	};

	namespace Internal
	{
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
				MMAPI::Monster::SpawnMonsterContext context{ Arguments[2]->ToInt64() };
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

		inline Aurie::AurieStatus RegisterMonsterSpawnHook(BeforeMonsterSpawnCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_SPAWN_MONSTER,
				reinterpret_cast<PVOID>(GmlScriptSpawnMonsterCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_monster_spawn_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}

	/// Activates Monster utility functions. Cascades to MMAPI::Dungeon::Enable so SpawnMonster can resolve
	/// the live DungeonRunner via TryGetDungeonRunnerContext.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Dungeon::Enable();
	}

	namespace Hooks
	{
		/// Registers a callback that runs before the game spawns a monster.
		/// Use ctx.SetMonster() to change which monster spawns, or ctx.Cancel() to prevent the spawn entirely.
		/// @param callback A function called with a mutable spawn context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeMonsterSpawn(Internal::BeforeMonsterSpawnCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_monster_spawn_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Monster::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterMonsterSpawnHook(callback);
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
