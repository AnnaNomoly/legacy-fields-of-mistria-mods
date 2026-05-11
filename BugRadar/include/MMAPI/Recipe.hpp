#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"
#include "Item.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Recipe
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_UNLOCK_RECIPE = "gml_Script_unlock_recipe@Ari@Ari";

		inline YYTK::RValue GetRecipeUnlocks()
		{
			YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
			if (ari.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			return ari.GetMember("recipe_unlocks");
		}

		inline YYTK::RValue GetRecipeComponent(int item_id, size_t component_index)
		{
			YYTK::RValue item = MMAPI::Item::GetItemData(item_id);
			if (item.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			if (!MMAPI::Engine::StructVariableExists(item, "recipe"))
				return {};

			YYTK::RValue recipe = item.GetMember("recipe");
			if (!MMAPI::Engine::StructVariableExists(recipe, "components"))
				return {};

			YYTK::RValue components = recipe.GetMember("components");
			if (!MMAPI::Engine::StructVariableExists(components, "__buffer"))
				return {};

			YYTK::RValue buffer = components.GetMember("__buffer");

			size_t component_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(buffer, component_count);
			if (component_index >= component_count)
				return {};

			YYTK::RValue* component = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(buffer, component_index, component);

			if (!component)
				return {};

			return *component;
		}
	}

	/// Activates Recipe utility functions that resolve Ari context.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Instance::Enable();
	}

	/// Returns true if Ari has unlocked the recipe for the given item ID.
	/// @param item_id The item ID for the recipe to check.
	inline bool IsUnlocked(int item_id)
	{
		if (item_id < 0)
			return false;

		YYTK::RValue recipe_unlocks = Internal::GetRecipeUnlocks();
		if (recipe_unlocks.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		size_t unlock_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(recipe_unlocks, unlock_count);
		if (static_cast<size_t>(item_id) >= unlock_count)
			return false;

		YYTK::RValue* recipe_unlocked = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(recipe_unlocks, static_cast<size_t>(item_id), recipe_unlocked);
		if (!recipe_unlocked)
			return false;

		return recipe_unlocked->ToDouble() != 0.0;
	}

	/// Unlocks a cooking recipe by item ID.
	/// @attention When show_popup is true, requires MMAPI::Recipe::Enable() to have been called; if the required
	/// context is unavailable, the recipe is still unlocked but the popup is skipped.
	/// @param item_id The item ID for the recipe to unlock.
	/// @param show_popup When true, displays the game's recipe unlocked popup if the recipe was newly unlocked.
	/// @return True if the recipe was newly unlocked; otherwise false.
	inline bool Unlock(int item_id, bool show_popup = true)
	{
		if (item_id < 0)
			return false;

		YYTK::RValue recipe_unlocks = Internal::GetRecipeUnlocks();
		if (recipe_unlocks.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		size_t unlock_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(recipe_unlocks, unlock_count);
		if (static_cast<size_t>(item_id) >= unlock_count)
			return false;

		if (IsUnlocked(item_id))
			return false;

		recipe_unlocks[item_id] = 1.0;

		if (show_popup)
		{
			YYTK::CInstance* Self  = nullptr;
			YYTK::CInstance* Other = nullptr;
			if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
				return true;

			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_UNLOCK_RECIPE, reinterpret_cast<PVOID*>(&gml_script));

			YYTK::RValue id = static_cast<double>(item_id);
			YYTK::RValue result;
			YYTK::RValue* args[1] = { &id };
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		}

		return true;
	}

	/// Sets the required count for a recipe component.
	/// @param item_id The item ID whose recipe should be modified.
	/// @param component_index The index of the recipe component to modify.
	/// @param count The new required component count.
	inline void SetComponentCount(int item_id, size_t component_index, int count)
	{
		YYTK::RValue component = Internal::GetRecipeComponent(item_id, component_index);
		if (component.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(component, "count", count);
	}

	/// Sets the crafting duration for a recipe component.
	/// @param item_id The item ID whose recipe should be modified.
	/// @param component_index The index of the recipe component to modify.
	/// @param duration The new component duration value.
	inline void SetComponentDuration(int item_id, size_t component_index, int duration)
	{
		YYTK::RValue component = Internal::GetRecipeComponent(item_id, component_index);
		if (component.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(component, "duration", duration);
	}
}
