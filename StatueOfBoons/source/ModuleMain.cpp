#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "StatueOfBoons";
static const char* const VERSION = "1.2.0";

// Config keys
static const char* const MANA_COST_KEY            = "mana_cost";
static const char* const ESSENCE_COST_KEY         = "essence_cost";
static const char* const PREVIOUS_BOONS_LIMIT_KEY = "previous_boons_limit";

// Per-save mod data keys
static const char* const ACTIVE_BOON_KEY           = "active_boon";
static const char* const PREVIOUS_BOONS_KEY        = "previous_boons";
static const char* const DRAGON_FAIRY_LOCATION_KEY = "dragon_fairy_location";
static const char* const DRAGON_FAIRY_CAUGHT_KEY   = "dragon_fairy_caught";

// Defaults
static const int DEFAULT_MANA_COST            = 1;
static const int DEFAULT_ESSENCE_COST         = 5;
static const int DEFAULT_PREVIOUS_BOONS_LIMIT = 7;

// Custom asset names
static const std::string CUSTOM_OBJECT_NAME    = "statue_of_boons";
static const std::string DRAGON_FAIRY_NAME     = "dragon_fairy";
static const std::string UNIDENTIFIED_ARTIFACT = "unidentified_artifact";

// Boon internal names (used in save data and dialogue key construction)
static const std::string BOON_NONE          = "none";
static const std::string BOON_OF_SPEED      = "boon_of_speed";
static const std::string BOON_OF_FORAGE     = "boon_of_forage";
static const std::string BOON_OF_FISHING    = "boon_of_fishing";
static const std::string BOON_OF_BUTTERFLY  = "boon_of_butterfly";
static const std::string BOON_OF_FRIENDSHIP = "boon_of_friendship";
static const std::string BOON_OF_STAMINA    = "boon_of_stamina";
static const std::string BOON_OF_MANA       = "boon_of_mana";

static const std::vector<std::string> ALL_BOONS = {
	BOON_OF_SPEED, BOON_OF_FORAGE, BOON_OF_FISHING, BOON_OF_BUTTERFLY,
	BOON_OF_FRIENDSHIP, BOON_OF_STAMINA, BOON_OF_MANA,
};

// Object categories the forage boon should affect.
static const std::vector<std::string> FORAGE_CATEGORIES = { "bush", "crop", "rock", "stump", "tree" };

// Dragon fairy spawn points per butterfly-boon location.
static const std::map<MMAPI::Location::Ids, std::vector<std::pair<int, int>>> DRAGON_FAIRY_SPAWN_POINTS = {
	{ MMAPI::Location::Ids::WesternRuins, {{145,128},{177, 98},{177,166}                                                                      } },
	{ MMAPI::Location::Ids::EasternRoad,  {{ 63, 69},{131, 69},{ 31,127},{190, 95},{114,144},{ 39,186},{111,241},{ 32,259}                    } },
	{ MMAPI::Location::Ids::Narrows,      {{138, 52},{ 41,108},{130,117},{176,145},{114,189},{195,217}                                        } },
	{ MMAPI::Location::Ids::HaydensFarm,  {{ 73, 45},{ 49, 83},{ 70,105},{164, 54},{185,115},{121,104}                                        } },
	{ MMAPI::Location::Ids::Beach,        {{303, 77},{162, 45},{175, 75},{267, 74},{259, 37}                                                  } },
};

// Boon dialogue localization key.
static const std::map<std::string, std::string> BOON_GRANTED_DIALOGUE = {
	{ BOON_OF_SPEED,      "Conversations/Mods/Statue of Boons/boon_of_speed/granted"      },
	{ BOON_OF_FORAGE,     "Conversations/Mods/Statue of Boons/boon_of_forage/granted"     },
	{ BOON_OF_FISHING,    "Conversations/Mods/Statue of Boons/boon_of_fishing/granted"    },
	{ BOON_OF_BUTTERFLY,  "Conversations/Mods/Statue of Boons/boon_of_butterfly/granted"  },
	{ BOON_OF_FRIENDSHIP, "Conversations/Mods/Statue of Boons/boon_of_friendship/granted" },
	{ BOON_OF_STAMINA,    "Conversations/Mods/Statue of Boons/boon_of_stamina/granted"    },
	{ BOON_OF_MANA,       "Conversations/Mods/Statue of Boons/boon_of_mana/granted"       },
};

