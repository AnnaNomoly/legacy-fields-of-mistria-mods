#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;

// ----- Mod metadata -----

static const char* const MOD_NAME = "DirectionalAttacks";
static const char* const VERSION  = "1.1.0";

// Items whose `use_item` action should be mouse-direction-aimed: fishing rods, nets, and swords.
// On use, the mod determines which compass quadrant the mouse cursor is in and forces Ari to face
// that direction before the action proceeds.
static const std::vector<std::string> DIRECTIONAL_ITEM_NAMES = {
	// Fishing Rods
	"fishing_rod_worn", "fishing_rod_copper", "fishing_rod_iron", "fishing_rod_silver", "fishing_rod_gold", "fishing_rod_mistril",
	// Nets
	"net_worn", "net_copper", "net_iron", "net_silver", "net_gold", "net_mistril",
	// Swords
	"sword_worn", "sword_copper", "sword_iron", "sword_silver", "sword_gold", "sword_mistril",
	"sword_scrap_metal", "sword_verdigris", "sword_crystal", "sword_tarnished_gold",
};

// ----- State -----

static bool startup_loaded = false;

// Item ids resolved from DIRECTIONAL_ITEM_NAMES at title-screen setup. Populated once.
static std::unordered_set<int> directional_item_ids;

// ----- Helpers -----

// Returns true if the point `p` lies inside the triangle defined by `p0`, `p1`, `p2`. Standard
// barycentric-coords sign test — preserved from the original mod.
bool PointInTriangle(double px, double py,
                     double p0x, double p0y,
                     double p1x, double p1y,
                     double p2x, double p2y)
{
	double A    = 0.5 * (-p1y * p2x + p0y * (-p1x + p2x) + p0x * (p1y - p2y) + p1x * p2y);
	double sign = (A < 0) ? -1.0 : 1.0;
	double s    = (p0y * p2x - p0x * p2y + (p2y - p0y) * px + (p0x - p2x) * py) * sign;
	double t    = (p0x * p1y - p0y * p1x + (p0y - p1y) * px + (p1x - p0x) * py) * sign;
	return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}

// Maps the mouse cursor position to one of four compass quadrants — N/S/E/W triangles emanating from
// the window center to each window edge. Returns std::nullopt if the cursor is exactly at the center
// (or off-screen) so no direction can be determined.
std::optional<MMAPI::Player::Cardinal> MouseDirectionToCardinal()
{
	double w  = MMAPI::Engine::GetWindowWidth();
	double h  = MMAPI::Engine::GetWindowHeight();
	double mx = MMAPI::Engine::GetMouseX();
	double my = MMAPI::Engine::GetMouseY();

	double cx = w / 2.0;
	double cy = h / 2.0;

	if (PointInTriangle(mx, my,  cx, cy,  0.0, 0.0,  w,   0.0)) return MMAPI::Player::Cardinal::North;
	if (PointInTriangle(mx, my,  cx, cy,  0.0, h,    w,   h  )) return MMAPI::Player::Cardinal::South;
	if (PointInTriangle(mx, my,  cx, cy,  w,   0.0,  w,   h  )) return MMAPI::Player::Cardinal::East;
	if (PointInTriangle(mx, my,  cx, cy,  0.0, 0.0,  0.0, h  )) return MMAPI::Player::Cardinal::West;
	return std::nullopt;
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	if (startup_loaded) return;

	for (const auto& name : DIRECTIONAL_ITEM_NAMES)
	{
		YYTK::RValue id = MMAPI::Item::GetIdFromInternalName(name);
		if (MMAPI::Engine::IsNumeric(id))
			directional_item_ids.insert(static_cast<int>(id.ToInt64()));
	}

	startup_loaded = true;
}

void OnBeforeUseItem(MMAPI::Item::UseItemContext& ctx)
{
	if (!ctx.IsAriUse()) return;
	if (!directional_item_ids.contains(ctx.GetItemId())) return;

	auto cardinal = MouseDirectionToCardinal();
	if (cardinal)
		MMAPI::Player::FaceCardinal(*cardinal);
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
	MMAPI::Player::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Item::Hooks::BeforeUseItem(OnBeforeUseItem);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
