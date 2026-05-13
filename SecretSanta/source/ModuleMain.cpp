#include <algorithm>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

#include <YYToolkit/YYTK_Shared.hpp>
#include <MMAPI/MMAPI.hpp>

using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

// ----- Mod metadata -----

static const char* const MOD_NAME = "SecretSanta";
static const char* const VERSION  = "1.3.0";

// ----- Cross-mod IPC (read/written by DigUpAnything to suppress the magical-snowflake substitution) -----

static const char* const YYTK_KEY                 = "__YYTK";
static const char* const SECRET_SANTA_KEY         = "SecretSanta";
static const char* const IGNORE_NEXT_DIG_SPOT_KEY = "ignore_next_dig_spot";

// ----- Mod data -----

static const std::string MAGICAL_SNOWFLAKE_ITEM_NAME = "magical_snowflake";

// Random gift pool for the day-27 surprise mail.
static const std::vector<std::string> GIFTS = {
	"berry_bowl", "beet_soup", "fried_rice", "vegetable_pot_pie", "floral_tea",
	"tulip_cake", "sushi_platter", "lobster_roll", "summer_salad", "vegetable_quiche"
};

// NPCs that require player progression to be "met" — filtered against T2's `<npc>_has_met` flag
// when building the eligible Secret Santa pool.
static const std::unordered_set<MMAPI::NPC::Ids> UNLOCKABLE_NPCS = {
	MMAPI::NPC::Ids::Caldarus,
	MMAPI::NPC::Ids::Darcy,
	MMAPI::NPC::Ids::Louis,
	MMAPI::NPC::Ids::Merri,
	MMAPI::NPC::Ids::Seridia,
	MMAPI::NPC::Ids::Taliferro,
	MMAPI::NPC::Ids::Vera,
	MMAPI::NPC::Ids::Wheedle,
};

// NPCs not currently in the live game — excluded from the eligible pool regardless of T2 state.
// TODO: remove these when the game releases them and the corresponding "Conversations/Mods/Secret Santa/<Name>/init" key is authored.
static const std::unordered_set<MMAPI::NPC::Ids> NOT_RELEASED_NPCS = {
	MMAPI::NPC::Ids::Stillwell,
	MMAPI::NPC::Ids::Zorel,
};

// JSON keys in the per-save data file.
static const std::string SENDER_KEY    = "sender";
static const std::string RECIPIENT_KEY = "recipient";
static const std::string MAIL_SENT_KEY = "mail_sent";

// ----- State -----

static bool        startup_loaded            = false;
static bool        mod_healthy               = true;
static int         magical_snowflake_item_id = -1;
static std::string save_prefix;
static std::string secret_santa_sender;
static std::string secret_santa_recipient;
static json        mod_save_data = json::object();
static std::mt19937 rng(std::random_device{}());

// ----- Helpers -----

// Initializes the `__YYTK.SecretSanta` cross-mod struct (with `version` + `ignore_next_dig_spot`).
// DigUpAnything reads/sets the flag to suppress this mod's winter snowflake substitution.
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

	if (!MMAPI::Engine::StructVariableExists(__YYTK, SECRET_SANTA_KEY))
	{
		YYTK::RValue secret_santa;
		YYTK::RValue version              = VERSION;
		YYTK::RValue ignore_next_dig_spot = false;
		MMAPI::Internal::module_interface->GetRunnerInterface().StructCreate(&secret_santa);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&secret_santa, "version",                  &version);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&secret_santa, IGNORE_NEXT_DIG_SPOT_KEY,   &ignore_next_dig_spot);
		MMAPI::Internal::module_interface->GetRunnerInterface().StructAddRValue(&__YYTK,       SECRET_SANTA_KEY,           &secret_santa);
	}
}

// Returns true if DigUpAnything (or any cross-mod caller) flagged the next dig spot as "ignore",
// and clears the flag. Returns false when the flag wasn't set.
bool ConsumeIgnoreNextDigSpot()
{
	if (!MMAPI::Internal::module_interface->CallBuiltin("variable_global_exists", { YYTK_KEY }).ToBoolean())
		return false;

	YYTK::RValue __YYTK = MMAPI::Engine::GlobalVariableGet(YYTK_KEY);
	if (!MMAPI::Engine::StructVariableExists(__YYTK, SECRET_SANTA_KEY))
		return false;

	YYTK::RValue secret_santa = MMAPI::Engine::StructVariableGet(__YYTK, SECRET_SANTA_KEY);
	if (!MMAPI::Engine::StructVariableExists(secret_santa, IGNORE_NEXT_DIG_SPOT_KEY))
		return false;

	bool was_set = MMAPI::Engine::StructVariableGet(secret_santa, IGNORE_NEXT_DIG_SPOT_KEY).ToBoolean();
	if (was_set)
		MMAPI::Engine::StructVariableSet(secret_santa, IGNORE_NEXT_DIG_SPOT_KEY, false);
	return was_set;
}

