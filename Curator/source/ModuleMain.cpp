#include <filesystem>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----

static const char* const MOD_NAME = "Curator";
static const char* const VERSION  = "1.2.0";

// Compile-time toggle: when true, the artifact swap only fires if Ari has the Unpeatable perk active.
// When false, the swap always fires on duplicate artifacts. The original mod shipped both variants;
// this build is the perk-gated one. Flip to `false` to build the non-perk variant.
static constexpr bool PERK_GATED = true;

// ----- Config keys + defaults -----

static const char* const ALLOW_ARTIFACTS_FROM_ANY_SET_KEY = "allow_artifacts_from_any_set";
static const char* const DISABLE_COMMON_FINDS_KEY         = "disable_common_finds";

static const bool DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET = false;
static const bool DEFAULT_DISABLE_COMMON_FINDS         = false;

// Internal names of items considered "common finds" — excluded from the all-sets fallback pool
// when `disable_common_finds` is enabled.
static const std::vector<std::string> COMMON_FINDS_ITEMS = {
	"clay", "peat", "shard_mass", "shards", "sod",
};

// Internal names of items explicitly disabled from the candidate pool. Empty for now; placeholder
// for future game-patch reactions.
static const std::vector<std::string> DISABLED_ITEMS = { };

static const std::string ARCHAEOLOGY_WING_NAME = "archaeology";

// ----- Cross-mod IPC (consumed by DigUpAnything to suppress this mod's artifact substitution) -----

static const char* const YYTK_KEY                 = "__YYTK";
static const char* const IGNORE_NEXT_DIG_SPOT_KEY = "ignore_next_dig_spot";

// ----- Config -----

struct CuratorConfig
{
	bool allow_artifacts_from_any_set = DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET;
	bool disable_common_finds         = DEFAULT_DISABLE_COMMON_FINDS;
};

void to_json(json& j, const CuratorConfig& c)
{
	j = json{
		{ ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, c.allow_artifacts_from_any_set },
		{ DISABLE_COMMON_FINDS_KEY,         c.disable_common_finds         },
	};
}

void from_json(const json& j, CuratorConfig& c)
{
	c.allow_artifacts_from_any_set = MMAPI::Config::GetValue<bool>(j, ALLOW_ARTIFACTS_FROM_ANY_SET_KEY, DEFAULT_ALLOW_ARTIFACTS_FROM_ANY_SET);
	c.disable_common_finds         = MMAPI::Config::GetValue<bool>(j, DISABLE_COMMON_FINDS_KEY,         DEFAULT_DISABLE_COMMON_FINDS);
}

// ----- State -----

static CuratorConfig config{};
static bool startup_loaded = false;

// Maps populated once at startup from `__museum_data.data[archaeology_wing].sets.inner.*.items`.
// archaeology_set_to_items: set internal name (e.g. "rare", "common") → item ids in that set.
// item_id_to_archaeology_set: reverse lookup.
// all_archaeology_items: flat set of every archaeology item id, minus disabled / (optionally) common-find items.
static std::map<std::string, std::vector<int>> archaeology_set_to_items;
static std::map<int, std::string>              item_id_to_archaeology_set;
static std::set<int>                           all_archaeology_items;

// Pre-resolved item ids for the disabled / common-find item-name lists. Both are conceptually sets
// (deduplicated, queried by membership rather than iterated in order).
static std::set<int> disabled_item_ids;
static std::set<int> common_finds_item_ids;

static std::mt19937 rng(std::random_device{}());

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
				MMAPI::Log::Error("No readable values in configuration file: %s!", path.string().c_str());
			config = CuratorConfig{};
		}
		else
		{
			config = j.get<CuratorConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = CuratorConfig{};
	}
}

// Resolves internal item names → item ids via the canonical MMAPI lookup. Skips names that don't
// resolve (they're either misspelled or removed from the game).
std::set<int> ResolveItemNames(const std::vector<std::string>& names)
{
	std::set<int> ids;
	for (const auto& name : names)
	{
		YYTK::RValue id = MMAPI::Item::GetIdFromInternalName(name);
		if (MMAPI::Engine::IsNumeric(id))
			ids.insert(static_cast<int>(id.ToInt64()));
	}
	return ids;
}

