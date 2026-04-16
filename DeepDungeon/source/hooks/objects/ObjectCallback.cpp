#include "../../utils/Utils.h"
#include "../../patterns/MonsterPatterns.h"

// The complete ObjectCallback function, verbatim from source
// Function signature: void ObjectCallback(IN FWCodeEvent& CodeEvent)

void ObjectCallback(
	IN FWCodeEvent& CodeEvent
)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self)
		return;

	if (!self->m_Object)
		return;

	if (strstr(self->m_Object->m_Name, "obj_ari"))
	{
		if (!script_name_to_reference_map.contains("obj_ari"))
			script_name_to_reference_map["obj_ari"] = { global_instance->GetRefMember("__ari")->ToInstance(), self };

		RValue x;
		g_ModuleInterface->GetBuiltin("x", self, NULL_INDEX, x);
		ari_x = x.ToDouble();

		RValue y;
		g_ModuleInterface->GetBuiltin("y", self, NULL_INDEX, y);
		ari_y = y.ToDouble();

		// Progression Mode Biome Cleared Rewards
		if (Config::config.disable_dungeon_lift && drop_biome_reward && ari_x != 0 && ari_y != 0 && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
		{
			// Upper Mines
			if (ari_current_gm_room == "rm_water_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_CHESTPIECE_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Tide Caverns
			else if (ari_current_gm_room == "rm_earth_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_HELMET_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Deep Earth
			else if (ari_current_gm_room == "rm_fire_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_GLOVES_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Lava Caves
			else if (ari_current_gm_room == "rm_ruins_seal")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_PANTS_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[RUINS_KEY_F80_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
			// Ruins
			else if (ari_current_gm_room == "rm_seridias_chamber")
			{
				drop_biome_reward = false;
				DropItem(item_name_to_id_map[CURSED_BOOTS_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
				DropItem(item_name_to_id_map[RUINS_KEY_F100_NAME], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
			}
		}

		// Floor Traps
		if (active_traps.contains(Traps::EXPLODING))
		{
			// Apply damage to Ari
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			int penalty = current_health * Config::config.exploding_trap_current_health_damage_percent / 100;
			SetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, current_health - penalty);

			// Apply damage to monsters
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "move"))
				{
					RValue monster_x;
					RValue monster_y;
					g_ModuleInterface->GetBuiltin("x", monster, NULL_INDEX, monster_x);
					g_ModuleInterface->GetBuiltin("y", monster, NULL_INDEX, monster_y);

					double distance = GetDistance(active_traps[Traps::EXPLODING].first, active_traps[Traps::EXPLODING].second, monster_x.ToInt64(), monster_y.ToInt64());
					if (distance <= 32)
					{
						double hit_points = monster->GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							int monster_hp_penalty = std::trunc(hit_points * Config::config.exploding_trap_current_health_damage_percent / 100);
							*monster->GetRefMember("hit_points") = max(0, hit_points - monster_hp_penalty);

							if (StructVariableExists(monster, "monster_id"))
							{
								RValue monster_id = monster->GetMember("monster_id");
								if (IsNumeric(monster_id))
									g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - The Exploding Trap damaged the monster: %s", MOD_NAME, VERSION, monster_id_to_name_map[monster_id.ToInt64()].c_str());
							}
						}
					}
				}
			}

			active_traps.erase(Traps::EXPLODING);
		}

		ApplyOfferingPenalties(global_instance->GetRefMember("__ari")->ToInstance(), self);

		// Process used items.
		RValue ari = self->ToRValue();
		if (StructVariableExists(ari, "fsm"))
		{
			RValue fsm = ari.GetMember("fsm");

			if (StructVariableExists(fsm, "state"))
			{
				RValue state = fsm.GetMember("state");
				if (StructVariableExists(state, "state_id"))
				{
					RValue state_id = state.GetMember("state_id");
					if (state_id.ToInt64() == player_state_to_id_map["hold_to_use"])
					{
						if (StructVariableExists(state, "did_action"))
						{
							RValue did_action = state.GetMember("did_action");
							if (did_action.ToBoolean())
							{
								if (sigil_item_used) // Necessary since did_action==true will get called a few times when the item is used.
								{
									sigil_item_used = false;

									if (held_item_id == sigil_to_item_id_map[Sigils::ALTERATION])
									{
										active_sigils.insert(Sigils::ALTERATION);
										sigil_of_alteration_monster_id = SelectRandomMonsterForAlteration();
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::CONCEALMENT])
										active_sigils.insert(Sigils::CONCEALMENT);
									if (held_item_id == sigil_to_item_id_map[Sigils::FORTIFICATION])
										active_sigils.insert(Sigils::FORTIFICATION);
									if (held_item_id == sigil_to_item_id_map[Sigils::FORTUNE])
									{
										active_sigils.insert(Sigils::FORTUNE);
										SpawnLadder(global_instance->GetRefMember("__ari")->ToInstance(), self, ari_x, ari_y);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::PROTECTION])
									{
										std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

										active_sigils.insert(Sigils::PROTECTION);
										RegisterStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["guardians_shield"], RValue(), 1, 2147483647.0);
										SetInvulnerabilityHits(2);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::RAGE])
										active_sigils.insert(Sigils::RAGE);
									if (held_item_id == sigil_to_item_id_map[Sigils::REDEMPTION])
									{
										std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

										active_sigils.insert(Sigils::REDEMPTION);
										RegisterStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"], RValue(), 1, 2147483647.0);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::SAFETY])
									{
										floor_trap_positions.clear();
										active_sigils.insert(Sigils::SAFETY);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::SERENITY])
									{
										// Undo Fey
										if (active_floor_enchantments.contains(FloorEnchantments::FEY))
										{
											std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

											ModifySpellCosts(true, true);
											CancelStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"]);
										}

										// Undo Blessed (Oracle Set Bonus)
										if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] > 0)
										{
											int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
											int adjusted_max_health = max_health - class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED];

											SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
											int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

											VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
											VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);

											class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = 0;
										}

										// Undo HP Penalty
										if (hp_penalty_amount > -1)
										{
											int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
											int adjusted_max_health = max_health + hp_penalty_amount;
											hp_penalty_amount = -1;

											SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
											int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

											VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
											VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);
										}

										active_floor_enchantments.clear();
										active_sigils.insert(Sigils::SERENITY);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::SILENCE])
										active_sigils.insert(Sigils::SILENCE);
									if (held_item_id == sigil_to_item_id_map[Sigils::STRENGTH])
										active_sigils.insert(Sigils::STRENGTH);
									if (held_item_id == sigil_to_item_id_map[Sigils::TEMPTATION])
										active_sigils.insert(Sigils::TEMPTATION);
									if (held_item_id == sigil_to_item_id_map[Sigils::SIGHT])
									{
										int unified_time = GetUnifiedTime(script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][0], script_name_to_reference_map[GML_SCRIPT_GET_UNIFIED_TIME][1]).ToInt64();
										RegisterStatusEffect(script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1], status_effect_name_to_id_map["sacred_light"], 0, unified_time, unified_time + 18000);
										active_sigils.insert(Sigils::SIGHT);
									}
									if (held_item_id == sigil_to_item_id_map[Sigils::INTUITION])
									{
										GenerateTreasureSpot(global_instance->GetRefMember("__ari")->ToInstance(), self);
										active_sigils.insert(Sigils::INTUITION);
									}

									if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
										UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
								}
								else if (greater_sigil_item_used)
								{
									greater_sigil_item_used = false;

									if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION])
									{
										// Undo Fey
										if (active_floor_enchantments.contains(FloorEnchantments::FEY))
										{
											std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

											ModifySpellCosts(true, true);
											CancelStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"]);
										}

										// Undo Blessed (Oracle Set Bonus)
										if (class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] > 0)
										{
											int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
											int adjusted_max_health = max_health - class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED];

											SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
											int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

											VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
											VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);

											class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::BLESSED] = 0;
										}

										// Undo HP Penalty
										if (hp_penalty_amount > -1)
										{
											int max_health = GetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();
											int adjusted_max_health = max_health + hp_penalty_amount;
											hp_penalty_amount = -1;

											SetMaxHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1], adjusted_max_health);
											int current_health = GetHealth(script_name_to_reference_map["obj_ari"][0], script_name_to_reference_map["obj_ari"][1]).ToInt64();

											VitalsMenuSetMaxHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], adjusted_max_health);
											VitalsMenuSetHealth(script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][0], script_name_to_reference_map[GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH][1], current_health, adjusted_max_health);
										}

										active_floor_enchantments.clear();
										active_sigils.insert(Sigils::SERENITY);
										active_greater_sigils.insert(GreaterSigils::BENEDICTION);
										ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, 999);
									}
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW])
									{
										for (CInstance* monster : current_floor_monsters)
										{
											if (StructVariableExists(monster, "monster_id") && StructVariableExists(monster, "hit_points"))
											{
												RValue monster_id = monster->GetMember("monster_id");
												double hit_points = monster->GetMember("hit_points").ToDouble();
												if (IsNumeric(monster_id) && std::isfinite(hit_points) && hit_points > 0)
													*monster->GetRefMember("hit_points") = 0;
											}
										}

										active_greater_sigils.insert(GreaterSigils::ASTRAL_FLOW);
									}
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL])
										active_greater_sigils.insert(GreaterSigils::CHAIN_SPELL);
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE])
									{
										active_greater_sigils.insert(GreaterSigils::SPIRIT_SURGE);
										ModifyStamina(global_instance->GetRefMember("__ari")->ToInstance(), self, 999);
									}
									else if (held_item_id == greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI])
									{
										active_greater_sigils.insert(GreaterSigils::MEIKYO_SHISUI);
										ScaleMistpoolArmor(true);
										ScaleMistpoolWeapon(true);
										ScaleMistpoolPickaxe(true);
										ScaleClassArmor(true);
									}

									if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
										UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
								}
								else if (salve_item_used)
								{
									salve_item_used = false;

									if (held_item_id == salve_name_to_id_map[HEALTH_SALVE_NAME])
										salves_used[HEALTH_SALVE_NAME]++;
									if (held_item_id == salve_name_to_id_map[STAMINA_SALVE_NAME])
										salves_used[STAMINA_SALVE_NAME]++;
									if (held_item_id == salve_name_to_id_map[MANA_SALVE_NAME])
										salves_used[MANA_SALVE_NAME]++;

									if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
										UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
								}
								else if (lift_key_used)
								{
									lift_key_used = false;
									biome_reward_disabled = true;

									if (held_item_id == item_name_to_id_map[UPPER_MINES_KEY_F5_NAME])
										EnterDungeon(4, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[UPPER_MINES_KEY_F10_NAME])
										EnterDungeon(9, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[UPPER_MINES_KEY_F15_NAME])
										EnterDungeon(14, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F20_NAME])
										EnterDungeon(19, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F25_NAME])
										EnterDungeon(24, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F30_NAME])
										EnterDungeon(29, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_KEY_F35_NAME])
										EnterDungeon(34, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F40_NAME])
										EnterDungeon(39, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F45_NAME])
										EnterDungeon(44, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F50_NAME])
										EnterDungeon(49, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_KEY_F55_NAME])
										EnterDungeon(54, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F60_NAME])
										EnterDungeon(59, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F65_NAME])
										EnterDungeon(64, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F70_NAME])
										EnterDungeon(69, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_KEY_F75_NAME])
										EnterDungeon(74, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F80_NAME])
										EnterDungeon(79, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F85_NAME])
										EnterDungeon(84, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F90_NAME])
										EnterDungeon(89, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F95_NAME])
										EnterDungeon(94, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									else if (held_item_id == item_name_to_id_map[RUINS_KEY_F100_NAME])
										EnterDungeon(99, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
								}
								else if (orb_item_used)
								{
									orb_item_used = false;
									biome_reward_disabled = true;

									// TODO: Other orbs
									if (held_item_id == item_name_to_id_map[TIDE_CAVERNS_ORB])
									{
										boss_battle = BossBattle::TIDE_CAVERNS_ORB;
										EnterDungeon(19, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
									else if (held_item_id == item_name_to_id_map[DEEP_EARTH_ORB])
									{
										boss_battle = BossBattle::DEEP_EARTH_ORB;
										EnterDungeon(39, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
									else if (held_item_id == item_name_to_id_map[LAVA_CAVES_ORB])
									{
										boss_battle = BossBattle::LAVA_CAVES_ORB;
										EnterDungeon(59, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
									else if (held_item_id == item_name_to_id_map[RUINS_ORB])
									{
										boss_battle = BossBattle::RUINS_ORB;
										EnterDungeon(79, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
									}
								}
								else if (heart_crystal_used)
								{
									heart_crystal_used = false;
									if (unmodified_base_health != -1)
										unmodified_base_health += 20;
								}
							}
						}
					}
				}
			}
		}

		// Restoration
		if (is_restoration_tracked_interval)
		{
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			if (current_health > 0)
				ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
			is_restoration_tracked_interval = false;
		}

		// Fumigate
		if (is_fumigate_tracked_interval)
		{
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			if (current_health > 0)
				ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, -1);

			is_fumigate_tracked_interval = false;
		}

		// Deep Wounds
		if (is_deep_wounds_tracked_interval)
		{
			int current_health = GetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			if (current_health > 0 && deep_wounds_damage_pool > 0)
			{
				int damage = std::clamp(deep_wounds_damage_pool * 10 / 100, 1, 10);
				damage = min(damage, deep_wounds_damage_pool);

				deep_wounds_damage_pool -= damage;
				ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, -1 * damage);
			}

			is_deep_wounds_tracked_interval = false;
		}

		// Drain (Dark Knight Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN] > 0)
		{
			int max_health = GetMaxHealth(global_instance->GetRefMember("__ari")->ToInstance(), self).ToInt64();
			int recovery = max_health * GetDarkKnightDrainPotency();

			ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, recovery);
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DRAIN] = 0;
		}

		// Soul Eater (Dark Knight Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] > 0)
		{
			ModifyHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, -1 * class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER]);
			class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::SOUL_EATER] = 0;
		}

		// Aspir (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR] > 0)
		{
			ModifyMana(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR] = 0;
		}

		// Mana Font (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT] >= 3)
		{
			ModifyMana(global_instance->GetRefMember("__ari")->ToInstance(), self, 4);
			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::MANA_FONT] = 0;
		}

		// Flood (Mage Set Bonus)
		if (class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] > 0 && current_time_in_seconds > class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] + 90)
		{
			for (CInstance* monster : current_floor_monsters)
			{
				if (StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster->GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points > 0)
						*monster->GetRefMember("hit_points") = hit_points - 1;
				}
			}

			class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] = 0;
			CastSpell(global_instance->GetRefMember("__ari")->ToInstance(), self, spell_name_to_id_map["summon_rain"]);
		}

		// Second Wind
		if (is_second_wind_tracked_interval)
		{
			ModifyStamina(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
			is_second_wind_tracked_interval = false;
		}

		// Fey
		if (active_floor_enchantments.contains(FloorEnchantments::FEY))
		{
			if (!fairy_buff_applied)
			{
				std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];
				RegisterStatusEffect(refs[0], refs[1], status_effect_name_to_id_map["fairy"], RValue(), 1, 2147483647.0);
				fairy_buff_applied = true;
			}
			ModifySpellCosts(false, true);
		}

		// Inner Fire
		if (active_offerings.contains(Offerings::INNER_FIRE) && !inner_fire_cast)
		{
			inner_fire_cast = true;
			CastSpell(global_instance->GetRefMember("__ari")->ToInstance(), self, spell_name_to_id_map["fire_breath"]); // TODO: Make sure this works using name_to_id_map
		}

		// Reckoning
		if (active_offerings.contains(Offerings::RECKONING) && !reckoning_applied)
		{
			reckoning_applied = true;
			SetHealth(global_instance->GetRefMember("__ari")->ToInstance(), self, 1);
		}

		// Stoneskin
		if (active_floor_enchantments.contains(FloorEnchantments::STONESKIN) && !stoneskin_applied)
		{
			stoneskin_applied = true;

			// TODO: Tune this
			if (floor_number < 20)
				stoneskin_shield_amount = 20;
			else if (floor_number < 40)
				stoneskin_shield_amount = 40;
			else if (floor_number < 60)
				stoneskin_shield_amount = 60;
			else if (floor_number < 60)
				stoneskin_shield_amount = 80;
			else
				stoneskin_shield_amount = 100;
		}

		// Chain Spell
		if (active_greater_sigils.contains(GreaterSigils::CHAIN_SPELL))
			ModifySpellCosts(false, true);

		TrackAriResources(global_instance->GetRefMember("__ari")->ToInstance(), self);
	}

	if (strstr(self->m_Object->m_Name, "obj_monster"))
	{
		RValue monster = self->ToRValue();
		if (StructVariableExists(monster, "monster_id"))
		{
			RValue monster_id = *monster.GetRefMember("monster_id");

			bool is_valid_monster_object = false;
			for (const auto& entry : monster_name_to_id_map) {
				if (entry.second == monster_id.ToInt64()) {
					is_valid_monster_object = true;
					break;
				}
			}

			if (is_valid_monster_object)
			{
				// Mimic Loot
				if (monster_id.ToInt64() == monster_name_to_id_map["mimic"] && !StructVariableExists(monster, "__deep_dungeon__mimic_drop_sigil") && StructVariableExists(monster, "fsm"))
				{
					RValue fsm = monster.GetMember("fsm");
					if (StructVariableExists(fsm, "state"))
					{
						RValue state = fsm.GetMember("state");
						if (StructVariableExists(state, "state_id"))
						{
							RValue state_id = state.GetMember("state_id");
							if (state_id.ToInt64() == monster_category_to_state_id_map["mimic"]["gobble"] && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
							{
								static thread_local pcg32 random_generator([] {
									std::random_device rd;
									return pcg32(
										(static_cast<uint64_t>(rd()) << 32) | rd(),
										(static_cast<uint64_t>(rd()) << 32) | rd()
									);
								}());
								std::uniform_int_distribution<size_t> random_sigil_distribution(0, magic_enum::enum_count<Sigils>() - 1);

								Sigils random_sigil = magic_enum::enum_value<Sigils>(random_sigil_distribution(random_generator));
								DropItem(sigil_to_item_id_map[random_sigil], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
								StructVariableSet(monster, "__deep_dungeon__mimic_drop_sigil", true);
							}
						}
					}
				}

				// Boss Battles
				if (boss_battle == BossBattle::TIDE_CAVERNS_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 20;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}
				else if (boss_battle == BossBattle::DEEP_EARTH_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 3;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}
				else if (boss_battle == BossBattle::LAVA_CAVES_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 3;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}
				else if (boss_battle == BossBattle::RUINS_ORB)
				{
					if (!StructVariableExists(monster, "__deep_dungeon__boss_monster") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = hit_points * 3;
							StructVariableSet(monster, "__deep_dungeon__boss_monster", true);
						}
					}
					else if (StructVariableExists(monster, "__deep_dungeon__boss_monster"))
						ModifyDreadBeastAttackPatterns(true, false, monster);
				}

				// Dread Beasts
				if (!dread_beast_configured && monster_id.ToInt64() == dread_beast_monster_id && !StructVariableExists(monster, "__deep_dungeon__dread_beast") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points))
					{
						*monster.GetRefMember("hit_points") = static_cast<int>(hit_points * Config::config.dread_beast_health_modifier);
						dread_beasts_configured++;
						if (dread_beast_monster_id != monster_name_to_id_map["rock_stack"] || dread_beasts_configured == 2)
							dread_beast_configured = true;

						StructVariableSet(monster, "__deep_dungeon__dread_beast", true);
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Configured Dread Beast: %s", MOD_NAME, VERSION, monster_id_to_name_map[monster_id.ToInt64()].c_str());
					}
				}
				else if (StructVariableExists(monster, "__deep_dungeon__dread_beast"))
					ModifyDreadBeastAttackPatterns(false, false, monster);

				// Track the monster
				if (!StructVariableExists(monster, "__deep_dungeon__current_floor_monsters") && StructVariableExists(monster, "hit_points"))
				{
					current_floor_monsters.push_back(self);
					StructVariableSet(monster, "__deep_dungeon__current_floor_monsters", true);
					StructVariableSet(monster, "__deep_dungeon__default_hit_points", monster.GetMember("hit_points").ToDouble());
				}

				// Grudge
				if (active_floor_enchantments.contains(FloorEnchantments::GRUDGE) && !StructVariableExists(monster, "__deep_dungeon__grudge_tracked") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						grudge_counter += 1;
						StructVariableSet(monster, "__deep_dungeon__grudge_tracked", true);
					}
				}

				// Regular loot drops
				if (!ari_current_gm_room.contains("seal") && !StructVariableExists(monster, "__deep_dungeon__loot_drop") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0 && script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
					{
						static thread_local pcg32 random_generator([] {
							std::random_device rd;
							return pcg32(
								(static_cast<uint64_t>(rd()) << 32) | rd(),
								(static_cast<uint64_t>(rd()) << 32) | rd()
							);
						}());
						std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);
						bool drop_lift_key = zero_to_ninety_nine_distribution(random_generator) < Config::config.lift_key_drop_chance ? true : false;

						if (floor_number < 20) // Upper Mines
						{
							DropItem(item_name_to_id_map["beast_coin_tiny"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 40) // Tide Caverns
						{
							DropItem(item_name_to_id_map["beast_coin_small"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 60) // Deep Earth
						{
							DropItem(item_name_to_id_map["beast_coin_medium"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 80) // Lava Caves
						{
							DropItem(item_name_to_id_map["beast_coin_large"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}
						else if (floor_number < 100) // Ruins
						{
							DropItem(item_name_to_id_map["beast_coin_giant"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							if (Config::config.disable_dungeon_lift && drop_lift_key)
								DropLiftKey();
							if (StructVariableExists(monster, "__deep_dungeon__dread_beast") && !StructVariableExists(monster, "__deep_dungeon__outbreak"))
							{
								bool drop_soul_stone = zero_to_ninety_nine_distribution(random_generator) < Config::config.soul_stone_drop_chance ? true : false;
								if (drop_soul_stone)
									DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
							}
						}

						StructVariableSet(monster, "__deep_dungeon__loot_drop", true);
					}
				}

				// Boss loot drops
				if (boss_battle != BossBattle::NONE && boss_monsters_configured > 0)
				{
					int boss_monsters_defeated = 0;
					for (CInstance* monster : current_floor_monsters)
					{
						if (StructVariableExists(monster, "hit_points"))
						{
							double hit_points = monster->GetMember("hit_points").ToDouble();
							if (std::isfinite(hit_points) && hit_points <= 0)
								boss_monsters_defeated++;
						}
					}

					if (boss_monsters_defeated == boss_monsters_configured)
					{
						if (script_name_to_reference_map.contains(GML_SCRIPT_DROP_ITEM))
							DropItem(GetRandomSoulStone(), ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);
						boss_battle = BossBattle::NONE;
						ResetCustomDrawFields();
					}
				}

				// Aspir (Mage Set Bonus)
				if (!StructVariableExists(monster, "__deep_dungeon__aspir_proc") && StructVariableExists(monster, "hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						static thread_local pcg32 random_generator([] {
							std::random_device rd;
							return pcg32(
								(static_cast<uint64_t>(rd()) << 32) | rd(),
								(static_cast<uint64_t>(rd()) << 32) | rd()
							);
						}());
						std::uniform_int_distribution<size_t> zero_to_ninety_nine_distribution(0, 99);

						bool aspir_proc = zero_to_ninety_nine_distribution(random_generator) < 15 ? true : false; // TODO: Tune this. Should Aspir have a 15% chance to proc?
						if (aspir_proc)
							class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ASPIR]++;

						StructVariableSet(monster, "__deep_dungeon__aspir_proc", true);
					}
				}

				// Holy Circle (Paladin Set Bonus)
				if (CountEquippedClassArmor()[Classes::PALADIN] > 0 && !StructVariableExists(monster, "__deep_dungeon__holy_circle_proc") && StructVariableExists(monster, "hit_points") && StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
				{
					double hit_points = monster.GetMember("hit_points").ToDouble();
					if (std::isfinite(hit_points) && hit_points <= 0)
					{
						double default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToDouble();
						*monster.GetRefMember("hit_points") = hit_points - std::trunc(default_hit_points * GetPaladinHolyCirclePotency());
						StructVariableSet(monster, "__deep_dungeon__holy_circle_proc", true);

						for (CInstance* current_floor_monster : current_floor_monsters)
						{
							if (StructVariableExists(current_floor_monster, "monster_id") && StructVariableExists(current_floor_monster, "hit_points") && StructVariableExists(current_floor_monster, "__deep_dungeon__default_hit_points"))
							{
								RValue current_floor_monster_id = current_floor_monster->GetMember("monster_id");
								double current_floor_monster_hit_points = current_floor_monster->GetMember("hit_points").ToDouble();
								if (IsNumeric(current_floor_monster_id) && current_floor_monster_id.ToInt64() != monster_name_to_id_map["mimic"] && IsNumeric(current_floor_monster_hit_points) && std::isfinite(current_floor_monster_hit_points) && current_floor_monster_hit_points > 0)
								{
									double current_floor_monster_default_hit_points = current_floor_monster->GetMember("__deep_dungeon__default_hit_points").ToDouble();
									*current_floor_monster->GetRefMember("hit_points") = current_floor_monster_hit_points - std::trunc(current_floor_monster_default_hit_points * GetPaladinHolyCirclePotency());
								}
							}
						}
					}
				}

				// Gloom
				if (active_floor_enchantments.contains(FloorEnchantments::GLOOM))
				{
					if (!StructVariableExists(monster, "__deep_dungeon__gloom_applied") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							*monster.GetRefMember("hit_points") = std::trunc(hit_points * Config::config.gloom_health_modifier);
							StructVariableSet(monster, "__deep_dungeon__default_hit_points", std::trunc(hit_points * Config::config.gloom_health_modifier));
							StructVariableSet(monster, "__deep_dungeon__gloom_applied", true);
						}
					}
				}

				// Reckoning
				if (active_offerings.contains(Offerings::RECKONING))
				{
					if (!StructVariableExists(monster, "__deep_dungeon__reckoning_applied") && StructVariableExists(monster, "hit_points"))
					{
						if (current_time_in_seconds < floor_start_time + 30) // All enemies seem to be created with HP var initialized before floor starts
						{
							*monster.GetRefMember("hit_points") = 1;
							StructVariableSet(monster, "__deep_dungeon__reckoning_applied", true);
						}
					}
				}

				// Spirit Link
				if (active_offerings.contains(Offerings::SPIRIT_LINK) && monster_id.ToInt64() != monster_name_to_id_map["mimic"])
				{
					if (StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							if (StructVariableExists(monster, "__deep_dungeon__spirit_link_applied") && spirit_link_combined_health_pool <= 0)
								StructVariableSet(monster, "hit_points", 0);
							else if (!StructVariableExists(monster, "__deep_dungeon__spirit_link_applied"))
							{
								spirit_link_combined_health_pool += hit_points;
								StructVariableSet(monster, "__deep_dungeon__spirit_link_applied", true);
								StructVariableSet(monster, "__deep_dungeon__spirit_link_damage", 0);

							}
							else if (StructVariableExists(monster, "__deep_dungeon__default_hit_points"))
							{
								int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
								int spirit_link_damage = monster.GetMember("__deep_dungeon__spirit_link_damage").ToInt64();
								if (hit_points < default_hit_points)
								{
									spirit_link_damage += (default_hit_points - hit_points);
									spirit_link_combined_health_pool -= (default_hit_points - hit_points);

									StructVariableSet(monster, "__deep_dungeon__spirit_link_damage", spirit_link_damage);
									StructVariableSet(monster, "hit_points", default_hit_points);
								}
							}
						}
					}
				}
				else if (StructVariableExists(monster, "__deep_dungeon__spirit_link_applied"))
				{
					if (StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points))
						{
							int default_hit_points = monster.GetMember("__deep_dungeon__default_hit_points").ToInt64();
							int spirit_link_damage = monster.GetMember("__deep_dungeon__spirit_link_damage").ToInt64();

							if(default_hit_points - spirit_link_damage > 0)
								StructVariableSet(monster, "hit_points", default_hit_points - spirit_link_damage);
							else
								StructVariableSet(monster, "hit_points", 0);
						}

						StructVariableRemove(monster, "__deep_dungeon__spirit_link_applied");
					}
				}

				// Sigil of Concealment
				if (active_sigils.contains(Sigils::CONCEALMENT))
				{
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue hit_points = monster.GetMember("hit_points");
						if (!StructVariableExists(monster, "__deep_dungeon__conceal_hit_points"))
							StructVariableSet(monster, "__deep_dungeon__conceal_hit_points", hit_points);

						RValue original_hit_points = monster.GetMember("__deep_dungeon__conceal_hit_points");
						if (hit_points.ToDouble() == original_hit_points.ToDouble())
						{
							StructVariableSet(monster, "aggro", false);
							if (monster_id.ToInt64() == monster_name_to_id_map["cat"] || monster_id.ToInt64() == monster_name_to_id_map["cat_void"] || monster_id.ToInt64() == monster_name_to_id_map["tome"] || monster_id.ToInt64() == monster_name_to_id_map["griffin_statue"])
							{
								StructVariableSet(monster, "friction_coefficient", 1);
								StructVariableSet(monster, "slippery_coefficient", 1);
							}
						}
						else
						{
							active_sigils.erase(Sigils::CONCEALMENT);
							CreateNotification(false, CONCEALMENT_LOST_NOTIFICATION_KEY, nullptr, nullptr);

							// Sneak Attack (Rogue Set Bonus)
							if (CountEquippedClassArmor()[Classes::ROGUE] >= 2)
								*monster.GetRefMember("hit_points") = 0;

							if (script_name_to_reference_map.contains(GML_SCRIPT_UPDATE_TOOLBAR_MENU))
								UpdateToolbarMenu(script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][0], script_name_to_reference_map[GML_SCRIPT_UPDATE_TOOLBAR_MENU][1]);
						}
					}
				}
				else
				{
					StructVariableRemove(monster, "__deep_dungeon__conceal_hit_points");
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						double hit_points = monster.GetMember("hit_points").ToDouble();
						if (std::isfinite(hit_points) && hit_points > 0 && (monster_id.ToInt64() == monster_name_to_id_map["cat"] || monster_id.ToInt64() == monster_name_to_id_map["cat_void"] || monster_id.ToInt64() == monster_name_to_id_map["tome"] || monster_id.ToInt64() == monster_name_to_id_map["griffin_statue"]))
						{
							StructVariableSet(monster, "friction_coefficient", 0.1);
							StructVariableSet(monster, "slippery_coefficient", 0.1);
						}
					}
				}


				// Sigil of Rage
				if (active_sigils.contains(Sigils::RAGE))
				{
					if (StructVariableExists(monster, "config") && StructVariableExists(monster, "hit_points"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue hit_points = monster.GetMember("hit_points");
						if (!StructVariableExists(monster, "__deep_dungeon__rage_hit_points"))
							StructVariableSet(monster, "__deep_dungeon__rage_hit_points", hit_points);

						RValue original_hit_points = monster.GetMember("__deep_dungeon__rage_hit_points");
						if (hit_points.ToDouble() != original_hit_points.ToDouble())
							*monster.GetRefMember("hit_points") = 0;
					}
				}
			}
		}
	}
}
