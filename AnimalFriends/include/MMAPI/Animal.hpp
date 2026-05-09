#pragma once

#include "Core.hpp"
#include "Engine.hpp"
#include "Instance.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Animal
{
	/// Source: globalInstance.__animal_xp
	/// Struct-backed enum; values follow alphabetical key order.
	enum class XpValues : int
	{
		Breed     = 0,
		Feed      = 1,
		GainHeart = 2,
		GoOutside = 3,
		Pet       = 4
	};

	namespace Internal
	{
		inline constexpr const char* GML_SCRIPT_GET_ALL_ANIMALS = "gml_Script_get_all_animals";
		inline constexpr const char* GML_SCRIPT_BELL_IN         = "gml_Script_bell_in@gml_Object_obj_farm_bell_Create_0";
		inline constexpr const char* GML_SCRIPT_BELL_OUT        = "gml_Script_bell_out@gml_Object_obj_farm_bell_Create_0";
		inline constexpr const char* GML_SCRIPT_CREATE_ANIMAL_CURRENCY_DANCE =
			"gml_Script_create_animal_currency_dance@gml_Object_obj_player_animal_Create_0";

		inline void SpawnShinyBeads(YYTK::CInstance* Self, YYTK::CInstance* Other, int amount)
		{
			YYTK::CScript* gml_script = nullptr;
			MMAPI::Internal::module_interface->GetNamedRoutinePointer(
				GML_SCRIPT_CREATE_ANIMAL_CURRENCY_DANCE,
				reinterpret_cast<PVOID*>(&gml_script)
			);

			YYTK::RValue num_beads = amount;
			YYTK::RValue spawn_beads = true;
			YYTK::RValue result;
			YYTK::RValue* args[2] = { &num_beads, &spawn_beads };
			gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 2, args);
		}

		inline bool TryGetAriContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			const auto& refs = MMAPI::Internal::instance_reference_map;
			if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
				return false;

			Self = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[0];
			Other = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];
			return true;
		}

		inline constexpr const char* ToGameKey(MMAPI::Animal::XpValues value)
		{
			switch (value)
			{
				case MMAPI::Animal::XpValues::Breed:
					return "breed";
				case MMAPI::Animal::XpValues::Feed:
					return "feed";
				case MMAPI::Animal::XpValues::GainHeart:
					return "gain_heart";
				case MMAPI::Animal::XpValues::GoOutside:
					return "go_outside";
				case MMAPI::Animal::XpValues::Pet:
					return "pet";
				default:
					return nullptr;
			}
		}
	}

	/// Returns the game's array-like collection of all player animals.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return The animal collection as an RValue, or an empty RValue if the required context is unavailable.
	inline YYTK::RValue GetAllAnimals()
	{
		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_GET_ALL_ANIMALS, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::CInstance* Self = nullptr;
		YYTK::CInstance* Other = nullptr;
		if (!Internal::TryGetAriContext(Self, Other))
			return {};

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(Self, Other, result, 0, nullptr);
		return result;
	}

	/// Returns the number of player animals.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @return The animal count, or 0 if the required context is unavailable.
	inline int GetAnimalCount()
	{
		YYTK::RValue all_animals = GetAllAnimals();
		if (all_animals.m_Kind != YYTK::VALUE_OBJECT)
			return 0;

		YYTK::RValue buffer = all_animals.GetMember("__buffer");
		if (buffer.m_Kind != YYTK::VALUE_ARRAY)
			return 0;

		size_t size = 0;
		MMAPI::Internal::module_interface->GetArraySize(buffer, size);
		return static_cast<int>(size);
	}

	/// Gets an animal XP value from MMAPI::Internal::global_instance.__animal_xp.
	/// @param xp_value The animal XP value to read.
	/// @return The XP value as an RValue, or undefined if unavailable.
	inline YYTK::RValue GetXpValue(MMAPI::Animal::XpValues xp_value)
	{
		if (!MMAPI::Internal::global_instance)
			return {};

		const char* xp_value_key = Internal::ToGameKey(xp_value);
		if (!xp_value_key)
			return {};

		YYTK::RValue animal_xp = MMAPI::Internal::global_instance->GetMember("__animal_xp");
		if (animal_xp.m_Kind == YYTK::VALUE_UNDEFINED)
			return {};

		return animal_xp.GetMember(xp_value_key);
	}

	/// Gets an animal XP value, falling back to a caller-provided default if unavailable.
	/// @param xp_value The animal XP value to read.
	/// @param default_value The value to return if the game data cannot be read.
	/// @return The animal XP value, or default_value if unavailable.
	inline int GetXpValueOrDefault(MMAPI::Animal::XpValues xp_value, int default_value)
	{
		YYTK::RValue xp = GetXpValue(xp_value);
		if (xp.m_Kind == YYTK::VALUE_UNDEFINED)
			return default_value;

		return static_cast<int>(xp.ToInt64());
	}

	/// Returns true if the animal has been pet today.
	inline bool HasBeenPet(YYTK::RValue animal)
	{
		return MMAPI::Engine::StructVariableGet(animal, "has_been_pat").ToBoolean();
	}

	/// Sets whether the animal has been pet today.
	inline void SetHasBeenPet(YYTK::RValue animal, bool value)
	{
		MMAPI::Engine::StructVariableSet(animal, "has_been_pat", value);
	}

	/// Returns true if the animal has eaten today.
	inline bool HasEaten(YYTK::RValue animal)
	{
		return MMAPI::Engine::StructVariableGet(animal, "has_eaten").ToBoolean();
	}

	/// Sets whether the animal has eaten today.
	inline void SetHasEaten(YYTK::RValue animal, bool value)
	{
		MMAPI::Engine::StructVariableSet(animal, "has_eaten", value);
	}

	/// Returns the animal's current heart points.
	inline int GetHeartPoints(YYTK::RValue animal)
	{
		return static_cast<int>(MMAPI::Engine::StructVariableGet(animal, "heart_points").ToInt64());
	}

	/// Sets the animal's heart points.
	inline void SetHeartPoints(YYTK::RValue animal, int value)
	{
		MMAPI::Engine::StructVariableSet(animal, "heart_points", value);
	}

	/// Adjusts the animal's heart points by the given signed amount.
	inline void ModifyHeartPoints(YYTK::RValue animal, int amount)
	{
		SetHeartPoints(animal, GetHeartPoints(animal) + amount);
	}

	/// Rings a farm bell to bring animals inside.
	/// @param farm_bell The obj_farm_bell instance to ring.
	inline void RingBellIn(YYTK::CInstance* farm_bell)
	{
		if (!farm_bell)
			return;

		YYTK::RValue bell = farm_bell->ToRValue();
		if (!MMAPI::Engine::StructVariableExists(bell, "bell_in"))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_BELL_IN, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(farm_bell, farm_bell, result, 0, nullptr);
	}

	/// Rings a farm bell to send animals outside.
	/// @param farm_bell The obj_farm_bell instance to ring.
	inline void RingBellOut(YYTK::CInstance* farm_bell)
	{
		if (!farm_bell)
			return;

		YYTK::RValue bell = farm_bell->ToRValue();
		if (!MMAPI::Engine::StructVariableExists(bell, "bell_out"))
			return;

		YYTK::CScript* gml_script = nullptr;
		MMAPI::Internal::module_interface->GetNamedRoutinePointer(Internal::GML_SCRIPT_BELL_OUT, reinterpret_cast<PVOID*>(&gml_script));

		YYTK::RValue result;
		gml_script->m_Functions->m_ScriptFunction(farm_bell, farm_bell, result, 0, nullptr);
	}

	/// Spawns shiny beads from an animal interaction.
	/// @attention Requires MMAPI::Instance::Internal::INSTANCE_OBJ_ARI to be registered via RegisterInstanceContext.
	/// @param animal The live animal instance to spawn beads from.
	/// @param amount The number of beads to spawn. Clamped to [1, 999].
	inline void SpawnShinyBeads(YYTK::CInstance* animal, int amount)
	{
		const auto& refs = MMAPI::Internal::instance_reference_map;
		if (!refs.contains(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI))
			return;

		if (amount <= 0)
			amount = 1;
		if (amount > 999)
			amount = 999;

		YYTK::CInstance* ari = refs.at(MMAPI::Instance::Internal::INSTANCE_OBJ_ARI)[1];
		Internal::SpawnShinyBeads(animal, ari, amount);
	}
}
