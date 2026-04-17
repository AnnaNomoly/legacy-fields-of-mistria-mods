#include "../../utils/Utils.h"

RValue& GmlScriptUseItemCallback(
	IN CInstance* Self, // Changes depending on the invocation context. For world interactables like a fountain, Self->m_Object->m_Name == "obj_world_fountain". For Ari using an item, Self->m_Object == NULL.
	IN CInstance* Other, // Changes depending on the invocation context. For world interactables like a fountain, Other->m_Object->m_Name == "Game". For Ari using an item, Other->m_Object->m_Name == "obj_ari".
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const bool is_ari_using_item = Self->m_Object == NULL && strstr(Other->m_Object->m_Name, "obj_ari");

	// Orbs
	if (is_ari_using_item && orb_items.contains(held_item_id) && ari_current_gm_room != "rm_mines_entry")
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use an orb at the mines entrance!", MOD_NAME, VERSION);
		CreateNotification(false, ORB_RESTRICTED_NOTIFICATION_KEY, Self, Other);
		return Result;
	}

	// Lift Keys
	if (is_ari_using_item && lift_key_items.contains(held_item_id) && ari_current_gm_room != "rm_mines_entry")
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use a lift key at the mines entrance!", MOD_NAME, VERSION);
		CreateNotification(false, LIFT_KEY_RESTRICTED_NOTIFICATION_KEY, Self, Other);
		return Result;
	}

	// Inhibiting Trap
	if (is_ari_using_item && active_traps.contains(Traps::INHIBITING) && held_item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use the Mistpool Sword due to the Inhibiting Trap's effect!", MOD_NAME, VERSION);
		CreateNotification(false, INHIBITED_PENALTY_NOTIFICATION_KEY, Self, Other);
		return Result;
	}

	// Item Penalty
	if (is_ari_using_item && active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY)
		&& deep_dungeon_items.contains(held_item_id) && held_item_id != sigil_to_item_id_map[Sigils::SERENITY] && held_item_id != item_name_to_id_map[MISTPOOL_SWORD_NAME] && !item_id_to_greater_sigil_map.contains(held_item_id))
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use that item due to the Item Penalty floor enchantment!", MOD_NAME, VERSION);
		CreateNotification(false, ITEM_PENALTY_NOTIFICATION_KEY, Self, Other);
		return Result;
	}

	if (AriCurrentGmRoomIsDungeonFloor())
	{
		if (is_ari_using_item)
		{
			if (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE)
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
			if ((held_item_id == salve_name_to_id_map[HEALTH_SALVE_NAME] && salves_used[HEALTH_SALVE_NAME] >= Config::config.health_salve_limit) || (held_item_id == salve_name_to_id_map[STAMINA_SALVE_NAME] && salves_used[STAMINA_SALVE_NAME] >= Config::config.stamina_salve_limit) || (held_item_id == salve_name_to_id_map[MANA_SALVE_NAME] && salves_used[MANA_SALVE_NAME] >= Config::config.mana_salve_limit))
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
		// Deep Dungeon Exclusive Items
		if (is_ari_using_item && deep_dungeon_items.contains(held_item_id))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You may only use Deep Dungeon specific items inside the dungeon!", MOD_NAME, VERSION);
			CreateNotification(false, ITEM_RESTRICTED_NOTIFICATION_KEY, Self, Other);
			return Result;
		}
	}

	sigil_item_used        = item_id_to_sigil_map.contains(held_item_id);
	greater_sigil_item_used = item_id_to_greater_sigil_map.contains(held_item_id);
	salve_item_used        = salve_items.contains(held_item_id);
	lift_key_used          = lift_key_items.contains(held_item_id);
	orb_item_used          = orb_items.contains(held_item_id);
	heart_crystal_used     = held_item_id == item_name_to_id_map["heart_crystal"];

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_USE_ITEM));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
