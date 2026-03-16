#include <random>
#include <fstream>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp> // YYTK v4
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const MOD_NAME = "SecretSanta";
static const char* const VERSION = "1.2.1";
static const char* const GML_SCRIPT_DAY = "gml_Script_day@Calendar@Calendar";
static const char* const GML_SCRIPT_SEASON = "gml_Script_season@Calendar@Calendar";
static const char* const GML_SCRIPT_YEAR = "gml_Script_year@Calendar@Calendar";
static const char* const GML_SCRIPT_PLAY_TEXT = "gml_Script_play_text@TextboxMenu@TextboxMenu";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const char* const GML_SCRIPT_SHOW_ROOM_TITLE = "gml_Script_show_room_title";
static const char* const GML_SCRIPT_LOAD_GAME = "gml_Script_load_game";
static const char* const GML_SCRIPT_SAVE_GAME = "gml_Script_save_game";
static const char* const GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT = "gml_Script_choose_random_artifact@Archaeology@Archaeology";
static const char* const GML_SCRIPT_T2_READ = "gml_Script_read@T2r@T2r";
static const char* const YYTK_KEY = "__YYTK";
static const char* const DIG_UP_ANYTHING_KEY = "DigUpAnything";
static const char* const SECRET_SANTA_KEY = "SecretSanta";
static const char* const IGNORE_NEXT_DIG_SPOT_KEY = "ignore_next_dig_spot";
static const double UNSET_INT = -1;
static const double UNSET_DOUBLE = -1.0;
static const std::string GIFTS[] = {
	"berry_bowl", "beet_soup", "fried_rice", "vegetable_pot_pie", "floral_tea",
	"tulip_cake", "sushi_platter", "lobster_roll", "summer_salad", "vegetable_quiche"
};
static const std::string ACTIVE_NPC_NAMES[] = { // All NPCs that are currently functional in game.
	"adeline", "balor", "caldarus", "celine", "darcy", "dell", "dozy", "eiland",
	"elsie", "errol", "hayden", "hemlock", "henrietta", "holt", "josephine",
	"juniper", "landen", "luc", "louis", "maple", "march", "merri",
	"olric", "nora", "reina", "ryis", "seridia", "taliferro", "terithia", "valen", "vera", "wheedle"
	// TODO When Released: Stillwell, Zorel
};
static const std::unordered_set<std::string> UNLOCKABLE_NPC_NAMES = { // NPCs that must be unlocked
	"caldarus", "darcy", "louis", "merri", "seridia", "taliferro", "vera", "wheedle"
	// TODO When Released: Stillwell, Zorel
};
static const std::string MAGICAL_SNOWFLAKE_ITEM_NAME = "magical_snowflake";

// JSON key names
static const std::string SENDER_KEY = "sender";
static const std::string RECIPIENT_KEY = "recipient";
static const std::string MAIL_SENT_KEY = "mail_sent";

// Gif received dialogue.
static const std::string ADELINE_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Adeline/init";
static const std::string BALOR_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Balor/init";
static const std::string CALDARUS_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Caldarus/init";
static const std::string CELINE_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Celine/init";
static const std::string DARCY_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Darcy/init";
static const std::string DELL_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Dell/init";
static const std::string DOZY_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Dozy/init";
static const std::string EILAND_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Eiland/init";
static const std::string ELSIE_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Elsie/init";
static const std::string ERROL_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Errol/init";
static const std::string HAYDEN_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Hayden/init";
static const std::string HEMLOCK_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Hemlock/init";
static const std::string HENRIETTA_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Henrietta/init";
static const std::string HOLT_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Holt/init";
static const std::string JOSEPHINE_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Josephine/init";
static const std::string JUNIPER_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Juniper/init";
static const std::string LANDEN_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Landen/init";
static const std::string LUC_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Luc/init";
static const std::string LOUIS_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Louis/init";
static const std::string MAPLE_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Maple/init";
static const std::string MARCH_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/March/init";
static const std::string MERRI_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Merri/init";
static const std::string OLRIC_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Olric/init";
static const std::string NORA_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Nora/init";
static const std::string REINA_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Reina/init";
static const std::string RYIS_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Ryis/init";
static const std::string SERIDIA_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Seridia/init";
// TODO: Stillwell
static const std::string TALIFERRO_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Taliferro/init";
static const std::string TERITHIA_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Terithia/init";
static const std::string VALEN_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Valen/init";
static const std::string VERA_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Vera/init";
static const std::string WHEEDLE_GIFT_RECEIVED_DIALOGUE_KEY = "Conversations/Mods/Secret Santa/Wheedle/init";
// TODO: Zorel

