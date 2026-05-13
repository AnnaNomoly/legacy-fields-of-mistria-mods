#include <algorithm>
#include <unordered_set>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "MagicalGirlAri";
static const char* const VERSION = "1.1.0";

// Config keys
static const char* const EASTER_EGG_REQUIREMENT_KEY       = "easter_egg_set_piece_requirement";
static const char* const BONUS_DAILY_MANA_REQUIREMENT_KEY = "bonus_daily_mana_set_piece_requirement";
static const char* const VANQUISH_ENEMY_REQUIREMENT_KEY   = "vanquish_enemy_set_piece_requirement";
static const char* const HEALTH_RECOVERY_REQUIREMENT_KEY  = "health_recovery_set_piece_requirement";

// Defaults
static const int DEFAULT_EASTER_EGG_REQUIREMENT       = 3;
static const int DEFAULT_BONUS_DAILY_MANA_REQUIREMENT = 3;
static const int DEFAULT_VANQUISH_ENEMY_REQUIREMENT   = 3;
static const int DEFAULT_HEALTH_RECOVERY_REQUIREMENT  = 3;

// Localization keys
static const std::string JUNIPER_EASTER_EGG_CONVERSATION_KEY = "Conversations/Mods/Magical Girl Ari/juniper_contract";

// Asset names
static const std::string MANA_POTION_ITEM_NAME = "mana_potion";
static const std::string HEART_SWORD_NAME      = "sword_verdigris";

// Core set pieces — outfit (dress/suit interchangeable) + footwear (heels/boots interchangeable).
static const std::unordered_set<std::string> CORE_SET_PIECE_NAMES = {
	"sailor_mistria",        // dress
	"sailor_mistria_m",      // suit
	"sailor_mistria_heels",
	"sailor_mistria_boots",
};

// Additional set pieces — hair + accessory variants.
static const std::unordered_set<std::string> ADDITIONAL_SET_PIECE_NAMES = {
	"princess_twintails",
	"serene_bun",
	"rini_bun",
	"princess_brioche",
	"sailor_mistria_pins",
	"sailor_mistria_pins_b",
	"sailor_mistria_pins_c",
};

struct MagicalGirlAriConfig
{
	int easter_egg_requirement       = DEFAULT_EASTER_EGG_REQUIREMENT;
	int bonus_daily_mana_requirement = DEFAULT_BONUS_DAILY_MANA_REQUIREMENT;
	int vanquish_enemy_requirement   = DEFAULT_VANQUISH_ENEMY_REQUIREMENT;
	int health_recovery_requirement  = DEFAULT_HEALTH_RECOVERY_REQUIREMENT;
};

void to_json(json& j, const MagicalGirlAriConfig& c)
{
	j = json{
		{ EASTER_EGG_REQUIREMENT_KEY,       c.easter_egg_requirement       },
		{ BONUS_DAILY_MANA_REQUIREMENT_KEY, c.bonus_daily_mana_requirement },
		{ VANQUISH_ENEMY_REQUIREMENT_KEY,   c.vanquish_enemy_requirement   },
		{ HEALTH_RECOVERY_REQUIREMENT_KEY,  c.health_recovery_requirement  },
	};
}

void from_json(const json& j, MagicalGirlAriConfig& c)
{
	c.easter_egg_requirement       = MMAPI::Config::GetValue<int>(j, EASTER_EGG_REQUIREMENT_KEY,       DEFAULT_EASTER_EGG_REQUIREMENT,       2, 3);
	c.bonus_daily_mana_requirement = MMAPI::Config::GetValue<int>(j, BONUS_DAILY_MANA_REQUIREMENT_KEY, DEFAULT_BONUS_DAILY_MANA_REQUIREMENT, 2, 3);
	c.vanquish_enemy_requirement   = MMAPI::Config::GetValue<int>(j, VANQUISH_ENEMY_REQUIREMENT_KEY,   DEFAULT_VANQUISH_ENEMY_REQUIREMENT,   0, 3);
	c.health_recovery_requirement  = MMAPI::Config::GetValue<int>(j, HEALTH_RECOVERY_REQUIREMENT_KEY,  DEFAULT_HEALTH_RECOVERY_REQUIREMENT,  0, 3);
}

// ----- State -----

static MagicalGirlAriConfig config = {};
static bool startup_loaded                = false;
static bool game_is_active                = false;
static bool is_new_day                    = false;
static bool modify_juniper_conversation   = false;
static bool heart_sword_equipped          = false;
static int  ari_health_recovery_pending   = 0;
static int  core_set_pieces_equipped      = 0;
static int  additional_set_pieces_equipped = 0;
static int  mana_potion_item_id           = -1;
static int  heart_sword_item_id           = -1;
static int  original_heart_sword_damage   = 10;
static std::vector<int> default_spell_costs;  // indexed by spell id

// ----- Helpers -----

