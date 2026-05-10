#pragma once

#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>
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

		/// Installs a batch of script hooks. Returns the first failure (and stops short-circuit),
		/// or AURIE_SUCCESS if every hook was installed (or already was).
		inline Aurie::AurieStatus InstallScriptHooks(std::initializer_list<std::pair<const char*, PVOID>> hooks)
		{
			for (const auto& [script_name, callback] : hooks)
			{
				Aurie::AurieStatus status = InstallScriptHook(script_name, callback);
				if (!Aurie::AurieSuccess(status))
					return status;
			}
			return Aurie::AURIE_SUCCESS;
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

	namespace Internal
	{
		// MMAPI-wide setup_main_screen hook. Owned by Core because its primary job is core infrastructure:
		// clearing script/instance contexts on return-to-title and dispatching the internal handler pub/sub list.
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
			MMAPI::ClearScriptContexts();

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

		inline Aurie::AurieStatus RegisterBeforeSetupMainScreenHook(BeforeSetupMainScreenCallback callback)
		{
			Aurie::AurieStatus status = InstallScriptHook(
				GML_SCRIPT_SETUP_MAIN_SCREEN,
				reinterpret_cast<PVOID>(GmlScriptBeforeSetupMainScreenCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;

			before_setup_main_screen_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}
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
