#include <cmath>
#include <map>
#include <string>
#include <unordered_set>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;

// ----- Mod metadata -----

static const char* const MOD_NAME = "FauxSigils";
static const char* const VERSION  = "1.1.0";

// ----- Localization keys -----

static const std::string SIGIL_RESTRICTED_NOTIFICATION_KEY = "Notifications/Mods/Faux Sigils/sigil_restricted";
static const std::string SIGIL_LIMIT_NOTIFICATION_KEY      = "Notifications/Mods/Faux Sigils/sigil_limit";
static const std::string CONCEALMENT_LOST_NOTIFICATION_KEY = "Notifications/Mods/Faux Sigils/Sigils/concealment/deactivated";

// ----- Sigil definitions -----

enum class Sigil
{
	Concealment,
	Fortification,
	Fortune,
	Protection,
	Rage,
	Redemption,
	Silence,
	Strength,
};

struct SigilDef
{
	std::string item_name;       // Internal item name (recipe_key).
	std::string sprite_normal;   // UI icon when the sigil is available.
	std::string sprite_disabled; // UI icon when the sigil is currently unusable (active or out-of-dungeon).
};

static const std::map<Sigil, SigilDef> SIGIL_DEFS = {
	{ Sigil::Concealment,   { "faux_sigil_of_concealment",   "spr_ui_item_faux_sigil_of_concealment",   "spr_ui_item_faux_sigil_of_concealment_disabled"   }},
	{ Sigil::Fortification, { "faux_sigil_of_fortification", "spr_ui_item_faux_sigil_of_fortification", "spr_ui_item_faux_sigil_of_fortification_disabled" }},
	{ Sigil::Fortune,       { "faux_sigil_of_fortune",       "spr_ui_item_faux_sigil_of_fortune",       "spr_ui_item_faux_sigil_of_fortune_disabled"       }},
	{ Sigil::Protection,    { "faux_sigil_of_protection",    "spr_ui_item_faux_sigil_of_protection",    "spr_ui_item_faux_sigil_of_protection_disabled"    }},
	{ Sigil::Rage,          { "faux_sigil_of_rage",          "spr_ui_item_faux_sigil_of_rage",          "spr_ui_item_faux_sigil_of_rage_disabled"          }},
	{ Sigil::Redemption,    { "faux_sigil_of_redemption",    "spr_ui_item_faux_sigil_of_redemption",    "spr_ui_item_faux_sigil_of_redemption_disabled"    }},
	{ Sigil::Silence,       { "faux_sigil_of_silence",       "spr_ui_item_faux_sigil_of_silence",       "spr_ui_item_faux_sigil_of_silence_disabled"       }},
	{ Sigil::Strength,      { "faux_sigil_of_strength",      "spr_ui_item_faux_sigil_of_strength",      "spr_ui_item_faux_sigil_of_strength_disabled"      }},
};

// ----- State -----

// Sigil ↔ item_id lookups, populated once at title-screen setup.
static std::map<Sigil, int> sigil_to_item_id;
static std::map<int, Sigil> item_id_to_sigil;

// Sigils currently active for the player. Cleared on floor transition (and on return-to-title).
static std::unordered_set<Sigil> active_sigils;

// One-shot: recipe-unlock all sigils when the game becomes interactive each session.
static bool recipes_unlocked = false;

// When the player triggers Sigil of Silence on floor N, monster spawns are suppressed on floor N+1
// (the silence carries through the room transition to the next floor's spawn pass, then clears).
static bool sigil_silence_armed = false;

// Set by BeforeUseItem when a sigil item-use passes all cancellation guards, consumed by
// AfterUseActionComplete to commit the activation. Mirrors the original mod's `sigil_item_used` flag.
static bool sigil_use_pending = false;

// ----- Helpers -----

bool IsInDungeonFloor()
{
	return MMAPI::Location::IsCurrentLocation(MMAPI::Location::Ids::Dungeon);
}

