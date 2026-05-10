#pragma once

#include "Core.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::T2
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_T2_READ = "gml_Script_read@T2r@T2r";

		inline YYTK::RValue& T2ReadContextCallback(IN YYTK::CInstance* Self, IN YYTK::CInstance* Other, OUT YYTK::RValue& Result, IN int ArgumentCount, IN YYTK::RValue** Arguments)
		{
			MMAPI::Internal::RegisterScriptContext(GML_SCRIPT_T2_READ, Self, Other);
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_T2_READ));
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}
	}

	/// Activates T2 utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_T2_READ,
			reinterpret_cast<PVOID>(Internal::T2ReadContextCallback)
		);
	}

	/// Reads a value from the game's T2 database by key.
	/// @attention Requires MMAPI::T2::Enable() to have been called.
	/// @param key The T2 key to read.
	/// @return The T2 value as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue Read(const std::string& key)
	{
		const auto& refs = MMAPI::Internal::script_reference_map;
		if (!refs.contains(Internal::GML_SCRIPT_T2_READ))
			return {};
		YYTK::CInstance* Self  = refs.at(Internal::GML_SCRIPT_T2_READ)[0];
		YYTK::CInstance* Other = refs.at(Internal::GML_SCRIPT_T2_READ)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_T2_READ, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		YYTK::RValue input = key.c_str();
		YYTK::RValue* args[1] = { &input };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		return result;
	}
}
