#pragma once

#include "Core.hpp"
#include "Hook.hpp"
#include "Log.hpp"
#include "Status.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Inventory
{
	namespace Internal
	{
		inline bool enabled = false;

		inline constexpr const char* GML_SCRIPT_DESERIALIZE_INVENTORY = "gml_Script_deserialize@anon@6096@__Inventory@Inventory";
		inline constexpr const char* GML_SCRIPT_COUNT_ITEM            = "gml_Script_item_id_quantity@anon@4106@__Inventory@Inventory";
		inline constexpr const char* GML_SCRIPT_REMOVE_ITEM           = "gml_Script_remove@anon@2021@__Inventory@Inventory";

		// Live Inventory Self/Other, latched from the deserialize hook.
		// Used by TryGetInventoryContext for callers outside any hook frame.
		inline YYTK::CInstance* inventory_self  = nullptr;
		inline YYTK::CInstance* inventory_other = nullptr;

		// Cleared from the setup_main_screen pub/sub when the player returns to the title menu.
		// Registered by Inventory::Enable().
		inline void ClearInventoryOnReturnToTitle(YYTK::CInstance* /*Self*/, YYTK::CInstance* /*Other*/)
		{
			inventory_self  = nullptr;
			inventory_other = nullptr;
		}

		inline YYTK::RValue& DeserializeInventoryContextCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			// Refresh on every fire so we always have the freshest Inventory pair.
			inventory_self  = Self;
			inventory_other = Other;

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_DESERIALIZE_INVENTORY)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		/// Resolves the Inventory's GML calling context, latched from the most recent deserialize call.
		/// Cleared automatically when the game returns to the title menu.
		/// @return True if an Inventory deserialize has been observed this session, false otherwise.
		inline bool TryGetInventoryContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			if (!inventory_self)
				return false;
			Self  = inventory_self;
			Other = inventory_other;
			return true;
		}
	}

	/// Activates Inventory utility functions. Installs the deserialize hook so the live Inventory Self/Other are latched
	/// for TryGetInventoryContext (cleared on return-to-title via the setup_main_screen pub/sub).
	/// @return Status::Success if the hooks are installed (or already were); otherwise a failure status.
	inline MMAPI::Status Enable()
	{
		if (Internal::enabled)
			return MMAPI::Status::Success;

		MMAPI::Log::Debug("MMAPI::Inventory::Enable() called");

		MMAPI::Internal::RegisterOnSetupMainScreenHandler(Internal::ClearInventoryOnReturnToTitle);

		MMAPI::Status status = MMAPI::Internal::InstallScriptHooks({
			{ MMAPI::Internal::GML_SCRIPT_SETUP_MAIN_SCREEN, reinterpret_cast<PVOID>(MMAPI::Internal::GmlScriptBeforeSetupMainScreenCallback) },
			{ Internal::GML_SCRIPT_DESERIALIZE_INVENTORY,    reinterpret_cast<PVOID>(Internal::DeserializeInventoryContextCallback) },
		});
		if (!MMAPI::IsSuccess(status))
			return status;

		Internal::enabled = true;
		return MMAPI::Status::Success;
	}

	/// Counts how many of an item Ari currently has.
	/// @attention Requires MMAPI::Inventory::Enable() to have been called.
	/// @param item_id The item ID to count.
	/// @return The count as an RValue, or undefined if the required context is unavailable.
	inline YYTK::RValue CountItem(int item_id)
	{
		MMAPI_REQUIRE_ENABLED("Inventory", {});

		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetInventoryContext(Self, Other))
			return {};

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
		MMAPI_REQUIRE_ENABLED_VOID("Inventory");

		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetInventoryContext(Self, Other))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_REMOVE_ITEM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue item = item_id;
		YYTK::RValue amount = quantity;
		YYTK::RValue result;
		YYTK::RValue* args[2] = { &item, &amount };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 2, args);
	}
}