// Returns true if Ari currently has the Fairy status effect active (Sigil of Redemption's revive buff).
bool FairyBuffIsActive()
{
	YYTK::RValue ari = MMAPI::Internal::global_instance->GetMember("__ari");
	YYTK::RValue status_effects = ari.GetMember("status_effects");
	YYTK::RValue effects        = status_effects.GetMember("effects");
	YYTK::RValue inner          = effects.GetMember("inner");

	std::string key = std::to_string(static_cast<int>(MMAPI::StatusEffect::Ids::Fairy));
	if (!MMAPI::Engine::StructVariableExists(inner, key.c_str()))
		return false;
	return inner.GetMember(key.c_str()).m_Kind == YYTK::VALUE_OBJECT;
}

// Picks the sprite a sigil item should display right now — disabled variant when the sigil is
// active, when an equivalent buff already covers Ari, or when she's not on a dungeon floor.
YYTK::RValue GetDynamicSigilSprite(int item_id)
{
	auto it = item_id_to_sigil.find(item_id);
	if (it == item_id_to_sigil.end()) return {};
	Sigil sigil = it->second;
	const SigilDef& def = SIGIL_DEFS.at(sigil);

	bool unavailable = active_sigils.contains(sigil) || !IsInDungeonFloor();

	if (sigil == Sigil::Protection)
	{
		YYTK::RValue hits = MMAPI::Player::GetInvulnerabilityHits();
		if (MMAPI::Engine::IsNumeric(hits) && hits.ToInt64() > 0)
			unavailable = true;
	}
	if (sigil == Sigil::Redemption && FairyBuffIsActive())
		unavailable = true;

	// Original gated the disabled sprite on `!GameIsPaused()` so the pause menu always shows the
	// "available" icon — preserved here so the pause-menu icon snapshot doesn't suddenly change.
	if (!MMAPI::Game::IsPaused() && unavailable)
		return MMAPI::Engine::AssetGetIndex(def.sprite_disabled);
	return MMAPI::Engine::AssetGetIndex(def.sprite_normal);
}

// Applies the sigil's effect (status effect registration, ladder spawn, etc.). Always invoked from
// the AfterUseActionComplete edge, so the item has already been consumed by the time we get here.
void ActivateSigil(Sigil sigil, CInstance* obj_ari)
{
	active_sigils.insert(sigil);

	switch (sigil)
	{
		case Sigil::Fortune:
		{
			if (!obj_ari) break;
			YYTK::RValue ari_rv = obj_ari->ToRValue();
			int64_t x = static_cast<int64_t>(ari_rv.GetMember("x").ToDouble());
			int64_t y = static_cast<int64_t>(ari_rv.GetMember("y").ToDouble());
			MMAPI::Dungeon::SpawnLadder(x, y);
			break;
		}
		case Sigil::Protection:
			MMAPI::StatusEffect::RegisterPersistent(MMAPI::StatusEffect::Ids::GuardiansShield);
			MMAPI::Player::ModifyInvulnerabilityHits(2);
			break;
		case Sigil::Redemption:
			MMAPI::StatusEffect::RegisterPersistent(MMAPI::StatusEffect::Ids::Fairy);
			break;
		default:
			break;
	}

	MMAPI::ToolbarMenu::ForceUpdate();
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	// Return-to-title resets: all transient state goes back to zero. Sigil id lookups (which depend
	// only on the game's item data) are populated once on the first title fire and kept.
	active_sigils.clear();
	recipes_unlocked    = false;
	sigil_silence_armed = false;
	sigil_use_pending   = false;

	if (!sigil_to_item_id.empty()) return;

	for (const auto& [sigil, def] : SIGIL_DEFS)
	{
		YYTK::RValue id_rv = MMAPI::Item::GetIdFromInternalName(def.item_name);
		if (!MMAPI::Engine::IsNumeric(id_rv)) continue;

		int item_id = static_cast<int>(id_rv.ToInt64());
		sigil_to_item_id[sigil]   = item_id;
		item_id_to_sigil[item_id] = sigil;

		// Sigil items inherit their health_modifier from food categorization; zero it so eating one
		// doesn't visibly nudge Ari's HP bar. The actual sigil effects fire from AfterUseActionComplete.
		MMAPI::Item::SetHealthModifier(item_id, 0.0);
	}
}