// Builds the archaeology-wing set lookup maps by walking
// `globalInstance.__museum_data.data[archaeology_wing_index].sets.inner.*.items`. The "archaeology"
// wing's index isn't fixed across game versions, so we resolve it by name from `__museum_wing__`.
void ParseArchaeologyMuseumData()
{
	archaeology_set_to_items.clear();
	item_id_to_archaeology_set.clear();
	all_archaeology_items.clear();

	YYTK::RValue museum_wing = MMAPI::Internal::global_instance->GetMember("__museum_wing__");
	size_t wing_count = 0;
	MMAPI::Internal::module_interface->GetArraySize(museum_wing, wing_count);

	int archaeology_wing_index = -1;
	for (size_t i = 0; i < wing_count; i++)
	{
		YYTK::RValue* entry = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(museum_wing, i, entry);
		if (entry && entry->m_Kind == YYTK::VALUE_STRING && entry->ToString() == ARCHAEOLOGY_WING_NAME)
		{
			archaeology_wing_index = static_cast<int>(i);
			break;
		}
	}

	if (archaeology_wing_index < 0)
	{
		MMAPI::Log::Error("Failed to find the \"%s\" museum wing!", ARCHAEOLOGY_WING_NAME.c_str());
		return;
	}

	YYTK::RValue museum_data = MMAPI::Internal::global_instance->GetMember("__museum_data");
	YYTK::RValue data = museum_data.GetMember("data");

	YYTK::RValue* archaeology_wing = nullptr;
	MMAPI::Internal::module_interface->GetArrayEntry(data, archaeology_wing_index, archaeology_wing);
	if (!archaeology_wing) return;

	YYTK::RValue sets_inner = archaeology_wing->GetMember("sets").GetMember("inner");

	// `sets.inner` is keyed by set names ("rare", "common", etc.). Enumerate, then iterate each
	// set's items array.
	std::vector<std::string> set_names;
	MMAPI::Internal::module_interface->EnumInstanceMembers(sets_inner,
		[&set_names](const char* name, YYTK::RValue* /*value*/) {
			set_names.emplace_back(name);
			return false;
		});

	for (const auto& set_name : set_names)
	{
		YYTK::RValue set = sets_inner.GetMember(set_name.c_str());
		YYTK::RValue items = set.GetMember("items");

		size_t item_count = 0;
		MMAPI::Internal::module_interface->GetArraySize(items, item_count);
		for (size_t j = 0; j < item_count; j++)
		{
			YYTK::RValue* entry = nullptr;
			MMAPI::Internal::module_interface->GetArrayEntry(items, j, entry);
			if (!entry || !MMAPI::Engine::IsNumeric(*entry)) continue;

			int item_id = static_cast<int>(entry->ToInt64());
			archaeology_set_to_items[set_name].push_back(item_id);
			item_id_to_archaeology_set[item_id] = set_name;
			all_archaeology_items.insert(item_id);
		}
	}

	for (int id : disabled_item_ids)
		all_archaeology_items.erase(id);

	if (config.disable_common_finds)
		for (int id : common_finds_item_ids)
			all_archaeology_items.erase(id);
}

// Reads + clears `__YYTK.Curator.ignore_next_dig_spot`. Returns true if it was set (DigUpAnything is
// taking over this dig). The flag is consume-once.
bool ConsumeIgnoreNextDigSpot()
{
	if (!MMAPI::Internal::module_interface->CallBuiltin("variable_global_exists", { YYTK_KEY }).ToBoolean())
		return false;

	YYTK::RValue __YYTK = MMAPI::Engine::GlobalVariableGet(YYTK_KEY);
	if (!MMAPI::Engine::StructVariableExists(__YYTK, MOD_NAME))
		return false;

	YYTK::RValue mod_struct = MMAPI::Engine::StructVariableGet(__YYTK, MOD_NAME);
	if (!MMAPI::Engine::StructVariableExists(mod_struct, IGNORE_NEXT_DIG_SPOT_KEY))
		return false;

	bool was_set = MMAPI::Engine::StructVariableGet(mod_struct, IGNORE_NEXT_DIG_SPOT_KEY).ToBoolean();
	if (was_set)
		MMAPI::Engine::StructVariableSet(mod_struct, IGNORE_NEXT_DIG_SPOT_KEY, false);
	return was_set;
}

// Initializes the `__YYTK.Curator` cross-mod struct (`version` + `ignore_next_dig_spot`). DigUpAnything
// sets the flag when it's about to substitute the next dig spot, so this mod knows to step aside.
void EnsureCrossModStructExists()
{
	YYTK::RValue __YYTK;
	if (!MMAPI::Internal::module_interface->CallBuiltin("variable_global_exists", { YYTK_KEY }).ToBoolean())
	{
		MMAPI::Internal::module_interface->GetRunnerInterface().StructCreate(&__YYTK);
		MMAPI::Engine::GlobalVariableSet(YYTK_KEY, __YYTK);
	}
	else
	{
		__YYTK = MMAPI::Engine::GlobalVariableGet(YYTK_KEY);
	}

	if (!MMAPI::Engine::StructVariableExists(__YYTK, MOD_NAME))
	{
		YYTK::RValue mod_struct;
		YYTK::RValue version_rv = VERSION;
		YYTK::RValue ignore_next_dig_spot = false;
		MMAPI::Internal::module_interface->GetRunnerInterface().StructCreate(&mod_struct);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&mod_struct, "version",                  &version_rv);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&mod_struct, IGNORE_NEXT_DIG_SPOT_KEY,   &ignore_next_dig_spot);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&__YYTK,     MOD_NAME,                   &mod_struct);
	}
}

