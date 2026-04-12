#include "../utils/Utils.h"

RValue& GmlScriptUseItemCallback(
	IN CInstance* Self, // Changes depending on the invocation context. For world interactables like a fountain, Self->m_Object->m_Name == "obj_world_fountain". For Ari using an item, Self->m_Object == NULL.
	IN CInstance* Other, // Changes depending on the invocation context. For world interactables like a fountain, Other->m_Object->m_Name == "Game". For Ari using an item, Other->m_Object->m_Name == "obj_ari".
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	// Orbs
	if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari") && orb_items.contains(held_item_id))
	{
		if (ari_current_gm_room != "rm_mines_entry")
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use an orb at the mines entrance!", MOD_NAME, VERSION);
			CreateNotification(false, ORB_RESTRICTED_NOTIFICATION_KEY, Self, Other);
			return Result;
		}
	}

	// Lift Keys
	if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari") && lift_key_items.contains(held_item_id))
	{
		if (ari_current_gm_room != "rm_mines_entry")
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use a lift key at the mines entrance!", MOD_NAME, VERSION);
			CreateNotification(false, LIFT_KEY_RESTRICTED_NOTIFICATION_KEY, Self, Other);
			return Result;
		}
	}

	// Inhibiting Trap
	if (active_traps.contains(Traps::INHIBITING))
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			if (held_item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use the Mistpool Sword due to the Inhibiting Trap's effect!", MOD_NAME, VERSION);
				CreateNotification(false, INHIBITED_PENALTY_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}

	// Item Penalty
	if (active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY))
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			if (deep_dungeon_items.contains(held_item_id) && held_item_id != sigil_to_item_id_map[Sigils::SERENITY] && held_item_id != item_name_to_id_map[MISTPOOL_SWORD_NAME] && !item_id_to_greater_sigil_map.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use that item due to the Item Penalty floor enchantment!", MOD_NAME, VERSION);
				CreateNotification(false, ITEM_PENALTY_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}

	if (AriCurrentGmRoomIsDungeonFloor())
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			if (configuration.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
			{
				// Sigil Items Restricted
				if (item_id_to_sigil_map.contains(held_item_id))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use sigils during boss battles!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_RESTRICTED_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Great Sigils Restricted
				if (item_id_to_greater_sigil_map.contains(held_item_id))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use lost scrolls during boss battles!", MOD_NAME, VERSION);
					CreateNotification(false, GREATER_SIGIL_RESTRICTED_NOTIFICATION_KEY, Self, Other);
					return Result;
				}
			}
			else
			{
				// Sigil Already Used
				if (item_id_to_sigil_map.contains(held_item_id) && active_sigils.contains(item_id_to_sigil_map[held_item_id]))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Greater Sigil Already Used
				if (item_id_to_greater_sigil_map.contains(held_item_id) && !active_greater_sigils.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - A lost scroll has already been used!", MOD_NAME, VERSION);
					CreateNotification(false, GREATER_SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Protection Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::PROTECTION] && GetInvulnerabilityHits() > 0)
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Redemption Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::REDEMPTION] && FairyBuffIsActive())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}

				// Condemn (Oracle Set Bonus)
				if (held_item_id == sigil_to_item_id_map[Sigils::TEMPTATION] && class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0)
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!", MOD_NAME, VERSION);
					CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY, Self, Other);
					return Result;
				}
			}

			// Salve Limits
			if ((held_item_id == salve_name_to_id_map[HEALTH_SALVE_NAME] && salves_used[HEALTH_SALVE_NAME] >= configuration.health_salve_limit) || (held_item_id == salve_name_to_id_map[STAMINA_SALVE_NAME] && salves_used[STAMINA_SALVE_NAME] >= configuration.stamina_salve_limit) || (held_item_id == salve_name_to_id_map[MANA_SALVE_NAME] && salves_used[MANA_SALVE_NAME] >= configuration.mana_salve_limit))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You have already used too many of that salve on the current floor!", MOD_NAME, VERSION);
				CreateNotification(false, SALVE_LIMIT_NOTIFICATION_KEY, Self, Other);
				return Result;
			}

			// Dungeon's Curse
			if (!deep_dungeon_items.contains(held_item_id) && restricted_items.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That item is prohibited in the Deep Dungeon!", MOD_NAME, VERSION);
				CreateNotification(false, ITEM_PROHIBITED_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}
	else
	{
		if (Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari"))
		{
			// Deep Dungeon Exclusive Items
			if (deep_dungeon_items.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You may only use Deep Dungeon specific items inside the dungeon!", MOD_NAME, VERSION);
				CreateNotification(false, ITEM_RESTRICTED_NOTIFICATION_KEY, Self, Other);
				return Result;
			}
		}
	}

	// Sigil Item
	sigil_item_used = false;
	if (item_id_to_sigil_map.contains(held_item_id))
		sigil_item_used = true;

	// Greater Sigil Item
	greater_sigil_item_used = false;
	if (item_id_to_greater_sigil_map.contains(held_item_id))
		greater_sigil_item_used = true;

	// Salve Item
	salve_item_used = false;
	if (salve_items.contains(held_item_id))
		salve_item_used = true;

	// Lift Key Item
	lift_key_used = false;
	if (lift_key_items.contains(held_item_id))
		lift_key_used = true;

	// Orb Item
	orb_item_used = false;
	if (orb_items.contains(held_item_id))
		orb_item_used = true;

	// Heart Crystal
	heart_crystal_used = false;
	if (held_item_id == item_name_to_id_map["heart_crystal"])
		heart_crystal_used = true;

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_USE_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptHeldItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_HELD_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Result.m_Kind != VALUE_UNDEFINED)
	{
		int item_id = Result.GetMember("item_id").ToInt64();
		if (held_item_id != item_id)
			held_item_id = item_id;
	}

	return Result;
}

