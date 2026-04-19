#include "Init.h"

// Forward declarations for ObjectCallback and all GmlScript*Callback functions
// (these are defined in the hooks/ files)
void ObjectCallback(IN FWCodeEvent& CodeEvent);
RValue& GmlScriptCancelStatusEffectCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptRegisterStatusEffectCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptVitalsMenuSetMaxHealthCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptModifyHealthCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptModifyStaminaCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptSpawnMonsterCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptCanCastSpellCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptCastSpellCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetMoveSpeedCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptDamageCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptStatusEffectManagerUpdateCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptTakePressCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptCheckValueCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptAttemptInteractCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptPlayTextCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptPlayConversationCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptUseItemCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptHeldItemCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptDropItemCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetMinutesCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetLocalizerCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetWeatherCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptTryLocationIdToStringCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptOnDungeonRoomStartCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGoToRoomCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptSetupMainScreenCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetEquipmentBonusFromCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptHudShouldShowCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptOnDrawGuiCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptDisplayResizeCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetUiIconCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptUpdateToolbarMenuCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptCreateItemPrototypesCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptSpawnLadderCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptDeserializeLiveItemCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptDeserializeInventoryCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetTreasureFromDistributionCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptCraftingMenuInitializeCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptCraftingMenuCloseCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptJournalMenuInitializeCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptJournalMenuCloseCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptStoreMenuInitializeCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptStoreMenuCloseCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptVertigoDrawWithColorCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptSceneAudioPlayerPlayCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptFindNpcBlipNoiseCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptSaveGameCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptAriFaceDirCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptOnBeginStepCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptRecipeGenerateInfusionsCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptBarkEmitterCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptT2ReadCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);
RValue& GmlScriptGetUnifiedTimeCallback(IN CInstance* Self, IN CInstance* Other, OUT RValue& Result, IN int ArgumentCount, IN RValue** Arguments);

// Single helper replacing all CreateHookGmlScript* functions:
static AurieStatus RegisterHook(const char* scriptName, PVOID callback)
{
    CScript* script = nullptr;
    AurieStatus status = g_ModuleInterface->GetNamedRoutinePointer(scriptName, (PVOID*)&script);
    if (!AurieSuccess(status))
    {
        g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, MOD_VERSION, scriptName);
        return status;
    }
    status = MmCreateHook(g_ArSelfModule, scriptName, script->m_Functions->m_ScriptFunction, callback, nullptr);
    if (!AurieSuccess(status))
        g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, MOD_VERSION, scriptName);
    return status;
}

