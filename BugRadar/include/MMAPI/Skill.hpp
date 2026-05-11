#pragma once

#include "Core.hpp"
#include "Instance.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Skill
{
	/// Source: globalInstance.__skill__
	enum class Ids : int
	{
		Archaeology   = 0,
		Blacksmithing = 1,
		Combat        = 2,
		Cooking       = 3,
		Farming       = 4,
		Fishing       = 5,
		Mining        = 6,
		Ranching      = 7,
		Woodcrafting  = 8
	};

	/// Total number of enumerators in Ids. Iterating [0, IdCount) covers every Ids value.
	inline constexpr int IdCount = 9;

	/// Invokes fn with every Ids value, in ascending order.
	template <typename Fn>
	inline void ForEachId(Fn fn)
	{
		for (int i = 0; i < IdCount; ++i)
			fn(static_cast<Ids>(i));
	}

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GAIN_XP = "gml_Script_gain_xp@Ari@Ari";
	}

	/// Activates Skill utility functions. Cascades to MMAPI::Instance::Enable so GainExperience can resolve
	/// Ari's calling context internally.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Instance::Enable();
	}

	/// Adds experience to one of Ari's player skills.
	/// @attention Requires MMAPI::Skill::Enable() to have been called.
	/// @param skill The player skill to receive experience.
	/// @param experience The amount of experience to add.
	inline void GainExperience(MMAPI::Skill::Ids skill, double experience)
	{
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GAIN_XP, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue skill_id = static_cast<int>(skill);
		YYTK::RValue xp = experience;
		YYTK::RValue result;
		YYTK::RValue* args[2] = { &skill_id, &xp };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 2, args);
	}
}