bool IsActiveNpc(MMAPI::NPC::Ids npc)
{
	if (NOT_RELEASED_NPCS.contains(npc))
		return false;
	if (!UNLOCKABLE_NPCS.contains(npc))
		return true;
	return MMAPI::T2::Read(MMAPI::NPC::GetInternalName(npc) + "_has_met").ToBoolean();
}

std::vector<std::string> GetEligibleNpcNames()
{
	std::vector<std::string> names;
	MMAPI::NPC::ForEachId([&names](MMAPI::NPC::Ids npc) {
		if (IsActiveNpc(npc))
			names.push_back(MMAPI::NPC::GetInternalName(npc));
	});
	return names;
}

std::string FormatDateString(int day, MMAPI::Calendar::Seasons season, int year)
{
	// 1-indexed season + year — matches the original mod's date keys so existing save files still resolve.
	return std::to_string(day) + "-" + std::to_string(static_cast<int>(season) + 1) + "-" + std::to_string(year);
}

std::string CapitalizeFirst(std::string s)
{
	if (!s.empty())
		s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
	return s;
}

void ResetTransientState()
{
	save_prefix.clear();
	secret_santa_sender.clear();
	secret_santa_recipient.clear();
	mod_save_data = json::object();
}

// ----- Hooks -----

void OnSetupMainScreen()
{
	ResetTransientState();

	if (!startup_loaded)
	{
		EnsureCrossModStructExists();

		YYTK::RValue snowflake_rv = MMAPI::Item::GetIdFromInternalName(MAGICAL_SNOWFLAKE_ITEM_NAME);
		if (MMAPI::Engine::IsNumeric(snowflake_rv))
		{
			magical_snowflake_item_id = static_cast<int>(snowflake_rv.ToInt64());
		}
		else
		{
			MMAPI::Log::Error("Failed to look up item ID for: %s. Mod will NOT function!", MAGICAL_SNOWFLAKE_ITEM_NAME.c_str());
			mod_healthy = false;
		}

		startup_loaded = true;
	}
}

void OnAfterLoadGame(MMAPI::Game::LoadGameContext& ctx)
{
	if (!mod_healthy) return;
	save_prefix   = ctx.GetSavePrefix();
	mod_save_data = MMAPI::ModSave::Read(save_prefix);
}

void OnBeforeSaveGame(MMAPI::Game::SaveGameContext& ctx)
{
	if (!mod_healthy) return;
	if (save_prefix.empty())
		save_prefix = ctx.GetSavePrefix();  // First save of a new game.
	if (save_prefix.empty()) return;
	MMAPI::ModSave::Write(save_prefix, mod_save_data);
}

void OnAfterShowRoomTitle()
{
	if (!mod_healthy) return;
	if (!MMAPI::Calendar::IsSeason(MMAPI::Calendar::Seasons::Winter)) return;

	YYTK::RValue day_rv  = MMAPI::Calendar::GetDay();   // 1-indexed
	YYTK::RValue year_rv = MMAPI::Calendar::GetYear();  // 1-indexed
	if (!MMAPI::Engine::IsNumeric(day_rv) || !MMAPI::Engine::IsNumeric(year_rv)) return;
	int day  = static_cast<int>(day_rv.ToInt64());
	int year = static_cast<int>(year_rv.ToInt64());

	// First time arriving in winter: drop the intro mail.
	if (!MMAPI::Mail::Exists("secret_santa_first_year"))
		MMAPI::Mail::SendMail("secret_santa_first_year");

	if (day < 20) return;

	const std::string year_key   = "year-" + std::to_string(year);
	const std::string day_20_key = FormatDateString(20, MMAPI::Calendar::Seasons::Winter, year);
	const std::string day_26_key = FormatDateString(26, MMAPI::Calendar::Seasons::Winter, year);
	const std::string day_27_key = FormatDateString(27, MMAPI::Calendar::Seasons::Winter, year);

	// First entry in the year window: roll a sender + recipient and snapshot mail-state stubs.
	if (!mod_save_data.contains(year_key))
	{
		std::vector<std::string> eligible = GetEligibleNpcNames();
		if (eligible.empty()) return;  // Edge case: no NPCs are met yet — bail rather than crash.

		std::uniform_int_distribution<size_t> dist(0, eligible.size() - 1);
		json year_obj = json::object();
		year_obj[SENDER_KEY]    = eligible[dist(rng)];
		year_obj[RECIPIENT_KEY] = eligible[dist(rng)];
		year_obj[day_20_key]    = json::object({ { MAIL_SENT_KEY, false } });
		year_obj[day_26_key]    = json::object({ { MAIL_SENT_KEY, false } });
		year_obj[day_27_key]    = json::object({ { MAIL_SENT_KEY, false } });
		mod_save_data[year_key] = year_obj;
	}

	secret_santa_sender    = mod_save_data[year_key][SENDER_KEY];
	secret_santa_recipient = mod_save_data[year_key][RECIPIENT_KEY];

	if (day == 20 && !mod_save_data[year_key][day_20_key][MAIL_SENT_KEY])
	{
		MMAPI::Mail::SendMail("secret_santa_notice_" + secret_santa_recipient);
		mod_save_data[year_key][day_20_key][MAIL_SENT_KEY] = true;
	}
	if (day == 26 && !mod_save_data[year_key][day_26_key][MAIL_SENT_KEY])
	{
		MMAPI::Mail::SendMail("secret_santa_reminder_" + secret_santa_recipient);
		mod_save_data[year_key][day_26_key][MAIL_SENT_KEY] = true;
	}
	if (day == 27 && !mod_save_data[year_key][day_27_key][MAIL_SENT_KEY])
	{
		std::uniform_int_distribution<size_t> gift_dist(0, GIFTS.size() - 1);
		const std::string& gift = GIFTS[gift_dist(rng)];
		MMAPI::Mail::SendMail("secret_santa_" + secret_santa_sender + "_" + gift);
		mod_save_data[year_key][day_27_key][MAIL_SENT_KEY] = true;
	}
}

