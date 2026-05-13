#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>
#include <MMAPI/Config.hpp>
#include <MMAPI/ModSave.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "SummoningCircle";
static const char* const VERSION = "1.1.0";

// Config keys
static const char* const SHIPPING_BIN_PRICE_KEY     = "summoning_circle_shipping_bin_price";
static const char* const STORE_PRICE_KEY            = "summoning_circle_store_price";
static const char* const CONFIRMATION_REQUIRED_KEY  = "summoning_circle_confirmation_required";

// Per-save mod data keys
static const char* const POSITIONS_KEY = "summoning_circle_positions";

// Defaults
static const int  DEFAULT_SHIPPING_BIN_PRICE    = 5000;
static const int  DEFAULT_STORE_PRICE           = 5000;
static const bool DEFAULT_CONFIRMATION_REQUIRED = true;

// Custom asset name (same string for both item and object).
static const std::string SUMMONING_CIRCLE_NAME = "summoning_circle";

// Localization keys
static const std::string TELEPORT_INTERACT_KEY              = "misc_local/Mods/Summoning Circle/teleport_interact";
static const std::string TWO_REQUIRED_NOTIFICATION_KEY      = "Notifications/Mods/Summoning Circle/two_required";
static const std::string TWO_ALREADY_PRESENT_KEY            = "Notifications/Mods/Summoning Circle/two_already_present";
static const std::string ACTIVATION_REQUIRED_CONVERSATION_KEY = "Conversations/Mods/Summoning Circle/activation_confirmation";
static const std::string ACTIVATION_ACCEPTED_KEY            = "Conversations/Mods/Summoning Circle/activation_confirmation/1";
static const std::string ACTIVATION_REJECTED_KEY            = "Conversations/Mods/Summoning Circle/activation_confirmation/2";
static const std::string INPUT_INTERACT_LOCAL_KEY           = "misc_local/input_interact";

struct SummoningCircleConfig
{
	int  shipping_bin_price    = DEFAULT_SHIPPING_BIN_PRICE;
	int  store_price           = DEFAULT_STORE_PRICE;
	bool confirmation_required = DEFAULT_CONFIRMATION_REQUIRED;
};

void to_json(json& j, const SummoningCircleConfig& c)
{
	j = json{
		{ SHIPPING_BIN_PRICE_KEY,    c.shipping_bin_price    },
		{ STORE_PRICE_KEY,           c.store_price           },
		{ CONFIRMATION_REQUIRED_KEY, c.confirmation_required },
	};
}

void from_json(const json& j, SummoningCircleConfig& c)
{
	c.shipping_bin_price    = MMAPI::Config::GetValue<int>(j,  SHIPPING_BIN_PRICE_KEY,    DEFAULT_SHIPPING_BIN_PRICE,    1, 10000);
	c.store_price           = MMAPI::Config::GetValue<int>(j,  STORE_PRICE_KEY,           DEFAULT_STORE_PRICE,           1, 10000);
	c.confirmation_required = MMAPI::Config::GetValue<bool>(j, CONFIRMATION_REQUIRED_KEY, DEFAULT_CONFIRMATION_REQUIRED);
}

// State
static SummoningCircleConfig config = {};
static std::string save_prefix;
static std::vector<std::pair<int, int>> circle_positions;
static int summoning_circle_object_id = -1;
static int summoning_circle_item_id   = -1;
static bool game_is_active            = false;
static bool farm_scanned_this_session = false;
static bool teleport_ari              = false;
static bool play_conversation         = false;
static std::pair<int, int> teleport_target_tile = {};
static double ari_x = 0;
static double ari_y = 0;

// -------- Helpers --------

void WriteModSaveFile()
{
	MMAPI::ModSave::Write(save_prefix, json{
		{ POSITIONS_KEY, circle_positions },
	});
}

void ReadModSaveFile()
{
	json data = MMAPI::ModSave::Read(save_prefix);
	if (data.empty()) return;
	circle_positions = data.value(POSITIONS_KEY, std::vector<std::pair<int, int>>{});
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
			config = SummoningCircleConfig{};
		}
		else
		{
			config = j.get<SummoningCircleConfig>();
		}

		MMAPI::Config::Save(path, config);
		MMAPI::Log::Info("Loaded configuration file: %s", path.string().c_str());
	}
	catch (const std::exception& e)
	{
		MMAPI::Log::Error("Error loading config: %s", e.what());
		config = SummoningCircleConfig{};
	}
}

