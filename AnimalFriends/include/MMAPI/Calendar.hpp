#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Calendar
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_UNIFIED_TIME = "gml_Script_unified_time@Calendar@Calendar";
		inline constexpr const char* GML_SCRIPT_GET_DAY          = "gml_Script_day@Calendar@Calendar";
		inline constexpr const char* GML_SCRIPT_GET_SEASON       = "gml_Script_season@Calendar@Calendar";
		inline constexpr const char* GML_SCRIPT_GET_YEAR         = "gml_Script_year@Calendar@Calendar";

		inline YYTK::RValue CallCalendarScript(const char* script_name)
		{
			const auto& refs = MMAPI::Internal::script_reference_map;
			if (!refs.contains(script_name))
				return {};
			YYTK::CInstance* Self  = refs.at(script_name)[0];
			YYTK::CInstance* Other = refs.at(script_name)[1];

			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(script_name, reinterpret_cast<PVOID*>(&gml_script));

			YYTK::RValue result;
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
			return result;
		}
	}

	/// Gets the current 0-indexed calendar day from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Internal::GML_SCRIPT_GET_DAY to be registered via RegisterScriptContext.
	/// @return The current 0-indexed calendar day as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetDay()
	{
		return Internal::CallCalendarScript(Internal::GML_SCRIPT_GET_DAY);
	}

	/// Gets the current 0-indexed calendar season from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Internal::GML_SCRIPT_GET_SEASON to be registered via RegisterScriptContext.
	/// @return The current 0-indexed calendar season as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetSeason()
	{
		return Internal::CallCalendarScript(Internal::GML_SCRIPT_GET_SEASON);
	}

	/// Gets the current 0-indexed calendar year from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Internal::GML_SCRIPT_GET_YEAR to be registered via RegisterScriptContext.
	/// @return The current 0-indexed calendar year as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetYear()
	{
		return Internal::CallCalendarScript(Internal::GML_SCRIPT_GET_YEAR);
	}

	/// Gets the current unified game time from the Calendar script context.
	/// @attention Requires MMAPI::Calendar::Internal::GML_SCRIPT_GET_UNIFIED_TIME to be registered via RegisterScriptContext.
	/// @return The current unified time as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue GetUnifiedTime()
	{
		return Internal::CallCalendarScript(Internal::GML_SCRIPT_GET_UNIFIED_TIME);
	}
}
