#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"

#include <algorithm>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Spell
{
	/// Source: globalInstance.__spell__
	enum class Ids : int
	{
		FireBreath  = 0,
		FullRestore = 1,
		Growth      = 2,
		SacredLight = 3,
		SummonRain  = 4
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_CAN_CAST_SPELL = "gml_Script_can_cast_spell";

		inline YYTK::RValue GetSpellData()
		{
			return MMAPI::Internal::global_instance->GetMember("__spells");
		}

		inline YYTK::RValue GetSpellData(int spell_id)
		{
			if (spell_id < 0)
				return {};

			YYTK::RValue spells = GetSpellData();

			size_t spell_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(spells, spell_count);

			if (static_cast<size_t>(spell_id) >= spell_count)
				return {};

			YYTK::RValue* spell = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(spells, static_cast<size_t>(spell_id), spell);

			return *spell;
		}

		inline YYTK::RValue GetCost(int spell_id)
		{
			YYTK::RValue spell = GetSpellData(spell_id);
			if (spell.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			return spell.GetMember("cost");
		}

		inline void SetCost(int spell_id, int cost)
		{
			YYTK::RValue spell = GetSpellData(spell_id);
			if (spell.m_Kind == YYTK::VALUE_UNDEFINED)
				return;

			MMAPI::Engine::StructVariableSet(spell, "cost", max(0, cost));
		}
	}

	/// Gets the mana cost of a spell from globalInstance.__spells.
	/// @param spell The spell to read.
	/// @return The spell's mana cost as an RValue, or undefined if the spell ID is out of bounds.
	inline YYTK::RValue GetCost(MMAPI::Spell::Ids spell)
	{
		return Internal::GetCost(static_cast<int>(spell));
	}

	/// Sets the mana cost of a spell in globalInstance.__spells. Negative values are clamped to 0.
	/// @param spell The spell to modify.
	/// @param cost The new mana cost.
	inline void SetCost(MMAPI::Spell::Ids spell, int cost)
	{
		Internal::SetCost(static_cast<int>(spell), cost);
	}

	/// Returns true if Ari can currently cast the given spell.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param spell The spell to check.
	inline bool CanCast(MMAPI::Spell::Ids spell)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return false;
		YYTK::CInstance* Self  = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
		YYTK::CInstance* Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_CAN_CAST_SPELL, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue spell_id = static_cast<int>(spell);
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &spell_id };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		return result.ToBoolean();
	}
}
