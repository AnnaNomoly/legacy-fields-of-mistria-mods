#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;

namespace fs = std::filesystem;

// ----- Mod metadata -----

static const char* const MOD_NAME = "SandboxModeCrafting";
static const char* const VERSION  = "1.2.0";

// ----- Config keys -----

static const char* const CONFIG_KEY_BLACKSMITHING     = "blacksmithing";
static const char* const CONFIG_KEY_COOKING           = "cooking";
static const char* const CONFIG_KEY_MILLING           = "milling";
static const char* const CONFIG_KEY_REFINING          = "refining";
static const char* const CONFIG_KEY_WOODCRAFTING      = "woodcrafting";
static const char* const CONFIG_KEY_DISABLE_TIME      = "disable_time";
static const char* const CONFIG_KEY_DISABLE_MATERIALS = "disable_materials";

// ----- Discipline classification (item tag → context) -----

static const char* const TAG_BLACKSMITHING = "blacksmithing";
static const char* const TAG_FURNITURE     = "furniture";    // → Woodcrafting
static const char* const TAG_MILLING       = "milling";
// Refining is detected by tag substring match on "refine" or "essence".

// ----- Configuration -----

struct DisciplineOptions
{
	bool disable_time      = true;
	bool disable_materials = true;
};

static DisciplineOptions blacksmithing;
static DisciplineOptions cooking;
static DisciplineOptions milling;
static DisciplineOptions refining;
static DisciplineOptions woodcrafting;

// ----- State -----

static bool startup_loaded = false;

// Returns the discipline options for the given crafting context, or nullptr if there's no
// matching discipline. Used by the pay/max/check hooks to dispatch on the active menu's context.
static const DisciplineOptions* OptionsForContext(MMAPI::Crafting::Context context)
{
	switch (context)
	{
		case MMAPI::Crafting::Context::Blacksmithing: return &blacksmithing;
		case MMAPI::Crafting::Context::Cooking:       return &cooking;
		case MMAPI::Crafting::Context::Milling:       return &milling;
		case MMAPI::Crafting::Context::Refining:      return &refining;
		case MMAPI::Crafting::Context::Woodcrafting:  return &woodcrafting;
	}
	return nullptr;
}

// ----- Config -----

static void LoadDisciplineOptions(const nlohmann::json& doc, const char* key, DisciplineOptions& options)
{
	if (!doc.contains(key) || !doc[key].is_object()) return;
	const nlohmann::json& discipline = doc[key];

	if (discipline.contains(CONFIG_KEY_DISABLE_TIME) && discipline[CONFIG_KEY_DISABLE_TIME].is_boolean())
		options.disable_time = discipline[CONFIG_KEY_DISABLE_TIME].get<bool>();
	if (discipline.contains(CONFIG_KEY_DISABLE_MATERIALS) && discipline[CONFIG_KEY_DISABLE_MATERIALS].is_boolean())
		options.disable_materials = discipline[CONFIG_KEY_DISABLE_MATERIALS].get<bool>();
}

static nlohmann::json SerializeDisciplineOptions(const DisciplineOptions& options)
{
	nlohmann::json j;
	j[CONFIG_KEY_DISABLE_TIME]      = options.disable_time;
	j[CONFIG_KEY_DISABLE_MATERIALS] = options.disable_materials;
	return j;
}

static void LoadConfig()
{
	fs::path config_path = MMAPI::Config::GetConfigPath(MOD_NAME);
	nlohmann::json config = MMAPI::Config::Load(config_path);

	LoadDisciplineOptions(config, CONFIG_KEY_BLACKSMITHING, blacksmithing);
	LoadDisciplineOptions(config, CONFIG_KEY_COOKING,       cooking);
	LoadDisciplineOptions(config, CONFIG_KEY_MILLING,       milling);
	LoadDisciplineOptions(config, CONFIG_KEY_REFINING,      refining);
	LoadDisciplineOptions(config, CONFIG_KEY_WOODCRAFTING,  woodcrafting);

	nlohmann::json roundtrip;
	roundtrip[CONFIG_KEY_BLACKSMITHING] = SerializeDisciplineOptions(blacksmithing);
	roundtrip[CONFIG_KEY_COOKING]       = SerializeDisciplineOptions(cooking);
	roundtrip[CONFIG_KEY_MILLING]       = SerializeDisciplineOptions(milling);
	roundtrip[CONFIG_KEY_REFINING]      = SerializeDisciplineOptions(refining);
	roundtrip[CONFIG_KEY_WOODCRAFTING]  = SerializeDisciplineOptions(woodcrafting);
	MMAPI::Config::Save(config_path, roundtrip);

	MMAPI::Log::Info("Config loaded.");
}

// ----- Discipline detection -----

