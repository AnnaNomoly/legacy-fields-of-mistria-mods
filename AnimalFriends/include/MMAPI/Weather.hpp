#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Weather
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";
	}

	/// Source: globalInstance.__weather__
	enum class Ids : int
	{
		Calm            = 0,
		Inclement       = 1,
		HeavyInclement  = 2,
		Special         = 3
	};

	/// Gets the current weather from the game's WeatherManager.
	/// @attention Requires MMAPI::Weather::Internal::GML_SCRIPT_GET_WEATHER to be registered via RegisterScriptContext.
	/// @param weather Receives the current weather when the call succeeds.
	/// @return True if the context and script were available; otherwise false.
	inline bool TryGetWeather(MMAPI::Weather::Ids& weather)
	{
		const auto& refs = MMAPI::Internal::script_reference_map;
		if (!refs.contains(Internal::GML_SCRIPT_GET_WEATHER))
			return false;
		YYTK::CInstance* Self  = refs.at(Internal::GML_SCRIPT_GET_WEATHER)[0];
		YYTK::CInstance* Other = refs.at(Internal::GML_SCRIPT_GET_WEATHER)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_WEATHER, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		int weather_id = static_cast<int>(result.ToInt64());
		if (weather_id < static_cast<int>(MMAPI::Weather::Ids::Calm) ||
		    weather_id > static_cast<int>(MMAPI::Weather::Ids::Special))
			return false;

		weather = static_cast<MMAPI::Weather::Ids>(weather_id);
		return true;
	}

	/// Returns true if the current weather matches weather.
	/// @attention Requires MMAPI::Weather::Internal::GML_SCRIPT_GET_WEATHER to be registered via RegisterScriptContext.
	/// @param weather The weather type to compare against.
	inline bool IsWeather(MMAPI::Weather::Ids weather)
	{
		MMAPI::Weather::Ids current_weather;
		if (!TryGetWeather(current_weather))
			return false;

		return current_weather == weather;
	}

	/// Returns true if the current weather is not inclement or heavy inclement.
	/// @attention Requires MMAPI::Weather::Internal::GML_SCRIPT_GET_WEATHER to be registered via RegisterScriptContext.
	inline bool IsSunny()
	{
		MMAPI::Weather::Ids weather;
		if (!TryGetWeather(weather))
			return false;

		return weather != MMAPI::Weather::Ids::HeavyInclement &&
		       weather != MMAPI::Weather::Ids::Inclement;
	}
}
