#pragma once
#include "../Globals.h"
#include "../config/Config.h"
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
void DrawVignette();
void FadeInImage(double seconds_per_cycle, int repeat_count);
void DrawDashedBorder(float dash_len, float dash_thk, float speed, float screen_width, float screen_height, uint64_t current_time_ms);
void PlaySoundEffect(const char* sound_name, int priority, double gain);
bool FairyBuffIsActive();
double GetInvulnerabilityHits();
void SetInvulnerabilityHits(double amount);
void SetFireBreathTime(double value);
void DisableAllPerks();
bool ItemHasBeenAcquired(int item_id);
void ModifyMonsterPrototypes();
void LoadDungeonBiomeCandidateMonsters();
void LoadObjectIds();
void LoadItems();
void SetItemHealthModifier(int item_id, double health_modifier);
void SetItemStaminaModifier(int item_id, double stamina_modifier);
void SetItemManaModifier(int item_id, double mana_modifier);
void SetItemShopPrice(int item_id, int store_price);
void ModifyItems();
void RefreshPrototypes();
void MarkDungeonTutorialUnseen();
void ModifyMistpoolWeaponSprites();
void ModifyMistpoolPickaxeSprites();
void ModifyBarkSprites();
void ScaleMistpoolWeapon(bool in_dungeon);
void ScaleMistpoolArmor(bool in_dungeon);
void ScaleMistpoolPickaxe(bool in_dungeon);
void ScaleClassArmor(bool in_dungeon);
int GetRandomSoulStone();

struct ArmorSetBonuses
{
	struct Cleric {
		int equipped;
		bool AutoRegen() const { return equipped >= 1; }
		bool DivineSeal() const { return equipped >= 3; }
		bool AfflatusMisery() const { return equipped == 5; }
	} cleric;

	struct DarkKnight {
		int equipped;
		bool Drain() const { return equipped >= 1; }
		bool DarkSeal() const { return equipped >= 3; }
		bool SoulEater() const { return equipped == 5; }
	} dark_knight;

	struct Mage {
		int equipped;
		bool Aspir() const { return equipped >= 1; }
		bool Flood() const { return equipped >= 2; }
		bool ElementalSeal() const { return equipped >= 3; }
		bool Quake() const { return equipped >= 4; }
		bool ManaFont() const { return equipped == 5; }
	} mage;

	struct Paladin {
		int equipped;
		bool HolyCircle() const { return equipped >= 1; }
		bool Temperance() const { return equipped >= 3; }
		bool HallowedGround() const { return equipped == 5; }
	} paladin;

	struct Rogue {
		int equipped;
		bool Flee()    const { return equipped >= 1; }
		bool Hide()    const { return equipped >= 2; }
		bool SneakAttack() const { return equipped >= 3; }
		bool DisarmTrap()  const { return equipped >= 4; }
		bool TreasureHunter() const { return equipped == 5; }
	} rogue;

	struct Oracle {
		int equipped;
		bool FullSet() const { return equipped == 5; }
	} oracle;
};

ArmorSetBonuses GetArmorSetBonuses();
std::set<std::string> GetEquippedArmor();
std::map<Classes, int> CountEquippedClassArmor();
std::map<int, int> GetClassArmorInfusions();
bool CanAffordSpell(MMAPI::Spell::Ids spell);
int GetClericAutoRegenPotency();
double GetDarkKnightDrainPotency();
double GetPaladinHolyCirclePotency();
int ScaleTemperanceDamage(int current_health, int max_health, int damage);
ElementalSealEffects GetRandomElementalSealEffect();
void LoadSpells();
bool AriCurrentGmRoomIsDungeonFloor();
RValue GetDynamicItemSprite(int item_id);
RValue GetDynamicUiSprite(std::string sprite_name);
std::unordered_set<FloorEnchantments> RandomFloorEnchantments(bool is_first_floor, DungeonBiomes dungeon_biome);
std::vector<int> GenerateRandomMonstersIdsForCurrentFloor(int monsters_to_spawn, const int monster_id_to_exclude = -1);
int SelectRandomMonsterForAlteration();
int ModifyMaxHealth(int value);
void SetMaxHealth(int value);
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
void UnlockRecipe(int item_id);
void UnlockLiftKeyRecipe(CInstance* Self, CInstance* Other);
void TeleportAriToRoom(CInstance* Self, CInstance* Other, int location_id, int x_coordinate, int y_coordinate);
void LoadStalagmiteAttackData();
void TickTimeStoppedSystems(CInstance* Self, CInstance* Other, int64_t tick_delta);
void WriteChallengeModeFile();
bool ReadChallengeModeFile();
void RemoveItemsFromInventoryForChallengeMode();
void DropItemsForChallengeMode(CInstance* Self, CInstance* Other);
void UpdateChallengeModeProgress();