double CalculateDistance(double x1, double y1, double x2, double y2)
{
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

void PruneMissingCircles()
{
	// Walk the active instances in the current room and keep only positions that match
	// a still-present summoning circle. Handles the case where furniture was removed
	// outside the running mod (e.g. another mod, save edit, game patch).
	CRoom* current_room = nullptr;
	if (!AurieSuccess(MMAPI::Internal::module_interface->GetCurrentRoomData(current_room)))
	{
		MMAPI::Log::Error("Failed to get current room data while pruning circles");
		return;
	}

	std::vector<std::pair<int, int>> existing;
	for (CInstance* inst = current_room->GetMembers().m_ActiveInstances.m_First;
	     inst != nullptr;
	     inst = inst->GetMembers().m_Flink)
	{
		YYTK::RValue object_id = MMAPI::Object::GetObjectId(inst);
		if (!MMAPI::Engine::IsNumeric(object_id) || object_id.ToInt64() != summoning_circle_object_id)
			continue;

		auto pos = MMAPI::Object::GetTopLeftPosition(inst);
		if (pos) existing.emplace_back(pos->x, pos->y);
	}

	std::erase_if(circle_positions, [&](const std::pair<int, int>& p) {
		return std::find(existing.begin(), existing.end(), p) == existing.end();
	});
}

// -------- Hooks --------

void OnSetupMainScreen()
{
	game_is_active = false;
	farm_scanned_this_session = false;
	teleport_ari = false;
	play_conversation = false;
	teleport_target_tile = {};
	circle_positions.clear();
	save_prefix.clear();
	ari_x = 0;
	ari_y = 0;
}

void OnGameActive()
{
	// First weather fire of the session — resolve runtime IDs and apply config-driven prices.
	if (summoning_circle_item_id < 0)
	{
		YYTK::RValue id = MMAPI::Item::GetIdFromInternalName(SUMMONING_CIRCLE_NAME);
		if (MMAPI::Engine::IsNumeric(id))
			summoning_circle_item_id = static_cast<int>(id.ToInt64());
	}
	if (summoning_circle_object_id < 0)
	{
		YYTK::RValue id = MMAPI::Object::GetIdFromInternalName(SUMMONING_CIRCLE_NAME);
		if (MMAPI::Engine::IsNumeric(id))
			summoning_circle_object_id = static_cast<int>(id.ToInt64());
	}

	if (summoning_circle_item_id >= 0)
	{
		MMAPI::Item::SetBinValue(summoning_circle_item_id,   config.shipping_bin_price);
		MMAPI::Item::SetStoreValue(summoning_circle_item_id, config.store_price);
	}

	game_is_active = true;
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	if (save_prefix.empty())
		save_prefix = ctx.GetSavePrefix();

	if (!save_prefix.empty())
		WriteModSaveFile();
}

void OnAfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	save_prefix = ctx.GetSavePrefix();
	ReadModSaveFile();
}

void OnBeforeInteract(MMAPI::Instance::InteractContext& ctx)
{
	if (!game_is_active) return;
	if (ctx.GetObjectId() != summoning_circle_object_id) return;

	// Take over the interact — the game's default handling for a placed summoning circle
	// (e.g. pickup/use prompts) is bypassed entirely; this mod handles everything.
	ctx.Cancel();

	// Two circles are required — interacting with the only one notifies the player.
	if (circle_positions.size() < 2)
	{
		MMAPI::Game::CreateNotification(false, TWO_REQUIRED_NOTIFICATION_KEY);
		MMAPI::Log::Info("Two summoning circles are required to use it.");
		return;
	}

	// Find the OTHER circle's tile position (the one the player isn't standing on).
	auto pos = MMAPI::Object::GetTopLeftPosition(ctx.GetTarget());
	if (!pos) return;

	for (const auto& [x, y] : circle_positions)
	{
		if (x != pos->x || y != pos->y)
		{
			teleport_target_tile = { x, y };
			break;
		}
	}

	if (config.confirmation_required)
		play_conversation = true;
	else
		teleport_ari = true;
}

void OnBeforeLocalizedString(MMAPI::Text::LocalizedStringContext& ctx)
{
	if (!game_is_active) return;
	if (ctx.GetKey() != INPUT_INTERACT_LOCAL_KEY) return;

	MMAPI::Location::Ids current;
	if (!MMAPI::Location::TryGetCurrentLocation(current) || current != MMAPI::Location::Ids::Farm)
		return;

	// Swap the "input_interact" prompt for the mod's "teleport" prompt when standing near a circle.
	// Circle positions are tile coords; convert to pixel coords (×8 + half write_size_x of 24px = +12) for the proximity check.
	for (const auto& [tx, ty] : circle_positions)
	{
		if (CalculateDistance(ari_x, ari_y, tx * 8 + 12, ty * 8 + 12) <= 44.0)
		{
			ctx.SetKey(TELEPORT_INTERACT_KEY);
			return;
		}
	}
}

