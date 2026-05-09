#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Infusion.hpp"

#include <map>
#include <string>
#include <vector>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Equipment
{
	namespace Internal
	{
		inline YYTK::RValue GetArmorSlots()
		{
			YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
			YYTK::RValue armor = ari.GetMember("armor");
			YYTK::RValue slots = armor.GetMember("slots");
			return slots.GetMember("__buffer");
		}
	}

	/// Gets the live item structs currently equipped in Ari's armor slots.
	/// @return A vector containing the equipped armor item structs.
	inline std::vector<YYTK::RValue> GetEquippedArmor()
	{
		std::vector<YYTK::RValue> equipped_armor;
		YYTK::RValue buffer = Internal::GetArmorSlots();

		size_t slot_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(buffer, slot_count);

		for (size_t i = 0; i < slot_count; i++)
		{
			YYTK::RValue* slot = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(buffer, i, slot);

			if (!slot || !MMAPI::Engine::StructVariableExists(*slot, "item"))
				continue;

			YYTK::RValue item = slot->GetMember("item");
			if (item.m_Kind == YYTK::VALUE_OBJECT)
				equipped_armor.push_back(item);
		}

		return equipped_armor;
	}

	/// Gets the internal item names for Ari's equipped armor.
	/// @return A vector of internal item recipe keys.
	inline std::vector<std::string> GetEquippedArmorInternalNames()
	{
		std::vector<std::string> internal_names;

		for (YYTK::RValue item : GetEquippedArmor())
		{
			if (!MMAPI::Engine::StructVariableExists(item, "prototype"))
				continue;

			YYTK::RValue prototype = item.GetMember("prototype");
			if (MMAPI::Engine::StructVariableExists(prototype, "recipe_key"))
				internal_names.push_back(prototype.GetMember("recipe_key").ToString());
		}

		return internal_names;
	}

	/// Counts infusions found on Ari's equipped armor.
	/// @return A map from infusion ID to count.
	inline std::map<MMAPI::Infusion::Ids, int> GetEquippedArmorInfusions()
	{
		std::map<MMAPI::Infusion::Ids, int> infusions;

		for (YYTK::RValue item : GetEquippedArmor())
		{
			if (!MMAPI::Engine::StructVariableExists(item, "infusion"))
				continue;

			YYTK::RValue infusion = item.GetMember("infusion");
			if (!MMAPI::Engine::IsNumeric(infusion))
				continue;

			int infusion_id = static_cast<int>(infusion.ToInt64());
			if (infusion_id < 0 || infusion_id > static_cast<int>(MMAPI::Infusion::Ids::VenomSword))
				continue;

			infusions[static_cast<MMAPI::Infusion::Ids>(infusion_id)]++;
		}

		return infusions;
	}
}
