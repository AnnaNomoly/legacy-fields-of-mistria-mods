#pragma once

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

		// Persisted Self/Other pairs captured from GML script hook callbacks, keyed by GML script name.
		inline std::map<std::string, std::vector<YYTK::CInstance*>> script_reference_map;

		// Persisted game object instance pairs, keyed by a descriptive instance name.
		// Used for objects like obj_ari whose context is captured from an object callback rather than a script hook.
		inline std::map<std::string, std::vector<YYTK::CInstance*>> instance_reference_map;

		inline std::map<std::string, bool> owned_script_hook_installed_map;

		inline Aurie::AurieStatus InstallScriptHook(const char* script_name, PVOID callback)
		{
			if (!self_module || !module_interface)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (owned_script_hook_installed_map[script_name])
				return Aurie::AURIE_SUCCESS;

			YYTK::CScript* gml_script = nullptr;
			Aurie::AurieStatus status = module_interface->GetNamedRoutinePointer(
				script_name,
				reinterpret_cast<PVOID*>(&gml_script)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			status = Aurie::MmCreateHook(
				self_module,
				script_name,
				gml_script->m_Functions->m_ScriptFunction,
				callback,
				nullptr
			);

			if (Aurie::AurieSuccess(status))
				owned_script_hook_installed_map[script_name] = true;

			return status;
		}

		/// Captures the Self and Other instance context for a GML script the first time it is seen.
		/// Used internally by MMAPI's per-module Enable() context-capture hooks.
		inline void RegisterScriptContext(const char* script_name, YYTK::CInstance* Self, YYTK::CInstance* Other)
		{
			if (!script_reference_map.contains(script_name))
				script_reference_map[script_name] = { Self, Other };
		}

		/// Captures a pair of game object instance pointers the first time it is seen.
		/// Used internally by MMAPI::Instance::Enable()'s EVENT_OBJECT_CALL dispatcher.
		inline void RegisterInstanceContext(const char* instance_name, YYTK::CInstance* Self, YYTK::CInstance* Other)
		{
			if (!instance_reference_map.contains(instance_name))
				instance_reference_map[instance_name] = { Self, Other };
		}
	}

	/// Clears all captured script and instance contexts.
	/// Call this from within the setup_main_screen hook callback to reset state when returning to the title screen.
	inline void ClearScriptContexts()
	{
		Internal::script_reference_map.clear();
		Internal::instance_reference_map.clear();
	}

	/// Initializes MMAPI. Call once from ModuleInitialize.
	/// @param module_interface The YYTKInterface pointer received in ModuleInitialize.
	/// @param global The GML global instance pointer obtained from GetGlobalInstance.
	/// @param module The Aurie module pointer received in ModuleInitialize.
	inline void Initialize(YYTK::YYTKInterface* module_interface, YYTK::CInstance* global, Aurie::AurieModule* module)
	{
		MMAPI::Internal::module_interface = module_interface;
		MMAPI::Internal::global_instance = global;
		MMAPI::Internal::self_module = module;
	}
}