// Boon already active dialogue localization key.
static const std::map<std::string, std::string> BOON_ACTIVE_DIALOGUE = {
	{ BOON_OF_SPEED,      "Conversations/Mods/Statue of Boons/boon_of_speed/active"      },
	{ BOON_OF_FORAGE,     "Conversations/Mods/Statue of Boons/boon_of_forage/active"     },
	{ BOON_OF_FISHING,    "Conversations/Mods/Statue of Boons/boon_of_fishing/active"    },
	{ BOON_OF_BUTTERFLY,  "Conversations/Mods/Statue of Boons/boon_of_butterfly/active"  },
	{ BOON_OF_FRIENDSHIP, "Conversations/Mods/Statue of Boons/boon_of_friendship/active" },
	{ BOON_OF_STAMINA,    "Conversations/Mods/Statue of Boons/boon_of_stamina/active"    },
	{ BOON_OF_MANA,       "Conversations/Mods/Statue of Boons/boon_of_mana/active"       },
};

// Other localization keys
static const std::string INTERACT_KEY                    = "misc_local/Mods/Statue of Boons/interact";
static const std::string CONVERSATION_KEY                = "Conversations/Mods/Statue of Boons/statue_of_boons";
static const std::string DIALOGUE_PLACEHOLDER_KEY        = "Conversations/Mods/Statue of Boons/placeholder";
static const std::string DIALOGUE_INSUFFICIENT_MANA_KEY    = "Conversations/Mods/Statue of Boons/insufficient_mana";
static const std::string DIALOGUE_INSUFFICIENT_ESSENCE_KEY = "Conversations/Mods/Statue of Boons/insufficient_essence";
static const std::string BUTTERFLY_DETECTED_KEY          = "Notifications/Mods/Statue of Boons/boon_of_butterfly/detected";
static const std::string CRYSTAL_BALL_LOCAL_KEY          = "misc_local/crystal_ball";

struct StatueOfBoonsConfig
{
	int mana_cost            = DEFAULT_MANA_COST;
	int essence_cost         = DEFAULT_ESSENCE_COST;
	int previous_boons_limit = DEFAULT_PREVIOUS_BOONS_LIMIT;
};

void to_json(json& j, const StatueOfBoonsConfig& c)
{
	j = json{
		{ MANA_COST_KEY,            c.mana_cost            },
		{ ESSENCE_COST_KEY,         c.essence_cost         },
		{ PREVIOUS_BOONS_LIMIT_KEY, c.previous_boons_limit },
	};
}

void from_json(const json& j, StatueOfBoonsConfig& c)
{
	c.mana_cost            = MMAPI::Config::GetValue<int>(j, MANA_COST_KEY,            DEFAULT_MANA_COST,            0, 16);
	c.essence_cost         = MMAPI::Config::GetValue<int>(j, ESSENCE_COST_KEY,         DEFAULT_ESSENCE_COST,         0, 1000);
	c.previous_boons_limit = MMAPI::Config::GetValue<int>(j, PREVIOUS_BOONS_LIMIT_KEY, DEFAULT_PREVIOUS_BOONS_LIMIT, 0, 7);
}

