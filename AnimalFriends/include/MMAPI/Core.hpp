#pragma once

#include "Status.hpp"

#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI
{
	namespace Internal
	{
		inline uint64_t GetCurrentSystemTime()
		{
			using namespace std::chrono;
			return static_cast<uint64_t>(
				duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()
			);
		}

		// Set by the mod at startup via Initialize().
		inline YYTK::YYTKInterface* module_interface = nullptr;

		// The GML global instance, typically obtained from the YYTK interface during initialization.
		inline YYTK::CInstance* global_instance = nullptr;

		// The Aurie module using MMAPI. Required for MMAPI-owned hooks.
		inline Aurie::AurieModule* self_module = nullptr;

		// Mod identity, passed to Initialize. Used for log attribution and per-mod log file paths.
		inline std::string mod_name;
		inline std::string mod_version;

		// Persisted game object instance pairs, keyed by a descriptive instance name.
		// Used for objects like obj_ari whose context is captured from an object callback rather than a script hook.
		inline std::map<std::string, std::vector<YYTK::CInstance*>> instance_reference_map;

		inline std::map<std::string, bool> owned_script_hook_installed_map;

		// Internal pub/sub list of handlers invoked from MMAPI's setup_main_screen hook,
		// before any user callback runs. Used by modules that need to (re)build state
		// when the game returns to the title screen with valid Self/Other context.
		using OnSetupMainScreenHandler = void(*)(YYTK::CInstance* Self, YYTK::CInstance* Other);
		inline std::vector<OnSetupMainScreenHandler> on_setup_main_screen_internal_handlers;

		inline void RegisterOnSetupMainScreenHandler(OnSetupMainScreenHandler handler)
		{
			for (auto existing : on_setup_main_screen_internal_handlers)
				if (existing == handler)
					return;
			on_setup_main_screen_internal_handlers.push_back(handler);
		}

		/// Captures a pair of game object instance pointers the first time it is seen.
		/// Used internally by MMAPI::Instance::Enable()'s EVENT_OBJECT_CALL dispatcher.
		inline void RegisterInstanceContext(const char* instance_name, YYTK::CInstance* Self, YYTK::CInstance* Other)
		{
			if (!instance_reference_map.contains(instance_name))
				instance_reference_map[instance_name] = { Self, Other };
		}

		// MMAPI-wide setup_main_screen hook. Owned by Core because its primary job is core infrastructure:
		// clearing the instance reference map on return-to-title and dispatching the internal handler pub/sub list.
		// `MMAPI::Game::Hooks::BeforeSetupMainScreen` is a thin user-facing wrapper that registers a callback here.
		inline constexpr const char* GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";

		using BeforeSetupMainScreenCallback = void(*)();
		inline BeforeSetupMainScreenCallback before_setup_main_screen_callback = nullptr;

		inline YYTK::RValue& GmlScriptBeforeSetupMainScreenCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			instance_reference_map.clear();

			for (auto handler : on_setup_main_screen_internal_handlers)
				handler(Self, Other);

			if (before_setup_main_screen_callback)
				before_setup_main_screen_callback();

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(self_module, GML_SCRIPT_SETUP_MAIN_SCREEN)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}
	}

	/// Initializes MMAPI. Call once from ModuleInitialize.
	/// @param module_interface The YYTKInterface pointer received in ModuleInitialize.
	/// @param global The GML global instance pointer obtained from GetGlobalInstance.
	/// @param module The Aurie module pointer received in ModuleInitialize.
	/// @param mod_name The mod's name, used for log attribution and the per-mod log file path.
	/// @param mod_version The mod's version string, used in console log prefixes.
	inline void Initialize(
		YYTK::YYTKInterface* module_interface,
		YYTK::CInstance* global,
		Aurie::AurieModule* module,
		const char* mod_name,
		const char* mod_version
	)
	{
		// Diagnostic: surface missing pointers loudly. Without these, every downstream MMAPI call is a
		// silent no-op (utility functions fail their preconditions; hook installs return NotInitialized).
		// Bypass MMAPI::Log here because Log itself reads module_interface — if that's the missing one,
		// Log has nowhere to write. Fall back to YYTK Print directly when possible.
		if (!module_interface || !global || !module)
		{
			if (module_interface)
			{
				module_interface->Print(YYTK::CM_LIGHTRED,
					"[MMAPI::Initialize] missing required pointer "
					"(module_interface=%p, global=%p, module=%p) -- downstream MMAPI calls will fail",
					static_cast<void*>(module_interface),
					static_cast<void*>(global),
					static_cast<void*>(module));
			}
			// No interface at all means we can't log; the only signal will be downstream failures.
		}

		MMAPI::Internal::module_interface = module_interface;
		MMAPI::Internal::global_instance  = global;
		MMAPI::Internal::self_module      = module;
		MMAPI::Internal::mod_name         = mod_name    ? mod_name    : "";
		MMAPI::Internal::mod_version      = mod_version ? mod_version : "";
	}
}
