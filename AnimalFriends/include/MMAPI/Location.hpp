#pragma once

#include "Core.hpp"
#include "Instance.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Location
{
	/// Source: locations.json.__location_id__
	enum class Ids : int
	{
		AbandonedMines          = 0,
		AbandonedPit            = 1,
		AdelinesBedroom         = 2,
		AdelinesOffice          = 3,
		Aldaria                 = 4,
		BalorsRoom              = 5,
		Bathhouse               = 6,
		BathhouseBath           = 7,
		BathhouseBedroom        = 8,
		BathhouseChangeRoom     = 9,
		Beach                   = 10,
		BeachSecret             = 11,
		BlacksmithRoomLeft      = 12,
		BlacksmithRoomRight     = 13,
		BlacksmithStore         = 14,
		CaldarusHouse           = 15,
		CelinesRoom             = 16,
		ClinicB1                = 17,
		ClinicF1                = 18,
		ClinicF2                = 19,
		DeepWoods               = 20,
		DellsBedroom            = 21,
		DragonswornGlade        = 22,
		Dungeon                 = 23,
		EarthSeal               = 24,
		EasternRoad             = 25,
		EilandsBedroom          = 26,
		EilandsOffice           = 27,
		ElsiesBedroom           = 28,
		ErrolsBedroom           = 29,
		Farm                    = 30,
		FireSeal                = 31,
		GeneralStoreHome        = 32,
		GeneralStoreStore       = 33,
		HaydensBedroom          = 34,
		HaydensFarm             = 35,
		HaydensHouse            = 36,
		HoltAndNorasBedroom     = 37,
		Inn                     = 38,
		JoAndHemlocksRoom       = 39,
		LandensHouseF1          = 40,
		LandensHouseF2          = 41,
		LargeBarn               = 42,
		LargeCoop               = 43,
		LargeGreenhouse         = 44,
		LucsRoom                = 45,
		ManorHouseDiningRoom    = 46,
		ManorHouseEntry         = 47,
		MaplesRoom              = 48,
		MediumBarn              = 49,
		MediumCoop              = 50,
		Mill                    = 51,
		MinesEntry              = 52,
		MuseumEntry             = 53,
		Narrows                 = 54,
		NarrowsSecret           = 55,
		PlayerHome              = 56,
		PlayerHomeEast          = 57,
		PlayerHomeNorth         = 58,
		PlayerHomeUpperCentral  = 59,
		PlayerHomeUpperEast     = 60,
		PlayerHomeUpperWest     = 61,
		PlayerHomeWest          = 62,
		PriestessQuarters       = 63,
		ReinasRoom              = 64,
		RuinsSeal               = 65,
		SeridiasChamber         = 66,
		SeridiasHouse           = 67,
		SeridiasHouseBack       = 68,
		SmallBarn               = 69,
		SmallCoop               = 70,
		SmallGreenhouse         = 71,
		Summit                  = 72,
		TerithiasHouse          = 73,
		Town                    = 74,
		VoidSeal                = 75,
		WaterSeal               = 76,
		WesternRuins            = 77
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_TELEPORT_ARI_TO_ROOM = "gml_Script_ari_teleport_to_room";
	}

	/// Gets a location's internal name.
	/// @param location The location to resolve.
	/// @return The location internal name as an RValue.
	inline YYTK::RValue GetInternalName(MMAPI::Location::Ids location)
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer("gml_Script_try_location_id_to_string", reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		YYTK::RValue location_id = static_cast<int>(location);
		YYTK::RValue* args[1] = { &location_id };
		gml_script->m_Functions->m_ScriptFunction(nullptr, nullptr, result, 1, args);
		return result;
	}

	/// Teleports Ari to the given location at the specified coordinates.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param location The target location.
	/// @param x The X coordinate within the target location to place Ari.
	/// @param y The Y coordinate within the target location to place Ari.
	inline void TeleportAri(MMAPI::Location::Ids location, int x, int y)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_TELEPORT_ARI_TO_ROOM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue location_id = static_cast<int>(location);
		YYTK::RValue rx = x;
		YYTK::RValue ry = y;
		YYTK::RValue result;
		YYTK::RValue* args[3] = { &location_id, &rx, &ry };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 3, args);
	}
}
