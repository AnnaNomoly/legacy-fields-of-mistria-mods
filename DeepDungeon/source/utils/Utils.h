#pragma once
#include "../Globals.h"
#include "../config/Config.h"
#include "../constants/GmlScripts.h"
#include "../constants/ItemNames.h"
#include "../constants/Keys.h"
#include "../constants/Timing.h"
#include "../constants/ArmorSets.h"
#include "../constants/EnchantmentGroups.h"
#include "../data/TrapSpawnPoints.h"

// ============================================================
// Utility / game-interaction function forward declarations
// ============================================================
void PrintError(std::exception_ptr eptr);
void ResetFade();
void ResetCustomDrawFields();
bool GameIsPaused();
bool IsNumeric(RValue value);
bool IsObject(RValue value);
bool StructVariableExists(RValue the_struct, const char* variable_name);
RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value);
void StructVariableRemove(RValue the_struct, const char* variable_name);
bool GlobalVariableExists(const char* variable_name);
RValue GlobalVariableGet(const char* variable_name);
RValue GlobalVariableSet(const char* variable_name, RValue value);
void CreateOrGetGlobalYYTKVariable();
void CreateModInfoInGlobalYYTKVariable();
double GetWindowWidth();
double GetWindowHeight();
uint64_t GetCurrentSystemTime();
int euclidean_modulo(int a, int m);
std::vector<double> linspace(double start, double end, std::size_t num);
double round_n(double value, int decimals);
std::complex<double> round_complex(const std::complex<double>& z, int decimals);
double GetDistance(int x1, int y1, int x2, int y2);
std::pair<int, int> GetVector(int x1, int y1, int x2, int y2);
int CalculateMeteorDamage(double distance);
bool FacingTrap(int ariX, int ariY, int trapX, int trapY);
std::pair<int, int> GetCenterOffset(int screen_center_x, int screen_center_y, int image_width, int image_height);
std::vector<double> GetCenter(std::vector<double> topLeft, std::vector<double> bottomRight);
void DrawRectangle(int color, float x1, float y1, float x2, float y2, bool outline);
void DrawImage(int x, int y, int transparency);
void FadeInImage(double seconds_per_cycle, int repeat_count);
void DrawDashedBorder(float dash_len, float dash_thk, float speed, float screen_width, float screen_height, uint64_t current_time_ms);
void PlaySoundEffect(const char* sound_name, int priority, double gain);
void LoadPerks();
bool FairyBuffIsActive();
double GetInvulnerabilityHits();
void SetInvulnerabilityHits(double amount);
void SetFireBreathTime(double value);
void DisableAllPerks();
bool ItemHasBeenAcquired(int item_id);
void LoadTutorials();
void LoadPlayerStates();
void LoadMonsterStates();
void LoadBarkData();
void LoadStatusEffects();
void LoadLocations();
void LoadInfusions();
void LoadMonsters();
void ModifyMonsterPrototypes();
void LoadDungeonBiomeCandidateMonsters();
void LoadObjectIds();
void LoadItems();
void SetItemHealthModifier(int item_id, double health_modifier);
void SetItemStaminaModifier(int item_id, double stamina_modifier);
void SetItemManaModifier(int item_id, double mana_modifier);
void SetItemShopPrice(int item_id, int store_price);
void ModifyItems();
void MarkDungeonTutorialUnseen();
void ModifyMistpoolWeaponSprites();
void ModifyMistpoolPickaxeSprites();
void ModifyBarkSprites();
void ScaleMistpoolWeapon(bool in_dungeon);
void ScaleMistpoolArmor(bool in_dungeon);
void ScaleMistpoolPickaxe(bool in_dungeon);
void ScaleClassArmor(bool in_dungeon);
int GetRandomSoulStone();
std::map<Classes, int> CountEquippedClassArmor();
std::map<int, int> GetClassArmorInfusions();
int GetClericAutoRegenPotency();
double GetDarkKnightDrainPotency();
double GetPaladinHolyCirclePotency();
int ScaleTemperanceDamage(int current_health, int max_health, int damage);
ElementalSealEffects GetRandomElementalSealEffect();
void LoadSpellIds();
void LoadSpells();
RValue LocalizeString(CInstance* Self, CInstance* Other, std::string localization_key);
RValue T2Read(CInstance* Self, CInstance* Other, std::string key);
RValue DeserializeLiveItem(CInstance* Self, CInstance* Other);
RValue InventoryCountItem(int item_id, CInstance* Self, CInstance* Other);
bool AriCurrentGmRoomIsDungeonFloor();
RValue GetDynamicItemSprite(int item_id);
RValue GetDynamicUiSprite(std::string sprite_name);
std::unordered_set<FloorEnchantments> RandomFloorEnchantments(bool is_first_floor, DungeonBiomes dungeon_biome);
std::vector<int> GenerateRandomMonstersIdsForCurrentFloor(int monsters_to_spawn, const int monster_id_to_exclude = -1);
int SelectRandomMonsterForAlteration();
RValue GetUnifiedTime(CInstance* Self, CInstance* Other);
RValue GetHealth(CInstance* Self, CInstance* Other);
RValue GetMaxHealth(CInstance* Self, CInstance* Other);
int ModifyMaxHealth(CInstance* Self, CInstance* Other, int value);
RValue GetStamina(CInstance* Self, CInstance* Other);
RValue GetMana(CInstance* Self, CInstance* Other);
void UpdateToolbarMenu(CInstance* Self, CInstance* Other);
void CreateNotification(bool ignore_cooldown, std::string notification_localization_str, CInstance* Self, CInstance* Other);
void CloseTextbox(CInstance* Self, CInstance* Other);
void SpawnTutorial(std::string tutorial_name, CInstance* Self, CInstance* Other);
void PlayConversation(std::string conversation_localization_str, CInstance* Self, CInstance* Other);
void EmitBark(CInstance* Self, CInstance* Other, RValue bark_id, RValue bark_type);
void SceneAudioPlayerStop(CInstance* Self, CInstance* Other);
void EnterDungeon(double dungeon_level, CInstance* Self, CInstance* Other);
void InventoryRemoveItem(int item_id, int quantity, CInstance* Self, CInstance* Other);
void SetHealth(CInstance* Self, CInstance* Other, int value);
void ModifyHealth(CInstance* Self, CInstance* Other, int value);
void SetMaxHealth(CInstance* Self, CInstance* Other, int value);
void VitalsMenuSetHealth(CInstance* Self, CInstance* Other, int current_health_value, int max_health_value);
void VitalsMenuSetMaxHealth(CInstance* Self, CInstance* Other, int value);
void ModifyStamina(CInstance* Self, CInstance* Other, int value);
void ModifyMana(CInstance* Self, CInstance* Other, int value);
void SpawnMonster(CInstance* Self, CInstance* Other, int room_x, int room_y, int monster_id);
void SpawnLadder(CInstance* Self, CInstance* Other, int64_t x_coord, int64_t y_coord);
void CancelStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id);
void CancelAllStatusEffects();
void RegisterStatusEffect(CInstance* Self, CInstance* Other, RValue status_effect_id, RValue amount, RValue start, RValue finish);
void CastSpell(CInstance* Self, CInstance* Other, int spell_id);
void DropItem(int item_id, double x_coord, double y_coord, CInstance* Self, CInstance* Other);
void DropLiftKey();
void ModifySpellCosts(bool reset_cost, bool in_dungeon);
void SetFloorNumber();
void GenerateFloorTraps();
void GenerateTreasureSpot(CInstance* Self, CInstance* Other);
void SpawnDreadBeast(CInstance* Self, CInstance* Other);
void SelectDreadBeast(CInstance* Self, CInstance* Other);
void RevealFloorTraps();
void ApplyFloorTraps(CInstance* Self, CInstance* Other);
void ProcessCustomAOEs();
void ProcessTreasureSpot(CInstance* Self, CInstance* Other);
void ProcessSpiritConcealment();
void ApplyOfferingPenalties(CInstance* Self, CInstance* Other);
void TrackAriResources(CInstance* Self, CInstance* Other);
void GenerateTreasureChestLoot(std::string object_name, CInstance* Self, CInstance* Other);
void ResetStaticFields(bool returned_to_title_screen);
void UnlockRecipe(int item_id, CInstance* Self, CInstance* Other);
void UnlockLiftKeyRecipe(CInstance* Self, CInstance* Other);
void TeleportAriToRoom(CInstance* Self, CInstance* Other, int location_id, int x_coordinate, int y_coordinate);
void LoadStalagmiteAttackData();
