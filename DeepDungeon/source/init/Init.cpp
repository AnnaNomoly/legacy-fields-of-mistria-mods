#include "Init.h"

#include <MMAPI/MMAPI.hpp>

// Forward declarations for hook callbacks (defined in the hooks/ files).
void OnObjAriCall(CInstance* self);
void OnObjMonsterCall(CInstance* self);
void BeforeCancelStatusEffect(MMAPI::StatusEffect::CancelStatusEffectContext& ctx);
void BeforeRegisterStatusEffect(MMAPI::StatusEffect::RegisterStatusEffectContext& ctx);
void BeforeHealthChange(MMAPI::Player::BeforeHealthChangeContext& ctx);
void AfterHealthChange(MMAPI::Player::AfterHealthChangeContext& ctx);
void BeforeStaminaChange(MMAPI::Player::BeforeStaminaChangeContext& ctx);
void BeforeMonsterSpawn(MMAPI::Monster::SpawnMonsterContext& ctx);
void AfterCanCastSpell(MMAPI::Spell::CanCastSpellContext& ctx);
void BeforeSpellCast(MMAPI::Spell::BeforeSpellCastContext& ctx);
void AfterSpellCast(MMAPI::Spell::AfterSpellCastContext& ctx);
void AfterMoveSpeed(MMAPI::Player::MoveSpeedContext& ctx);
void BeforeDamage(MMAPI::Damage::BeforeDamageContext& ctx);
void AfterDamage(MMAPI::Damage::AfterDamageContext& ctx);
void AfterTakePress(MMAPI::Input::TakePressContext& ctx);
void BeforeCheckValue(MMAPI::Input::CheckValueContext& ctx);
void BeforeAttemptInteract(MMAPI::Instance::AttemptInteractContext& ctx);
void BeforePlayText(MMAPI::Text::PlayTextContext& ctx);
void BeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx);
void BeforeUseItem(MMAPI::Item::UseItemContext& ctx);
void AfterHeldItem(MMAPI::Player::HeldItemContext& ctx);
void AfterUseAction(MMAPI::Player::AfterUseActionContext& ctx);
void BeforeDropItem(MMAPI::Item::DropItemContext& ctx);
void BeforeGiveItem(MMAPI::Item::GiveItemContext& ctx);
void AfterClockUpdate(MMAPI::Calendar::ClockUpdateContext& ctx);
void BeforeLocalizedString(MMAPI::Text::LocalizedStringContext& ctx);
void AfterLocalizedString(MMAPI::Text::AfterLocalizedStringContext& ctx);
void AfterGameActive();
void AfterDungeonRoomStart(MMAPI::Dungeon::DungeonRoomStartContext& ctx);
void BeforeGoToRoom(MMAPI::Location::BeforeGoToRoomContext& ctx);
void AfterGoToRoom(MMAPI::Location::AfterGoToRoomContext& ctx);
void BeforeSetupMainScreen();
void AfterGetEquipmentBonus(MMAPI::Equipment::EquipmentBonusContext& ctx);
void AfterHudShouldShow(MMAPI::Display::HudShouldShowContext& ctx);
void AfterDrawGui();
void AfterDisplayResize(MMAPI::Display::DisplayResizeContext& ctx);
void AfterGetUiIcon(MMAPI::Item::GetUiIconContext& ctx);
void AfterCreateItemPrototypes(MMAPI::Item::CreateItemPrototypesContext& ctx);
void BeforeSpawnLadder(MMAPI::Dungeon::SpawnLadderContext& ctx);
void BeforeGetTreasure(MMAPI::Item::GetTreasureContext& ctx);
void AfterCraftingMenuOpen();
void AfterCraftingMenuClose();
void AfterJournalMenuOpen();
void AfterJournalMenuClose();
void AfterStoreMenuOpen();
void AfterStoreMenuClose();
void BeforeVertigoDrawWithColor(MMAPI::Display::VertigoDrawWithColorContext& ctx);
void BeforePlayAudio(MMAPI::Game::PlayAudioContext& ctx);
void AfterFindBlipNoise(MMAPI::NPC::FindBlipNoiseContext& ctx);
void BeforeSaveGame(MMAPI::Game::SaveGameContext& ctx);
void AfterLoadGame(MMAPI::Game::LoadGameContext& ctx);
void BeforeFaceDir(MMAPI::Player::FaceDirContext& ctx);
void BeforeBeginStep();
void AfterGenerateInfusions(MMAPI::Recipe::GenerateInfusionsContext& ctx);
void BeforePlayHealVfx(MMAPI::Display::PlayHealVfxContext& ctx);

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

    // Initialize MMAPI. global_instance may not yet be populated at ModuleInitialize time;
    // DD's SetupMainScreen callback obtains it lazily on first fire. We pass whatever
    // GetGlobalInstance returns now (likely nullptr) — MMAPI tolerates a null global at
    // Initialize time, and we'll refresh it from SetupMainScreen before any MMAPI utility
    // that reads global_instance is called at runtime.
    {
        CInstance* mmapi_global = nullptr;
        g_ModuleInterface->GetGlobalInstance(&mmapi_global);
        MMAPI::Log::SetSinks(MMAPI::Log::Sinks::Console | MMAPI::Log::Sinks::File);
        MMAPI::Log::SetLevel(MMAPI::Log::Level::Trace);
        MMAPI::Initialize(g_ModuleInterface, mmapi_global, g_ArSelfModule, MOD_NAME, MOD_VERSION);
    }

    // MMAPI-managed hooks (the original HOOK_TABLE has been fully migrated).
    // Per-tick obj_ari / obj_monster handlers — replaces the legacy single EVENT_OBJECT_CALL
    // dispatcher. MMAPI's internal dispatcher handles null-check, pause-skip, and the
    // INSTANCE_OBJ_ARI latch automatically.
    MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, OnObjAriCall);

    // 0.2.0 removed the umbrella Objects::Monster in favor of per-subtype enums. The
    // original "obj_monster" string never resolved to a real object — this callback
    // wasn't firing under bundled 0.1.0. Expanding to every primary monster type
    // (plus Mimic, which OnObjMonsterCall specializes via monster_id) so the
    // dispatcher actually delivers ticks now. Excluded:
    //   - Barrel: a destructible, not a combat monster
    //   - *Projectile / *Bomb / Bat sonic variants / MiteSecondary / Stars:
    //     projectiles and secondary effects, no meaningful HP behavior
    const MMAPI::Instance::Objects monster_types[] = {
        MMAPI::Instance::Objects::MonsterBat,
        MMAPI::Instance::Objects::MonsterCat,
        MMAPI::Instance::Objects::MonsterClod,
        MMAPI::Instance::Objects::MonsterEnchantern,
        MMAPI::Instance::Objects::MonsterMimic,
        MMAPI::Instance::Objects::MonsterMite,
        MMAPI::Instance::Objects::MonsterRockStack,
        MMAPI::Instance::Objects::MonsterSap,
        MMAPI::Instance::Objects::MonsterShroom,
        MMAPI::Instance::Objects::MonsterSpirit,
        MMAPI::Instance::Objects::MonsterStatue,
        MMAPI::Instance::Objects::MonsterTome,
    };
    for (auto obj : monster_types)
        MMAPI::Instance::Hooks::OnObjectCall(obj, OnObjMonsterCall);
    MMAPI::Anchor::Hooks::BeforeBeginStep(BeforeBeginStep);
    MMAPI::NPC::Hooks::AfterFindBlipNoise(AfterFindBlipNoise);
    MMAPI::Equipment::Hooks::AfterGetEquipmentBonus(AfterGetEquipmentBonus);
    MMAPI::Damage::Hooks::BeforeDamage(BeforeDamage);
    MMAPI::Damage::Hooks::AfterDamage(AfterDamage);
    MMAPI::Crafting::Hooks::AfterMenuOpen(AfterCraftingMenuOpen);
    MMAPI::Crafting::Hooks::AfterMenuClose(AfterCraftingMenuClose);

    // Game batch — unlocks setup_main_screen and the Instance/Weather cascades.
    MMAPI::Game::Hooks::BeforeSetupMainScreen(BeforeSetupMainScreen);
    MMAPI::Game::Hooks::AfterLoadGame(AfterLoadGame);
    MMAPI::Game::Hooks::BeforeSaveGame(BeforeSaveGame);
    MMAPI::Game::Hooks::BeforePlayAudio(BeforePlayAudio);
    MMAPI::Game::Hooks::AfterJournalMenuOpen(AfterJournalMenuOpen);
    MMAPI::Game::Hooks::AfterJournalMenuClose(AfterJournalMenuClose);
    MMAPI::Game::Hooks::AfterStoreMenuOpen(AfterStoreMenuOpen);
    MMAPI::Game::Hooks::AfterStoreMenuClose(AfterStoreMenuClose);
    MMAPI::Game::Hooks::AfterGameActive(AfterGameActive);
    MMAPI::Instance::Hooks::BeforeAttemptInteract(BeforeAttemptInteract);

    // Latcher-only modules — Enable() installs the internal latches that replace
    // the deleted DD-side latcher hooks (VitalsMenuSetMaxHealth, T2Read, BarkEmitter,
    // UpdateToolbarMenu, DeserializeInventory).
    MMAPI::VitalsMenu::Enable();
    MMAPI::T2::Enable();
    MMAPI::Bark::Enable();
    MMAPI::ToolbarMenu::Enable();
    MMAPI::Inventory::Enable();

    // Calendar batch (replaces hooks/dungeon/GetMinutes.cpp + deleted GetUnifiedTime latcher).
    MMAPI::Calendar::Hooks::AfterClockUpdate(AfterClockUpdate);

    // Input batch.
    MMAPI::Input::Hooks::AfterTakePress(AfterTakePress);
    MMAPI::Input::Hooks::BeforeCheckValue(BeforeCheckValue);

    // Spell batch.
    MMAPI::Spell::Hooks::AfterCanCastSpell(AfterCanCastSpell);
    MMAPI::Spell::Hooks::BeforeSpellCast(BeforeSpellCast);
    MMAPI::Spell::Hooks::AfterSpellCast(AfterSpellCast);

    // StatusEffect batch (the third hook, manager_update, was a pure latcher and is gone).
    MMAPI::StatusEffect::Hooks::BeforeRegisterStatusEffect(BeforeRegisterStatusEffect);
    MMAPI::StatusEffect::Hooks::BeforeCancelStatusEffect(BeforeCancelStatusEffect);

    // Player batch.
    MMAPI::Player::Hooks::BeforeHealthChange(BeforeHealthChange);
    MMAPI::Player::Hooks::AfterHealthChange(AfterHealthChange);
    MMAPI::Player::Hooks::BeforeStaminaChange(BeforeStaminaChange);
    MMAPI::Player::Hooks::AfterMoveSpeed(AfterMoveSpeed);
    MMAPI::Player::Hooks::AfterHeldItem(AfterHeldItem);
    MMAPI::Player::Hooks::AfterUseActionComplete(AfterUseAction);
    MMAPI::Player::Hooks::BeforeFaceDir(BeforeFaceDir);

    // Text batch.
    MMAPI::Text::Hooks::BeforePlayText(BeforePlayText);
    MMAPI::Text::Hooks::BeforePlayConversation(BeforePlayConversation);
    MMAPI::Text::Hooks::BeforeLocalizedString(BeforeLocalizedString);
    MMAPI::Text::Hooks::AfterLocalizedString(AfterLocalizedString);

    // Display batch.
    MMAPI::Display::Hooks::AfterHudShouldShow(AfterHudShouldShow);
    MMAPI::Display::Hooks::AfterDrawGui(AfterDrawGui);
    MMAPI::Display::Hooks::AfterDisplayResize(AfterDisplayResize);
    MMAPI::Display::Hooks::BeforeVertigoDrawWithColor(BeforeVertigoDrawWithColor);
    MMAPI::Display::Hooks::BeforePlayHealVfx(BeforePlayHealVfx);

    // Item batch.
    MMAPI::Item::Hooks::BeforeUseItem(BeforeUseItem);
    MMAPI::Item::Hooks::BeforeDropItem(BeforeDropItem);
    MMAPI::Item::Hooks::BeforeGiveItem(BeforeGiveItem);
    MMAPI::Item::Hooks::AfterGetUiIcon(AfterGetUiIcon);
    MMAPI::Item::Hooks::AfterCreateItemPrototypes(AfterCreateItemPrototypes);
    MMAPI::Item::Hooks::BeforeGetTreasure(BeforeGetTreasure);

    // Recipe batch (now safe: Recipe::Enable cascades through Item, which is enabled above).
    MMAPI::Recipe::Hooks::AfterGenerateInfusions(AfterGenerateInfusions);

    // Dungeon / Location batch (TryLocationIdToString.cpp deleted — was_in_dungeon_floor
    // is now captured in AfterGoToRoom before ari_current_gm_room is updated).
    MMAPI::Location::Hooks::BeforeGoToRoom(BeforeGoToRoom);
    MMAPI::Location::Hooks::AfterGoToRoom(AfterGoToRoom);
    MMAPI::Dungeon::Hooks::BeforeSpawnLadder(BeforeSpawnLadder);
    MMAPI::Dungeon::Hooks::AfterDungeonRoomStart(AfterDungeonRoomStart);

    // Monster batch.
    MMAPI::Monster::Hooks::BeforeMonsterSpawn(BeforeMonsterSpawn);

    MMAPI::Log::DumpDependencyGraphTree();
    g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, MOD_VERSION);
    return AURIE_SUCCESS;
}