void LoadOrCreateConfigFile()
{
	try
	{
		auto path = MMAPI::Config::GetConfigPath(MOD_NAME);
		bool existed = std::filesystem::exists(path);
		json j = MMAPI::Config::Load(path);

		if (!existed)
			MMAPI::Log::Warn("Configuration file was not found. Creating file: %s", path.string().c_str());

		if (j.empty())
		{
			if (existed)
				MMAPI::Log::Error("No readable values found in mod configuration file: %s!", path.string().c_str());
			config = MagicalGirlAriConfig{};
		}
		else
		{
			config = j.get<MagicalGirlAriConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = MagicalGirlAriConfig{};
	}
}

void LoadDefaultSpellCosts()
{
	default_spell_costs.clear();
	default_spell_costs.reserve(MMAPI::Spell::IdCount);
	for (int i = 0; i < MMAPI::Spell::IdCount; i++)
	{
		YYTK::RValue cost = MMAPI::Spell::GetCost(static_cast<MMAPI::Spell::Ids>(i));
		default_spell_costs.push_back(MMAPI::Engine::IsNumeric(cost) ? static_cast<int>(cost.ToInt64()) : 0);
	}
}

void LoadOriginalHeartSwordDamage()
{
	YYTK::RValue dmg = MMAPI::Item::GetDamage(heart_sword_item_id);
	if (MMAPI::Engine::IsNumeric(dmg))
		original_heart_sword_damage = static_cast<int>(dmg.ToInt64());
}

// Applies a spell-cost reduction across all spells. Reduction = number of core set pieces equipped, capped at 2.
void ApplySpellCostReduction(int reduction)
{
	if (default_spell_costs.empty()) return;
	for (int i = 0; i < MMAPI::Spell::IdCount; i++)
	{
		int new_cost = (std::max)(0, default_spell_costs[i] - reduction);
		MMAPI::Spell::SetCost(static_cast<MMAPI::Spell::Ids>(i), new_cost);
	}
}

// Counts the player's currently-equipped set pieces. Two outputs: (core, additional).
std::pair<int, int> CountEquippedSetPieces()
{
	int core = 0;
	int additional = 0;
	for (const auto& name : CORE_SET_PIECE_NAMES)
		if (MMAPI::Cosmetic::IsEquipped(name)) core++;
	for (const auto& name : ADDITIONAL_SET_PIECE_NAMES)
		if (MMAPI::Cosmetic::IsEquipped(name)) additional++;
	return { core, additional };
}

int TotalSetPieces() { return core_set_pieces_equipped + additional_set_pieces_equipped; }

// ----- Hooks -----

void OnSetupMainScreen()
{
	if (!startup_loaded)
	{
		// First-time setup: load reference data at title screen so it's ready before gameplay.
		LoadOrCreateConfigFile();
		LoadDefaultSpellCosts();

		YYTK::RValue mana_potion_rv = MMAPI::Item::GetIdFromInternalName(MANA_POTION_ITEM_NAME);
		if (MMAPI::Engine::IsNumeric(mana_potion_rv))
			mana_potion_item_id = static_cast<int>(mana_potion_rv.ToInt64());

		YYTK::RValue heart_sword_rv = MMAPI::Item::GetIdFromInternalName(HEART_SWORD_NAME);
		if (MMAPI::Engine::IsNumeric(heart_sword_rv))
			heart_sword_item_id = static_cast<int>(heart_sword_rv.ToInt64());

		LoadOriginalHeartSwordDamage();
		startup_loaded = true;
	}

	// Return-to-title reset.
	game_is_active = false;
	is_new_day = false;
	modify_juniper_conversation = false;
	heart_sword_equipped = false;
	ari_health_recovery_pending = 0;
	core_set_pieces_equipped = 0;
	additional_set_pieces_equipped = 0;
}

void OnGameActive()
{
	game_is_active = true;
}

void OnBeforeNewDay()
{
	is_new_day = true;
}

void OnAfterHeldItem(MMAPI::Player::HeldItemContext& ctx)
{
	heart_sword_equipped = (ctx.GetItemId() == heart_sword_item_id);
}

void OnBeforeDamage(MMAPI::Damage::BeforeDamageContext& ctx)
{
	// Vanquish Enemy: with the heart sword equipped and enough set pieces, every non-Ari hit that
	// would deal damage becomes a one-shot critical.
	if (!heart_sword_equipped) return;
	if (TotalSetPieces() < config.vanquish_enemy_requirement) return;

	if (ctx.IsTargetAri()) return;  // Don't one-shot Ari herself.
	if (ctx.IsMiss())      return;  // Don't crit a guaranteed miss.

	ctx.SetCritical(true);
	ctx.SetAmount(9999.0);
}

void OnBeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx)
{
	if (!game_is_active) return;
	if (!modify_juniper_conversation) return;

	std::string_view key = ctx.GetKey();
	if (key.find("gift_lines") != std::string_view::npos
	    && key.find("Conversations/Bank/Juniper") != std::string_view::npos)
	{
		modify_juniper_conversation = false;
		ctx.SetKey(JUNIPER_EASTER_EGG_CONVERSATION_KEY);
	}
}