// State
static StatueOfBoonsConfig config = {};
static std::string         active_boon;
static std::set<std::string> previous_boons;
static std::string         dragon_fairy_location_name;
static bool                dragon_fairy_caught = false;
static bool                modify_items_added  = false;
static bool                reduce_ari_mana     = false;
static bool                reduce_ari_essence  = false;
static bool                custom_object_used  = false;
static bool                game_is_active      = false;
static bool                statues_scanned_this_session = false;
static std::string         save_prefix;
static std::set<std::pair<int, int>> statue_positions;
static std::map<int, int>  default_spell_costs;
static std::set<int>       forage_boon_objects;
static int                 dragon_fairy_item_id = -1;
static int                 unidentified_artifact_item_id = -1;
static double              ari_x = 0;
static double              ari_y = 0;
static std::random_device  rd;
static std::mt19937        gen(rd());

bool AnyBoonActive() { return !active_boon.empty() && active_boon != BOON_NONE; }

void LoadDefaultSpellCosts()
{
	default_spell_costs.clear();
	MMAPI::Spell::ForEachId([](MMAPI::Spell::Ids spell) {
		YYTK::RValue cost = MMAPI::Spell::GetCost(spell);
		if (MMAPI::Engine::IsNumeric(cost))
			default_spell_costs[static_cast<int>(spell)] = static_cast<int>(cost.ToInt64());
	});
}

void LoadForageBoonObjects()
{
	forage_boon_objects.clear();

	std::set<int> forage_category_ids;
	for (const auto& name : FORAGE_CATEGORIES)
	{
		YYTK::RValue id = MMAPI::Object::GetCategoryIdFromInternalName(name);
		if (MMAPI::Engine::IsNumeric(id))
			forage_category_ids.insert(static_cast<int>(id.ToInt64()));
	}

	YYTK::RValue node_prototypes = MMAPI::Internal::global_instance->GetMember("__node_prototypes");
	size_t count = 0;
	MMAPI::Internal::module_interface->GetArraySize(node_prototypes, count);
	for (size_t i = 0; i < count; i++)
	{
		YYTK::RValue* prototype = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(node_prototypes, i, prototype);
		if (!prototype) continue;

		int category_id = static_cast<int>(prototype->GetMember("category_id").ToInt64());
		if (!forage_category_ids.contains(category_id)) continue;

		int object_id = static_cast<int>(prototype->GetMember("object_id").ToInt64());
		forage_boon_objects.insert(object_id);
	}
}

void ApplySpellCosts()
{
	bool zero_costs = (active_boon == BOON_OF_MANA);
	MMAPI::Spell::ForEachId([zero_costs](MMAPI::Spell::Ids spell) {
		int spell_id = static_cast<int>(spell);
		int default_cost = default_spell_costs.contains(spell_id) ? default_spell_costs.at(spell_id) : 0;
		MMAPI::Spell::SetCost(spell, zero_costs ? 0 : default_cost);
	});
}

void ApplyDragonFairyPrice()
{
	if (dragon_fairy_item_id < 0) return;
	YYTK::RValue level_rv = MMAPI::Renown::GetCurrentLevel();
	if (!MMAPI::Engine::IsNumeric(level_rv)) return;
	int level = static_cast<int>(level_rv.ToInt64());
	MMAPI::Item::SetBinValue(dragon_fairy_item_id, level * 500);
}

void ScanFarmForStatues()
{
	statue_positions.clear();

	CRoom* current_room = nullptr;
	if (!AurieSuccess(MMAPI::Internal::module_interface->GetCurrentRoomData(current_room)))
	{
		MMAPI::Log::Error("Failed to get current room data while scanning for statues");
		return;
	}

	YYTK::RValue statue_id_rv = MMAPI::Object::GetIdFromInternalName(CUSTOM_OBJECT_NAME);
	if (!MMAPI::Engine::IsNumeric(statue_id_rv)) return;
	int statue_object_id = static_cast<int>(statue_id_rv.ToInt64());

	for (CInstance* inst = current_room->GetMembers().m_ActiveInstances.m_First;
	     inst != nullptr;
	     inst = inst->GetMembers().m_Flink)
	{
		YYTK::RValue object_id = MMAPI::Object::GetObjectId(inst);
		if (!MMAPI::Engine::IsNumeric(object_id) || object_id.ToInt64() != statue_object_id) continue;

		auto pos = MMAPI::Object::GetTopLeftPosition(inst);
		if (pos) statue_positions.emplace(pos->x, pos->y);
	}
}