RValue& GmlScriptDropItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
		script_name_to_reference_map[GML_SCRIPT_DROP_ITEM] = { Self, Other };

	if (ari_current_gm_room.contains("rm_mines"))
	{
		bool chance_to_spawn_glowstone = false;

		if (Arguments[0]->m_Kind == VALUE_ARRAY)
		{
			size_t array_length;
			g_ModuleInterface->GetArraySize(*Arguments[0], array_length);

			for (size_t i = 0; i < array_length; i++)
			{
				RValue* array_element;
				g_ModuleInterface->GetArrayEntry(*Arguments[0], i, array_element);

				if (StructVariableExists(*array_element, "item_id"))
				{
					int item_id = array_element->GetMember("item_id").ToInt64();
					if (item_id == item_name_to_id_map["ore_stone"] && ItemHasBeenAcquired(item_id))
						chance_to_spawn_glowstone = true;
				}
			}
		}
		else if (Arguments[0]->m_Kind == VALUE_INT64 && Arguments[0]->ToInt64() == item_name_to_id_map["ore_stone"] && ItemHasBeenAcquired(Arguments[0]->ToInt64()))
			chance_to_spawn_glowstone = true;

		// TODO: Should there be some RNG for dropping glowstone?
		if (chance_to_spawn_glowstone)
		{
			if (floor_number < 20) // Upper Mines
				DropItem(item_name_to_id_map["glow_stone_tiny"], ari_x, ari_y, Self, Other);
			else if (floor_number < 40) // Tide Caverns
				DropItem(item_name_to_id_map["glow_stone_small"], ari_x, ari_y, Self, Other);
			else if (floor_number < 60) // Deep Earth
				DropItem(item_name_to_id_map["glow_stone_medium"], ari_x, ari_y, Self, Other);
			else if (floor_number < 80) // Lava Caves
				DropItem(item_name_to_id_map["glow_stone_large"], ari_x, ari_y, Self, Other);
			else if (floor_number < 100) // Ruins
				DropItem(item_name_to_id_map["glow_stone_giant"], ari_x, ari_y, Self, Other);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DROP_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptDeserializeLiveItemCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DESERIALIZE_LIVE_ITEM))
		script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_LIVE_ITEM] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DESERIALIZE_LIVE_ITEM));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptDeserializeInventoryCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_DESERIALIZE_INVENTORY))
		script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DESERIALIZE_INVENTORY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetTreasureFromDistributionCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (Self != nullptr && StructVariableExists(Self, "object_id"))
	{
		int object_id = Self->GetMember("object_id").ToInt64();
		if (object_id_to_name_map.contains(object_id))
		{
			std::string object_name = object_id_to_name_map[object_id];
			if (DUNGEON_TREASURE_CHEST_NAMES.contains(object_name) && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
				GenerateTreasureChestLoot(object_name, Self, Other);
		}
	}
	else if (Other != nullptr && StructVariableExists(Other, "object_id"))
	{
		int object_id = Other->GetMember("object_id").ToInt64();
		if (object_id_to_name_map.contains(object_id))
		{
			std::string object_name = object_id_to_name_map[object_id];
			if (DUNGEON_TREASURE_CHEST_NAMES.contains(object_name) && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
				GenerateTreasureChestLoot(object_name, Self, Other);
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptGetUiIconCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_ITEM_UI_ICON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (Self != nullptr && !crafting_menu_open)
	{
		RValue self = Self->ToRValue();
		if (StructVariableExists(self, "item_id"))
		{
			int item_id = self.GetMember("item_id").ToInt64();

			bool modify_icon = false;
			if (deep_dungeon_items.contains(item_id))
				modify_icon = true;
			else if (item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME] || item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME] || item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME] || item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME] || item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME])
				modify_icon = true;
			else if (item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME])
				modify_icon = true;

			if (modify_icon)
				Result = GetDynamicItemSprite(item_id);
		}
	}

	return Result;
}
