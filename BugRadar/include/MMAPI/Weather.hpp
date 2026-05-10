#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Weather
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_WEATHER = "gml_Script_get_weather@WeatherManager@Weather";

		using BeforeGetWeatherCallback = void(*)();
		inline BeforeGetWeatherCallback before_get_weather_callback = nullptr;

		inline YYTK::RValue& GetWeatherContextCallback(IN YYTK::CInstance* Self, IN YYTK::CInstance* Other, OUT YYTK::RValue& Result, IN int ArgumentCount, IN YYTK::RValue** Arguments)
		{
			MMAPI::Internal::RegisterScriptContext(GML_SCRIPT_GET_WEATHER, Self, Other);

			if (before_get_weather_callback)
				before_get_weather_callback();

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_GET_WEATHER));
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}
	}

	/// Activates Weather utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_GET_WEATHER,
			reinterpret_cast<PVOID>(Internal::GetWeatherContextCallback)
		);
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
	/// @attention Requires MMAPI::Weather::Enable() to have been called.
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
	/// @attention Requires MMAPI::Weather::Enable() to have been called.
	/// @param weather The weather type to compare against.
	inline bool IsWeather(MMAPI::Weather::Ids weather)
	{
		MMAPI::Weather::Ids current_weather;
		if (!TryGetWeather(current_weather))
			return false;

		return current_weather == weather;
	}

	/// Returns true if the current weather is not inclement or heavy inclement.
	/// @attention Requires MMAPI::Weather::Enable() to have been called.
	inline bool IsSunny()
	{
		MMAPI::Weather::Ids weather;
		if (!TryGetWeather(weather))
			return false;

		return weather != MMAPI::Weather::Ids::HeavyInclement &&
		       weather != MMAPI::Weather::Ids::Inclement;
	}

	namespace Hooks
	{
		/// Registers a callback that runs before the game's WeatherManager get_weather script.
		/// Useful as a "game is active" signal: get_weather runs continuously during gameplay but never on the title screen,
		/// so the first invocation after returning from the title screen reliably indicates the save has loaded.
		/// @param callback A function called before the original get_weather script runs.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeGetWeather(Internal::BeforeGetWeatherCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_get_weather_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				Internal::GML_SCRIPT_GET_WEATHER,
				reinterpret_cast<PVOID>(Internal::GetWeatherContextCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			Internal::before_get_weather_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}
}