// Returns true if an item's tag buffer contains the given tag (exact match or substring depending
// on `substring`). The game's tags are stored in a wrapped container at item.tags.__buffer.
static bool ItemHasTagMatching(YYTK::RValue item, const char* needle, bool substring)
{
	if (!MMAPI::Engine::StructVariableExists(item, "tags")) return false;
	YYTK::RValue tags = item.GetMember("tags");
	if (!MMAPI::Engine::StructVariableExists(tags, "__buffer")) return false;
	YYTK::RValue buffer = tags.GetMember("__buffer");

	size_t count = 0;
	MMAPI::Internal::module_interface->GetArraySize(buffer, count);
	for (size_t i = 0; i < count; ++i)
	{
		YYTK::RValue* entry = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(buffer, i, entry);
		if (!entry || entry->m_Kind != YYTK::VALUE_STRING) continue;

		std::string s = entry->ToString();
		if (substring)
		{
			if (s.find(needle) != std::string::npos) return true;
		}
		else if (s == needle) return true;
	}
	return false;
}

// Detects which discipline an item belongs to and returns whether its disable_time is configured.
// Cooking is distinguished by the kitchen_tier_requirement field (rather than a tag); other
// disciplines are tag-driven. Returns false if the item belongs to no recognized discipline OR
// its discipline's disable_time is false.
static bool ShouldZeroItemCraftTime(YYTK::RValue item)
{
	// Cooking: has a kitchen_tier_requirement field.
	if (MMAPI::Engine::StructVariableExists(item, "kitchen_tier_requirement")
		&& MMAPI::Engine::IsNumeric(item.GetMember("kitchen_tier_requirement")))
	{
		return cooking.disable_time;
	}

	// Tag-driven disciplines.
	if (ItemHasTagMatching(item, TAG_BLACKSMITHING, /*substring=*/false))
		return blacksmithing.disable_time;
	if (ItemHasTagMatching(item, "refine", /*substring=*/true)
		|| ItemHasTagMatching(item, "essence", /*substring=*/true))
		return refining.disable_time;
	if (ItemHasTagMatching(item, TAG_FURNITURE, /*substring=*/false))
		return woodcrafting.disable_time;
	if (ItemHasTagMatching(item, TAG_MILLING, /*substring=*/false))
		return milling.disable_time;

	return false;
}

// At startup, walks every item with a recipe and zeroes its component durations if the item's
// discipline has disable_time enabled. The mutation is one-shot — once applied, the recipe
// stays zero-duration for the rest of the session.
static void ApplyDisableTimeMutations()
{
	int mutated = 0;
	MMAPI::Item::ForEachItem([&](int item_id)
	{
		YYTK::RValue item = MMAPI::Item::GetItemData(item_id);
		if (item.m_Kind == YYTK::VALUE_UNDEFINED) return;
		if (!MMAPI::Engine::StructVariableExists(item, "recipe")) return;
		if (!MMAPI::Engine::StructVariableExists(item, "crafting_level_requirement")) return;

		if (!ShouldZeroItemCraftTime(item)) return;

		MMAPI::Recipe::ForEachComponent(item_id, [&](size_t index, YYTK::RValue /*component*/)
		{
			MMAPI::Recipe::SetComponentDuration(item_id, index, 0);
		});
		++mutated;
	});

	MMAPI::Log::Info("Zeroed crafting durations for %d items.", mutated);
}

// ----- Hooks -----

// Returns true if the active crafting context has disable_materials enabled. Used by all three
// material-related crafting hooks to decide whether to short-circuit / override game logic.
static bool ActiveDisciplineDisablesMaterials()
{
	auto context = MMAPI::Crafting::TryGetCurrentContext();
	if (!context) return false;

	const DisciplineOptions* options = OptionsForContext(*context);
	return options && options->disable_materials;
}

void OnBeforePayComponentCosts(MMAPI::Crafting::BeforePayComponentCostsContext& ctx)
{
	if (ActiveDisciplineDisablesMaterials())
		ctx.Cancel();
}

void OnAfterMaximumCrafts(MMAPI::Crafting::AfterMaximumCraftsContext& ctx)
{
	if (ActiveDisciplineDisablesMaterials())
		ctx.SetResult(999);
}

void OnAfterCheckItemCraftable(MMAPI::Crafting::AfterCheckItemCraftableContext& ctx)
{
	if (ActiveDisciplineDisablesMaterials())
		ctx.SetResult(true);
}

void OnBeforeSetupMainScreen()
{
	if (startup_loaded) return;

	LoadConfig();
	ApplyDisableTimeMutations();
	startup_loaded = true;
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

	MMAPI::Game::Enable();
	MMAPI::Item::Enable();
	MMAPI::Crafting::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnBeforeSetupMainScreen);
	MMAPI::Crafting::Hooks::BeforePayComponentCosts(OnBeforePayComponentCosts);
	MMAPI::Crafting::Hooks::AfterMaximumCrafts(OnAfterMaximumCrafts);
	MMAPI::Crafting::Hooks::AfterCheckItemCraftable(OnAfterCheckItemCraftable);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