static YYTKInterface* g_ModuleInterface = nullptr;
static RValue __YYTK;
static bool load_on_start = true;
static bool mod_healthy = true;
static int day = UNSET_DOUBLE;
static int season = UNSET_DOUBLE;
static int year = UNSET_DOUBLE;
static int magical_snowflake_item_id = UNSET_INT;
static std::string save_prefix = "";
static std::string mod_folder = "";
static std::string secret_santa_sender = "";
static std::string secret_santa_recipient = "";
static json json_object = json::object();
static RValue custom_dialogue_value;
static RValue* custom_dialogue_value_ptr = nullptr;
static std::map<std::string, std::vector<CInstance*>> script_name_to_reference_map = {};

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
}

bool GlobalVariableExists(const char* variable_name)
{
	RValue global_variable_exists = g_ModuleInterface->CallBuiltin(
		"variable_global_exists",
		{ variable_name }
	);

	return RValueAsBool(global_variable_exists);
}

RValue GlobalVariableGet(const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_get",
		{ variable_name }
	);
}

RValue GlobalVariableSet(const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"variable_global_set",
		{ variable_name, value }
	);
}

bool StructVariableExists(RValue the_struct, const char* variable_name)
{
	RValue struct_exists = g_ModuleInterface->CallBuiltin(
		"struct_exists",
		{ the_struct, variable_name }
	);

	return RValueAsBool(struct_exists);
}

RValue StructVariableGet(RValue the_struct, const char* variable_name)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ the_struct, variable_name }
	);
}

RValue StructVariableSet(RValue the_struct, const char* variable_name, RValue value)
{
	return g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ the_struct, variable_name, value }
	);
}

void CreateOrGetGlobalYYTKVariable()
{
	if (!GlobalVariableExists(YYTK_KEY))
	{
		g_ModuleInterface->GetRunnerInterface().StructCreate(&__YYTK);
		GlobalVariableSet(YYTK_KEY, __YYTK);
	}
	else
		__YYTK = GlobalVariableGet(YYTK_KEY);
}

void CreateModInfoInGlobalYYTKVariable()
{
	if (!StructVariableExists(__YYTK, SECRET_SANTA_KEY))
	{
		RValue SecretSanta;
		RValue version = VERSION;
		RValue ignore_next_dig_spot = false;
		g_ModuleInterface->GetRunnerInterface().StructCreate(&SecretSanta);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&SecretSanta, "version", &version);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&SecretSanta, IGNORE_NEXT_DIG_SPOT_KEY, &ignore_next_dig_spot);
		g_ModuleInterface->GetRunnerInterface().StructAddRValue(&__YYTK, SECRET_SANTA_KEY, &SecretSanta);
	}
}

bool IgnoreNextDigSpot()
{
	if (GlobalVariableExists(YYTK_KEY))
	{
		RValue __YYTK = GlobalVariableGet(YYTK_KEY);
		if (StructVariableExists(__YYTK, SECRET_SANTA_KEY))
		{
			RValue SecretSanta = StructVariableGet(__YYTK, SECRET_SANTA_KEY);
			if (StructVariableExists(SecretSanta, IGNORE_NEXT_DIG_SPOT_KEY))
			{
				RValue ignore_next_dig_spot = StructVariableGet(SecretSanta, IGNORE_NEXT_DIG_SPOT_KEY);
				return RValueAsBool(ignore_next_dig_spot);
			}
		}
	}

	return false;
}

void PrintException(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", VERSION, e.what());
	}
}

