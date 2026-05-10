#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"

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

	struct CanCastSpellContext
	{
		int m_spell_id = 0;
		bool m_result = false;

		/// Returns the spell being checked.
		MMAPI::Spell::Ids GetSpell() const { return static_cast<MMAPI::Spell::Ids>(m_spell_id); }

		/// Returns the can-cast result as computed by the game.
		bool GetResult() const { return m_result; }

		/// Overrides whether the game allows casting this spell.
		void SetResult(bool can_cast) { m_result = can_cast; }
	};

	struct CastSpellContext
	{
		int m_spell_id = 0;
		bool m_cancelled = false;

		/// Returns the spell being cast.
		MMAPI::Spell::Ids GetSpell() const { return static_cast<MMAPI::Spell::Ids>(m_spell_id); }

		/// Prevents the game's cast_spell script from running.
		void Cancel() { m_cancelled = true; }

		/// Returns true if the callback has cancelled this spell cast.
		bool IsCancelled() const { return m_cancelled; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_CAN_CAST_SPELL = "gml_Script_can_cast_spell";
		inline constexpr const char* GML_SCRIPT_CAST_SPELL      = "gml_Script_cast_spell";

		using AfterCanCastSpellCallback = void(*)(MMAPI::Spell::CanCastSpellContext&);
		using BeforeSpellCastCallback    = void(*)(MMAPI::Spell::CastSpellContext&);

		inline AfterCanCastSpellCallback after_can_cast_spell_callback = nullptr;
		inline BeforeSpellCastCallback    before_spell_cast_callback     = nullptr;

		inline YYTK::RValue& GmlScriptCanCastSpellCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CAN_CAST_SPELL)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Spell::CanCastSpellContext context{ Arguments[0]->ToInt64(), Result.ToBoolean() };
				after_can_cast_spell_callback(context);
				Result = context.m_result;
			}

			return Result;
		}

		inline YYTK::RValue& GmlScriptCastSpellCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Spell::CastSpellContext context{ Arguments[0]->ToInt64() };
				before_spell_cast_callback(context);

				if (context.m_cancelled)
					return Result;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_CAST_SPELL)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterCanCastSpellHook(AfterCanCastSpellCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_CAN_CAST_SPELL,
				reinterpret_cast<PVOID>(GmlScriptCanCastSpellCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			after_can_cast_spell_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline Aurie::AurieStatus RegisterSpellCastHook(BeforeSpellCastCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_CAST_SPELL,
				reinterpret_cast<PVOID>(GmlScriptCastSpellCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_spell_cast_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

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
		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return false;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_CAN_CAST_SPELL, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue spell_id = static_cast<int>(spell);
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &spell_id };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		return result.ToBoolean();
	}

	namespace Hooks
	{
		/// Registers a callback that can override the game's can_cast_spell result after it runs.
		/// Use ctx.SetResult(bool) to allow or block casting the spell.
		/// @param callback A function called with the can-cast context after the game evaluates it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus AfterCanCastSpell(Internal::AfterCanCastSpellCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::after_can_cast_spell_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterCanCastSpellHook(callback);
		}

		/// Registers a callback that runs before the game's cast_spell script executes.
		/// Call ctx.Cancel() to prevent the game from processing the spell cast.
		/// @param callback A function called with the cast spell context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeSpellCast(Internal::BeforeSpellCastCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_spell_cast_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterSpellCastHook(callback);
		}
	}
}
