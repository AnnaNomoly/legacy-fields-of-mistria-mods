#pragma once

#include "Core.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::T2
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_T2_READ = "gml_Script_read@T2r@T2r";
	}

	/// Reads a value from the game's T2 database by key.
	/// @attention Requires MMAPI::T2::Internal::GML_SCRIPT_T2_READ to be registered via RegisterScriptContext.
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
