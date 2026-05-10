#pragma once

#include "Core.hpp"
#include "Engine.hpp"

#include <string>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::NPC
{
	/// Source: globalInstance.__npc_id__
	enum class Ids : int
	{
		Adeline   = 0,
		Balor     = 1,
		Caldarus  = 2,
		Celine    = 3,
		Darcy     = 4,
		Dell      = 5,
		Dozy      = 6,
		Eiland    = 7,
		Elsie     = 8,
		Errol     = 9,
		Hayden    = 10,
		Hemlock   = 11,
		Henrietta = 12,
		Holt      = 13,
		Josephine = 14,
		Juniper   = 15,
		Landen    = 16,
		Louis     = 17,
		Luc       = 18,
		Maple     = 19,
		March     = 20,
		Merri     = 21,
		Nora      = 22,
		Olric     = 23,
		Reina     = 24,
		Ryis      = 25,
		Seridia   = 26,
		Stillwell = 27,
		Taliferro = 28,
		Terithia  = 29,
		Valen     = 30,
		Vera      = 31,
		Wheedle   = 32,
		Zorel     = 33
	};

	struct HeartPointsChangedContext
	{
		double m_amount = 0.0;

		double GetAmount() const { return m_amount; }
		void SetAmount(double amount) { m_amount = amount; }
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_ADD_HEART_POINTS = "gml_Script_add_heart_points@Npc@Npc";
		inline constexpr const char* GML_SCRIPT_NPC_RECEIVE_GIFT = "gml_Script_receive_gift@gml_Object_par_NPC_Create_0";

		using BeforeHeartPointsChangeCallback = void(*)(MMAPI::NPC::HeartPointsChangedContext&);
		using BeforeReceiveGiftCallback = void(*)(YYTK::CInstance* npc, int item_id);

		inline BeforeHeartPointsChangeCallback before_heart_points_change_callback = nullptr;
		inline BeforeReceiveGiftCallback before_receive_gift_callback = nullptr;

		inline bool TryGetNumericArgument(YYTK::RValue** Arguments, int ArgumentCount, int index, double& value)
		{
			if (!Arguments || index < 0 || index >= ArgumentCount || !Arguments[index])
				return false;

			if (Arguments[index]->m_Kind != YYTK::VALUE_REAL &&
			    Arguments[index]->m_Kind != YYTK::VALUE_INT32 &&
			    Arguments[index]->m_Kind != YYTK::VALUE_INT64)
				return false;

			value = Arguments[index]->ToDouble();
			return true;
		}

		inline void SetNumericArgument(YYTK::RValue** Arguments, int index, double value)
		{
			*Arguments[index] = value;
		}

		inline YYTK::RValue& GmlScriptAddHeartPointsCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			double amount = 0.0;
			if (before_heart_points_change_callback && TryGetNumericArgument(Arguments, ArgumentCount, 0, amount))
			{
				MMAPI::NPC::HeartPointsChangedContext context{ amount };
				before_heart_points_change_callback(context);
				SetNumericArgument(Arguments, 0, context.m_amount);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_ADD_HEART_POINTS
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline Aurie::AurieStatus RegisterHeartPointsChangedHook(BeforeHeartPointsChangeCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_ADD_HEART_POINTS,
				reinterpret_cast<PVOID>(GmlScriptAddHeartPointsCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_heart_points_change_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue& GmlScriptNpcReceiveGiftCallback(
			IN YYTK::CInstance* Self,
			IN YYTK::CInstance* Other,
			OUT YYTK::RValue& Result,
			IN int ArgumentCount,
			IN YYTK::RValue** Arguments
		)
		{
			if (Self && Arguments && ArgumentCount == 1 && Arguments[0] && Arguments[0]->m_Kind == YYTK::VALUE_OBJECT)
			{
				int item_id = static_cast<int>(Arguments[0]->GetMember("item_id").ToInt64());
				if (before_receive_gift_callback)
					before_receive_gift_callback(Self, item_id);
			}

			const auto original = reinterpret_cast<YYTK::PFUNC_YYGMLScript>(
				Aurie::MmGetHookTrampoline(
					MMAPI::Internal::self_module,
					GML_SCRIPT_NPC_RECEIVE_GIFT
				)
			);

			original(Self, Other, Result, ArgumentCount, Arguments);
			return Result;
		}

		inline Aurie::AurieStatus RegisterReceiveGiftHook(BeforeReceiveGiftCallback callback)
		{
			Aurie::AurieStatus status = MMAPI::Internal::InstallScriptHook(
				GML_SCRIPT_NPC_RECEIVE_GIFT,
				reinterpret_cast<PVOID>(GmlScriptNpcReceiveGiftCallback)
			);

			if (!Aurie::AurieSuccess(status))
				return status;

			before_receive_gift_callback = callback;
			return Aurie::AURIE_SUCCESS;
		}

		inline YYTK::RValue GetIdFromInternalName(const std::string& internal_name)
		{
			if (!MMAPI::Internal::global_instance)
				return {};

			YYTK::RValue npc_ids = MMAPI::Internal::global_instance->GetMember("__npc_id__");
			size_t npc_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(npc_ids, npc_count);

			for (size_t i = 0; i < npc_count; i++)
			{
				YYTK::RValue* npc_id = nullptr;
				MMAPI::Internal::module_interface->GetArrayEntry(npc_ids, i, npc_id);

				if (npc_id && npc_id->ToString() == internal_name.c_str())
					return static_cast<double>(i);
			}

			return {};
		}

		inline YYTK::RValue GetNpcDatabase()
		{
			if (!MMAPI::Internal::global_instance)
				return {};

			return *MMAPI::Internal::global_instance->GetRefMember("__npc_database");
		}

		inline YYTK::RValue GetNpcPrototypes()
		{
			if (!MMAPI::Internal::global_instance)
				return {};

			return *MMAPI::Internal::global_instance->GetRefMember("__npc_prototypes");
		}

		inline YYTK::RValue GetNpcPrototype(int npc_id)
		{
			if (npc_id < 0)
				return {};

			YYTK::RValue npc_prototypes = GetNpcPrototypes();
			if (npc_prototypes.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			size_t npc_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(npc_prototypes, npc_count);
			if (static_cast<size_t>(npc_id) >= npc_count)
				return {};

			YYTK::RValue* npc_prototype = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(npc_prototypes, static_cast<size_t>(npc_id), npc_prototype);
			if (!npc_prototype)
				return {};

			return *npc_prototype;
		}

		inline YYTK::RValue GetData(int npc_id)
		{
			if (npc_id < 0)
				return {};

			YYTK::RValue npc_database = GetNpcDatabase();
			if (npc_database.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			size_t npc_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(npc_database, npc_count);
			if (static_cast<size_t>(npc_id) >= npc_count)
				return {};

			YYTK::RValue* npc = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(npc_database, static_cast<size_t>(npc_id), npc);
			if (!npc)
				return {};

			return *npc;
		}

		inline YYTK::RValue GetGiftBuffer(int npc_id, const char* gift_member)
		{
			YYTK::RValue npc_prototype = GetNpcPrototype(npc_id);
			if (npc_prototype.m_Kind == YYTK::VALUE_UNDEFINED)
				return {};

			if (!MMAPI::Engine::StructVariableExists(npc_prototype, gift_member))
				return {};

			YYTK::RValue gifts = npc_prototype.GetMember(gift_member);
			if (!MMAPI::Engine::StructVariableExists(gifts, "__buffer"))
				return {};

			return gifts.GetMember("__buffer");
		}

		inline bool GiftBufferContains(YYTK::RValue gift_buffer, int item_id)
		{
			if (gift_buffer.m_Kind == YYTK::VALUE_UNDEFINED)
				return false;

			size_t gift_count = 0;
			MMAPI::Internal::module_interface->GetArraySize(gift_buffer, gift_count);

			for (size_t i = 0; i < gift_count; i++)
			{
				YYTK::RValue* gift = nullptr;
				MMAPI::Internal::module_interface->GetArrayEntry(gift_buffer, i, gift);

				if (gift && gift->ToInt64() == item_id)
					return true;
			}

			return false;
		}

		inline YYTK::RValue GetKnownGiftPreferences(YYTK::CInstance* npc)
		{
			if (!npc)
				return {};

			YYTK::RValue npc_value = npc->ToRValue();
			if (!MMAPI::Engine::StructVariableExists(npc_value, "me"))
				return {};

			YYTK::RValue me = npc_value.GetMember("me");
			if (!MMAPI::Engine::StructVariableExists(me, "known_gift_preferences"))
				return {};

			YYTK::RValue known_gift_preferences = me.GetMember("known_gift_preferences");
			if (!MMAPI::Engine::StructVariableExists(known_gift_preferences, "inner"))
				return {};

			return known_gift_preferences.GetMember("inner");
		}
	}

	/// Gets the internal numeric ID for an NPC.
	/// @param npc The NPC to convert.
	/// @return The NPC's internal numeric ID.
	inline int GetId(MMAPI::NPC::Ids npc)
	{
		return static_cast<int>(npc);
	}

	/// Gets the NPC database entry for an NPC.
	/// @param npc The NPC to read.
	/// @return The NPC data struct as an RValue, or undefined if the ID is out of bounds.
	inline YYTK::RValue GetData(MMAPI::NPC::Ids npc)
	{
		return Internal::GetData(GetId(npc));
	}

	/// Gets the item IDs this NPC likes as gifts.
	/// @param npc The NPC to read.
	/// @return The liked gift item ID buffer as an RValue, or undefined if the NPC is not found.
	inline YYTK::RValue GetLikedGifts(MMAPI::NPC::Ids npc)
	{
		return Internal::GetGiftBuffer(GetId(npc), "liked_gifts");
	}

	/// Gets the item IDs this NPC loves as gifts.
	/// @param npc The NPC to read.
	/// @return The loved gift item ID buffer as an RValue, or undefined if the NPC is not found.
	inline YYTK::RValue GetLovedGifts(MMAPI::NPC::Ids npc)
	{
		return Internal::GetGiftBuffer(GetId(npc), "loved_gifts");
	}

	/// Returns true if the NPC likes the given item as a gift.
	/// @param npc The NPC to check.
	/// @param item_id The item ID to check.
	inline bool LikesGift(MMAPI::NPC::Ids npc, int item_id)
	{
		return Internal::GiftBufferContains(GetLikedGifts(npc), item_id);
	}

	/// Returns true if the NPC loves the given item as a gift.
	/// @param npc The NPC to check.
	/// @param item_id The item ID to check.
	inline bool LovesGift(MMAPI::NPC::Ids npc, int item_id)
	{
		return Internal::GiftBufferContains(GetLovedGifts(npc), item_id);
	}

	/// Returns true if the live NPC instance knows the given gift preference.
	/// @param npc The live NPC instance.
	/// @param item_id The item ID to check.
	inline bool KnowsGiftPreference(YYTK::CInstance* npc, int item_id)
	{
		YYTK::RValue known_gift_preferences = Internal::GetKnownGiftPreferences(npc);
		if (known_gift_preferences.m_Kind == YYTK::VALUE_UNDEFINED)
			return false;

		return MMAPI::Internal::module_interface->CallBuiltin("struct_exists", { known_gift_preferences, item_id }).ToBoolean();
	}

	/// Marks the given gift preference as learned on the live NPC instance.
	/// @param npc The live NPC instance.
	/// @param item_id The item ID to mark as learned.
	inline void LearnGiftPreference(YYTK::CInstance* npc, int item_id)
	{
		YYTK::RValue known_gift_preferences = Internal::GetKnownGiftPreferences(npc);
		if (known_gift_preferences.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(known_gift_preferences, item_id, 0.0);
	}

	/// Gets an NPC's heart points.
	/// @param npc The NPC to read.
	/// @return The NPC's heart points, or 0 if the NPC is not found.
	inline int GetHeartPoints(MMAPI::NPC::Ids npc)
	{
		YYTK::RValue npc_data = GetData(npc);
		if (npc_data.m_Kind == YYTK::VALUE_UNDEFINED)
			return 0;

		return static_cast<int>(npc_data.GetMember("heart_points").ToInt64());
	}

	/// Sets an NPC's heart points.
	/// @param npc The NPC to modify.
	/// @param value The heart point value to assign.
	inline void SetHeartPoints(MMAPI::NPC::Ids npc, int value)
	{
		YYTK::RValue npc_data = GetData(npc);
		if (npc_data.m_Kind == YYTK::VALUE_UNDEFINED)
			return;

		MMAPI::Engine::StructVariableSet(npc_data, "heart_points", value);
	}

	/// Adjusts an NPC's heart points.
	/// @param npc The NPC to modify.
	/// @param amount The signed amount to add to the NPC's heart points.
	inline void ModifyHeartPoints(MMAPI::NPC::Ids npc, int amount)
	{
		SetHeartPoints(npc, GetHeartPoints(npc) + amount);
	}

	namespace Hooks
	{
		/// Registers a callback that runs when an NPC's heart points change through the game's add heart points script.
		/// @param callback A function called with a mutable heart point change context. Use ctx.SetAmount() to modify the amount applied.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeHeartPointsChange(Internal::BeforeHeartPointsChangeCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_heart_points_change_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterHeartPointsChangedHook(callback);
		}

		/// Registers a callback that runs when an NPC receives a gift.
		/// @param callback A function called with the live NPC instance and received item ID.
		/// @return AURIE_SUCCESS if the hook was installed; AURIE_OBJECT_ALREADY_EXISTS if a callback is already registered; otherwise the Aurie failure status.
		inline Aurie::AurieStatus BeforeReceiveGift(Internal::BeforeReceiveGiftCallback callback)
		{
			if (!callback)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::before_receive_gift_callback)
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			return Internal::RegisterReceiveGiftHook(callback);
		}
	}
}
