#pragma once

#include "Core.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include <initializer_list>
#include <utility>

namespace MMAPI::Internal
{
	/// Installs a single Aurie script hook for the given GML script name. Idempotent: a second
	/// install for the same script is a no-op that returns Success. Logs at Debug on first install.
	inline MMAPI::Status InstallScriptHook(const char* script_name, PVOID callback)
	{
		if (owned_script_hook_installed_map[script_name])
			return MMAPI::Status::Success;

		if (!module_interface || !self_module)
			return MMAPI::Status::NotInitialized;

		YYTK::CScript* gml_script = nullptr;
		Aurie::AurieStatus status = module_interface->GetNamedRoutinePointer(
			script_name,
			reinterpret_cast<PVOID*>(&gml_script)
		);

		if (!Aurie::AurieSuccess(status))
			return MMAPI::Status::InstallFailed;

		status = Aurie::MmCreateHook(
			self_module,
			script_name,
			gml_script->m_Functions->m_ScriptFunction,
			callback,
			nullptr
		);

		if (!Aurie::AurieSuccess(status))
			return MMAPI::Status::InstallFailed;

		owned_script_hook_installed_map[script_name] = true;
		MMAPI::Log::Debug("Installed script hook: %s", script_name);
		return MMAPI::Status::Success;
	}

	/// Installs a batch of script hooks. Returns the first failure (and stops short-circuit),
	/// or Status::Success if every hook was installed (or already was).
	inline MMAPI::Status InstallScriptHooks(std::initializer_list<std::pair<const char*, PVOID>> hooks)
	{
		for (const auto& [script_name, callback] : hooks)
		{
			MMAPI::Status status = InstallScriptHook(script_name, callback);
			if (!MMAPI::IsSuccess(status))
				return status;
		}
		return MMAPI::Status::Success;
	}

	/// Stores a callback into a module-owned slot, with null and duplicate guards plus diagnostic logging.
	/// All public `Hooks::*` registrars route through this template — it is the single chokepoint where
	/// cross-cutting registration behavior (logging, telemetry, conflict detection) lives.
	///
	/// @param hook_name Human-readable identifier used in log messages (e.g. "Player::AfterMoveSpeed").
	/// @param slot Reference to the module's callback storage (nullptr-initialized at module scope).
	/// @param callback The callback being registered.
	/// @return Status::InvalidParameter if callback is null; Status::AlreadyRegistered if slot is occupied;
	///         Status::Success otherwise.
	template <typename CallbackT>
	inline MMAPI::Status RegisterHook(const char* hook_name, CallbackT& slot, CallbackT callback)
	{
		if (!callback)
		{
			MMAPI::Log::Warn("RegisterHook(%s): null callback", hook_name);
			return MMAPI::Status::InvalidParameter;
		}

		if (slot)
		{
			MMAPI::Log::Warn("RegisterHook(%s): already registered", hook_name);
			return MMAPI::Status::AlreadyRegistered;
		}

		slot = callback;
		MMAPI::Log::Debug("Registered hook: %s", hook_name);
		return MMAPI::Status::Success;
	}
}
