#pragma once

#include "Core.hpp"
#include "Game.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Calendar
{
	inline constexpr int NightStartTimeInSeconds = 72000;

	/// Source: globalInstance.__day__
	enum class Weekdays : int
	{
		Monday    = 0,
		Tuesday   = 1,
		Wednesday = 2,
		Thursday  = 3,
		Friday    = 4,
		Saturday  = 5,
		Sunday    = 6
	};

	/// Source: globalInstance.__season__
	enum class Seasons : int
	{
		Spring = 0,
		Summer = 1,
		Fall   = 2,
		Winter = 3
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_UNIFIED_TIME = "gml_Script_unified_time@Calendar@Calendar";
		inline constexpr const char* GML_SCRIPT_GET_DAY          = "gml_Script_day@Calendar@Calendar";
		inline constexpr const char* GML_SCRIPT_GET_SEASON       = "gml_Script_season@Calendar@Calendar";
		inline constexpr const char* GML_SCRIPT_GET_YEAR         = "gml_Script_year@Calendar@Calendar";

		// Live Calendar Self/Other, latched from the unified_time hook (fires every frame during gameplay).
		// All four Calendar scripts are bound to the same Calendar singleton, so a single latch serves all of them.
		// Used by TryGetCalendarContext for callers outside any hook frame.
		inline YYTK::CInstance* calendar_self  = nullptr;
		inline YYTK::CInstance* calendar_other = nullptr;

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by Calendar::Enable().
		inline void ClearCalendarOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			calendar_self  = nullptr;
			calendar_other = nullptr;
		}

		inline YYTK::RValue& UnifiedTimeContextCallback(IN YYTK::CInstance* Self, IN YYTK::CInstance* Other, OUT YYTK::RValue& Result, IN int ArgumentCount, IN YYTK::RValue** Arguments)
		{
			// Refresh the latched pair every tick.
			calendar_self  = Self;
			calendar_other = Other;

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_GET_UNIFIED_TIME));
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		/// Resolves the Calendar's GML calling context, latched from the most recent unified_time tick.
		/// Cleared automatically when the game returns to the title menu.
		/// @return True if a Calendar unified_time call has been observed this session, false otherwise.
		inline bool TryGetCalendarContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			if (!calendar_self)
				return false;
			Self  = calendar_self;
			Other = calendar_other;
			return true;
		}

		inline YYTK::RValue CallCalendarScript(const char* script_name)
		{
			YYTK::CInstance* Self  = nullptr;
			YYTK::CInstance* Other = nullptr;
			if (!TryGetCalendarContext(Self, Other))
				return {};

			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(script_name, reinterpret_cast<PVOID*>(&gml_script));

			YYTK::RValue result;
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
			return result;
		}

		inline YYTK::RValue GetDay()
		{
			return CallCalendarScript(GML_SCRIPT_GET_DAY);
		}

		inline YYTK::RValue GetSeason()
		{
			return CallCalendarScript(GML_SCRIPT_GET_SEASON);
		}

		inline YYTK::RValue GetYear()
		{
			return CallCalendarScript(GML_SCRIPT_GET_YEAR);
		}
	}

	/// Activates Calendar utility functions. Installs the unified_time hook so the live Calendar Self/Other are latched
	/// for TryGetCalendarContext (cleared on return-to-title via the setup_main_screen pub/sub). All Calendar scripts
	/// share the same singleton context, so a single latch serves day/season/year as well.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearCalendarOnReturnToTitle);

		return MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_GET_UNIFIED_TIME,         reinterpret_cast<PVOID>(Internal::UnifiedTimeContextCallback) },
		});
	}

	/// Gets the current 1-indexed day of the month from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @return The current day of the month from 1 to 28 as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetDay()
	{
		YYTK::RValue day = Internal::GetDay();
		if (day.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return static_cast<int>(day.ToInt64()) + 1;
	}

	/// Gets the current weekday from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @param weekday The current weekday.
	/// @return True if the weekday was resolved, false if the required context is unavailable.
	inline bool TryGetWeekday(MMAPI::Calendar::Weekdays& weekday)
	{
		YYTK::RValue day_of_month = GetDay();
		if (day_of_month.m_Kind == YYTK::VALUE_UNDEFINED || day_of_month.m_Kind == YYTK::VALUE_UNSET)
			return false;

		int weekday_id = (static_cast<int>(day_of_month.ToInt64()) - 1) % 7;
		if (weekday_id < static_cast<int>(MMAPI::Calendar::Weekdays::Monday) ||
		    weekday_id > static_cast<int>(MMAPI::Calendar::Weekdays::Sunday))
			return false;

		weekday = static_cast<MMAPI::Calendar::Weekdays>(weekday_id);
		return true;
	}

	/// Returns true if the current weekday matches weekday.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @param weekday The weekday to compare against.
	inline bool IsWeekday(MMAPI::Calendar::Weekdays weekday)
	{
		MMAPI::Calendar::Weekdays current_weekday;
		if (!TryGetWeekday(current_weekday))
			return false;

		return current_weekday == weekday;
	}

	/// Gets the current season from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @param season The current season.
	/// @return True if the season was resolved, false if the required context is unavailable.
	inline bool TryGetSeason(MMAPI::Calendar::Seasons& season)
	{
		YYTK::RValue current_season = Internal::GetSeason();
		if (current_season.m_Kind == YYTK::VALUE_UNDEFINED || current_season.m_Kind == YYTK::VALUE_UNSET)
			return false;

		int season_id = static_cast<int>(current_season.ToInt64());
		if (season_id < static_cast<int>(MMAPI::Calendar::Seasons::Spring) ||
		    season_id > static_cast<int>(MMAPI::Calendar::Seasons::Winter))
			return false;

		season = static_cast<MMAPI::Calendar::Seasons>(season_id);
		return true;
	}

	/// Returns true if the current season matches season.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @param season The season to compare against.
	inline bool IsSeason(MMAPI::Calendar::Seasons season)
	{
		MMAPI::Calendar::Seasons current_season;
		if (!TryGetSeason(current_season))
			return false;

		return current_season == season;
	}

	/// Gets the current 1-indexed calendar year from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @return The current calendar year as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetYear()
	{
		YYTK::RValue year = Internal::GetYear();
		if (year.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return static_cast<int>(year.ToInt64()) + 1;
	}

	/// Returns true if the current game clock time is night.
	/// Night starts at 8 PM, matching the game's daytime/nighttime split.
	inline bool IsNight()
	{
		return MMAPI::Game::GetCurrentTimeInSeconds() >= NightStartTimeInSeconds;
	}

	/// Returns true if the current game clock time is day.
	/// Day is any time before 8 PM, matching the game's daytime/nighttime split.
	inline bool IsDay()
	{
		return !IsNight();
	}

	/// Gets the current unified game time from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Enable() to have been called.
	/// @return The current unified time as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetUnifiedTime()
	{
		return Internal::CallCalendarScript(Internal::GML_SCRIPT_GET_UNIFIED_TIME);
	}
}
