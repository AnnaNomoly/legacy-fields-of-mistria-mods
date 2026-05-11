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

		// Live WeatherManager Self/Other, latched from the get_weather hook.
		// Used by TryGetWeatherManagerContext for callers outside any hook frame.
		inline YYTK::CInstance* weather_manager_self  = nullptr;
		inline YYTK::CInstance* weather_manager_other = nullptr;

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by Weather::Enable().
		inline void ClearWeatherManagerOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			weather_manager_self  = nullptr;
			weather_manager_other = nullptr;
		}

		/// Resolves the WeatherManager's GML calling context, latched from the most recent get_weather call.
		/// Cleared automatically when the game returns to the title menu.
		/// @return True if a get_weather call has been observed this session, false otherwise.
		inline bool TryGetWeatherManagerContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			if (!weather_manager_self)
				return false;
			Self  = weather_manager_self;
			Other = weather_manager_other;
			return true;
		}

		inline YYTK::RValue& GetWeatherContextCallback(IN YYTK::CInstance* Self, IN YYTK::CInstance* Other, OUT YYTK::RValue& Result, IN int ArgumentCount, IN YYTK::RValue** Arguments)
		{
			// Refresh on every fire.
			weather_manager_self  = Self;
			weather_manager_other = Other;

			if (before_get_weather_callback)
				before_get_weather_callback();

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_GET_WEATHER));
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}
	}

	/// Activates Weather utility functions. Installs the get_weather hook so the live WeatherManager Self/Other are latched
	/// for TryGetWeatherManagerContext (cleared on return-to-title via the setup_main_screen pub/sub).
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearWeatherManagerOnReturnToTitle);

		return MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_GET_WEATHER,              reinterpret_cast<PVOID>(Internal::GetWeatherContextCallback) },
		});
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
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetWeatherManagerContext(Self, Other))
			return false;

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

			Aurie::AurieStatus status = MMAPI::Weather::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			Internal::before_get_weather_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
	}
}
