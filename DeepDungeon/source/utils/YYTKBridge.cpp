#include "Utils.h"

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool IsObject(RValue value)
{
	return value.m_Kind == VALUE_OBJECT;
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return struct_exists.ToBoolean();
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void StructVariableRemove(RValue the_struct, const char* variable_name)
{
	if (StructVariableExists(the_struct, variable_name))
	{
		RValue struct_exists = g_ModuleInterface->CallBuiltin(
			"struct_remove",
			{ the_struct, variable_name }
		);
	}
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return global_variable_exists.ToBoolean();
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists("__YYTK"))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet("__YYTK", __YYTK);
	}
	else
		__YYTK = GlobalVariableGet("__YYTK");
}

void CreateModInfoInGlobalYYTKVariable()
{
	if (!StructVariableExists(__YYTK, MOD_NAME))
	{
		RValue deep_dungeon;
		RValue version = VERSION;
		RValue floor = floor_number;
		g_ModuleInterface->GetRunnerInterface().StructCreate(&deep_dungeon);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "version", &version);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&deep_dungeon, "floor", &floor);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&__YYTK, MOD_NAME, &deep_dungeon);
	}
}

double GetWindowWidth()
{
	RValue window_width = g_ModuleInterface->CallBuiltin("window_get_width", {});
	return window_width.ToDouble();
}

double GetWindowHeight()
{
	RValue window_height = g_ModuleInterface->CallBuiltin("window_get_height", {});
	return window_height.ToDouble();
}

void PlaySoundEffect(const char* sound_name, int priority, double gain)
{
	const auto sound_index = g_ModuleInterface->CallBuiltin(
		"asset_get_index",
		{ sound_name }
	);

	g_ModuleInterface->CallBuiltin(
		"audio_play_sound",
		{ sound_index, 100, false, gain }
	);
}

RValue LocalizeString(CInstance* Self, CInstance* Other, std::string localization_key)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_LOCALIZER,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(localization_key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void SpawnLadder(CInstance* Self, CInstance* Other, int64_t x_coord, int64_t y_coord)
{
	CScript* gml_Script_spawn_ladder = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_LADDER,
		(PVOID*)&gml_Script_spawn_ladder
	);

	RValue x = (x_coord * 2) / 16;
	RValue y = (y_coord * 2) / 16;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* rvalue_array[2] = { x_ptr, y_ptr };
	RValue retval;
	gml_Script_spawn_ladder->m_Functions->m_ScriptFunction(
		Self,
		Other,
		retval,
		2,
		rvalue_array
	);
}

void SpawnMonster(CInstance* Self, CInstance* Other, int room_x, int room_y, int monster_id)
{
	CScript* gml_script_spawn_monster = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_MONSTER,
		(PVOID*)&gml_script_spawn_monster
	);

	RValue x = room_x;
	RValue y = room_y;
	RValue monster = monster_id;

	RValue result;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* monster_ptr = &monster;
	RValue* arguments[3] = { x_ptr, y_ptr, monster_ptr };

	gml_script_spawn_monster->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		arguments
	);
}

void TeleportAriToRoom(CInstance* Self, CInstance* Other, int location_id, int x_coordinate, int y_coordinate)
{
	CScript* gml_script_ari_teleport_to_room = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_TELEPORT_ARI_TO_ROOM,
		(PVOID*)&gml_script_ari_teleport_to_room
	);

	RValue retval;
	RValue location = location_id;
	RValue x = x_coordinate;
	RValue y = y_coordinate;
	RValue* location_ptr = &location;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* argument_array[3] = { location_ptr, x_ptr, y_ptr };
	gml_script_ari_teleport_to_room->m_Functions->m_ScriptFunction(
		Self,
		Other,
		retval,
		3,
		argument_array
	);
}

void CreateNotification(bool ignore_cooldown, std::string notification_localization_str, CInstance* Self, CInstance* Other)
{
	uint64_t current_system_time = GetCurrentSystemTime();
	if (ignore_cooldown || current_system_time > notification_name_to_last_display_time_map[notification_localization_str] + 5000)
	{
		CScript* gml_script_create_notification = nullptr;
		g_ModuleInterface->GetNamedRoutinePointer(
			GML_SCRIPT_CREATE_NOTIFICATION,
			(PVOID*)&gml_script_create_notification
		);

		RValue result;
		RValue notification = RValue(notification_localization_str);
		RValue* notification_ptr = &notification;
		gml_script_create_notification->m_Functions->m_ScriptFunction(
			Self,
			Other,
			result,
			1,
			{ &notification_ptr }
		);

		notification_name_to_last_display_time_map[notification_localization_str] = current_system_time;
	}
}

void PlayConversation(std::string conversation_localization_str, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_play_conversation = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_CONVERSATION,
		(PVOID*)&gml_script_play_conversation
	);

	RValue zero = 0;
	RValue conversation = RValue(conversation_localization_str);
	RValue undefined;

	RValue* zero_ptr = &zero;
	RValue* conversation_ptr = &conversation;
	RValue* undefined_ptr = &undefined;

	RValue result;
	RValue* arguments[4] = { zero_ptr, conversation_ptr, undefined_ptr, undefined_ptr };

	gml_script_play_conversation->m_Functions->m_ScriptFunction(
		Self,
		Self,
		result,
		4,
		arguments
	);
}

