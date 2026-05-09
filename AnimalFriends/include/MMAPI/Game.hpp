#pragma once

#include "Core.hpp"
#include "Instance.hpp"

#include <string>
#include <unordered_map>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Game
{
	/// Source: globalInstance.__xp_values
	/// Struct-backed enum; values follow alphabetical key order.
	enum class XpValues : int
	{
		BreakDigSpot     = 0,
		CatchGiantFish   = 1,
		CatchLargeFish   = 2,
		CatchMediumFish  = 3,
		CatchSmallFish   = 4,
		HarvestCrop      = 5,
		HarvestDiveSpot  = 6,
		IdentifyArtifact = 7,
		PlantSeed        = 8,
		TillDirt         = 9,
		WaterDirt        = 10
	};

	namespace Internal
	{
		inline std::unordered_map<std::string, uint64_t> notification_last_display_time;
		inline constexpr const char* GML_SCRIPT_CREATE_NOTIFICATION   = "gml_Script_create_notification";
		inline constexpr const char* GML_SCRIPT_SELL_SHIPPING_BIN_ITEMS = "gml_Script_sell_shipping_bin_items";

		inline constexpr const char* ToGameKey(MMAPI::Game::XpValues value)
		{
			switch (value)
			{
				case MMAPI::Game::XpValues::BreakDigSpot:
					return "break_dig_spot";
				case MMAPI::Game::XpValues::CatchGiantFish:
					return "catch_giant_fish";
				case MMAPI::Game::XpValues::CatchLargeFish:
					return "catch_large_fish";
				case MMAPI::Game::XpValues::CatchMediumFish:
					return "catch_medium_fish";
				case MMAPI::Game::XpValues::CatchSmallFish:
					return "catch_small_fish";
				case MMAPI::Game::XpValues::HarvestCrop:
					return "harvest_crop";
				case MMAPI::Game::XpValues::HarvestDiveSpot:
					return "harvest_dive_spot";
				case MMAPI::Game::XpValues::IdentifyArtifact:
					return "identify_artifact";
				case MMAPI::Game::XpValues::PlantSeed:
					return "plant_seed";
				case MMAPI::Game::XpValues::TillDirt:
					return "till_dirt";
				case MMAPI::Game::XpValues::WaterDirt:
					return "water_dirt";
				default:
					return nullptr;
			}
		}
	}

	/// Returns true if the game is currently paused.
	inline bool IsPaused()
	{
		if (!MMAPI::Internal::global_instance)
			return false;

		YYTK::RValue pause_status = MMAPI::Internal::global_instance->GetMember("__pause_status");
		return pause_status.ToInt64() > 0;
	}

	/// Returns true if the game window currently has focus.
	inline bool WindowHasFocus()
	{
		YYTK::RValue window_has_focus = MMAPI::Internal::module_interface->CallBuiltin("window_has_focus", {});
		return window_has_focus.ToBoolean();
	}

	/// Returns the current game clock time in seconds from MMAPI::Internal::global_instance.__clock.time.
	inline int GetCurrentTimeInSeconds()
	{
		return static_cast<int>(
			MMAPI::Internal::global_instance
				->GetMember("__clock")
				.GetMember("time")
				.ToInt64()
		);
	}

	/// Returns the current GM room name.
	/// @return The current GM room name, or an empty string if it cannot be read.
	inline std::string GetCurrentRoomName()
	{
		if (!MMAPI::Internal::module_interface)
			return "";

		YYTK::RValue room_id;
		Aurie::AurieStatus status = MMAPI::Internal::module_interface->GetBuiltin("room", nullptr, NULL_INDEX, room_id);
		if (!Aurie::AurieSuccess(status))
			return "";

		YYTK::RValue room_name = MMAPI::Internal::module_interface->CallBuiltin("room_get_name", { room_id });
		if (room_name.m_Kind == YYTK::VALUE_UNDEFINED || room_name.m_Kind == YYTK::VALUE_UNSET)
			return "";

		return room_name.ToString();
	}

	/// Returns true if the current GM room name exactly matches room_name.
	/// @param room_name The GM room name to compare against.
	inline bool IsCurrentRoom(const std::string& room_name)
	{
		return GetCurrentRoomName() == room_name;
	}

	/// Returns true if the current GM room name contains room_name_part.
	/// @param room_name_part The text to search for in the current GM room name.
	inline bool CurrentRoomNameContains(const std::string& room_name_part)
	{
		return GetCurrentRoomName().contains(room_name_part);
	}

	/// Gets an XP value from MMAPI::Internal::global_instance.__xp_values.
	/// @param xp_value The XP value to read.
	/// @return The XP value as an RValue, or undefined if unavailable.
	inline YYTK::RValue GetXpValue(MMAPI::Game::XpValues xp_value)
	{
		if (!MMAPI::Internal::global_instance)
			return {};

		const char* xp_value_key = Internal::ToGameKey(xp_value);
		if (!xp_value_key)
			return {};

		YYTK::RValue xp_values = MMAPI::Internal::global_instance->GetMember("__xp_values");
		if (xp_values.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return xp_values.GetMember(xp_value_key);
	}

	/// Displays a localized notification popup.
	/// @param Self The GML instance invoking the notification (passed through to the script call).
	/// @param Other The GML other instance context (passed through to the script call).
	/// @param ignore_cooldown When true, bypasses the 5-second per-key cooldown and always displays the notification.
	/// @param notification_key Localization string key for the notification text.
	inline void CreateNotification(
		YYTK::CInstance* Self,
		YYTK::CInstance* Other,
		bool ignore_cooldown,
		const std::string& notification_key
	)
	{
		uint64_t now = MMAPI::Internal::GetCurrentSystemTime();
		if (!ignore_cooldown && now <= Internal::notification_last_display_time[notification_key] + 5000)
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(
			Internal::GML_SCRIPT_CREATE_NOTIFICATION,
			reinterpret_cast<PVOID*>(&gml_script)
		);

		YYTK::RValue result;
		YYTK::RValue notification_rv(notification_key);
		YYTK::RValue* notification_ptr = &notification_rv;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, { &notification_ptr });

		Internal::notification_last_display_time[notification_key] = now;
	}

	/// Sells the current shipping bin contents.
	/// @return The sale result as an RValue, or undefined if obj_ari has not been registered.
	inline YYTK::RValue SellShippingBinItems()
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return {};
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_SELL_SHIPPING_BIN_ITEMS, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

}
