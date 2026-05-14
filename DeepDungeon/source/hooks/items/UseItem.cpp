#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

// MMAPI Item::Hooks::BeforeUseItem callback.
void BeforeUseItem(MMAPI::Item::UseItemContext& ctx)
{
	const bool is_ari_using_item = ctx.IsAriUse();

	// Orbs
	if (is_ari_using_item && orb_items.contains(held_item_id) && ari_current_gm_room != "rm_mines_entry")
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use an orb at the mines entrance!", MOD_NAME, MOD_VERSION);
		MMAPI::Game::CreateNotification(false, ORB_RESTRICTED_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
	}

	// Lift Keys
	if (is_ari_using_item && lift_key_items.contains(held_item_id) && ari_current_gm_room != "rm_mines_entry")
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are only allowed to use a lift key at the mines entrance!");
		MMAPI::Game::CreateNotification(false, LIFT_KEY_RESTRICTED_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
	}

	// Inhibiting Trap
	if (is_ari_using_item && active_traps.contains(Traps::INHIBITING) && held_item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use the Mistpool Sword due to the Inhibiting Trap's effect!");
		MMAPI::Game::CreateNotification(false, INHIBITED_PENALTY_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
	}

	// Item Penalty
	if (is_ari_using_item && active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY)
		&& deep_dungeon_items.contains(held_item_id) && held_item_id != sigil_to_item_id_map[Sigils::SERENITY] && held_item_id != item_name_to_id_map[MISTPOOL_SWORD_NAME] && !item_id_to_greater_sigil_map.contains(held_item_id))
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use that item due to the Item Penalty floor enchantment!");
		MMAPI::Game::CreateNotification(false, ITEM_PENALTY_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
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
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use sigils during boss battles!");
					MMAPI::Game::CreateNotification(false, SIGIL_RESTRICTED_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Great Sigils Restricted
				if (item_id_to_greater_sigil_map.contains(held_item_id))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use lost scrolls during boss battles!");
					MMAPI::Game::CreateNotification(false, GREATER_SIGIL_RESTRICTED_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Dread Contracts Restricted
				if (dread_contract_items.contains(held_item_id))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You are unable to use dread contracts during boss battles!");
					MMAPI::Game::CreateNotification(false, DREAD_CONTRACT_RESTRICTED_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}
			}
			else
			{
				// Sigil Already Used
				if (item_id_to_sigil_map.contains(held_item_id) && active_sigils.contains(item_id_to_sigil_map[held_item_id]))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!");
					MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Greater Sigil Already Used
				if (item_id_to_greater_sigil_map.contains(held_item_id) && !active_greater_sigils.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - A lost scroll has already been used!");
					MMAPI::Game::CreateNotification(false, GREATER_SIGIL_LIMIT_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Dread Contract Already Used
				if (dread_contract_items.contains(held_item_id) && (!active_dread_contracts.empty() || !queued_offerings.empty()))
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - A dread contract has already been used!");
					MMAPI::Game::CreateNotification(false, DREAD_CONTRACT_LIMIT_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Dread Contract Biome Check
				if (dread_contract_items.contains(held_item_id))
				{
					std::string biome = floor_number_to_biome_name_map[floor_number];
					if ((biome == "Upper Mines" && held_item_id != item_name_to_id_map[UPPER_MINES_DREAD_CONTRACT]) ||
						(biome == "The Tide Caverns" && held_item_id != item_name_to_id_map[TIDE_CAVERNS_DREAD_CONTRACT]) ||
						(biome == "Deep Earth" && held_item_id != item_name_to_id_map[DEEP_EARTH_DREAD_CONTRACT]) ||
						(biome == "The Lava Caves" && held_item_id != item_name_to_id_map[LAVA_CAVES_DREAD_CONTRACT]) ||
						(biome == "Ancient Ruins" && held_item_id != item_name_to_id_map[RUINS_DREAD_CONTRACT]))
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You can't use that dread contract in this biome!");
						MMAPI::Game::CreateNotification(false, DREAD_CONTRACT_BIOME_NOTIFICATION_KEY);
						ctx.Cancel();
						return;
					}
				}

				// Protection Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::PROTECTION] && GetInvulnerabilityHits() > 0)
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!");
					MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Redemption Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::REDEMPTION] && FairyBuffIsActive())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!");
					MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}

				// Temptation Already Active
				if (held_item_id == sigil_to_item_id_map[Sigils::TEMPTATION] && !queued_offerings.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That sigil is already active!");
					MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
					ctx.Cancel();
					return;
				}
			}

			// Salve Limits
			if ((held_item_id == item_name_to_id_map[HEALTH_SALVE_NAME] && salves_used[HEALTH_SALVE_NAME] >= Config::config.health_salve_limit) || (held_item_id == item_name_to_id_map[STAMINA_SALVE_NAME] && salves_used[STAMINA_SALVE_NAME] >= Config::config.stamina_salve_limit) || (held_item_id == item_name_to_id_map[MANA_SALVE_NAME] && salves_used[MANA_SALVE_NAME] >= Config::config.mana_salve_limit))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You have already used too many of that salve on the current floor!");
				MMAPI::Game::CreateNotification(false, SALVE_LIMIT_NOTIFICATION_KEY);
				ctx.Cancel();
				return;
			}

			// Dungeon's Curse
			if (!deep_dungeon_items.contains(held_item_id) && restricted_items.contains(held_item_id))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - That item is prohibited in the Deep Dungeon!");
				MMAPI::Game::CreateNotification(false, ITEM_PROHIBITED_NOTIFICATION_KEY);
				ctx.Cancel();
				return;
			}
		}
	}
	else
	{
		// Deep Dungeon Exclusive Items
		if (is_ari_using_item && deep_dungeon_items.contains(held_item_id))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - You may only use Deep Dungeon specific items inside the dungeon!");
			MMAPI::Game::CreateNotification(false, ITEM_RESTRICTED_NOTIFICATION_KEY);
			ctx.Cancel();
			return;
		}
	}

}