void CloseTextbox(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_close_textbox = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CLOSE_TEXTBOX,
		(PVOID*)&gml_script_close_textbox
	);

	RValue result;
	gml_script_close_textbox->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

void EmitBark(CInstance* Self, CInstance* Other, RValue bark_id, RValue bark_type)
{
	CScript* gml_script_register_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_BARK_EMITTER_EMIT,
		(PVOID*)&gml_script_register_status_effect
	);

	RValue result;
	RValue* bark_id_ptr = &bark_id;
	RValue* bark_type_ptr = &bark_type;
	RValue* argument_array[2] = { bark_id_ptr, bark_type_ptr };

	gml_script_register_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		2,
		argument_array
	);
}

RValue T2Read(CInstance* Self, CInstance* Other, std::string key)
{
	CScript* gml_script_get_localizer = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_T2_READ,
		(PVOID*)&gml_script_get_localizer
	);

	RValue result;
	RValue input = RValue(key);
	RValue* input_ptr = &input;
	gml_script_get_localizer->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void SceneAudioPlayerStop(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SCENE_AUDIO_PLAYER_STOP,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

void SpawnTutorial(std::string tutorial_name, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_create_notification = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SPAWN_TUTORIAL,
		(PVOID*)&gml_script_create_notification
	);

	RValue result;
	RValue notification = tutorial_name_to_id_map[tutorial_name];
	RValue* notification_ptr = &notification;
	gml_script_create_notification->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &notification_ptr }
	);
}

void UpdateToolbarMenu(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_update_toolbar_menu = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_UPDATE_TOOLBAR_MENU,
		(PVOID*)&gml_script_update_toolbar_menu
	);

	RValue result;
	gml_script_update_toolbar_menu->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);
}

RValue DeserializeLiveItem(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_deserialize_live_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DESERIALIZE_LIVE_ITEM,
		(PVOID*)&gml_script_deserialize_live_item
	);

	std::map<std::string, RValue> rvalue_map = {
		{ "cosmetic", RValue() }, // UNDEFINED
		{ "item_id", RValue("sword_scrap_metal")}, // STRING
		{ "infusion", RValue() }, // UNDEFINED
		{ "animal_cosmetic", RValue() }, // UNDEFINED
		{ "date_photo", RValue() }, // UNDEFINED
		{ "inner_item", RValue() }, // UNDEFINED
		{ "gold_to_gain", RValue() }, // UNDEFINED
		{ "auto_use", false }, // BOOL
		{ "pet_cosmetic_set_name", RValue() } // UNDEFINED
	};

	RValue result;
	RValue input = rvalue_map;
	RValue* input_ptr = &input;
	gml_script_deserialize_live_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

void DropItem(int item_id, double x_coord, double y_coord, CInstance* Self, CInstance* Other)
{
	RValue item = DeserializeLiveItem(Self, Other);
	*item.GetRefMember("prototype") = item_id_to_prototype_map[item_id];
	*item.GetRefMember("item_id") = item_id;

	CScript* gml_script_drop_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DROP_ITEM,
		(PVOID*)&gml_script_drop_item
	);

	RValue x = x_coord;
	RValue y = y_coord;
	RValue undefined;

	RValue* item_ptr = &item;
	RValue* x_ptr = &x;
	RValue* y_ptr = &y;
	RValue* undefined_ptr = &undefined;

	RValue result;
	RValue* arguments[4] = { item_ptr, x_ptr, y_ptr, undefined_ptr };

	gml_script_drop_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		4,
		arguments
	);
}

void EnterDungeon(double dungeon_level, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_enter_dungeon = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_ENTER_DUNGEON,
		(PVOID*)&gml_script_enter_dungeon
	);

	RValue result;
	RValue level = dungeon_level;
	RValue undefined;
	RValue* level_ptr = &level;
	RValue* undefined_ptr = &undefined;
	RValue* arguments[3] = { level_ptr, undefined_ptr, undefined_ptr };

	gml_script_enter_dungeon->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		arguments
	);
}

RValue InventoryCountItem(int item_id, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_inventory_count_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INVENTORY_COUNT_ITEM,
		(PVOID*)&gml_script_inventory_count_item
	);

	RValue result;
	RValue item = item_id;
	RValue* item_ptr = &item;

	gml_script_inventory_count_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &item_ptr }
	);

	return result;
}

void InventoryRemoveItem(int item_id, int quantity, CInstance* Self, CInstance* Other)
{
	CScript* gml_script_inventory_remove_item = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_INVENTORY_REMOVE_ITEM,
		(PVOID*)&gml_script_inventory_remove_item
	);

	RValue result;
	RValue item = item_id;
	RValue amount = quantity;
	RValue* item_ptr = &item;
	RValue* amount_ptr = &amount;
	RValue* arguments[2] = { item_ptr, amount_ptr };

	gml_script_inventory_remove_item->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		2,
		arguments
	);
}