std::string FormatDateString(int day, int season, int year)
{
	return std::to_string(day) + "-" + std::to_string(season) + "-" + std::to_string(year);
}

void ReadModFile()
{
	// Read the custom mod data file.
	if (save_prefix.length() != 0 && mod_folder.length() != 0)
	{
		std::exception_ptr eptr;
		try
		{
			std::ifstream in_stream(mod_folder + "\\" + save_prefix + ".json");
			if (in_stream.good())
			{
				json_object = json::parse(in_stream);
			}
			else
			{
				json_object = {};
			}
		}
		catch (...)
		{
			mod_healthy = false;
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred reading the mod file.", MOD_NAME, VERSION);

			eptr = std::current_exception();
			PrintException(eptr);
		}
	}
	else
	{
		mod_healthy = false;
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No save prefix detected when trying to read the mod file!", MOD_NAME, VERSION);
	}
}

void SaveModFile()
{
	// Write the custom mod data file.
	if (save_prefix.length() != 0 && mod_folder.length() != 0)
	{
		std::exception_ptr eptr;
		try
		{
			std::ofstream out_stream(mod_folder + "\\" + save_prefix + ".json");
			out_stream << std::setw(4) << json_object << std::endl;
			out_stream.close();
			g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Saved the mod file!", MOD_NAME, VERSION);
		}
		catch (...)
		{
			mod_healthy = false;
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred writing the mod file.", MOD_NAME, VERSION);

			eptr = std::current_exception();
			PrintException(eptr);
		}
	}
	else
	{
		mod_healthy = false;
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No save prefix detected when trying to save the mod file!", MOD_NAME, VERSION);
	}
}

void SendMail(std::string mail_name_str)
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	RValue __ari = *global_instance->GetRefMember("__ari");
	RValue inbox = *__ari.GetRefMember("inbox");
	RValue contents = *inbox.GetRefMember("contents");
	RValue contents_buffer = *contents.GetRefMember("__buffer");

	RValue mail;
	RValue mail_items_taken = false;
	RValue mail_name = RValue(mail_name_str);
	RValue mail_read = false;
	g_ModuleInterface->GetRunnerInterface().StructCreate(&mail);
	g_ModuleInterface->GetRunnerInterface().StructAddRValue(&mail, "items_taken", &mail_items_taken);
	g_ModuleInterface->GetRunnerInterface().StructAddRValue(&mail, "name", &mail_name);
	g_ModuleInterface->GetRunnerInterface().StructAddRValue(&mail, "read", &mail_read);

	g_ModuleInterface->CallBuiltin(
		"array_push",
		{ contents_buffer, mail }
	);

	RValue size = g_ModuleInterface->CallBuiltin(
		"struct_get",
		{ contents, "__count" }
	);
	RValue new_size = 1.0 + size.m_Real;

	g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ contents, "__count", new_size }
	);
	g_ModuleInterface->CallBuiltin(
		"struct_set",
		{ contents, "__internal_size", new_size }
	);
}

bool MailExists(std::string mail_name_str)
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	RValue __ari = global_instance->GetMember("__ari");
	RValue inbox = __ari.GetMember("inbox");
	RValue contents = inbox.GetMember("contents");
	RValue contents_buffer = contents.GetMember("__buffer");

	size_t size = 0;
	g_ModuleInterface->GetArraySize(contents_buffer, size);

	for (size_t i = 0; i < size; i++)
	{
		RValue* entry = nullptr;
		g_ModuleInterface->GetArrayEntry(contents_buffer, i, entry);

		RValue name = entry->GetMember("name");
		if (name.ToString() == mail_name_str.c_str())
			return true;
	}

	return false;
}

int GetNpcId(std::string npc_name)
{
	CInstance* global_instance = nullptr;
	g_ModuleInterface->GetGlobalInstance(&global_instance);

	RValue __npc_id__ = global_instance->GetMember("__npc_id__");
	size_t size = 0;
	g_ModuleInterface->GetArraySize(__npc_id__, size);

	for (size_t i = 0; i < size; i++)
	{
		RValue* element = nullptr;
		g_ModuleInterface->GetArrayEntry(__npc_id__, i, element);

		std::string element_str = element->ToString();
		if (element_str == npc_name)
		{
			return static_cast<int>(i);
		}
	}

	return UNSET_INT;
}

