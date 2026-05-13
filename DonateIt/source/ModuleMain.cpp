#include <filesystem>
#include <string>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----

static const char* const MOD_NAME = "DonateIt";
static const char* const VERSION  = "1.1.0";

// ----- Config keys + defaults -----

static const char* const HIGHLIGHT_ITEMS_IN_MENUS_KEY  = "highlight_items_in_menus";
static const char* const HIGHLIGHT_FISH_IN_WORLD_KEY   = "highlight_fish_in_world";
static const char* const HIGHLIGHT_BUGS_IN_WORLD_KEY   = "highlight_bugs_in_world";
static const char* const HIGHLIGHT_FORAGE_IN_WORLD_KEY = "highlight_forage_in_world";

static const bool DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS  = true;
static const bool DEFAULT_HIGHLIGHT_FISH_IN_WORLD   = true;
static const bool DEFAULT_HIGHLIGHT_BUGS_IN_WORLD   = true;
static const bool DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD = true;

// Suffix appended to a sprite's name to produce its "this is donatable" highlight variant
// (e.g. "spr_fish_bass" → "spr_fish_bass_donatable"). Conventional in the mod's sprite-pack.
static const std::string DONATABLE_SPRITE_SUFFIX = "_donatable";

// Per-instance struct tag set the first time DonateIt swaps a fish/bug's sprites — used as a
// "we've already processed this one" guard so we don't redo the sprite work every tick, and so the
// per-frame image_speed pinning can target only instances we modified.
static const char* const PROCESSED_FISH_TAG = "__donate_it__processed_fish";
static const char* const PROCESSED_BUG_TAG  = "__donate_it__processed_bug";

// The image_speed value applied every frame to processed (donatable) fish and bugs. Slows their
// animation noticeably as a visual cue that they're notable. Preserved from the original mod.
static constexpr double DONATABLE_IMAGE_SPEED = 0.125;

// ----- Config -----

struct DonateItConfig
{
	bool highlight_items_in_menus  = DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS;
	bool highlight_fish_in_world   = DEFAULT_HIGHLIGHT_FISH_IN_WORLD;
	bool highlight_bugs_in_world   = DEFAULT_HIGHLIGHT_BUGS_IN_WORLD;
	bool highlight_forage_in_world = DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD;
};

void to_json(json& j, const DonateItConfig& c)
{
	j = json{
		{ HIGHLIGHT_ITEMS_IN_MENUS_KEY,  c.highlight_items_in_menus  },
		{ HIGHLIGHT_FISH_IN_WORLD_KEY,   c.highlight_fish_in_world   },
		{ HIGHLIGHT_BUGS_IN_WORLD_KEY,   c.highlight_bugs_in_world   },
		{ HIGHLIGHT_FORAGE_IN_WORLD_KEY, c.highlight_forage_in_world },
	};
}

void from_json(const json& j, DonateItConfig& c)
{
	c.highlight_items_in_menus  = MMAPI::Config::GetValue<bool>(j, HIGHLIGHT_ITEMS_IN_MENUS_KEY,  DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS);
	c.highlight_fish_in_world   = MMAPI::Config::GetValue<bool>(j, HIGHLIGHT_FISH_IN_WORLD_KEY,   DEFAULT_HIGHLIGHT_FISH_IN_WORLD);
	c.highlight_bugs_in_world   = MMAPI::Config::GetValue<bool>(j, HIGHLIGHT_BUGS_IN_WORLD_KEY,   DEFAULT_HIGHLIGHT_BUGS_IN_WORLD);
	c.highlight_forage_in_world = MMAPI::Config::GetValue<bool>(j, HIGHLIGHT_FORAGE_IN_WORLD_KEY, DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD);
}

// ----- State -----

static DonateItConfig config{};
static bool startup_loaded = false;

// Category ids cached at startup from globalInstance.__object_category__ — used to identify forage
// nodes (only objects in the "bush" and "crop" categories are eligible for the harvest-based
// donatable check).
static int bush_category_id = -1;
static int crop_category_id = -1;

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
			config = DonateItConfig{};
		}
		else
		{
			config = j.get<DonateItConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = DonateItConfig{};
	}
}