void OnBeforePlayConversation(MMAPI::Text::PlayConversationContext& ctx)
{
	if (!mod_healthy) return;
	if (secret_santa_recipient.empty()) return;
	if (!MMAPI::Calendar::IsSeason(MMAPI::Calendar::Seasons::Winter)) return;

	YYTK::RValue day_rv = MMAPI::Calendar::GetDay();
	if (!MMAPI::Engine::IsNumeric(day_rv) || day_rv.ToInt64() != 27) return;

	std::string_view key = ctx.GetKey();
	if (key.find("gift_lines") == std::string_view::npos) return;

	std::string capitalized = CapitalizeFirst(secret_santa_recipient);
	std::string bank_prefix = "Conversations/Bank/" + capitalized;
	if (key.find(bank_prefix) == std::string_view::npos) return;

	// Substitute the gift-line conversation with the recipient-specific Secret Santa init dialogue
	// and award bonus heart points.
	ctx.SetKey("Conversations/Mods/Secret Santa/" + capitalized + "/init");

	auto recipient_id = MMAPI::NPC::TryFromInternalName(secret_santa_recipient);
	if (recipient_id)
	{
		MMAPI::NPC::ModifyHeartPoints(*recipient_id, 20);
		MMAPI::Log::Info("Added 20 bonus heart points for NPC: %s", secret_santa_recipient.c_str());
	}
}

void OnAfterChooseRandomArtifact(MMAPI::Archaeology::AfterChooseRandomArtifactContext& ctx)
{
	if (!mod_healthy) return;
	if (ConsumeIgnoreNextDigSpot()) return;
	if (!MMAPI::Calendar::IsSeason(MMAPI::Calendar::Seasons::Winter)) return;

	// 1-in-10 chance to substitute the rolled artifact with a Magical Snowflake (preserved from original).
	std::uniform_int_distribution<int> dist(1, 10);
	if (dist(rng) != 7) return;

	ctx.SetItemId(magical_snowflake_item_id);
	MMAPI::Log::Info("You found a Magical Snowflake!");
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
	MMAPI::Calendar::Enable();
	MMAPI::Game::Enable();
	MMAPI::Item::Enable();
	MMAPI::Location::Enable();
	MMAPI::NPC::Enable();
	MMAPI::T2::Enable();
	MMAPI::Text::Enable();

	MMAPI::Game::Hooks::BeforeSetupMainScreen(OnSetupMainScreen);
	MMAPI::Game::Hooks::AfterLoadGame(OnAfterLoadGame);
	MMAPI::Game::Hooks::BeforeSaveGame(OnBeforeSaveGame);
	MMAPI::Location::Hooks::AfterShowRoomTitle(OnAfterShowRoomTitle);
	MMAPI::Text::Hooks::BeforePlayConversation(OnBeforePlayConversation);
	MMAPI::Archaeology::Hooks::AfterChooseRandomArtifact(OnAfterChooseRandomArtifact);

	MMAPI::Log::Info("Plugin started!");
	return AURIE_SUCCESS;
}
