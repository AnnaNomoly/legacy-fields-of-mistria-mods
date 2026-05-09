#pragma once

#include "Core.hpp"
#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Monster
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_SPAWN_MONSTER = "gml_Script_spawn_monster";
	}

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

	/// Spawns a monster at the given room coordinates on the current dungeon floor.
	/// @param Self The GML instance invoking the spawn (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param room_x The X position in room coordinates to spawn the monster at.
	/// @param room_y The Y position in room coordinates to spawn the monster at.
	/// @param monster The monster type to spawn.
	inline void SpawnMonster(YYTK::CInstance* Self, YYTK::CInstance* Other, int room_x, int room_y, MMAPI::Monster::Ids monster)
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SPAWN_MONSTER, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue x = room_x;
		YYTK::RValue y = room_y;
		YYTK::RValue monster_id = static_cast<int>(monster);
		YYTK::RValue result;
		YYTK::RValue* arguments[3] = { &x, &y, &monster_id };

		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 3, arguments);
	}
}