// Returns true if the given item_id is in the museum's donatable list AND hasn't been donated yet.
// All three sprite-swap paths share this gate.
bool NeedsDonating(int item_id)
{
	return MMAPI::Item::IsDonatable(item_id) && !MMAPI::Item::HasBeenDonated(item_id);
}

// Resolves the "_donatable" highlight variant of a sprite asset. Returns VALUE_UNDEFINED if the
// input isn't a sprite or no variant exists in the asset table.
YYTK::RValue ResolveDonatableSprite(YYTK::RValue sprite_asset)
{
	if (sprite_asset.m_Kind != YYTK::VALUE_REF) return {};

	YYTK::RValue asset_type = MMAPI::Internal::module_interface->CallBuiltin("asset_get_type", { sprite_asset });
	if (asset_type.ToInt64() != static_cast<int64_t>(MMAPI::Engine::AssetType::Sprite)) return {};

	YYTK::RValue name_rv = MMAPI::Internal::module_interface->CallBuiltin("sprite_get_name", { sprite_asset });
	if (name_rv.m_Kind != YYTK::VALUE_STRING) return {};

	std::string variant_name = name_rv.ToString() + DONATABLE_SPRITE_SUFFIX;
	YYTK::RValue variant = MMAPI::Engine::AssetGetIndex(variant_name);
	return variant.m_Kind == YYTK::VALUE_REF ? variant : YYTK::RValue();
}

// Swaps each sprite asset in an instance-variable sprite array (e.g. obj_fish's `move_sprites` /
// `idle_sprites` GameMaker arrays) for its donatable variant, in place. No-op for entries that
// don't have a `_donatable` counterpart.
void SwapDonatableSpriteArrayInPlace(YYTK::RValue array)
{
	size_t length = 0;
	MMAPI::Internal::module_interface->GetArraySize(array, length);
	for (size_t i = 0; i < length; i++)
	{
		YYTK::RValue* entry = nullptr;
		MMAPI::Internal::module_interface->GetArrayEntry(array, i, entry);
		if (!entry) continue;

		YYTK::RValue variant = ResolveDonatableSprite(*entry);
		if (variant.m_Kind == YYTK::VALUE_REF)
			MMAPI::Internal::module_interface->CallBuiltin("array_set", { array, static_cast<int>(i), variant });
	}
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	if (startup_loaded) return;

	LoadOrCreateConfigFile();

	// Cache the bush + crop category ids (used to filter forage nodes in BeforeNodeRendererSetSprite).
	YYTK::RValue bush_rv = MMAPI::Object::GetCategoryIdFromInternalName("bush");
	YYTK::RValue crop_rv = MMAPI::Object::GetCategoryIdFromInternalName("crop");
	if (MMAPI::Engine::IsNumeric(bush_rv)) bush_category_id = static_cast<int>(bush_rv.ToInt64());
	if (MMAPI::Engine::IsNumeric(crop_rv)) crop_category_id = static_cast<int>(crop_rv.ToInt64());

	startup_loaded = true;
}

void OnAfterGetUiIcon(MMAPI::Item::GetUiIconContext& ctx)
{
	if (!config.highlight_items_in_menus) return;
	if (!NeedsDonating(ctx.GetItemId())) return;

	YYTK::RValue variant = ResolveDonatableSprite(ctx.GetSpriteAsset());
	if (variant.m_Kind == YYTK::VALUE_REF)
		ctx.SetSpriteAsset(variant);
}

void OnBeforeNodeRendererSetSprite(MMAPI::Object::NodeRendererSetSpriteContext& ctx)
{
	if (!config.highlight_forage_in_world) return;
	if (bush_category_id < 0 && crop_category_id < 0) return;

	YYTK::RValue node = MMAPI::Object::GetNode(ctx.GetSelf());
	if (node.m_Kind == YYTK::VALUE_UNDEFINED) return;

	YYTK::RValue prototype = MMAPI::Object::GetPrototype(node);
	if (prototype.m_Kind == YYTK::VALUE_UNDEFINED) return;

	// Only bushes and crops have a harvest item_id we care about.
	YYTK::RValue category_rv = MMAPI::Object::GetCategoryId(prototype);
	if (!MMAPI::Engine::IsNumeric(category_rv)) return;
	int category_id = static_cast<int>(category_rv.ToInt64());
	if (category_id != bush_category_id && category_id != crop_category_id) return;

	if (!MMAPI::Engine::StructVariableExists(prototype, "harvest")) return;
	YYTK::RValue harvest = prototype.GetMember("harvest");
	if (!MMAPI::Engine::IsNumeric(harvest)) return;

	int harvest_item_id = static_cast<int>(harvest.ToInt64());
	if (!NeedsDonating(harvest_item_id)) return;

	YYTK::RValue variant = ResolveDonatableSprite(ctx.GetSpriteAsset());
	if (variant.m_Kind == YYTK::VALUE_REF)
		ctx.SetSpriteAsset(variant);
}