void ResetSessionState()
{
	game_is_active = false;
	statues_scanned_this_session = false;
	save_prefix.clear();
	statue_positions.clear();
	previous_boons.clear();
	dragon_fairy_location_name.clear();
	dragon_fairy_caught = false;
	active_boon.clear();
	custom_object_used = false;
	reduce_ari_mana = false;
	reduce_ari_essence = false;
	modify_items_added = false;
	ApplySpellCosts();
}

void ResetEndOfDayState()
{
	if (AnyBoonActive())
		previous_boons.insert(active_boon);
	active_boon.clear();
	dragon_fairy_caught = false;
	dragon_fairy_location_name.clear();
	custom_object_used = false;
	reduce_ari_mana = false;
	reduce_ari_essence = false;
	modify_items_added = false;
	ApplySpellCosts();
}

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
			config = StatueOfBoonsConfig{};
		}
		else
		{
			config = j.get<StatueOfBoonsConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = StatueOfBoonsConfig{};
	}
}

void OnSetupMainScreen()
{
	ResetSessionState();
}

void OnGameActive()
{
	// Populate runtime data tables.
	if (dragon_fairy_item_id < 0)
	{
		YYTK::RValue id = MMAPI::Item::GetIdFromInternalName(DRAGON_FAIRY_NAME);
		if (MMAPI::Engine::IsNumeric(id))
			dragon_fairy_item_id = static_cast<int>(id.ToInt64());
	}
	if (unidentified_artifact_item_id < 0)
	{
		YYTK::RValue id = MMAPI::Item::GetIdFromInternalName(UNIDENTIFIED_ARTIFACT);
		if (MMAPI::Engine::IsNumeric(id))
			unidentified_artifact_item_id = static_cast<int>(id.ToInt64());
	}
	if (default_spell_costs.empty())
		LoadDefaultSpellCosts();
	if (forage_boon_objects.empty())
		LoadForageBoonObjects();

	game_is_active = true;
	ApplySpellCosts();
	ApplyDragonFairyPrice();
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	if (save_prefix.empty())
		save_prefix = ctx.GetSavePrefix();

	MMAPI::ModSave::Write(save_prefix, json{
		{ ACTIVE_BOON_KEY,           AnyBoonActive() ? active_boon : BOON_NONE },
		{ PREVIOUS_BOONS_KEY,        previous_boons                            },
		{ DRAGON_FAIRY_LOCATION_KEY, dragon_fairy_location_name                },
		{ DRAGON_FAIRY_CAUGHT_KEY,   dragon_fairy_caught                       },
	});
}

void OnAfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	save_prefix = ctx.GetSavePrefix();

	json data = MMAPI::ModSave::Read(save_prefix);
	if (data.empty()) return;

	std::string saved_boon = data.value(ACTIVE_BOON_KEY, std::string());
	active_boon = (saved_boon == BOON_NONE) ? std::string() : saved_boon;
	previous_boons             = data.value(PREVIOUS_BOONS_KEY,        std::set<std::string>());
	dragon_fairy_location_name = data.value(DRAGON_FAIRY_LOCATION_KEY, std::string());
	dragon_fairy_caught        = data.value(DRAGON_FAIRY_CAUGHT_KEY,   false);

	ApplySpellCosts();
}

void OnAfterEndDay()
{
	ResetEndOfDayState();
}

void OnAfterMoveSpeed(MMAPI::Player::MoveSpeedContext& ctx)
{
	if (active_boon == BOON_OF_SPEED)
		ctx.AddModifier(0.5);
}