void OnAfterGameActive()
{
	if (recipes_unlocked) return;
	recipes_unlocked = true;
	for (const auto& [sigil, item_id] : sigil_to_item_id)
		MMAPI::Recipe::Unlock(item_id, /*show_popup=*/false);
}

void OnBeforeUseItem(MMAPI::Item::UseItemContext& ctx)
{
	sigil_use_pending = false;

	if (!ctx.IsAriUse()) return;
	auto it = item_id_to_sigil.find(ctx.GetItemId());
	if (it == item_id_to_sigil.end()) return;
	Sigil sigil = it->second;

	if (!IsInDungeonFloor())
	{
		MMAPI::Log::Warn("You may only use Faux Sigils inside the dungeon!");
		MMAPI::Game::CreateNotification(false, SIGIL_RESTRICTED_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
	}

	if (active_sigils.contains(sigil))
	{
		MMAPI::Log::Warn("That sigil is already active!");
		MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
	}

	if (sigil == Sigil::Protection)
	{
		YYTK::RValue hits = MMAPI::Player::GetInvulnerabilityHits();
		if (MMAPI::Engine::IsNumeric(hits) && hits.ToInt64() > 0)
		{
			MMAPI::Log::Warn("That sigil is already active!");
			MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
			ctx.Cancel();
			return;
		}
	}

	if (sigil == Sigil::Redemption && FairyBuffIsActive())
	{
		MMAPI::Log::Warn("That sigil is already active!");
		MMAPI::Game::CreateNotification(false, SIGIL_LIMIT_NOTIFICATION_KEY);
		ctx.Cancel();
		return;
	}

	// All cancellation guards passed — arm activation for the next use-action edge.
	sigil_use_pending = true;
}

void OnAfterUseActionComplete(MMAPI::Player::AfterUseActionContext& ctx)
{
	if (!sigil_use_pending) return;
	sigil_use_pending = false;

	auto it = item_id_to_sigil.find(ctx.GetItemId());
	if (it == item_id_to_sigil.end()) return;
	ActivateSigil(it->second, ctx.GetSelf());
}

void OnAfterGetUiIcon(MMAPI::Item::GetUiIconContext& ctx)
{
	if (!item_id_to_sigil.contains(ctx.GetItemId())) return;
	ctx.SetSpriteAsset(GetDynamicSigilSprite(ctx.GetItemId()));
}

void OnBeforeDamage(MMAPI::Damage::BeforeDamageContext& ctx)
{
	YYTK::RValue* dd = ctx.damage_data;
	if (!dd) return;

	// Sigil of Fortification — 50% reduction to incoming damage on Ari, applied once per packet.
	if (active_sigils.contains(Sigil::Fortification) && ctx.IsTargetAri()
		&& !MMAPI::Engine::StructVariableExists(*dd, "__faux_sigils__fortification_applied"))
	{
		double damage = ctx.GetAmount();
		int penalty = static_cast<int>(std::trunc(damage * 0.50));
		ctx.SetAmount(damage - penalty);
		MMAPI::Engine::StructVariableSet(*dd, "__faux_sigils__fortification_applied", true);
	}

	// Sigil of Strength — 50% bonus to outgoing damage, applied once per packet.
	if (active_sigils.contains(Sigil::Strength) && !ctx.IsTargetAri()
		&& !MMAPI::Engine::StructVariableExists(*dd, "__faux_sigils__strength_applied"))
	{
		ctx.SetAmount(std::trunc(ctx.GetAmount() * 1.5));
		MMAPI::Engine::StructVariableSet(*dd, "__faux_sigils__strength_applied", true);
	}

	// Sigil of Rage — every non-miss outgoing hit becomes a 9999 critical.
	if (active_sigils.contains(Sigil::Rage) && !ctx.IsTargetAri() && !ctx.IsMiss())
	{
		ctx.SetCritical(true);
		ctx.SetAmount(9999.0);
	}
}

void OnBeforeMonsterSpawn(MMAPI::Monster::SpawnMonsterContext& ctx)
{
	if (sigil_silence_armed) ctx.Cancel();
}

void OnAfterGoToRoom(MMAPI::Location::AfterGoToRoomContext& /*ctx*/)
{
	// Each floor transition: arm silence for the upcoming spawn pass (if it was active), then clear
	// all sigils since they only persist within a single floor. Toolbar refreshes to show available icons.
	sigil_silence_armed = active_sigils.contains(Sigil::Silence);
	active_sigils.clear();
	MMAPI::ToolbarMenu::ForceUpdate();
}

void OnMonsterTick(CInstance* self)
{
	if (active_sigils.empty()) return;
	if (!self) return;

	YYTK::RValue monster = self->ToRValue();
	if (!MMAPI::Engine::StructVariableExists(monster, "config")) return;
	if (!MMAPI::Engine::StructVariableExists(monster, "hit_points")) return;

	YYTK::RValue hit_points = monster.GetMember("hit_points");

	// Sigil of Concealment — suppress monster aggro until something damages this monster. We snapshot
	// the monster's HP on first sight; any deviation from that baseline means Ari hit it, which breaks
	// concealment immediately for ALL monsters this floor.
	if (active_sigils.contains(Sigil::Concealment))
	{
		if (!MMAPI::Engine::StructVariableExists(monster, "__faux_sigils__conceal_hit_points"))
			MMAPI::Engine::StructVariableSet(monster, "__faux_sigils__conceal_hit_points", hit_points);

		YYTK::RValue baseline = monster.GetMember("__faux_sigils__conceal_hit_points");
		if (hit_points.ToDouble() == baseline.ToDouble())
		{
			MMAPI::Engine::StructVariableSet(monster, "aggro", false);
		}
		else
		{
			active_sigils.erase(Sigil::Concealment);
			MMAPI::Game::CreateNotification(false, CONCEALMENT_LOST_NOTIFICATION_KEY);
			MMAPI::ToolbarMenu::ForceUpdate();
		}
	}

	// Sigil of Rage — same one-hit-kill behavior: snapshot HP, instakill if Ari has hit it.
	if (active_sigils.contains(Sigil::Rage))
	{
		if (!MMAPI::Engine::StructVariableExists(monster, "__faux_sigils__rage_hit_points"))
			MMAPI::Engine::StructVariableSet(monster, "__faux_sigils__rage_hit_points", hit_points);

		YYTK::RValue baseline = monster.GetMember("__faux_sigils__rage_hit_points");
		if (hit_points.ToDouble() != baseline.ToDouble())
			*monster.GetRefMember("hit_points") = 0.0;
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	YYTKInterface* module_interface = nullptr;
	AurieStatus status = ObGetInterface("YYTK_Main", (AurieInterfaceBase*&)module_interface);
	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	module_interface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CInstance* global_instance = nullptr;
	module_interface->GetGlobalInstance(&global_instance);
	MMAPI::Initialize(module_interface, global_instance, g_ArSelfModule, MOD_NAME, VERSION);

	MMAPI::Damage::Enable();
	MMAPI::Dungeon::Enable();
	MMAPI::Game::Enable();
	MMAPI::Item::Enable();
	MMAPI::Location::Enable();
	MMAPI::Monster::Enable();
	MMAPI::Player::Enable();
	MMAPI::Recipe::Enable();
	MMAPI::StatusEffect::Enable();
	MMAPI::ToolbarMenu::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnAfterGameActive);
	MMAPI::Item::Hooks::BeforeUseItem(OnBeforeUseItem);
	MMAPI::Item::Hooks::AfterGetUiIcon(OnAfterGetUiIcon);
	MMAPI::Player::Hooks::AfterUseActionComplete(OnAfterUseActionComplete);
	MMAPI::Damage::Hooks::BeforeDamage(OnBeforeDamage);
	MMAPI::Monster::Hooks::BeforeMonsterSpawn(OnBeforeMonsterSpawn);
	MMAPI::Location::Hooks::AfterGoToRoom(OnAfterGoToRoom);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Monster, OnMonsterTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
