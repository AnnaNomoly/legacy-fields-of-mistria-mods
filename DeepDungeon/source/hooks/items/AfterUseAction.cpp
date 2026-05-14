#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::UI;
using namespace State::Maps;

static void ClearFloorEnchantments()
{
	// Undo Fey
	if (active_floor_enchantments.contains(FloorEnchantments::FEY))
	{
		ModifySpellCosts(true, true);
		MMAPI::StatusEffect::Cancel(MMAPI::StatusEffect::Ids::Fairy);
	}

	// Undo Blessed (Oracle Set Bonus)
	if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] > 0)
	{
		int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
		int adjusted_max_health = max_health - class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED];
		SetMaxHealth(adjusted_max_health);
		int current_health = MMAPI::Player::GetHealth().ToInt64();
		MMAPI::VitalsMenu::SetMaxHealth(adjusted_max_health);
		MMAPI::VitalsMenu::SetHealth(current_health, adjusted_max_health);
		class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = 0;
	}

	// Undo HP Penalty
	if (hp_penalty_amount > -1)
	{
		int max_health = MMAPI::Player::GetMaxHealth().ToInt64();
		int adjusted_max_health = max_health + hp_penalty_amount;
		hp_penalty_amount = -1;
		SetMaxHealth(adjusted_max_health);
		int current_health = MMAPI::Player::GetHealth().ToInt64();
		MMAPI::VitalsMenu::SetMaxHealth(adjusted_max_health);
		MMAPI::VitalsMenu::SetHealth(current_health, adjusted_max_health);
	}

	active_floor_enchantments.clear();
	active_sigils.insert(Sigils::SERENITY);
}