void AddHeartPoints(std::string npc_name)
{
	int npc_id = GetNpcId(npc_name);
	if (npc_id != UNSET_INT)
	{
		CInstance* global_instance = nullptr;
		g_ModuleInterface->GetGlobalInstance(&global_instance);

		RValue __npc_database = *global_instance->GetRefMember("__npc_database");
		RValue npc_database_0 = __npc_database[npc_id];
		RValue original_heart_points = *npc_database_0.GetRefMember("heart_points");
		RValue modified_heart_points = original_heart_points.ToDouble() + 20.0;

		g_ModuleInterface->CallBuiltin(
			"struct_set", {
				npc_database_0, "heart_points", modified_heart_points
			}
		);

		g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Added %d bonus heart points for NPC: %s", MOD_NAME, VERSION, 20, npc_name.c_str());
	}
	else
	{
		g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Unable to lookup ID for NPC: %s", MOD_NAME, VERSION, npc_name.c_str());
	}
}

RValue ReadValueFromT2Database(std::string t2_key, CInstance* Self, CInstance* Other)
{
	CScript* gml_script = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_T2_READ,
		(PVOID*)&gml_script
	);

	RValue result;
	RValue input = RValue(t2_key);
	RValue* input_ptr = &input;
	gml_script->m_Functions->m_ScriptFunction(
		Self,
		Other,
		result,
		1,
		{ &input_ptr }
	);

	return result;
}

int GetItemId(CInstance* self, CInstance* other, std::string item_name)
{
	CScript* gml_script_try_string_to_item_id = nullptr;
	g_ModuleInterface->GetNamedRoutinePointer(
		"gml_Script_try_string_to_item_id",
		(PVOID*)&gml_script_try_string_to_item_id
	);

	RValue result;
	RValue argument = RValue(item_name);
	RValue* argument_ptr = &argument;
	gml_script_try_string_to_item_id->m_Functions->m_ScriptFunction(
		self,
		other,
		result,
		1,
		{ &argument_ptr }
	);

	if (result.m_Kind == VALUE_REAL || result.m_Kind == VALUE_INT64 || result.m_Kind == VALUE_INT32)
		return result.ToInt64();
	return UNSET_INT;
}

std::vector<std::string> GetUnlockedNpcs(CInstance* Self, CInstance* Other)
{
	std::vector<std::string> unlocked_npcs = {};

	for (std::string npc : ACTIVE_NPC_NAMES)
	{
		if (!UNLOCKABLE_NPC_NAMES.contains(npc) || ReadValueFromT2Database(npc + "_has_met", Self, Other).ToBoolean())
			unlocked_npcs.push_back(npc);
	}


	
	return unlocked_npcs;
}

void ResetStaticFields(bool returnedToTitleScreen)
{
	if (returnedToTitleScreen)
	{
		day = UNSET_DOUBLE;
		season = UNSET_DOUBLE;
		year = UNSET_DOUBLE;
		save_prefix = "";
		secret_santa_sender = "";
		secret_santa_recipient = "";
		json_object = {};
		custom_dialogue_value = "";
		custom_dialogue_value_ptr = nullptr;
	}
}

RValue& GmlScriptCalendarDayCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_DAY));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (mod_healthy)
	{
		// Result is a VALUE_REAL that is the 0-indexed calendar day
		day = Result.ToInt64() + 1;
	}

	return Result;
}

RValue& GmlScriptCalendarSeasonCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SEASON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (mod_healthy)
	{
		// Result is a VALUE_REAL that is the 0-indexed calendar season
		season = Result.ToInt64() + 1;
	}

	return Result;
}

RValue& GmlScriptCalendarYearCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_YEAR));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (mod_healthy)
	{
		// Result is a VALUE_REAL that is the 0-indexed calendar year
		year = Result.ToInt64() + 1;
	}

	return Result;
}

RValue& GmlScriptPlayTextCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		if (season == 4 && day == 27)
		{
			std::string conversation_name = Arguments[0]->ToString();
			std::string conversation_name_lowercase = conversation_name;
			std::transform(conversation_name_lowercase.begin(), conversation_name_lowercase.end(), conversation_name_lowercase.begin(), [](unsigned char c) { return std::tolower(c); });

			// Dialog involves the secret santa recipient.
			if (conversation_name_lowercase.contains(secret_santa_recipient))
			{
				// Dialog is a gift line.
				if (conversation_name.contains("gift_lines"))
				{
					// Speaker is Adeline.
					if (conversation_name.contains("Conversations/Bank/Adeline"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(ADELINE_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Balor.
					if (conversation_name.contains("Conversations/Bank/Balor"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(BALOR_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Caldarus.
					if (conversation_name.contains("Conversations/Bank/Caldarus"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(CALDARUS_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Celine.
					if (conversation_name.contains("Conversations/Bank/Celine"))
					{
						custom_dialogue_value = RValue(CELINE_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Darcy.
					if (conversation_name.contains("Conversations/Bank/Darcy"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(DARCY_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Dell.
					if (conversation_name.contains("Conversations/Bank/Dell"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(DELL_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Dozy.
					if (conversation_name.contains("Conversations/Bank/Dozy"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(DOZY_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Eiland.
					if (conversation_name.contains("Conversations/Bank/Eiland"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(EILAND_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Elsie.
					if (conversation_name.contains("Conversations/Bank/Elsie"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(ELSIE_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Errol.
					if (conversation_name.contains("Conversations/Bank/Errol"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(ERROL_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Hayden.
					if (conversation_name.contains("Conversations/Bank/Hayden"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(HAYDEN_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Hemlock.
					if (conversation_name.contains("Conversations/Bank/Hemlock"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(HEMLOCK_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Henrietta.
					if (conversation_name.contains("Conversations/Bank/Henrietta"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(HENRIETTA_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Holt.
					if (conversation_name.contains("Conversations/Bank/Holt"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(HOLT_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Josephine.
					if (conversation_name.contains("Conversations/Bank/Josephine"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(JOSEPHINE_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Juniper.
					if (conversation_name.contains("Conversations/Bank/Juniper"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(JUNIPER_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Landen.
					if (conversation_name.contains("Conversations/Bank/Landen"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(LANDEN_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Luc.
					if (conversation_name.contains("Conversations/Bank/Luc"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(LUC_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Louis.
					if (conversation_name.contains("Conversations/Bank/Louis"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(LOUIS_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Maple.
					if (conversation_name.contains("Conversations/Bank/Maple"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(MAPLE_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is March.
					if (conversation_name.contains("Conversations/Bank/March"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(MARCH_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Merri.
					if (conversation_name.contains("Conversations/Bank/Merri"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(MERRI_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Olric.
					if (conversation_name.contains("Conversations/Bank/Olric"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(OLRIC_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Nora.
					if (conversation_name.contains("Conversations/Bank/Nora"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(NORA_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Reina.
					if (conversation_name.contains("Conversations/Bank/Reina"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(REINA_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Ryis.
					if (conversation_name.contains("Conversations/Bank/Ryis"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(RYIS_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Seridia.
					if (conversation_name.contains("Conversations/Bank/Seridia"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(SERIDIA_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// TODO: Stillwell

					// Speaker is Taliferro.
					if (conversation_name.contains("Conversations/Bank/Taliferro"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(TALIFERRO_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// Speaker is Terithia.
					if (conversation_name.contains("Conversations/Bank/Terithia"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(TERITHIA_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Valen.
					if (conversation_name.contains("Conversations/Bank/Valen"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(VALEN_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Vera.
					if (conversation_name.contains("Conversations/Bank/Vera"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(VERA_GIFT_RECEIVED_DIALOGUE_KEY);
						RValue* custom_dialog_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialog_ptr;
					}

					// Speaker is Wheedle.
					if (conversation_name.contains("Conversations/Bank/Wheedle"))
					{
						// Override the gift dialog.
						custom_dialogue_value = RValue(WHEEDLE_GIFT_RECEIVED_DIALOGUE_KEY);
						custom_dialogue_value_ptr = &custom_dialogue_value;
						Arguments[0] = custom_dialogue_value_ptr;
					}

					// TODO: Zorel

					// Increase heart points.
					AddHeartPoints(secret_santa_recipient);
				}
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_TEXT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptSetupMainScreenCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	ResetStaticFields(true);

	if (load_on_start)
	{
		CreateOrGetGlobalYYTKVariable();
		CreateModInfoInGlobalYYTKVariable();

		std::exception_ptr eptr;
		try
		{
			// Try to find the mod_data directory.
			std::string current_dir = std::filesystem::current_path().string();
			std::string mod_data_folder = current_dir + "\\mod_data";
			if (!std::filesystem::exists(mod_data_folder))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"mod_data\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, mod_data_folder.c_str());
				std::filesystem::create_directory(mod_data_folder);
			}

			// Try to find the mod_data\SecretSanta directory.
			std::string secret_santa_folder = mod_data_folder + "\\SecretSanta";
			if (!std::filesystem::exists(secret_santa_folder))
			{
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"SecretSanta\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, secret_santa_folder.c_str());
				std::filesystem::create_directory(secret_santa_folder);
			}

			mod_folder = secret_santa_folder;
		}
		catch (...)
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred when locating the mod directory.", MOD_NAME, VERSION);

			eptr = std::current_exception();
			PrintException(eptr);
			mod_healthy = false;
		}

		int item_id = GetItemId(Self, Other, MAGICAL_SNOWFLAKE_ITEM_NAME);
		if (item_id != UNSET_INT)
		{
			magical_snowflake_item_id = item_id;
		}
		else
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to look up item ID for: magical_snowflake.", MOD_NAME, VERSION);
			mod_healthy = false;
		}

		if (!mod_healthy)
		{
			g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - The mod has an unhealthy configuration and will NOT function!", MOD_NAME, VERSION);
		}

		load_on_start = false;
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SETUP_MAIN_SCREEN));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptShowRoomTitleCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		if (season == 4) // winter
		{
			const std::string current_year_winter_20_date_string = FormatDateString(20, 4, year);
			const std::string current_year_winter_26_date_string = FormatDateString(26, 4, year);
			const std::string current_year_winter_27_date_string = FormatDateString(27, 4, year);
			const std::string current_year_object_key = "year-" + std::to_string(year);

			if (!MailExists("secret_santa_first_year"))
				SendMail("secret_santa_first_year");

			if (day >= 20)
			{
				if (!json_object.contains(current_year_object_key))
				{
					// Randomly determine the secret santa sender and recipient.
					std::random_device rd;
					std::mt19937 gen(rd());

					const std::vector<std::string> unlocked_npcs = GetUnlockedNpcs(script_name_to_reference_map[GML_SCRIPT_T2_READ][0], script_name_to_reference_map[GML_SCRIPT_T2_READ][1]);
					std::uniform_int_distribution<> distr(0, static_cast<int>(unlocked_npcs.size() - 1));

					int random_sender = distr(gen);
					int random_recipient = distr(gen);

					// Create the current year object.
					json current_year_object = json::object();
					current_year_object[SENDER_KEY] = unlocked_npcs[random_sender];
					current_year_object[RECIPIENT_KEY] = unlocked_npcs[random_recipient];

					// Create the date objects.
					json current_year_winter_20_object = json::object();
					current_year_winter_20_object[MAIL_SENT_KEY] = false;

					json current_year_winter_26_object = json::object();
					current_year_winter_26_object[MAIL_SENT_KEY] = false;

					json current_year_winter_27_object = json::object();
					current_year_winter_27_object[MAIL_SENT_KEY] = false;

					// Add the date objects to the current year object.
					current_year_object[current_year_winter_20_date_string] = current_year_winter_20_object;
					current_year_object[current_year_winter_26_date_string] = current_year_winter_26_object;
					current_year_object[current_year_winter_27_date_string] = current_year_winter_27_object;

					// Update the JSON object.
					json_object[current_year_object_key] = current_year_object;
				}

				// Set the static sender and recipient fields using the JSON data.
				secret_santa_sender = json_object[current_year_object_key][SENDER_KEY];
				secret_santa_recipient = json_object[current_year_object_key][RECIPIENT_KEY];

				if (day == 20)
				{
					bool mail_sent = json_object[current_year_object_key][current_year_winter_20_date_string][MAIL_SENT_KEY];
					if (!mail_sent)
					{
						std::string mail_name_str = "secret_santa_notice_" + secret_santa_recipient;
						SendMail(mail_name_str);

						json_object[current_year_object_key][current_year_winter_20_date_string][MAIL_SENT_KEY] = true;
					}
				}

				if (day == 26)
				{
					bool mail_sent = json_object[current_year_object_key][current_year_winter_26_date_string][MAIL_SENT_KEY];
					if (!mail_sent)
					{
						std::string mail_name_str = "secret_santa_reminder_" + secret_santa_recipient;
						SendMail(mail_name_str);

						json_object[current_year_object_key][current_year_winter_26_date_string][MAIL_SENT_KEY] = true;
					}
				}

				if (day == 27)
				{
					bool mail_sent = json_object[current_year_object_key][current_year_winter_27_date_string][MAIL_SENT_KEY];
					if (!mail_sent)
					{
						// Randomly select a gift.
						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_int_distribution<> distr(0, static_cast<int>(GIFTS->size() - 1));
						int random_gift = distr(gen);

						auto random_gift_str = GIFTS[random_gift];

						std::string mail_name_str = "secret_santa_" + secret_santa_sender + "_" + random_gift_str;
						SendMail(mail_name_str);

						json_object[current_year_object_key][current_year_winter_27_date_string][MAIL_SENT_KEY] = true;
					}
				}
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SHOW_ROOM_TITLE));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptLoadGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		// Get the save file name.
		std::string save_file = std::string(Arguments[0]->GetRefMember("save_path")->ToString());
		std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
		std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);

		// Get the save prefix.
		std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
		std::size_t second_hyphen_index = save_name.find_last_of("-");
		save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));

		// Read from the custom mod data file.
		ReadModFile();
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_LOAD_GAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptSaveGameCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (mod_healthy)
	{
		// No save prefix has been detected. This should only happen when a new game is started.
		if (save_prefix.size() == 0)
		{
			// Get the save file name.
			std::string save_file = Arguments[0]->ToString();
			std::size_t save_file_name_delimiter_index = save_file.find_last_of("/");
			std::string save_name = save_file.substr(save_file_name_delimiter_index + 1);

			// Check it's a valid value.
			if (save_name.contains("undefined") == std::string::npos)
			{
				// Get the save prefix.
				std::size_t first_hyphen_index = save_name.find_first_of("-") + 1;
				std::size_t second_hyphen_index = save_name.find_last_of("-");
				save_prefix = save_name.substr(first_hyphen_index, (second_hyphen_index - first_hyphen_index));
			}
		}

		SaveModFile();
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_SAVE_GAME));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

RValue& GmlScriptChooseRandomArtifactCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (mod_healthy)
	{
		if (!IgnoreNextDigSpot())
		{
			if (season == 4) // winter
			{
				// Randomly roll 1/20 chance to get a Magical Snowflake item.
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distr(1, 10);
				int random_int = distr(gen);

				if (random_int == 7)
				{
					g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - You found a Magical Snowflake!", MOD_NAME, VERSION);
					Result.m_i64 = magical_snowflake_item_id;
				}
			}
		}
		else
		{
			RValue SecretSanta = StructVariableGet(__YYTK, "SecretSanta");
			RValue ignore_next_dig_spot = false;
			StructVariableSet(SecretSanta, "ignore_next_dig_spot", ignore_next_dig_spot);
		}
	}

	return Result;
}

RValue& GmlScriptT2ReadCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!script_name_to_reference_map.contains(GML_SCRIPT_T2_READ))
		script_name_to_reference_map[GML_SCRIPT_T2_READ] = { Self, Other };

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_T2_READ));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	return Result;
}

void CreateHookGmlScriptCalendarDay(AurieStatus& status)
{
	CScript* gml_script_calendar_day = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_DAY,
		(PVOID*)&gml_script_calendar_day
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DAY);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_DAY,
		gml_script_calendar_day->m_Functions->m_ScriptFunction,
		GmlScriptCalendarDayCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_DAY);
	}
}

void CreateHookGmlScriptCalendarSeason(AurieStatus& status)
{
	CScript* gml_script_calendar_season = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SEASON,
		(PVOID*)&gml_script_calendar_season
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SEASON);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SEASON,
		gml_script_calendar_season->m_Functions->m_ScriptFunction,
		GmlScriptCalendarSeasonCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SEASON);
	}
}

void CreateHookGmlScriptCalendarYear(AurieStatus& status)
{
	CScript* gml_script_calendar_year = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_YEAR,
		(PVOID*)&gml_script_calendar_year
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_YEAR);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_YEAR,
		gml_script_calendar_year->m_Functions->m_ScriptFunction,
		GmlScriptCalendarYearCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_YEAR);
	}
}

void CreateHookGmlScriptPlayText(AurieStatus& status)
{
	CScript* gml_script_play_text = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_PLAY_TEXT,
		(PVOID*)&gml_script_play_text
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_TEXT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_PLAY_TEXT,
		gml_script_play_text->m_Functions->m_ScriptFunction,
		GmlScriptPlayTextCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_PLAY_TEXT);
	}
}

void CreateHookGmlScriptSetupMainScreen(AurieStatus& status)
{
	CScript* gml_script_setup_main_screen = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		(PVOID*)&gml_script_setup_main_screen
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SETUP_MAIN_SCREEN,
		gml_script_setup_main_screen->m_Functions->m_ScriptFunction,
		GmlScriptSetupMainScreenCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
	}
}

void CreateHookGmlScriptShowRoomTitle(AurieStatus& status)
{
	CScript* gml_Script_show_room_title = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SHOW_ROOM_TITLE,
		(PVOID*)&gml_Script_show_room_title
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SHOW_ROOM_TITLE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SHOW_ROOM_TITLE,
		gml_Script_show_room_title->m_Functions->m_ScriptFunction,
		GmlScriptShowRoomTitleCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SHOW_ROOM_TITLE);
	}
}

void CreateHookGmlScriptLoadGame(AurieStatus& status)
{
	CScript* gml_script_load_game = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_LOAD_GAME,
		(PVOID*)&gml_script_load_game
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LOAD_GAME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_LOAD_GAME,
		gml_script_load_game->m_Functions->m_ScriptFunction,
		GmlScriptLoadGameCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_LOAD_GAME);
	}
}

void CreateHookGmlScriptSaveGame(AurieStatus& status)
{
	CScript* gml_script_save_game = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_SAVE_GAME,
		(PVOID*)&gml_script_save_game
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_SAVE_GAME,
		gml_script_save_game->m_Functions->m_ScriptFunction,
		GmlScriptSaveGameCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SAVE_GAME);
	}
}

void CreateHookGmlScriptChooseRandomArtifact(AurieStatus& status)
{
	CScript* gml_script_choose_random_artifact = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT,
		(PVOID*)&gml_script_choose_random_artifact
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT,
		gml_script_choose_random_artifact->m_Functions->m_ScriptFunction,
		GmlScriptChooseRandomArtifactCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_CHOOSE_RANDOM_ARTIFACT);
	}
}

void CreateHookGmlScriptT2Read(AurieStatus& status)
{
	CScript* gml_script_t2_read = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_T2_READ,
		(PVOID*)&gml_script_t2_read
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_T2_READ);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_T2_READ,
		gml_script_t2_read->m_Functions->m_ScriptFunction,
		GmlScriptT2ReadCallback,
		nullptr
	);


	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_T2_READ);
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath) {
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateHookGmlScriptCalendarDay(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCalendarSeason(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptCalendarYear(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptPlayText(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSetupMainScreen(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptShowRoomTitle(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptLoadGame(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptSaveGame(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptChooseRandomArtifact(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptT2Read(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}