void OnBeforePlayText(MMAPI::Text::PlayTextContext& ctx)
{
	if (!game_is_active) return;

	std::string_view key = ctx.GetKey();
	if (key == ACTIVATION_ACCEPTED_KEY)
	{
		teleport_ari = true;
		MMAPI::Log::Info("Teleporting Ari.");
		MMAPI::Text::CloseTextbox();
		ctx.Cancel();
	}
	else if (key == ACTIVATION_REJECTED_KEY)
	{
		MMAPI::Text::CloseTextbox();
		ctx.Cancel();
	}
}

void OnBeforeFurniturePlaced(MMAPI::Object::BeforeFurniturePlacedContext& ctx)
{
	if (!game_is_active) return;
	if (ctx.GetObjectId() != summoning_circle_object_id) return;

	// Cap at 2 circles per save — refuse placement and notify if the player already has two.
	if (circle_positions.size() >= 2)
	{
		MMAPI::Game::CreateNotification(false, TWO_ALREADY_PRESENT_KEY);
		MMAPI::Log::Info("Two summoning circles already on the farm — placement refused.");
		ctx.Cancel();
	}
}

void OnAfterFurniturePlaced(MMAPI::Object::FurniturePlacedContext& ctx)
{
	if (!game_is_active) return;
	if (ctx.GetObjectId() != summoning_circle_object_id) return;

	auto pos = ctx.GetTopLeftPosition();
	if (pos) circle_positions.emplace_back(pos->x, pos->y);
}

void OnAfterObjectErased(MMAPI::Object::ObjectErasedContext& ctx)
{
	if (!game_is_active) return;

	MMAPI::Location::Ids current;
	if (!MMAPI::Location::TryGetCurrentLocation(current) || current != MMAPI::Location::Ids::Farm)
		return;

	if (ctx.GetObjectId() != summoning_circle_object_id) return;

	auto pos = ctx.GetTopLeftPosition();
	if (!pos) return;

	std::erase_if(circle_positions, [&](const std::pair<int, int>& p) {
		return p.first == pos->x && p.second == pos->y;
	});
}

void OnAfterRoomStart(MMAPI::Weather::AfterRoomStartContext& /*ctx*/)
{
	// One-shot prune of stale circle positions when the player first reaches the farm in this session.
	if (farm_scanned_this_session || !game_is_active) return;

	MMAPI::Location::Ids current;
	if (MMAPI::Location::TryGetCurrentLocation(current) && current == MMAPI::Location::Ids::Farm)
	{
		farm_scanned_this_session = true;
		PruneMissingCircles();
	}
}

void HandleAri(CInstance* self)
{
	if (!game_is_active) return;

	YYTK::RValue x; MMAPI::Internal::module_interface->GetBuiltin("x", self, NULL_INDEX, x);
	YYTK::RValue y; MMAPI::Internal::module_interface->GetBuiltin("y", self, NULL_INDEX, y);
	ari_x = x.ToDouble();
	ari_y = y.ToDouble();

	if (teleport_ari)
	{
		teleport_ari = false;
		// Convert tile coords to pixel coords (×8) and offset by half the 24px write_size_x for centering.
		int px = (teleport_target_tile.first  * 8) + 12;
		int py = (teleport_target_tile.second * 8) + 12;
		teleport_target_tile = {};
		MMAPI::Location::TeleportAri(MMAPI::Location::Ids::Farm, px, py);
	}

	if (play_conversation)
	{
		play_conversation = false;
		// Fire the activation-confirmation conversation. The accept/reject selections route
		// through play_text where OnBeforePlayText cancels the original and (for accept)
		// flags the teleport.
		MMAPI::Text::PlayConversation(ACTIVATION_REQUIRED_CONVERSATION_KEY);
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

	MMAPI::Item::Enable();
	MMAPI::Location::Enable();
	MMAPI::Object::Enable();
	MMAPI::Text::Enable();
	MMAPI::Game::Enable();

	LoadOrCreateConfigFile();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterGameActive(OnGameActive);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Game::Hooks::AfterLoadGame(OnAfterLoadGame);
	MMAPI::Instance::Hooks::BeforeInteract(OnBeforeInteract);
	MMAPI::Instance::Hooks::OnObjectCall(MMAPI::Instance::Objects::Ari, HandleAri);
	MMAPI::Text::Hooks::BeforeLocalizedString(OnBeforeLocalizedString);
	MMAPI::Text::Hooks::BeforePlayText(OnBeforePlayText);
	MMAPI::Object::Hooks::BeforeFurniturePlaced(OnBeforeFurniturePlaced);
	MMAPI::Object::Hooks::AfterFurniturePlaced(OnAfterFurniturePlaced);
	MMAPI::Object::Hooks::AfterObjectErased(OnAfterObjectErased);
	MMAPI::Weather::Hooks::AfterRoomStart(OnAfterRoomStart);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
