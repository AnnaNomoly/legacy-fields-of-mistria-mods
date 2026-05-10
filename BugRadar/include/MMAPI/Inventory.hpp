#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Inventory
{
	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_DESERIALIZE_INVENTORY = "gml_Script_deserialize@anon@6096@__Inventory@Inventory";
		inline constexpr const char* GML_SCRIPT_COUNT_ITEM            = "gml_Script_item_id_quantity@anon@4106@__Inventory@Inventory";
		inline constexpr const char* GML_SCRIPT_REMOVE_ITEM           = "gml_Script_remove@anon@2021@__Inventory@Inventory";

		inline YYTK::RValue& DeserializeInventoryContextCallback(IN YYTK::CInstance* Self, IN YYTK::CInstance* Other, OUT YYTK::RValue& Result, IN int ArgumentCount, IN YYTK::RValue** Arguments)
		{
			MMAPI::Internal::RegisterScriptContext(GML_SCRIPT_DESERIALIZE_INVENTORY, Self, Other);
			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_DESERIALIZE_INVENTORY));
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}
	}

	/// Activates Inventory utility functions that directly call game scripts.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		return MMAPI::Internal::InstallScriptHook(
			Internal::GML_SCRIPT_DESERIALIZE_INVENTORY,
			reinterpret_cast<PVOID>(Internal::DeserializeInventoryContextCallback)
		);
	}

	/// Counts how many of an item Ari currently has.
	/// @attention Requires MMAPI::Inventory::Enable() to have been called.
	/// @param item_id The item ID to count.
	/// @return The count as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue CountItem(int item_id)
	{
		const auto& refs = MMAPI::Internal::script_reference_map;
		if (!refs.contains(Internal::GML_SCRIPT_DESERIALIZE_INVENTORY))
			return {};
		YYTK::CInstance* Self  = refs.at(Internal::GML_SCRIPT_DESERIALIZE_INVENTORY)[0];
		YYTK::CInstance* Other = refs.at(Internal::GML_SCRIPT_DESERIALIZE_INVENTORY)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_COUNT_ITEM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue item = item_id;
		YYTK::RValue result;
		YYTK::RValue* args[1] = { &item };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
		return result;
	}

	/// Removes a quantity of an item from Ari's inventory.
	/// @attention Requires MMAPI::Inventory::Enable() to have been called.
	/// @param item_id The item ID to remove.
	/// @param quantity The quantity to remove.
	inline void RemoveItem(int item_id, int quantity)
	{
		const auto& refs = MMAPI::Internal::script_reference_map;
		if (!refs.contains(Internal::GML_SCRIPT_DESERIALIZE_INVENTORY))
			return;
		YYTK::CInstance* Self  = refs.at(Internal::GML_SCRIPT_DESERIALIZE_INVENTORY)[0];
		YYTK::CInstance* Other = refs.at(Internal::GML_SCRIPT_DESERIALIZE_INVENTORY)[1];

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_REMOVE_ITEM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue item = item_id;
		YYTK::RValue amount = quantity;
		YYTK::RValue result;
		YYTK::RValue* args[2] = { &item, &amount };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 2, args);
	}
}