void OnBeforeStaminaChange(MMAPI::Player::BeforeStaminaChangeContext& ctx)
{
	if (active_boon == BOON_OF_STAMINA && ctx.GetAmount() < 0)
		ctx.SetAmount(0);
}

void OnBeforeHeartPointsChange(MMAPI::NPC::HeartPointsChangedContext& ctx)
{
	if (active_boon == BOON_OF_FRIENDSHIP)
		ctx.SetAmount(std::ceil(1.5 * ctx.GetAmount()));
}

void OnBeforeGiveItem(MMAPI::Item::GiveItemContext& ctx)
{
	int item_id = ctx.GetItemId();

	// Track dragon fairy capture for the butterfly boon.
	if (active_boon == BOON_OF_BUTTERFLY && !dragon_fairy_caught && item_id == dragon_fairy_item_id)
	{
		MMAPI::Location::Ids current;
		if (MMAPI::Location::TryGetCurrentLocation(current)
		    && MMAPI::Location::LocationIdToString(current) == dragon_fairy_location_name)
		{
			dragon_fairy_caught = true;
		}
	}

	// Forage & Fishing boon: double the quantity of the item being given.
	if (modify_items_added)
	{
		modify_items_added = false;

		MMAPI::Location::Ids current;
		bool in_farm = MMAPI::Location::TryGetCurrentLocation(current)
		            && current == MMAPI::Location::Ids::Farm;

		if (!in_farm
		    && item_id != unidentified_artifact_item_id
		    && MMAPI::Item::HasBeenAcquired(item_id))
		{
			ctx.SetQuantity(ctx.GetQuantity() * 2);
		}
	}
}

void OnBeforeAttemptInteract(MMAPI::Instance::AttemptInteractContext& ctx)
{
	YYTK::RValue object_id_rv = MMAPI::Object::GetObjectId(ctx.GetSelf());
	if (!MMAPI::Engine::IsNumeric(object_id_rv)) return;
	int object_id = static_cast<int>(object_id_rv.ToInt64());

	if (MMAPI::Object::IsInternalName(object_id, CUSTOM_OBJECT_NAME))
	{
		custom_object_used = true;
		return;
	}

	if (active_boon == BOON_OF_FORAGE && forage_boon_objects.contains(object_id))
		modify_items_added = true;
}

void OnBeforeLocalizedString(MMAPI::Text::LocalizedStringContext& ctx)
{
	if (!game_is_active) return;
	if (ctx.GetKey() != CRYSTAL_BALL_LOCAL_KEY) return;

	MMAPI::Location::Ids current;
	if (!MMAPI::Location::TryGetCurrentLocation(current) || current != MMAPI::Location::Ids::Farm)
		return;

	// Statue positions are stored in tile coords; convert to pixel coords for the proximity check.
	for (const auto& [tx, ty] : statue_positions)
	{
		double dx = ari_x - (tx * 8 + 8);
		double dy = ari_y - (ty * 8 + 8);
		if (std::sqrt(dx * dx + dy * dy) <= 44.0)
		{
			ctx.SetKey(INTERACT_KEY);
			return;
		}
	}
}

void OnBeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx)
{
	if (custom_object_used)
	{
		custom_object_used = false;
		ctx.SetKey(CONVERSATION_KEY);
	}
}