// Table of { scriptName, callback } in the exact order they appear in the original ModuleInitialize call sequence
static const struct HookEntry { const char* name; PVOID callback; } HOOK_TABLE[] = {
    { GML_SCRIPT_CANCEL_STATUS_EFFECT,          (PVOID)GmlScriptCancelStatusEffectCallback          },
    { GML_SCRIPT_REGISTER_STATUS_EFFECT,        (PVOID)GmlScriptRegisterStatusEffectCallback        },
    { GML_SCRIPT_VITALS_MENU_SET_MAX_HEALTH,    (PVOID)GmlScriptVitalsMenuSetMaxHealthCallback      },
    { GML_SCRIPT_MODIFY_HEALTH,                 (PVOID)GmlScriptModifyHealthCallback                },
    { GML_SCRIPT_MODIFY_STAMINA,                (PVOID)GmlScriptModifyStaminaCallback               },
    { GML_SCRIPT_SPAWN_MONSTER,                 (PVOID)GmlScriptSpawnMonsterCallback                },
    { GML_SCRIPT_CAN_CAST_SPELL,                (PVOID)GmlScriptCanCastSpellCallback                },
    { GML_SCRIPT_CAST_SPELL,                    (PVOID)GmlScriptCastSpellCallback                   },
    { GML_SCRIPT_GET_MOVE_SPEED,                (PVOID)GmlScriptGetMoveSpeedCallback                },
    { GML_SCRIPT_DAMAGE,                        (PVOID)GmlScriptDamageCallback                      },
    { GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE,  (PVOID)GmlScriptStatusEffectManagerUpdateCallback   },
    { GML_SCRIPT_TAKE_PRESS,                    (PVOID)GmlScriptTakePressCallback                   },
    { GML_SCRIPT_CHECK_VALUE,                   (PVOID)GmlScriptCheckValueCallback                  },
    { GML_SCRIPT_ATTEMPT_INTERACT,              (PVOID)GmlScriptAttemptInteractCallback             },
    { GML_SCRIPT_PLAY_TEXT,                     (PVOID)GmlScriptPlayTextCallback                    },
    { GML_SCRIPT_PLAY_CONVERSATION,             (PVOID)GmlScriptPlayConversationCallback            },
    { GML_SCRIPT_USE_ITEM,                      (PVOID)GmlScriptUseItemCallback                     },
    { GML_SCRIPT_HELD_ITEM,                     (PVOID)GmlScriptHeldItemCallback                    },
    { GML_SCRIPT_DROP_ITEM,                     (PVOID)GmlScriptDropItemCallback                    },
    { GML_SCRIPT_GET_MINUTES,                   (PVOID)GmlScriptGetMinutesCallback                  },
    { GML_SCRIPT_GET_LOCALIZER,                 (PVOID)GmlScriptGetLocalizerCallback                },
    { GML_SCRIPT_GET_WEATHER,                   (PVOID)GmlScriptGetWeatherCallback                  },
    { GML_SCRIPT_TRY_LOCATION_ID_TO_STRING,     (PVOID)GmlScriptTryLocationIdToStringCallback       },
    { GML_SCRIPT_ON_DUNGEON_ROOM_START,         (PVOID)GmlScriptOnDungeonRoomStartCallback          },
    { GML_SCRIPT_GO_TO_ROOM,                    (PVOID)GmlScriptGoToRoomCallback                    },
    { GML_SCRIPT_SETUP_MAIN_SCREEN,             (PVOID)GmlScriptSetupMainScreenCallback             },
    { GML_SCRIPT_GET_EQUIPMENT_BONUS_FROM,      (PVOID)GmlScriptGetEquipmentBonusFromCallback       },
    { GML_SCRIPT_HUD_SHOULD_SHOW,               (PVOID)GmlScriptHudShouldShowCallback               },
    { GML_SCRIPT_ON_DRAW_GUI,                   (PVOID)GmlScriptOnDrawGuiCallback                   },
    { GML_SCRIPT_DISPLAY_RESIZE,                (PVOID)GmlScriptDisplayResizeCallback               },
    { GML_SCRIPT_GET_ITEM_UI_ICON,              (PVOID)GmlScriptGetUiIconCallback                   },
    { GML_SCRIPT_UPDATE_TOOLBAR_MENU,           (PVOID)GmlScriptUpdateToolbarMenuCallback           },
    { GML_SCRIPT_CREATE_ITEM_PROTOTYPES,        (PVOID)GmlScriptCreateItemPrototypesCallback        },
    { GML_SCRIPT_SPAWN_LADDER,                  (PVOID)GmlScriptSpawnLadderCallback                 },
    { GML_SCRIPT_DESERIALIZE_LIVE_ITEM,         (PVOID)GmlScriptDeserializeLiveItemCallback         },
    { GML_SCRIPT_DESERIALIZE_INVENTORY,         (PVOID)GmlScriptDeserializeInventoryCallback        },
    { GML_SCRIPT_GET_TREASURE_FROM_DISTRIBUTION,(PVOID)GmlScriptGetTreasureFromDistributionCallback },
    { GML_SCRIPT_CRAFTING_MENU_INITIALIZE,      (PVOID)GmlScriptCraftingMenuInitializeCallback      },
    { GML_SCRIPT_CRAFTING_MENU_CLOSE,           (PVOID)GmlScriptCraftingMenuCloseCallback           },
    { GML_SCRIPT_JOURNAL_MENU_INITIALIZE,       (PVOID)GmlScriptJournalMenuInitializeCallback       },
    { GML_SCRIPT_JOURNAL_MENU_CLOSE,            (PVOID)GmlScriptJournalMenuCloseCallback            },
    { GML_SCRIPT_STORE_MENU_INITIALIZE,         (PVOID)GmlScriptStoreMenuInitializeCallback         },
    { GML_SCRIPT_STORE_MENU_CLOSE,              (PVOID)GmlScriptStoreMenuCloseCallback              },
    { GML_SCRIPT_VERTIGO_DRAW_WITH_COLOR,       (PVOID)GmlScriptVertigoDrawWithColorCallback        },
    { GML_SCRIPT_SCENE_AUDIO_PLAYER_PLAY,       (PVOID)GmlScriptSceneAudioPlayerPlayCallback        },
    { GML_SCRIPT_FIND_NPC_BLIP_NOISE,           (PVOID)GmlScriptFindNpcBlipNoiseCallback            },
    { GML_SCRIPT_SAVE_GAME,                     (PVOID)GmlScriptSaveGameCallback                    },
    { GML_SCRIPT_ARI_FACE_DIR,                  (PVOID)GmlScriptAriFaceDirCallback                  },
    { GML_SCRIPT_ON_BEGIN_STEP,                 (PVOID)GmlScriptOnBeginStepCallback                 },
    { GML_SCRIPT_RECIPE_GENERATE_INFUSIONS,     (PVOID)GmlScriptRecipeGenerateInfusionsCallback     },
    { GML_SCRIPT_BARK_EMITTER,                  (PVOID)GmlScriptBarkEmitterCallback                 },
    { GML_SCRIPT_T2_READ,                       (PVOID)GmlScriptT2ReadCallback                      },
    { GML_SCRIPT_GET_UNIFIED_TIME,              (PVOID)GmlScriptGetUnifiedTimeCallback              },
};

EXPORTED AurieStatus ModuleInitialize(
    IN AurieModule* Module,
    IN const fs::path& ModulePath
)
{
    UNREFERENCED_PARAMETER(ModulePath);

    AurieStatus status = AURIE_SUCCESS;

    status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)(g_ModuleInterface));
    if (!AurieSuccess(status))
        return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

    g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, MOD_VERSION);

    // Register object callback
    status = g_ModuleInterface->CreateCallback(g_ArSelfModule, EVENT_OBJECT_CALL, ObjectCallback, 0);
    if (!AurieSuccess(status))
    {
        g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, MOD_VERSION);
        return status;
    }

    // Register all script hooks
    for (const auto& entry : HOOK_TABLE)
    {
        status = RegisterHook(entry.name, entry.callback);
        if (!AurieSuccess(status))
        {
            g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, MOD_VERSION);
            return status;
        }
    }

    g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, MOD_VERSION);
    return AURIE_SUCCESS;
}