// Picks an un-donated item from `candidates`. Returns -1 if every candidate has been donated.
int PickUndonated(const std::vector<int>& candidates)
{
	std::vector<int> available;
	available.reserve(candidates.size());
	for (int id : candidates)
		if (!MMAPI::Item::HasBeenDonated(id)) available.push_back(id);

	if (available.empty()) return -1;
	std::uniform_int_distribution<size_t> dist(0, available.size() - 1);
	return available[dist(rng)];
}

// Same as PickUndonated but for a set-based source. Lifted to a separate overload to avoid copying
// `all_archaeology_items` into a vector at the call site.
int PickUndonated(const std::set<int>& candidates)
{
	std::vector<int> available;
	available.reserve(candidates.size());
	for (int id : candidates)
		if (!MMAPI::Item::HasBeenDonated(id)) available.push_back(id);

	if (available.empty()) return -1;
	std::uniform_int_distribution<size_t> dist(0, available.size() - 1);
	return available[dist(rng)];
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	if (startup_loaded) return;

	LoadOrCreateConfigFile();
	EnsureCrossModStructExists();
	disabled_item_ids     = ResolveItemNames(DISABLED_ITEMS);
	common_finds_item_ids = ResolveItemNames(COMMON_FINDS_ITEMS);
	ParseArchaeologyMuseumData();

	startup_loaded = true;
}

// The artifact-substitution logic. Fires after the game's choose_random_artifact roll. If the rolled
// item is an already-donated archaeology item, swap it for an un-donated one from the same set —
// falling back to any set, then to "anything not common-finds" (if configured).
void OnAfterChooseRandomArtifact(MMAPI::Archaeology::AfterChooseRandomArtifactContext& ctx)
{
	// Cross-mod IPC: DigUpAnything signaled it's about to take over this dig — step aside.
	if (ConsumeIgnoreNextDigSpot())
		return;

	if (PERK_GATED && !MMAPI::Perk::IsActive(MMAPI::Perk::Ids::Unpeatable))
		return;

	int rolled_id = ctx.GetItemId();
	if (rolled_id < 0) return;

	// Only act on archaeology items the player has already donated.
	auto set_it = item_id_to_archaeology_set.find(rolled_id);
	if (set_it == item_id_to_archaeology_set.end()) return;
	if (!MMAPI::Item::HasBeenDonated(rolled_id)) return;

	const std::string& set_name = set_it->second;

	// First try: pick an un-donated item from the same set.
	std::vector<int> set_candidates;
	set_candidates.reserve(archaeology_set_to_items[set_name].size());
	for (int id : archaeology_set_to_items[set_name])
		if (!disabled_item_ids.contains(id))
			set_candidates.push_back(id);

	int replacement = PickUndonated(set_candidates);
	if (replacement >= 0)
	{
		ctx.SetItemId(replacement);
		MMAPI::Log::Info("Changed the donated artifact to a missing one from the matching set: %s!", set_name.c_str());
		return;
	}

	// Fallback 1: same-set is exhausted. Try any archaeology item from any set.
	if (config.allow_artifacts_from_any_set)
	{
		replacement = PickUndonated(all_archaeology_items);
		if (replacement >= 0)
		{
			ctx.SetItemId(replacement);
			MMAPI::Log::Info("Changed the donated artifact to a missing one from any set!");
			return;
		}

		// Fallback 2: literally everything donated. If disable_common_finds is on, pick anything from
		// the (common-finds-excluded) set; otherwise let the dupe stand.
		if (config.disable_common_finds && !all_archaeology_items.empty())
		{
			std::uniform_int_distribution<size_t> dist(0, all_archaeology_items.size() - 1);
			auto it = std::next(all_archaeology_items.begin(), dist(rng));
			ctx.SetItemId(*it);
			MMAPI::Log::Info("All artifacts have been donated! The artifact was a Common Finds item and was randomly replaced.");
			return;
		}

		MMAPI::Log::Info("All artifacts have been donated! The dig spot was unmodified.");
		return;
	}

	MMAPI::Log::Warn("All artifacts have been already donated for the matching set: %s!", set_name.c_str());
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

	MMAPI::Archaeology::Enable();
	MMAPI::Game::Enable();
	MMAPI::Item::Enable();
	MMAPI::Perk::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Archaeology::Hooks::AfterChooseRandomArtifact(OnAfterChooseRandomArtifact);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