void OnBeforePlayText(MMAPI::Text::PlayTextContext& ctx)
{
	if (ctx.GetKey() != DIALOGUE_PLACEHOLDER_KEY) return;

	// Boon already active
	if (AnyBoonActive())
	{
		auto it = BOON_ACTIVE_DIALOGUE.find(active_boon);
		if (it != BOON_ACTIVE_DIALOGUE.end())
			ctx.SetKey(it->second);
		return;
	}

	// Insufficient resources
	YYTK::RValue mana_rv    = MMAPI::Player::GetMana();
	YYTK::RValue essence_rv = MMAPI::Player::GetEssence();
	int current_mana    = MMAPI::Engine::IsNumeric(mana_rv)    ? static_cast<int>(mana_rv.ToInt64())    : 0;
	int current_essence = MMAPI::Engine::IsNumeric(essence_rv) ? static_cast<int>(essence_rv.ToInt64()) : 0;

	if (current_mana < config.mana_cost)
	{
		ctx.SetKey(DIALOGUE_INSUFFICIENT_MANA_KEY);
		return;
	}
	if (current_essence < config.essence_cost)
	{
		ctx.SetKey(DIALOGUE_INSUFFICIENT_ESSENCE_KEY);
		return;
	}

	// Grant a boon
	if (config.mana_cost > 0)    reduce_ari_mana    = true;
	if (config.essence_cost > 0) reduce_ari_essence = true;

	// Reset the previous-boons buffer once it hits the configured limit (or covers every boon).
	if (previous_boons.size() >= static_cast<size_t>(config.previous_boons_limit)
	    || previous_boons.size() == ALL_BOONS.size())
		previous_boons.clear();

	// Pick a boon, excluding any in the previous-boons buffer.
	std::vector<std::string> available = ALL_BOONS;
	for (const auto& prev : previous_boons)
		available.erase(std::remove(available.begin(), available.end(), prev), available.end());
	if (available.empty()) available = ALL_BOONS;  // safety fallback

	std::uniform_int_distribution<> pick(0, static_cast<int>(available.size()) - 1);
	std::string chosen = available[pick(gen)];
	active_boon = chosen;

	if (chosen == BOON_OF_BUTTERFLY)
	{
		// Pick a random location for the dragon fairy to spawn in.
		std::vector<MMAPI::Location::Ids> locs;
		locs.reserve(DRAGON_FAIRY_SPAWN_POINTS.size());
		for (const auto& [loc, _] : DRAGON_FAIRY_SPAWN_POINTS)
			locs.push_back(loc);

		std::uniform_int_distribution<> pick_loc(0, static_cast<int>(locs.size()) - 1);
		dragon_fairy_location_name = MMAPI::Location::LocationIdToString(locs[pick_loc(gen)]);
		dragon_fairy_caught = false;
	}

	if (chosen == BOON_OF_MANA)
		ApplySpellCosts();

	auto granted_it = BOON_GRANTED_DIALOGUE.find(chosen);
	if (granted_it != BOON_GRANTED_DIALOGUE.end())
		ctx.SetKey(granted_it->second);
}

void OnAfterShowRoomTitle()
{
	// Butterfly boon: spawn a dragon fairy when the player enters the chosen location.
	if (active_boon != BOON_OF_BUTTERFLY || dragon_fairy_caught)
		return;

	MMAPI::Location::Ids current;
	if (!MMAPI::Location::TryGetCurrentLocation(current))
		return;

	std::string current_name = MMAPI::Location::LocationIdToString(current);
	if (current_name != dragon_fairy_location_name || !DRAGON_FAIRY_SPAWN_POINTS.contains(current))
		return;

	const auto& spawn_points = DRAGON_FAIRY_SPAWN_POINTS.at(current);
	std::uniform_int_distribution<> pick_spawn(0, static_cast<int>(spawn_points.size()) - 1);
	const auto& [x, y] = spawn_points[pick_spawn(gen)];
	MMAPI::Bug::SpawnCustom(x, y, dragon_fairy_item_id);
	MMAPI::Game::CreateNotification(false, BUTTERFLY_DETECTED_KEY);
}

void OnAfterRoomStart(MMAPI::Weather::AfterRoomStartContext& /*ctx*/)
{
	// Scan the farm for placed statues
	if (statues_scanned_this_session || !game_is_active)
		return;

	MMAPI::Location::Ids current;
	if (MMAPI::Location::TryGetCurrentLocation(current) && current == MMAPI::Location::Ids::Farm)
	{
		statues_scanned_this_session = true;
		ScanFarmForStatues();
	}
}

