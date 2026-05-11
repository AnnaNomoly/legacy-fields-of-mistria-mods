#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"
#include "Text.hpp"

#include <map>
#include <string>
#include <vector>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Item
{
	struct UseItemContext
	{
		int               m_item_id   = -1;
		YYTK::CInstance*  m_self      = nullptr;
		YYTK::CInstance*  m_other     = nullptr;
		bool              m_cancelled = false;

		int GetItemId() const { return m_item_id; }
		YYTK::CInstance* GetSelf() const { return m_self; }
		YYTK::CInstance* GetOther() const { return m_other; }
		void Cancel() { m_cancelled = true; }
		bool IsCancelled() const { return m_cancelled; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_DESERIALIZE_LIVE_ITEM = "gml_Script_deserialize_live_item";
		inline constexpr const char* GML_SCRIPT_DROP_ITEM             = "gml_Script_drop_item";
		inline constexpr const char* GML_SCRIPT_USE_ITEM              = "gml_Script_use_item";

		using BeforeUseItemCallback = void(*)(MMAPI::Item::UseItemContext&);

		inline BeforeUseItemCallback before_use_item_callback = nullptr;

		inline YYTK::RValue GetItemData()
		{
			return MMAPI::Internal::global_instance->GetMember("__item_data");
		}

		inline YYTK::RValue GetItemData(int item_id)
		{
			if (item_id < 0)
				return {};

			YYTK::RValue item_data = GetItemData();

			size_t item_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(item_data, item_count);

			if (static_cast<size_t>(item_id) >= item_count)
				return {};

			YYTK::RValue* item = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(item_data, static_cast<size_t>(item_id), item);

			return *item;
		}

		inline YYTK::RValue GetItemValue(int item_id)
		{
			YYTK::RValue item = GetItemData(item_id);
			if (item.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			if (!MMAPI::Engine::StructVariableExists(item, "value"))
				return {};

			return item.GetMember("value");
		}

		inline YYTK::RValue DeserializeLiveItem(YYTK::CInstance* Self, YYTK::CInstance* Other)
		{
			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(GML_SCRIPT_DESERIALIZE_LIVE_ITEM, reinterpret_cast<PVOID*>(&gml_script));

			std::map<std::string, YYTK::RValue> live_item_data = {
				{ "cosmetic", YYTK::RValue() },
				{ "item_id", YYTK::RValue("sword_scrap_metal") },
				{ "infusion", YYTK::RValue() },
				{ "animal_cosmetic", YYTK::RValue() },
				{ "date_photo", YYTK::RValue() },
				{ "inner_item", YYTK::RValue() },
				{ "gold_to_gain", YYTK::RValue() },
				{ "auto_use", false },
				{ "pet_cosmetic_set_name", YYTK::RValue() }
			};

			YYTK::RValue input = live_item_data;
			YYTK::RValue result;
			YYTK::RValue* args[1] = { &input };
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 1, args);
			return result;
		}

		inline YYTK::RValue& GmlScriptUseItemCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (before_use_item_callback && Arguments && ArgumentCount >= 1 && Arguments[0])
			{
				MMAPI::Item::UseItemContext context{
					static_cast<int>(Arguments[0]->GetMember("item_id").ToInt64()),
					Self,
					Other
				};
				before_use_item_callback(context);

				if (context.m_cancelled)
					return Result;
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(MMAPI::Internal::self_module, GML_SCRIPT_USE_ITEM)
			);
			original(Self, Other, Result, ArgumentCount, Arguments);

			return Result;
		}

		inline Aurie::AurieStatus RegisterUseItemHook(BeforeUseItemCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_USE_ITEM,
				reinterpret_cast<PVOID>(GmlScriptUseItemCallback)
			);
			if (!Aurie::AurieSuccess(status))
				return status;

			before_use_item_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

	}

	/// Returns the game's `__item_data` array containing every item's struct definition.
	/// @return The full item data collection as an RValue, or undefined if MMAPI has not been initialized.
	inline YYTK::RValue GetItemData()
	{
		return Internal::GetItemData();
	}

	/// Returns the struct definition for a single item.
	/// @param item_id The item ID to read.
	/// @return The item struct as an RValue, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetItemData(int item_id)
	{
		return Internal::GetItemData(item_id);
	}

	/// Gets the maximum stack size for an item.
	/// @param item_id The item ID to read.
	/// @return The max stack size as an RValue, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetMaxStack(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("max_stack");
	}

	/// Gets the shipping bin sale value for an item.
	/// @param item_id The item ID to read.
	/// @return The bin value as an RValue, or undefined if the item ID is out of bounds or has no bin value.
	inline YYTK::RValue GetBinValue(int item_id)
	{
		YYTK::RValue value = Internal::GetItemValue(item_id);
		if (value.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		if (!MMAPI::Engine::StructVariableExists(value, "bin"))
			return {};

		return value.GetMember("bin");
	}

	/// Sets the shipping bin sale value for an item.
	/// @param item_id The item ID to modify.
	/// @param bin_value The new bin sale value.
	inline void SetBinValue(int item_id, int bin_value)
	{
		YYTK::RValue value = Internal::GetItemValue(item_id);
		if (value.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(value, "bin", bin_value);
	}

	/// Gets the store purchase price for an item.
	/// @param item_id The item ID to read.
	/// @return The store value as an RValue, or undefined if the item ID is out of bounds or has no store value.
	inline YYTK::RValue GetStoreValue(int item_id)
	{
		YYTK::RValue value = Internal::GetItemValue(item_id);
		if (value.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		if (!MMAPI::Engine::StructVariableExists(value, "store"))
			return {};

		return value.GetMember("store");
	}

	/// Sets the store purchase price for an item.
	/// @param item_id The item ID to modify.
	/// @param store_value The new store price.
	inline void SetStoreValue(int item_id, int store_value)
	{
		YYTK::RValue value = Internal::GetItemValue(item_id);
		if (value.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(value, "store", store_value);
	}

	/// Gets the renown value awarded when an item is sold in the shipping bin.
	/// @param item_id The item ID to read.
	/// @return The renown value as an RValue, or undefined if the item ID is out of bounds or has no renown value.
	inline YYTK::RValue GetRenownValue(int item_id)
	{
		YYTK::RValue value = Internal::GetItemValue(item_id);
		if (value.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		if (!MMAPI::Engine::StructVariableExists(value, "renown"))
			return {};

		return value.GetMember("renown");
	}

	/// Gets the internal recipe key string for an item.
	/// @param item_id The item ID to read.
	/// @return The recipe key as an RValue string, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetInternalName(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("recipe_key");
	}

	/// Gets the localization key for an item's display name.
	/// @param item_id The item ID to read.
	/// @return The localization key as an RValue string, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetLocalizationKey(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("name_key");
	}

	/// Gets the localized display name for an item.
	/// @attention Requires MMAPI::Item::Enable() to have been called.
	/// @param item_id The item ID to read.
	/// @return The localized display name as an RValue string, or undefined if the required context is unavailable.
	inline YYTK::RValue GetLocalizedName(int item_id)
	{
		YYTK::RValue name_key = GetLocalizationKey(item_id);
		if (name_key.m_Kind != YYTK::VALUE_STRING)
			return {};

		return MMAPI::Text::GetLocalizedString(name_key.ToString());
	}

	/// Gets an item ID from its internal recipe key string.
	/// @param internal_name The recipe key to look up (e.g. "sword_scrap_metal").
	/// @return The item ID as an RValue, or undefined if no item with that recipe key exists.
	inline YYTK::RValue GetIdFromInternalName(std::string internal_name)
	{
		YYTK::RValue item_data = Internal::GetItemData();

		size_t item_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(item_data, item_count);

		for (size_t i = 0; i < item_count; i++)
		{
			YYTK::RValue* item = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(item_data, i, item);

			if (item->GetMember("recipe_key").ToString() == internal_name)
				return static_cast<int64_t>(i);
		}

		return {};
	}

	/// Returns true if the item has the given tag in its item data.
	/// @param item_id The item ID to check.
	/// @param tag The tag string to search for.
	inline bool HasTag(int item_id, const std::string& tag)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		if (!MMAPI::Engine::StructVariableExists(item, "tags"))
			return false;

		YYTK::RValue tags = item.GetMember("tags");
		if (!MMAPI::Engine::StructVariableExists(tags, "__buffer"))
			return false;

		YYTK::RValue buffer = tags.GetMember("__buffer");
		size_t tag_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(buffer, tag_count);

		for (size_t i = 0; i < tag_count; i++)
		{
			YYTK::RValue* item_tag = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(buffer, i, item_tag);

			if (item_tag && item_tag->ToString() == tag)
				return true;
		}

		return false;
	}

	/// Gets all item IDs whose item data contains the given tag.
	/// @param tag The item tag to search for.
	/// @return A vector containing every matching item ID.
	inline std::vector<int> GetIdsWithTag(const std::string& tag)
	{
		std::vector<int> item_ids;

		YYTK::RValue item_data = Internal::GetItemData();

		size_t item_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(item_data, item_count);

		for (size_t i = 0; i < item_count; i++)
		{
			if (HasTag(static_cast<int>(i), tag))
				item_ids.push_back(static_cast<int>(i));
		}

		return item_ids;
	}

	/// Gets the stamina modifier for an item.
	/// @param item_id The item ID to read.
	/// @return The stamina modifier as an RValue, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetStaminaModifier(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("stamina_modifier");
	}

	/// Sets the stamina modifier for an item.
	/// @param item_id The item ID to modify.
	/// @param stamina_modifier The new stamina modifier value.
	inline void SetStaminaModifier(int item_id, double stamina_modifier)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(item, "stamina_modifier", stamina_modifier);
	}

	/// Gets the health modifier for an item.
	/// @param item_id The item ID to read.
	/// @return The health modifier as an RValue, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetHealthModifier(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("health_modifier");
	}

	/// Sets the health modifier for an item.
	/// @param item_id The item ID to modify.
	/// @param health_modifier The new health modifier value.
	inline void SetHealthModifier(int item_id, double health_modifier)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(item, "health_modifier", health_modifier);
	}

	/// Gets the mana modifier for an item.
	/// @param item_id The item ID to read.
	/// @return The mana modifier as an RValue, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetManaModifier(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("mana_modifier");
	}

	/// Sets the mana modifier for an item.
	/// @param item_id The item ID to modify.
	/// @param mana_modifier The new mana modifier value.
	inline void SetManaModifier(int item_id, double mana_modifier)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(item, "mana_modifier", mana_modifier);
	}

	/// Gets the damage value for an item.
	/// @param item_id The item ID to read.
	/// @return The damage value as an RValue, or undefined if the item ID is out of bounds.
	inline YYTK::RValue GetDamage(int item_id)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return item.GetMember("damage");
	}

	/// Sets the damage value for an item.
	/// @param item_id The item ID to modify.
	/// @param damage The new damage value.
	inline void SetDamage(int item_id, double damage)
	{
		YYTK::RValue item = Internal::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(item, "damage", damage);
	}

	/// Returns true if the item has been donated to the museum.
	/// @param item_id The item ID to check.
	inline bool HasBeenDonated(int item_id)
	{
		if (item_id < 0)
			return false;

		YYTK::RValue museum_progress_data = MMAPI::Internal::global_instance->GetMember("__museum_progress_data");

		size_t item_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(museum_progress_data, item_count);
		if (static_cast<size_t>(item_id) >= item_count)
			return false;

		YYTK::RValue* item_donated = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(museum_progress_data, static_cast<size_t>(item_id), item_donated);
		return item_donated->ToBoolean();
	}

	/// Returns true if Ari has ever acquired the item (tracked in __ari.items_acquired).
	/// @param item_id The item ID to check.
	inline bool HasBeenAcquired(int item_id)
	{
		if (item_id < 0)
			return false;

		YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
		YYTK::RValue items_acquired = ari.GetMember("items_acquired");

		size_t item_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(items_acquired, item_count);
		if (static_cast<size_t>(item_id) >= item_count)
			return false;

		YYTK::RValue* item_acquired = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(items_acquired, static_cast<size_t>(item_id), item_acquired);
		return item_acquired->ToBoolean();
	}

	/// Activates Item utility functions. Cascades to MMAPI::Instance::Enable so Item::Drop can resolve Ari's
	/// GML calling context, and to MMAPI::Text::Enable so Item::GetLocalizedName can resolve the Localizer.
	/// @return AURIE_SUCCESS if the hooks are installed (or already were); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		Aurie::AurieStatus status = MMAPI::Instance::Enable();
		if (!Aurie::AurieSuccess(status))
			return status;
		return MMAPI::Text::Enable();
	}

	/// Drops an item at the given room coordinates.
	/// @attention Requires MMAPI::Item::Enable() to have been called.
	/// @param item_id The item ID to drop.
	/// @param x The room x coordinate.
	/// @param y The room y coordinate.
	inline void Drop(int item_id, double x, double y)
	{
		if (item_id < 0)
			return;

		YYTK::CInstance* Self  = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!MMAPI::Instance::Internal::TryGetAriContext(Self, Other))
			return;

		YYTK::RValue item = Internal::DeserializeLiveItem(Self, Other);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		YYTK::RValue item_data = Internal::GetItemData(item_id);
		if (item_data.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		*item.GetRefMember("prototype") = item_data;
		*item.GetRefMember("item_id") = item_id;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_DROP_ITEM, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue x_value = x;
		YYTK::RValue y_value = y;
		YYTK::RValue undefined;
		YYTK::RValue result;
		YYTK::RValue* args[4] = { &item, &x_value, &y_value, &undefined };
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 4, args);
	}

	namespace Hooks
	{
		/// Registers a callback that runs before the game processes an item use.
		/// Call ctx.Cancel() to prevent the game from processing the item use.
		/// @param callback A function called with a mutable use context before the game processes it.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeUseItem(Internal::BeforeUseItemCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_use_item_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Item::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			return Internal::RegisterUseItemHook(callback);
		}
	}
}