RValue GetUnifiedTime(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_unified_time = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_UNIFIED_TIME,
		(PVOID*)&gml_script_get_unified_time
	);

	RValue result;
	gml_script_get_unified_time->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void CancelStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id)
{
	CScript* gml_script_cancel_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CANCEL_STATUS_EFFECT,
		(PVOID*)&gml_script_cancel_status_effect
	);

	RValue result;
	RValue* status_effect_id_ptr = &status_effect_id;

	gml_script_cancel_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &status_effect_id_ptr }
	);
}

void CancelAllStatusEffects()
{
	std::vector<CInstance*> refs = script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE];

	// Remove all status effects
	for (int i = 0; i <= status_effect_name_to_id_map.size(); i++)
		CancelStatusEffect(refs[0], refs[1], i);
}

void RegisterStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id, RValue amount, RValue start, RValue finish)
{
	CScript* gml_script_register_status_effect = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_REGISTER_STATUS_EFFECT,
		(PVOID*)&gml_script_register_status_effect
	);

	RValue result;
	RValue* status_effect_id_ptr = &status_effect_id;
	RValue* amount_ptr = &amount;
	RValue* start_ptr = &start;
	RValue* finish_ptr = &finish;
	RValue* argument_array[4] = { status_effect_id_ptr, amount_ptr, start_ptr, finish_ptr };

	gml_script_register_status_effect->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		4,
		argument_array
	);
}

RValue GetHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_HEALTH,
		(PVOID*)&gml_script_get_health
	);

	RValue result;
	gml_script_get_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void SetHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_set_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SET_HEALTH,
		(PVOID*)&gml_script_set_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_set_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

void ModifyHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_HEALTH,
		(PVOID*)&gml_script_modify_health
	);

	RValue result;
	RValue health_modifier = value;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_modify_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

RValue GetMaxHealth(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MAX_HEALTH,
		(PVOID*)&gml_script_get_max_health
	);

	RValue result;
	gml_script_get_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void SetMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	*__ari.GetRefMember("base_health") = value;

	RValue current_health = GetHealth(Self, Other);
	if (current_health.ToInt64() > value)
		SetHealth(Self, Other, value);
}

int ModifyMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	int max_health = __ari.GetMember("base_health").ToInt64() + value;
	*__ari.GetRefMember("base_health") = max_health;

	return max_health;
}

void VitalsMenuSetHealth(CInstance* Self, CInstance* Other, int current_health_value, int max_health_value)
{
	CScript* gml_script_vitals_menu_set_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_health
	);

	RValue result;
	RValue current_health = current_health_value;
	RValue max_health = max_health_value;
	RValue* current_health_ptr = &current_health;
	RValue* max_health_ptr = &max_health;
	RValue arg2 = false;
	RValue* arg2_ptr = &arg2;
	RValue* argument_array[3] = { current_health_ptr, max_health_ptr, arg2_ptr };

	gml_script_vitals_menu_set_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		3,
		argument_array
	);
}

void VitalsMenuSetMaxHealth(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_vitals_menu_set_max_health = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH,
		(PVOID*)&gml_script_vitals_menu_set_max_health
	);

	RValue result;
	RValue max_health = value;
	RValue* max_health_ptr = &max_health;

	gml_script_vitals_menu_set_max_health->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &max_health_ptr }
	);
}

RValue GetStamina(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_STAMINA,
		(PVOID*)&gml_script_get_stamina
	);

	RValue result;
	gml_script_get_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void ModifyStamina(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_stamina = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_STAMINA,
		(PVOID*)&gml_script_modify_stamina
	);

	RValue result;
	RValue stamina_modifier = value;
	RValue* stamina_modifier_ptr = &stamina_modifier;

	gml_script_modify_stamina->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &stamina_modifier_ptr }
	);
}

RValue GetMana(CInstance* Self, CInstance* Other)
{
	CScript* gml_script_get_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_MANA,
		(PVOID*)&gml_script_get_mana
	);

	RValue result;
	gml_script_get_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		0,
		nullptr
	);

	return result;
}

void ModifyMana(CInstance* Self, CInstance* Other, int value)
{
	CScript* gml_script_modify_mana = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_MODIFY_MANA,
		(PVOID*)&gml_script_modify_mana
	);

	RValue result;
	RValue mana_modifier = value;
	RValue* mana_modifier_ptr = &mana_modifier;

	gml_script_modify_mana->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &mana_modifier_ptr }
	);
}

void CastSpell(CInstance* Self, CInstance* Other, int spell_id)
{
	CScript* gml_script_cast_spell = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CAST_SPELL,
		(PVOID*)&gml_script_cast_spell
	);

	RValue result;
	RValue health_modifier = spell_id;
	RValue* health_modifier_ptr = &health_modifier;

	gml_script_cast_spell->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &health_modifier_ptr }
	);
}

void UnlockRecipe(int item_id, CInstance* Self, CInstance* Other)
{
	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue recipe_unlocks = *__ari.GetRefMember("recipe_unlocks");

	if (recipe_unlocks[item_id].m_Real == 0.0)
		recipe_unlocks[item_id] = 1.0; // This value is ultimately what unlocks the recipe.
}
