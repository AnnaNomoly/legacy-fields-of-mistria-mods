#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Game.hpp"
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
		inline constexpr const char* GML_SCRIPT_TRY_LOCATION_ID_TO_STRING = "gml_Script_try_location_id_to_string";
		inline constexpr const char* GML_SCRIPT_TELEPORT_ARI_TO_ROOM      = "gml_Script_ari_teleport_to_room";

		inline int cached_current_location_id = -1;

		inline YYTK::RValue GetLocationData(int location_id)
		{
			if (!MMAPI::Internal::global_instance)
				return {};

			YYTK::RValue locations = MMAPI::Internal::global_instance->GetMember("__locations");
			size_t location_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(locations, location_count);

			if (location_id < 0 || location_id >= static_cast<int>(location_count))
				return {};

			YYTK::RValue* location = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(locations, location_id, location);
			if (!location)
				return {};

			return *location;
		}

		inline YYTK::RValue& GmlScriptTryLocationIdToStringCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_TRY_LOCATION_ID_TO_STRING)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			// The game calls try_location_id_to_string for the location it is currently displaying or saving.
			// Capture the most-recent valid id; serve TryGetCurrentLocation from this cache to avoid the fatal
			// errors that polling location_id_to_gm_room hits for procedural locations (Dungeon, etc.).
			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				int id = static_cast<int>(Arguments[0]->ToInt64());
				if (id >= static_cast<int>(MMAPI::Location::Ids::AbandonedMines) &&
				    id <= static_cast<int>(MMAPI::Location::Ids::WesternRuins))
					cached_current_location_id = id;
			}

			return Result;
		}
	}

	/// Activates Location utility functions that track the current location.
	/// Installs an internal hook on `try_location_id_to_string` that observes the game's location queries
	/// and caches the most-recently-seen id, which `TryGetCurrentLocation` reads from.
	/// @return AURIE_SUCCESS if the hook is installed (or already was); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,
			reinterpret_cast<PVOID>(Internal::GmlScriptTryLocationIdToStringCallback)
		);
	}

	/// Gets a location's internal name.
	/// @param location The location to resolve.
	/// @return The location internal name as an RValue.
	inline YYTK::RValue GetInternalName(MMAPI::Location::Ids location)
	{
		if (!MMAPI::Internal::global_instance)
			return {};

		YYTK::RValue location_ids = MMAPI::Internal::global_instance->GetMember("__location_id__");
		size_t location_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(location_ids, location_count);

		int location_id = static_cast<int>(location);
		if (location_id < 0 || location_id >= static_cast<int>(location_count))
			return {};

		YYTK::RValue* internal_name = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(location_ids, location_id, internal_name);
		if (!internal_name)
			return {};

		return *internal_name;
	}

	/// Gets the current location, observed from the game's `try_location_id_to_string` queries.
	/// @attention Requires MMAPI::Location::Enable() to have been called.
	/// @param location The current location.
	/// @return True if a location has been observed, false if the cache is cold (the game hasn't queried a location yet).
	inline bool TryGetCurrentLocation(MMAPI::Location::Ids& location)
	{
		if (Internal::cached_current_location_id < 0)
			return false;

		location = static_cast<MMAPI::Location::Ids>(Internal::cached_current_location_id);
		return true;
	}

	/// Returns true if the current location matches location.
	/// @param location The location to compare against.
	inline bool IsCurrentLocation(MMAPI::Location::Ids location)
	{
		MMAPI::Location::Ids current_location;
		if (!TryGetCurrentLocation(current_location))
			return false;

		return current_location == location;
	}

	/// Returns true if the location is marked as outdoors in globalInstance.__locations.
	/// @param location The location to check.
	inline bool IsOutdoors(MMAPI::Location::Ids location)
	{
		YYTK::RValue location_data = Internal::GetLocationData(static_cast<int>(location));
		if (location_data.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		return location_data.GetMember("outdoor").ToBoolean();
	}

	/// Returns true if the location is marked as indoors in globalInstance.__locations.
	/// @param location The location to check.
	inline bool IsIndoors(MMAPI::Location::Ids location)
	{
		YYTK::RValue location_data = Internal::GetLocationData(static_cast<int>(location));
		if (location_data.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		return !location_data.GetMember("outdoor").ToBoolean();
	}

	/// Returns true if the current location is marked as outdoors in globalInstance.__locations.
	inline bool IsCurrentLocationOutdoors()
	{
		MMAPI::Location::Ids current_location;
		if (!TryGetCurrentLocation(current_location))
			return false;

		return IsOutdoors(current_location);
	}

	/// Returns true if the current location is marked as indoors in globalInstance.__locations.
	inline bool IsCurrentLocationIndoors()
	{
		MMAPI::Location::Ids current_location;
		if (!TryGetCurrentLocation(current_location))
			return false;

		return IsIndoors(current_location);
	}

	/// Teleports Ari to the given location at the specified coordinates.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param location The target location.
	/// @param x The X coordinate within the target location to place Ari.
	/// @param y The Y coordinate within the target location to place Ari.
	inline void TeleportAri(MMAPI::Location::Ids location, int x, int y)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

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