void HandleAri(CInstance* self)
{
	YYTK::RValue x; MMAPI::Internal::module_interface->GetBuiltin("x", self, NULL_INDEX, x);
	YYTK::RValue y; MMAPI::Internal::module_interface->GetBuiltin("y", self, NULL_INDEX, y);
	ari_x = x.ToDouble();
	ari_y = y.ToDouble();

	if (reduce_ari_mana)
	{
		reduce_ari_mana = false;
		MMAPI::Player::ModifyMana(-config.mana_cost);
	}
	if (reduce_ari_essence)
	{
		reduce_ari_essence = false;
		MMAPI::Player::ModifyEssence(-config.essence_cost);
	}
}

void OnAfterFishCelebration()
{
	if (active_boon == BOON_OF_FISHING) modify_items_added = true;
}

void OnAfterDiveCelebration()
{
	if (active_boon == BOON_OF_FISHING) modify_items_added = true;
}

void OnAfterFurniturePlaced(MMAPI::Object::FurniturePlacedContext& ctx)
{
	if (!game_is_active) return;

	int object_id = ctx.GetObjectId();
	if (object_id < 0) return;

	if (MMAPI::Object::IsInternalName(object_id, CUSTOM_OBJECT_NAME))
	{
		auto pos = ctx.GetTopLeftPosition();
		if (pos) statue_positions.emplace(pos->x, pos->y);
	}
}

void OnAfterObjectErased(MMAPI::Object::ObjectErasedContext& ctx)
{
	if (!game_is_active) return;

	MMAPI::Location::Ids current;
	if (!MMAPI::Location::TryGetCurrentLocation(current) || current != MMAPI::Location::Ids::Farm)
		return;

	int object_id = ctx.GetObjectId();
	if (object_id < 0) return;

	if (MMAPI::Object::IsInternalName(object_id, CUSTOM_OBJECT_NAME))
	{
		auto pos = ctx.GetTopLeftPosition();
		if (pos) statue_positions.erase({ pos->x, pos->y });
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

	MMAPI::Bug::Enable();
	MMAPI::Fish::Enable();
	MMAPI::Item::Enable();
	MMAPI::Location::Enable();
	MMAPI::NPC::Enable();
	MMAPI::Object::Enable();
	MMAPI::Player::Enable();
	MMAPI::Spell::Enable();
	MMAPI::Text::Enable();
	MMAPI::Game::Enable();

	LoadOrCreateConfigFile();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::AfterEndDay(OnAfterEndDay);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Game::Hooks::AfterLoadGame(OnAfterLoadGame);
	MMAPI::Player::Hooks::AfterMoveSpeed(OnAfterMoveSpeed);
	MMAPI::Player::Hooks::BeforeStaminaChange(OnBeforeStaminaChange);
	MMAPI::NPC::Hooks::BeforeHeartPointsChange(OnBeforeHeartPointsChange);
	MMAPI::Item::Hooks::BeforeGiveItem(OnBeforeGiveItem);
	MMAPI::Instance::Hooks::BeforeAttemptInteract(OnBeforeAttemptInteract);
	MMAPI::Text::Hooks::BeforeLocalizedString(OnBeforeLocalizedString);
	MMAPI::Text::Hooks::BeforePlayConversation(OnBeforePlayConversation);
	MMAPI::Text::Hooks::BeforePlayText(OnBeforePlayText);
	MMAPI::Location::Hooks::AfterShowRoomTitle(OnAfterShowRoomTitle);
	MMAPI::Weather::Hooks::AfterRoomStart(OnAfterRoomStart);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, HandleAri);
	MMAPI::Fish::Hooks::AfterFishCelebration(OnAfterFishCelebration);
	MMAPI::Fish::Hooks::AfterDiveCelebration(OnAfterDiveCelebration);
	MMAPI::Object::Hooks::AfterFurniturePlaced(OnAfterFurniturePlaced);
	MMAPI::Object::Hooks::AfterObjectErased(OnAfterObjectErased);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
