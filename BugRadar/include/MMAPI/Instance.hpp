#pragma once

#include "Core.hpp"

#include <map>
#include <string>
#include <string_view>

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Instance
{
	enum class Objects : int
	{
		// Player
		Ari,

		// World
		AssetObject,
		Bug,
		Door,
		FarmBell,
		Fish,
		WorldFountain,

		// Monsters
		Monster,
		MonsterMimic,

		// Dungeon
		Dragonshrine,
		DungeonElevator,
		DungeonLadderDown,
		DungeonRitualAltar,

		// NPCs (matches NPC::Ids ordering)
		Adeline,
		Balor,
		Caldarus,
		Celine,
		Darcy,
		Dell,
		Dozy,
		Eiland,
		Elsie,
		Errol,
		Hayden,
		Hemlock,
		Henrietta,
		Holt,
		Josephine,
		Juniper,
		Landen,
		Louis,
		Luc,
		Maple,
		March,
		Merri,
		Nora,
		Olric,
		Reina,
		Ryis,
		Seridia,
		Stillwell,
		Taliferro,
		Terithia,
		Valen,
		Vera,
		Wheedle,
		Zorel
	};

	using OnObjectCallCallback = void(*)(YYTK::CInstance* self);

	/// Returns true if the instance's GML object name exactly matches object_name.
	/// @param instance The YYTK instance to check.
	/// @param object_name The exact GML object name to match.
	inline bool IsNamed(YYTK::CInstance* instance, std::string_view object_name)
	{
		return instance &&
		       instance->m_Object &&
		       instance->m_Object->m_Name &&
		       std::string_view(instance->m_Object->m_Name) == object_name;
	}

	namespace Internal
	{
		inline constexpr const char* INSTANCE_OBJ_ARI = "obj_ari";

		inline constexpr const char* ToObjectName(MMAPI::Instance::Objects obj)
		{
			switch (obj)
			{
				case MMAPI::Instance::Objects::Ari:                return "obj_ari";
				case MMAPI::Instance::Objects::AssetObject:        return "obj_assetobject";
				case MMAPI::Instance::Objects::Bug:                return "obj_bug";
				case MMAPI::Instance::Objects::Door:               return "obj_door";
				case MMAPI::Instance::Objects::FarmBell:           return "obj_farm_bell";
				case MMAPI::Instance::Objects::Fish:               return "obj_fish";
				case MMAPI::Instance::Objects::WorldFountain:      return "obj_world_fountain";
				case MMAPI::Instance::Objects::Monster:            return "obj_monster";
				case MMAPI::Instance::Objects::MonsterMimic:       return "obj_monster_mimic";
				case MMAPI::Instance::Objects::Dragonshrine:       return "obj_dragonshrine";
				case MMAPI::Instance::Objects::DungeonElevator:    return "obj_dungeon_elevator";
				case MMAPI::Instance::Objects::DungeonLadderDown:  return "obj_dungeon_ladder_down";
				case MMAPI::Instance::Objects::DungeonRitualAltar: return "obj_dungeon_ritual_altar";
				case MMAPI::Instance::Objects::Adeline:            return "obj_adeline";
				case MMAPI::Instance::Objects::Balor:              return "obj_balor";
				case MMAPI::Instance::Objects::Caldarus:           return "obj_caldarus";
				case MMAPI::Instance::Objects::Celine:             return "obj_celine";
				case MMAPI::Instance::Objects::Darcy:              return "obj_darcy";
				case MMAPI::Instance::Objects::Dell:               return "obj_dell";
				case MMAPI::Instance::Objects::Dozy:               return "obj_dozy";
				case MMAPI::Instance::Objects::Eiland:             return "obj_eiland";
				case MMAPI::Instance::Objects::Elsie:              return "obj_elsie";
				case MMAPI::Instance::Objects::Errol:              return "obj_errol";
				case MMAPI::Instance::Objects::Hayden:             return "obj_hayden";
				case MMAPI::Instance::Objects::Hemlock:            return "obj_hemlock";
				case MMAPI::Instance::Objects::Henrietta:          return "obj_henrietta";
				case MMAPI::Instance::Objects::Holt:               return "obj_holt";
				case MMAPI::Instance::Objects::Josephine:          return "obj_josephine";
				case MMAPI::Instance::Objects::Juniper:            return "obj_juniper";
				case MMAPI::Instance::Objects::Landen:             return "obj_landen";
				case MMAPI::Instance::Objects::Louis:              return "obj_louis";
				case MMAPI::Instance::Objects::Luc:                return "obj_luc";
				case MMAPI::Instance::Objects::Maple:              return "obj_maple";
				case MMAPI::Instance::Objects::March:              return "obj_march";
				case MMAPI::Instance::Objects::Merri:              return "obj_merri";
				case MMAPI::Instance::Objects::Nora:               return "obj_nora";
				case MMAPI::Instance::Objects::Olric:              return "obj_olric";
				case MMAPI::Instance::Objects::Reina:              return "obj_reina";
				case MMAPI::Instance::Objects::Ryis:               return "obj_ryis";
				case MMAPI::Instance::Objects::Seridia:            return "obj_seridia";
				case MMAPI::Instance::Objects::Stillwell:          return "obj_stillwell";
				case MMAPI::Instance::Objects::Taliferro:          return "obj_taliferro";
				case MMAPI::Instance::Objects::Terithia:           return "obj_terithia";
				case MMAPI::Instance::Objects::Valen:              return "obj_valen";
				case MMAPI::Instance::Objects::Vera:               return "obj_vera";
				case MMAPI::Instance::Objects::Wheedle:            return "obj_wheedle";
				case MMAPI::Instance::Objects::Zorel:              return "obj_zorel";
				default:                                           return nullptr;
			}
		}

		inline std::map<std::string, OnObjectCallCallback> object_call_callbacks;
		inline bool object_dispatcher_installed = false;

		inline bool IsGamePaused()
		{
			return MMAPI::Internal::global_instance->GetRefMember("__pause_status")->m_i64 > 0;
		}

		/// Resolves Ari's GML calling context: Self = the global __ari struct, Other = the live obj_ari instance.
		/// @return True if both pointers were resolved, false if Instance::Enable() hasn't captured a tick yet.
		inline bool TryGetAriContext(YYTK::CInstance*& Self, YYTK::CInstance*& Other)
		{
			const auto& refs = MMAPI::Internal::instance_reference_map;
			if (!refs.contains(INSTANCE_OBJ_ARI))
				return false;
			Self  = MMAPI::Internal::global_instance->GetRefMember("__ari")->ToInstance();
			Other = refs.at(INSTANCE_OBJ_ARI)[0];
			return true;
		}

		inline void ObjectCallbackDispatcher(IN YYTK::FWCodeEvent& CodeEvent)
		{
			auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

			if (!self || !self->m_Object || !self->m_Object->m_Name || IsGamePaused())
				return;

			if (IsNamed(self, "obj_ari") &&
			    !MMAPI::Internal::instance_reference_map.contains(INSTANCE_OBJ_ARI))
			{
				MMAPI::Internal::instance_reference_map[INSTANCE_OBJ_ARI] = { self };
			}

			for (const auto& [registered_name, callback] : object_call_callbacks)
			{
				if (IsNamed(self, registered_name))
				{
					callback(self);
					return;
				}
			}
		}
	}

	/// Activates the EVENT_OBJECT_CALL dispatcher used by Instance hooks and other modules' Enable().
	/// @return AURIE_SUCCESS if the dispatcher is installed (or already was); otherwise the Aurie failure status.
	inline Aurie::AurieStatus Enable()
	{
		if (Internal::object_dispatcher_installed)
			return Aurie::AURIE_SUCCESS;

		Aurie::AurieStatus status = MMAPI::Internal::module_interface->CreateCallback(
			MMAPI::Internal::self_module,
			YYTK::EVENT_OBJECT_CALL,
			Internal::ObjectCallbackDispatcher,
			0
		);

		if (Aurie::AurieSuccess(status))
			Internal::object_dispatcher_installed = true;

		return status;
	}

	namespace Hooks
	{
		/// Registers a callback that runs once per object tick when the GML object name contains the given fragment.
		/// Skips paused-game ticks automatically.
		/// @attention Requires MMAPI::Instance::Enable() to have been called.
		/// @param object_name The GML object name fragment to match (e.g. "obj_farm_bell").
		/// @param callback A function called with the live instance on each tick.
		/// @return AURIE_SUCCESS if the callback was registered; AURIE_OBJECT_ALREADY_EXISTS if a callback for this name is already registered; otherwise AURIE_INVALID_PARAMETER.
		inline Aurie::AurieStatus OnObjectCall(const char* object_name, MMAPI::Instance::OnObjectCallCallback callback)
		{
			if (!callback || !object_name)
				return Aurie::AURIE_INVALID_PARAMETER;

			if (Internal::object_call_callbacks.contains(object_name))
				return Aurie::AURIE_OBJECT_ALREADY_EXISTS;

			Aurie::AurieStatus status = MMAPI::Instance::Enable();
			if (!Aurie::AurieSuccess(status))
				return status;

			Internal::object_call_callbacks[object_name] = callback;
			return Aurie::AURIE_SUCCESS;
		}

		/// Registers a callback that runs once per object tick for the given known game object.
		/// Skips paused-game ticks automatically.
		/// @attention Requires MMAPI::Instance::Enable() to have been called.
		/// @param object The well-known game object to subscribe to.
		/// @param callback A function called with the live instance on each tick.
		/// @return AURIE_SUCCESS if the callback was registered; AURIE_OBJECT_ALREADY_EXISTS if a callback for this object is already registered; otherwise AURIE_INVALID_PARAMETER.
		inline Aurie::AurieStatus OnObjectCall(MMAPI::Instance::Objects object, MMAPI::Instance::OnObjectCallCallback callback)
		{
			return OnObjectCall(Internal::ToObjectName(object), callback);
		}
	}
}