// Handles obj_fish in the world: on first sight, swap its move/idle sprite arrays to the donatable
// variants (one-shot, guarded by a struct tag). On every subsequent tick, pin image_speed so the
// donatable animation plays at the slower visual-cue rate.
void OnFishTick(CInstance* self)
{
	if (!self) return;

	YYTK::RValue self_rv = self->ToRValue();

	if (config.highlight_fish_in_world
		&& !MMAPI::Engine::StructVariableExists(self_rv, PROCESSED_FISH_TAG)
		&& MMAPI::Engine::StructVariableExists(self_rv, "fish_loot"))
	{
		MMAPI::Engine::StructVariableSet(self_rv, PROCESSED_FISH_TAG, true);

		int item_id = static_cast<int>(self_rv.GetMember("fish_loot").GetMember("item").GetMember("item_id").ToInt64());
		if (NeedsDonating(item_id))
		{
			if (MMAPI::Engine::StructVariableExists(self_rv, "move_sprites"))
				SwapDonatableSpriteArrayInPlace(self_rv.GetMember("move_sprites"));
			if (MMAPI::Engine::StructVariableExists(self_rv, "idle_sprites"))
				SwapDonatableSpriteArrayInPlace(self_rv.GetMember("idle_sprites"));
		}
	}
	else if (MMAPI::Engine::StructVariableExists(self_rv, PROCESSED_FISH_TAG))
	{
		MMAPI::Engine::InstanceVariableSet(self, "image_speed", DONATABLE_IMAGE_SPEED);
	}
}

// Same idea as OnFishTick — obj_bug uses singular move_sprite / idle_sprite struct fields instead
// of arrays, and reads item_id directly off the instance rather than via fish_loot.
void OnBugTick(CInstance* self)
{
	if (!self) return;

	YYTK::RValue self_rv = self->ToRValue();

	if (config.highlight_bugs_in_world
		&& !MMAPI::Engine::StructVariableExists(self_rv, PROCESSED_BUG_TAG)
		&& MMAPI::Engine::StructVariableExists(self_rv, "item_id"))
	{
		MMAPI::Engine::StructVariableSet(self_rv, PROCESSED_BUG_TAG, true);

		int item_id = static_cast<int>(self_rv.GetMember("item_id").ToInt64());
		if (NeedsDonating(item_id))
		{
			if (MMAPI::Engine::StructVariableExists(self_rv, "move_sprite"))
			{
				YYTK::RValue variant = ResolveDonatableSprite(self_rv.GetMember("move_sprite"));
				if (variant.m_Kind == YYTK::VALUE_REF)
					MMAPI::Engine::StructVariableSet(self_rv, "move_sprite", variant);
			}
			if (MMAPI::Engine::StructVariableExists(self_rv, "idle_sprite"))
			{
				YYTK::RValue variant = ResolveDonatableSprite(self_rv.GetMember("idle_sprite"));
				if (variant.m_Kind == YYTK::VALUE_REF)
					MMAPI::Engine::StructVariableSet(self_rv, "idle_sprite", variant);
			}
		}
	}
	else if (MMAPI::Engine::StructVariableExists(self_rv, PROCESSED_BUG_TAG))
	{
		MMAPI::Engine::InstanceVariableSet(self, "image_speed", DONATABLE_IMAGE_SPEED);
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

	MMAPI::Game::Enable();
	MMAPI::Item::Enable();
	MMAPI::Object::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Item::Hooks::AfterGetUiIcon(OnAfterGetUiIcon);
	MMAPI::Object::Hooks::BeforeNodeRendererSetSprite(OnBeforeNodeRendererSetSprite);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Fish, OnFishTick);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Bug,  OnBugTick);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
