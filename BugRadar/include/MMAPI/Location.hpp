#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"

#include <string>
#include <unordered_map>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Location
{
	/// Source: globalInstance.__location_id__
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

	/// Total number of enumerators in Ids. Iterating [0, IdCount) covers every Ids value.
	inline constexpr int IdCount = 78;

	/// Invokes fn with every Ids value, in ascending order.
	template <typename Fn>
	inline void ForEachId(Fn fn)
	{
		for (int i = 0; i < IdCount; ++i)
			fn(static_cast<Ids>(i));
	}

	struct GoToRoomContext
	{
		std::string m_room_name;

		std::string_view GetRoomName() const { return m_room_name; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_LOCATION_ID_TO_GM_ROOM = "gml_Script_location_id_to_gm_room";
		inline constexpr const char* GML_SCRIPT_GO_TO_ROOM             = "gml_Script_goto_gm_room";
		inline constexpr const char* GML_SCRIPT_TELEPORT_ARI_TO_ROOM   = "gml_Script_ari_teleport_to_room";

		inline constexpr const char* INTERNAL_NAME_DUNGEON = "dungeon";
		inline constexpr const char* GM_ROOM_PREFIX_MINES  = "rm_mines";

		// Pre-built lookup maps; populated by BuildMaps() via the setup_main_screen pub/sub.
		inline std::unordered_map<int, std::string>         location_id_to_internal_name_map;
		inline std::unordered_map<std::string, int>         location_internal_name_to_id_map;
		inline std::unordered_map<std::string, int>         gm_room_name_to_location_id_map;
		inline std::unordered_map<int, std::string>         location_id_to_gm_room_name_map;

		// Current GM room — updated from the goto_gm_room hook's Result.gm_room.
		// The game defers `room` changes to end-of-step, so this is the only reliable
		// signal of where the player will be on the next frame.
		inline std::string current_gm_room_name;

		using AfterGoToRoomCallback = void(*)(MMAPI::Location::GoToRoomContext&);
		inline AfterGoToRoomCallback after_go_to_room_callback = nullptr;

		// Internal pub/sub list of handlers invoked from the goto_gm_room hook after the original runs,
		// before the public AfterGoToRoom callback. Used by modules that need to react to room transitions
		// (e.g. Dungeon's floor-number tracking) without occupying the single public hook slot.
		// Handlers must be set before the room transition's end-of-step deferral so subsequent on_room_start
		// hooks see consistent state.
		using OnGoToRoomHandler = void(*)(const std::string& room_name);
		inline std::vector<OnGoToRoomHandler> on_go_to_room_internal_handlers;

		inline void RegisterOnGoToRoomHandler(OnGoToRoomHandler handler)
		{
			for (auto existing : on_go_to_room_internal_handlers)
				if (existing == handler)
					return;
			on_go_to_room_internal_handlers.push_back(handler);
		}

		inline YYTK::RValue GetLocationData(int location_id)
		{
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

		inline std::string LocationIdToGmRoomName(YYTK::CInstance* Self, YYTK::CInstance* Other, int location_id)
		{
			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(
				GML_SCRIPT_LOCATION_ID_TO_GM_ROOM,
				reinterpret_cast<PVOID*>(&gml_script)
			);
			if (!gml_script)
				return {};

			YYTK::RValue result;
			YYTK::RValue location = location_id;
			YYTK::RValue* location_ptr = &location;
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, { &location_ptr });

			YYTK::RValue room_name = MMAPI::Internal::module_interface->CallBuiltin("room_get_name", { result });
			if (room_name.m_Kind == YYTK::VALUE_UNDEFINED || room_name.m_Kind == YYTK::VALUE_UNSET)
				return {};

			return room_name.ToString();
		}

		// Pre-computes the location ↔ GM room lookup maps.
		// Called from MMAPI's setup_main_screen pub/sub when valid Self/Other are available.
		inline void BuildMaps(YYTK::CInstance* Self, YYTK::CInstance* Other)
		{
			location_id_to_internal_name_map.clear();
			location_internal_name_to_id_map.clear();
			gm_room_name_to_location_id_map.clear();
			location_id_to_gm_room_name_map.clear();

			YYTK::RValue location_ids = MMAPI::Internal::global_instance->GetMember("__location_id__");
			size_t array_length = 0;
			MMAPI::Internal::module_interface->GetArraySize(location_ids, array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				YYTK::RValue* element = nullptr;
				MMAPI::Internal::module_interface->GetArrayEntry(location_ids, i, element);
				if (!element)
					continue;

				std::string internal_name = element->ToString();
				int id = static_cast<int>(i);
				location_id_to_internal_name_map[id]            = internal_name;
				location_internal_name_to_id_map[internal_name] = id;
			}

			for (const auto& [id, internal_name] : location_id_to_internal_name_map)
			{
				// DUNGEON is a procedural location — calling location_id_to_gm_room on it crashes the game.
				if (internal_name == INTERNAL_NAME_DUNGEON)
					continue;

				std::string gm_room_name = LocationIdToGmRoomName(Self, Other, id);
				if (gm_room_name.empty())
					continue;

				gm_room_name_to_location_id_map[gm_room_name] = id;
				location_id_to_gm_room_name_map[id]           = gm_room_name;
			}
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

			std::string room_name;
			if (room_name_rv.m_Kind != YYTK::VALUE_UNDEFINED && room_name_rv.m_Kind != YYTK::VALUE_UNSET)
				room_name = room_name_rv.ToString();

			current_gm_room_name = room_name;

			// Procedurally-generated dungeon rooms aren't enumerated at startup, so map them to DUNGEON on first sight.
			if (!room_name.empty()
				&& !gm_room_name_to_location_id_map.contains(room_name)
				&& room_name.find(GM_ROOM_PREFIX_MINES) != std::string::npos
				&& location_internal_name_to_id_map.contains(INTERNAL_NAME_DUNGEON))
			{
				gm_room_name_to_location_id_map[room_name] = location_internal_name_to_id_map[INTERNAL_NAME_DUNGEON];
			}

			for (auto handler : on_go_to_room_internal_handlers)
				handler(room_name);

			if (after_go_to_room_callback)
			{
				MMAPI::Location::GoToRoomContext context{ room_name };
				after_go_to_room_callback(context);
			}

			return Result;
		}
	}

	/// Activates Location utility functions that track the current location.
	/// Installs an internal handler that builds GM-room ↔ location lookup maps at title-screen setup,
	/// and a hook on `goto_gm_room` that tracks the player's current GM room. `TryGetCurrentLocation`
	/// reads from those structures.
	/// @return AURIE_SUCCESS if the hook is installed (or already was); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		Aurie::AurieStatus status = MMAPI::Instance::Enable();
		if (!Aurie::AurieSuccess(status))
			return status;

		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::BuildMaps);

		return MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_GO_TO_ROOM,               reinterpret_cast<PVOID>(Internal::GmlScriptGoToRoomCallback) },
		});
	}

	/// Gets a location's internal name.
	/// @param location The location to resolve.
	/// @return The location internal name as an RValue.
	inline YYTK::RValue GetInternalName(MMAPI::Location::Ids location)
	{
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

	/// Gets the current location, resolved from the player's current GM room via pre-computed lookup maps.
	/// @attention Requires MMAPI::Location::Enable() to have been called.
	/// @param location The current location.
	/// @return True if the current GM room maps to a known location; false if no GM-room transition has been observed yet.
	inline bool TryGetCurrentLocation(MMAPI::Location::Ids& location)
	{
		if (Internal::current_gm_room_name.empty())
			return false;

		auto it = Internal::gm_room_name_to_location_id_map.find(Internal::current_gm_room_name);
		if (it == Internal::gm_room_name_to_location_id_map.end())
			return false;

		location = static_cast<MMAPI::Location::Ids>(it->second);
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
	/// @attention Requires MMAPI::Location::Enable() to have been called.
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

	namespace Hooks
	{
		/// Registers a callback that runs after the game transitions to a new GM room.
		/// Use ctx.GetRoomName() to read the name of the room that was entered.
		/// @param callback A function called with the room transition context after the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterGoToRoom(Internal::AfterGoToRoomCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_go_to_room_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Location::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			Internal::after_go_to_room_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}
}
