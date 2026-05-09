#pragma once

#include "Core.hpp"
#include "Engine.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Cosmetic
{
	namespace Internal
	{
		inline YYTK::RValue GetCosmeticUnlocks()
		{
			if (!MMAPI::Internal::global_instance)
				return {};

			YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
			if (ari.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			YYTK::RValue cosmetic_unlocks = ari.GetMember("cosmetic_unlocks");
			if (cosmetic_unlocks.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			return cosmetic_unlocks.GetMember("inner");
		}
	}

	/// Returns true if Ari has unlocked the given cosmetic.
	/// @param cosmetic_name The internal cosmetic name to check.
	inline bool IsUnlocked(const std::string& cosmetic_name)
	{
		YYTK::RValue cosmetic_unlocks = Internal::GetCosmeticUnlocks();
		if (cosmetic_unlocks.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		return MMAPI::Engine::StructVariableExists(cosmetic_unlocks, cosmetic_name.c_str());
	}

	/// Unlocks the given cosmetic for Ari.
	/// @param cosmetic_name The internal cosmetic name to unlock.
	/// @return True if the cosmetic was newly unlocked; otherwise false.
	inline bool Unlock(const std::string& cosmetic_name)
	{
		YYTK::RValue cosmetic_unlocks = Internal::GetCosmeticUnlocks();
		if (cosmetic_unlocks.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		if (IsUnlocked(cosmetic_name))
			return false;

		MMAPI::Engine::StructVariableSet(cosmetic_unlocks, cosmetic_name.c_str(), 0.0);
		return true;
	}
}