void OnBeforeReceiveGift(CInstance* npc, int item_id)
{
	if (!npc || !npc->m_Object || !npc->m_Object->m_Name) return;
	if (std::string_view(npc->m_Object->m_Name) != "obj_juniper") return;
	if (item_id != mana_potion_item_id) return;

	if (TotalSetPieces() >= config.easter_egg_requirement)
		modify_juniper_conversation = true;
}

void OnBeforeManaChange(MMAPI::Player::BeforeManaChangeContext& ctx)
{
	// On the first mana change of a new day, grant +1 bonus mana if enough set pieces are worn.
	if (!is_new_day) return;
	is_new_day = false;

	if (TotalSetPieces() >= config.bonus_daily_mana_requirement)
		ctx.SetAmount(ctx.GetAmount() + 1);
}

void OnAriTick(CInstance* self)
{
	if (!game_is_active) return;

	// Scale the heart sword's damage by Ari's current mana (regardless of whether it's equipped —
	// the change applies to the item template so an unequip → equip cycle reflects current mana).
	YYTK::RValue mana_rv = MMAPI::Player::GetMana();
	int current_mana = MMAPI::Engine::IsNumeric(mana_rv) ? static_cast<int>(mana_rv.ToInt64()) : 0;
	MMAPI::Item::SetDamage(heart_sword_item_id, static_cast<double>(original_heart_sword_damage + current_mana));

	// Update equipped-set-piece counts.
	auto [core, additional] = CountEquippedSetPieces();
	core_set_pieces_equipped = core;
	additional_set_pieces_equipped = additional;

	// Spell cost reduction: -1 per core set piece, capped at -2.
	ApplySpellCostReduction((std::min)(2, core_set_pieces_equipped));

	// Health Recovery: consume any pending heal queued from a monster death.
	if (ari_health_recovery_pending > 0)
	{
		MMAPI::Player::ModifyHealth(ari_health_recovery_pending);
		ari_health_recovery_pending = 0;
	}
}

void OnMonsterTick(CInstance* self)
{
	// Both Vanquish Enemy and Health Recovery only apply when wielding the heart sword.
	if (!heart_sword_equipped) return;

	YYTK::RValue self_rv = self->ToRValue();
	if (!MMAPI::Engine::StructVariableExists(self_rv, "hit_points")) return;
	YYTK::RValue hp = self_rv.GetMember("hit_points");
	if (!MMAPI::Engine::IsNumeric(hp)) return;

	int total = TotalSetPieces();

	// Vanquish Enemy: if HP has dropped from its original value (i.e. we hit it once), zero it out.
	// Caches the first-observed HP on the monster struct so we know when it's been damaged.
	if (total >= config.vanquish_enemy_requirement
	    && MMAPI::Engine::StructVariableExists(self_rv, "config"))
	{
		if (!MMAPI::Engine::StructVariableExists(self_rv, "__magical_girl_ari__vanquish_hit_points"))
			MMAPI::Engine::StructVariableSet(self_rv, "__magical_girl_ari__vanquish_hit_points", hp);

		YYTK::RValue original_hp = self_rv.GetMember("__magical_girl_ari__vanquish_hit_points");
		if (MMAPI::Engine::IsNumeric(original_hp) && hp.ToDouble() != original_hp.ToDouble())
			*self_rv.GetRefMember("hit_points") = 0.0;
	}

	// Health Recovery: queue +10 HP on the next obj_ari tick the first time we see this monster at hp<=0.
	if (total >= config.health_recovery_requirement
	    && !MMAPI::Engine::StructVariableExists(self_rv, "__magical_girl_ari__processed_monster_death")
	    && hp.ToDouble() <= 0)
	{
		MMAPI::Engine::StructVariableSet(self_rv, "__magical_girl_ari__processed_monster_death", true);
		ari_health_recovery_pending += 10;
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)(module_interface));
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Damage::Enable();
	MMAPI::Item::Enable();
	MMAPI::NPC::Enable();
	MMAPI::Player::Enable();
	MMAPI::Spell::Enable();
	MMAPI::Text::Enable();
	MMAPI::Game::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::BeforeNewDay(OnBeforeNewDay);
	MMAPI::Player::Hooks::AfterHeldItem(OnAfterHeldItem);
	MMAPI::Player::Hooks::BeforeManaChange(OnBeforeManaChange);
	MMAPI::Damage::Hooks::BeforeDamage(OnBeforeDamage);
	MMAPI::Text::Hooks::BeforePlayConversation(OnBeforePlayConversation);
	MMAPI::NPC::Hooks::BeforeReceiveGift(OnBeforeReceiveGift);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari,     OnAriTick);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Monster, OnMonsterTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