// MMAPI Player::Hooks::AfterUseActionComplete callback. Fires once per rising edge
// of `state == HoldToUse && state.did_action` on obj_ari's FSM, with item_id pinned
// to the consumed item even after the game has removed it from inventory.
//
// Replaces DD's previous per-tick polling in OnObjAriCall::AriProcessUsedItems plus the
// sigil_item_used / greater_sigil_item_used / salve_item_used / lift_key_used / orb_item_used
// / dread_contract_used / heart_crystal_used flags set by BeforeUseItem. Items that don't
// pass BeforeUseItem's restriction checks never reach this callback because ctx.Cancel()
// there prevents the FSM transition.
void AfterUseAction(MMAPI::Player::AfterUseActionContext& ctx)
{
	const int item_id = ctx.GetItemId();
	CInstance* self = ctx.GetSelf();
	CInstance* ari_instance = global_instance->GetRefMember("__ari")->ToInstance();

	if (item_id_to_sigil_map.contains(item_id))
	{
		if (item_id == sigil_to_item_id_map[Sigils::ALTERATION])
		{
			active_sigils.insert(Sigils::ALTERATION);
			sigil_of_alteration_monster_id = SelectRandomMonsterForAlteration();
		}
		else if (item_id == sigil_to_item_id_map[Sigils::CONCEALMENT])
			active_sigils.insert(Sigils::CONCEALMENT);
		else if (item_id == sigil_to_item_id_map[Sigils::FORTIFICATION])
			active_sigils.insert(Sigils::FORTIFICATION);
		else if (item_id == sigil_to_item_id_map[Sigils::FORTUNE])
		{
			active_sigils.insert(Sigils::FORTUNE);
			MMAPI::Dungeon::SpawnLadder(ari_x, ari_y);
		}
		else if (item_id == sigil_to_item_id_map[Sigils::PROTECTION])
		{
			active_sigils.insert(Sigils::PROTECTION);
			MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::GuardiansShield), RValue(), 1, MMAPI::StatusEffect::InfiniteDuration);
			SetInvulnerabilityHits(2);
		}
		else if (item_id == sigil_to_item_id_map[Sigils::RAGE])
			active_sigils.insert(Sigils::RAGE);
		else if (item_id == sigil_to_item_id_map[Sigils::REDEMPTION])
		{
			active_sigils.insert(Sigils::REDEMPTION);
			MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::Fairy), RValue(), 1, MMAPI::StatusEffect::InfiniteDuration);
		}
		else if (item_id == sigil_to_item_id_map[Sigils::SAFETY])
		{
			floor_trap_positions.clear();
			active_sigils.insert(Sigils::SAFETY);
		}
		else if (item_id == sigil_to_item_id_map[Sigils::SERENITY])
			ClearFloorEnchantments();
		else if (item_id == sigil_to_item_id_map[Sigils::SILENCE])
			active_sigils.insert(Sigils::SILENCE);
		else if (item_id == sigil_to_item_id_map[Sigils::STRENGTH])
			active_sigils.insert(Sigils::STRENGTH);
		else if (item_id == sigil_to_item_id_map[Sigils::TEMPTATION])
			active_sigils.insert(Sigils::TEMPTATION);
		else if (item_id == sigil_to_item_id_map[Sigils::SIGHT])
		{
			int unified_time = MMAPI::Calendar::GetUnifiedTime().ToInt64();
			MMAPI::StatusEffect::Internal::RegisterById(static_cast<int>(MMAPI::StatusEffect::Ids::SacredLight), 0, unified_time, unified_time + 18000);
			active_sigils.insert(Sigils::SIGHT);
		}
		else if (item_id == sigil_to_item_id_map[Sigils::INTUITION])
		{
			GenerateTreasureSpot(ari_instance, self);
			active_sigils.insert(Sigils::INTUITION);
		}

		MMAPI::ToolbarMenu::ForceUpdate();
	}
	else if (item_id_to_greater_sigil_map.contains(item_id))
	{
		if (item_id == greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION])
		{
			ClearFloorEnchantments();
			active_greater_sigils.insert(GreaterSigils::BENEDICTION);
			MMAPI::Player::ModifyHealth(999);
		}
		else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW])
		{
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points"))
				{
					RValue monster_id = monster->GetMember("monster_id");
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (IsNumeric(monster_id) && std::isfinite(hit_points) && hit_points > 0)
						*monster->GetRefMember("hit_points") = 0;

					if (monster_id.ToInt64() == static_cast<int>(MMAPI::Monster::Ids::Tome))
					{
						RValue monster_instance_exists = g_ModuleInterface->CallBuiltin("instance_exists", { monster });
						if (monster_instance_exists.ToBoolean())
							g_ModuleInterface->CallBuiltin("instance_destroy", { monster });
					}
				}
			}

			active_greater_sigils.insert(GreaterSigils::ASTRAL_FLOW);
		}
		else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL])
			active_greater_sigils.insert(GreaterSigils::CHAIN_SPELL);
		else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE])
		{
			active_greater_sigils.insert(GreaterSigils::SPIRIT_SURGE);
			MMAPI::Player::ModifyStamina(999);
		}
		else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI])
		{
			active_greater_sigils.insert(GreaterSigils::MEIKYO_SHISUI);
			ScaleMistpoolArmor(true);
			ScaleMistpoolWeapon(true);
			ScaleMistpoolPickaxe(true);
			ScaleClassArmor(true);
		}

		MMAPI::ToolbarMenu::ForceUpdate();
	}
	else if (salve_items.contains(item_id))
	{
		if (item_id == item_name_to_id_map[HEALTH_SALVE_NAME])
			salves_used[HEALTH_SALVE_NAME]++;
		else if (item_id == item_name_to_id_map[STAMINA_SALVE_NAME])
			salves_used[STAMINA_SALVE_NAME]++;
		else if (item_id == item_name_to_id_map[MANA_SALVE_NAME])
			salves_used[MANA_SALVE_NAME]++;

		MMAPI::ToolbarMenu::ForceUpdate();
	}
	else if (lift_key_items.contains(item_id))
	{
		biome_reward_disabled = true;

		if (item_id == item_name_to_id_map[UPPER_MINES_KEY_F5_NAME])
			MMAPI::Dungeon::EnterDungeon(4);
		else if (item_id == item_name_to_id_map[UPPER_MINES_KEY_F10_NAME])
			MMAPI::Dungeon::EnterDungeon(9);
		else if (item_id == item_name_to_id_map[UPPER_MINES_KEY_F15_NAME])
			MMAPI::Dungeon::EnterDungeon(14);
		else if (item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME])
			MMAPI::Dungeon::EnterDungeon(19);
		else if (item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME])
			MMAPI::Dungeon::EnterDungeon(24);
		else if (item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME])
			MMAPI::Dungeon::EnterDungeon(29);
		else if (item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME])
			MMAPI::Dungeon::EnterDungeon(34);
		else if (item_id == item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME])
			MMAPI::Dungeon::EnterDungeon(39);
		else if (item_id == item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME])
			MMAPI::Dungeon::EnterDungeon(44);
		else if (item_id == item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME])
			MMAPI::Dungeon::EnterDungeon(49);
		else if (item_id == item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME])
			MMAPI::Dungeon::EnterDungeon(54);
		else if (item_id == item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME])
			MMAPI::Dungeon::EnterDungeon(59);
		else if (item_id == item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME])
			MMAPI::Dungeon::EnterDungeon(64);
		else if (item_id == item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME])
			MMAPI::Dungeon::EnterDungeon(69);
		else if (item_id == item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME])
			MMAPI::Dungeon::EnterDungeon(74);
		else if (item_id == item_name_to_id_map[RUINS_KEY_F80_NAME])
			MMAPI::Dungeon::EnterDungeon(79);
		else if (item_id == item_name_to_id_map[RUINS_KEY_F85_NAME])
			MMAPI::Dungeon::EnterDungeon(84);
		else if (item_id == item_name_to_id_map[RUINS_KEY_F90_NAME])
			MMAPI::Dungeon::EnterDungeon(89);
		else if (item_id == item_name_to_id_map[RUINS_KEY_F95_NAME])
			MMAPI::Dungeon::EnterDungeon(94);
		else if (item_id == item_name_to_id_map[RUINS_KEY_F100_NAME])
			MMAPI::Dungeon::EnterDungeon(99);
	}
	else if (orb_items.contains(item_id))
	{
		biome_reward_disabled = true;

		// TODO: Other orbs
		if (item_id == item_name_to_id_map[TIDE_CAVERNS_ORB])
		{
			boss_battle = BossBattle::TIDE_CAVERNS_ORB;
			MMAPI::Dungeon::EnterDungeon(19);
		}
		else if (item_id == item_name_to_id_map[DEEP_EARTH_ORB])
		{
			boss_battle = BossBattle::DEEP_EARTH_ORB;
			MMAPI::Dungeon::EnterDungeon(39);
		}
		else if (item_id == item_name_to_id_map[LAVA_CAVES_ORB])
		{
			boss_battle = BossBattle::LAVA_CAVES_ORB;
			MMAPI::Dungeon::EnterDungeon(59);
		}
		else if (item_id == item_name_to_id_map[RUINS_ORB])
		{
			boss_battle = BossBattle::RUINS_ORB;
			MMAPI::Dungeon::EnterDungeon(79);
		}
	}
	else if (dread_contract_items.contains(item_id))
	{
		offering_chance_occurred = true;
		queued_offerings.insert(Offerings::DREAD);
		active_dread_contracts.insert(item_id);
	}
	else if (item_id == item_name_to_id_map["heart_crystal"])
	{
		if (unmodified_base_health != -1)
			unmodified_base_health += 20;
	}
}
