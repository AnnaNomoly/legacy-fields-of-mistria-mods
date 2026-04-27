#include <set>
#include <map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <YYToolkit/YYTK_Shared.hpp>
using namespace Aurie;
using namespace YYTK;
using json = nlohmann::json;

static const char* const VERSION = "1.0.1";
static const char* const MOD_NAME = "DonateIt";
static const char* const GML_SCRIPT_GET_ITEM_UI_ICON = "gml_Script_get_ui_icon@anon@4244@LiveItem@LiveItem";
static const char* const GML_SCRIPT_NODE_OBJECT_SET_SPRITE = "gml_Script_set_sprite@gml_Object_obj_node_renderer_Create_0";
static const char* const GML_SCRIPT_SETUP_MAIN_SCREEN = "gml_Script_setup_main_screen@TitleMenu@TitleMenu";
static const std::string HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY = "highlight_items_in_menus";
static const std::string HIGHLIGHT_FISH_IN_WORLD_JSON_KEY = "highlight_fish_in_world";
static const std::string HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY = "highlight_bugs_in_world";
static const std::string HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY = "highlight_forage_in_world";
static const bool DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS = true;
static const bool DEFAULT_HIGHLIGHT_FISH_IN_WORLD = true;
static const bool DEFAULT_HIGHLIGHT_BUGS_IN_WORLD = true;
static const bool DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD = true;

static struct Configuration {
	bool highlight_items_in_menus = DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS;
	bool highlight_fish_in_world = DEFAULT_HIGHLIGHT_FISH_IN_WORLD;
	bool highlight_bugs_in_world = DEFAULT_HIGHLIGHT_BUGS_IN_WORLD;
	bool highlight_forage_in_world = DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD;
};

static YYTKInterface* g_ModuleInterface = nullptr;
static CInstance* global_instance = nullptr;
static bool load_on_start = true;
static std::set<int> donatable_items = {};
static std::set<std::string> donatable_item_names = {};
static std::map<std::string, int> item_name_to_id_map = {};
static std::map<int, std::string> item_id_to_name_map = {};
static std::map<std::string, int> object_category_to_id_map = {};
static Configuration configuration = Configuration();

void PrintError(std::exception_ptr eptr)
{
	try {
		if (eptr) {
			std::rethrow_exception(eptr);
		}
	}
	catch (const std::exception& e) {
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Error: %s", MOD_NAME, VERSION, e.what());
	}
}

json CreateConfigJson(bool use_defaults)
{
	json config_json = {
		{ HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY, use_defaults ? DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS : configuration.highlight_items_in_menus },
		{ HIGHLIGHT_FISH_IN_WORLD_JSON_KEY, use_defaults ? DEFAULT_HIGHLIGHT_FISH_IN_WORLD : configuration.highlight_fish_in_world },
		{ HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY, use_defaults ? DEFAULT_HIGHLIGHT_BUGS_IN_WORLD : configuration.highlight_bugs_in_world },
		{ HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY, use_defaults ? DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD : configuration.highlight_forage_in_world }
	};
	return config_json;
}

void CreateOrLoadConfigFile()
{
	// Load config file.
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

		// Try to find the mod_data/DonateIt directory.
		std::string donate_it_folder = mod_data_folder + "\\DonateIt";
		if (!std::filesystem::exists(donate_it_folder))
		{
			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DonateIt\" directory was not found. Creating directory: %s", MOD_NAME, VERSION, donate_it_folder.c_str());
			std::filesystem::create_directory(donate_it_folder);
		}

		// Try to find the mod_data/DonateIt/DonateIt.json config file.
		bool update_config_file = false;
		std::string config_file = donate_it_folder + "\\" + "DonateIt.json";
		std::ifstream in_stream(config_file);
		if (in_stream.good())
		{
			try
			{
				json json_object = json::parse(in_stream);

				// Check if the json_object is empty.
				if (json_object.empty())
				{
					g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - No values found in mod configuration file: %s!", MOD_NAME, VERSION, config_file.c_str());
					g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Add your desired values to the configuration file, otherwise defaults will be used.", MOD_NAME, VERSION);
				}
				else
				{
					// Try loading the highlight_items_in_menus value.
					if (json_object.contains(HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY))
					{
						configuration.highlight_items_in_menus = json_object[HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY, configuration.highlight_items_in_menus ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_ITEMS_IN_MENUS_JSON_KEY, DEFAULT_HIGHLIGHT_ITEMS_IN_MENUS ? "true" : "false");
					}

					// Try loading the highlight_fish_in_world value.
					if (json_object.contains(HIGHLIGHT_FISH_IN_WORLD_JSON_KEY))
					{
						configuration.highlight_fish_in_world = json_object[HIGHLIGHT_FISH_IN_WORLD_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_FISH_IN_WORLD_JSON_KEY, configuration.highlight_fish_in_world ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, HIGHLIGHT_FISH_IN_WORLD_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_FISH_IN_WORLD_JSON_KEY, DEFAULT_HIGHLIGHT_FISH_IN_WORLD ? "true" : "false");
					}

					// Try loading the highlight_bugs_in_world value.
					if (json_object.contains(HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY))
					{
						configuration.highlight_bugs_in_world = json_object[HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY, configuration.highlight_bugs_in_world ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_BUGS_IN_WORLD_JSON_KEY, DEFAULT_HIGHLIGHT_BUGS_IN_WORLD ? "true" : "false");
					}

					// Try loading the highlight_forage_in_world value.
					if (json_object.contains(HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY))
					{
						configuration.highlight_forage_in_world = json_object[HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY];
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using CUSTOM \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY, configuration.highlight_forage_in_world ? "true" : "false");
					}
					else
					{
						g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Missing \"%s\" value in mod configuration file: %s!", MOD_NAME, VERSION, HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY, config_file.c_str());
						g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Using DEFAULT \"%s\" value: %s!", MOD_NAME, VERSION, HIGHLIGHT_FORAGE_IN_WORLD_JSON_KEY, DEFAULT_HIGHLIGHT_FORAGE_IN_WORLD ? "true" : "false");
					}
				}

				update_config_file = true;
			}
			catch (...)
			{
				eptr = std::current_exception();
				PrintError(eptr);

				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to parse JSON from configuration file: %s", MOD_NAME, VERSION, config_file.c_str());
				g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - Make sure the file is valid JSON!", MOD_NAME, VERSION);
			}

			in_stream.close();
		}
		else
		{
			in_stream.close();

			g_ModuleInterface->Print(CM_LIGHTYELLOW, "[%s %s] - The \"DonateIt.json\" file was not found. Creating file: %s", MOD_NAME, VERSION, config_file.c_str());

			json default_config_json = CreateConfigJson(true);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << default_config_json << std::endl;
			out_stream.close();
		}

		if (update_config_file)
		{
			json config_json = CreateConfigJson(false);
			std::ofstream out_stream(config_file);
			out_stream << std::setw(4) << config_json << std::endl;
			out_stream.close();
		}
	}
	catch (...)
	{
		eptr = std::current_exception();
		PrintError(eptr);

		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - An error occurred loading the mod configuration file.", MOD_NAME, VERSION);
	}
}

bool IsNumeric(RValue value)
{
	return value.m_Kind == VALUE_INT32 || value.m_Kind == VALUE_INT64 || value.m_Kind == VALUE_REAL;
}

bool RValueAsBool(RValue value)
{
	if (value.m_Kind == VALUE_BOOL && value.m_Real == 1)
		return true;
	return false;
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

void LoadItemData()
{
	RValue __item_data = *global_instance->GetRefMember("__item_data");
	size_t array_length;
	g_ModuleInterface->GetArraySize(__item_data, array_length);

	// Load all items.
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(__item_data, i, array_element);

		RValue name_key = *array_element->GetRefMember("name_key"); // The item's localization key
		if (name_key.m_Kind != VALUE_NULL && name_key.m_Kind != VALUE_UNDEFINED && name_key.m_Kind != VALUE_UNSET)
		{
			RValue item_id = *array_element->GetRefMember("item_id");
			RValue recipe_key = *array_element->GetRefMember("recipe_key"); // The internal item original_sprite_name
			item_name_to_id_map[recipe_key.ToString()] = item_id.ToInt64();
			item_id_to_name_map[item_id.ToInt64()] = recipe_key.ToString();
		}
	}
}

void LoadDonatableItems()
{
	RValue museum_data = global_instance->GetMember("__museum_data");

	RValue data = museum_data.GetMember("data"); // array
	RValue data_length = g_ModuleInterface->CallBuiltin("array_length", { data });

	for (size_t i = 0; i < data_length.ToInt64(); ++i)
	{
		RValue data_entry = g_ModuleInterface->CallBuiltin("array_get", { data, i });
		RValue sets = data_entry.GetMember("sets");
		RValue inner = sets.GetMember("inner");

		std::vector<std::string> struct_field_names = {};
		auto GetStructFieldNames = [&](IN const char* MemberName, IN OUT RValue* Value) {
			struct_field_names.push_back(MemberName);
			return false;
		};

		g_ModuleInterface->EnumInstanceMembers(inner, GetStructFieldNames);
		for (size_t j = 0; j < struct_field_names.size(); ++j)
		{
			RValue set = inner.GetMember(struct_field_names[j]);

			RValue items = set.GetMember("items");
			RValue items_length = g_ModuleInterface->CallBuiltin("array_length", { items });

			for (size_t k = 0; k < items_length.ToInt64(); ++k)
			{
				RValue item = g_ModuleInterface->CallBuiltin("array_get", { items, k });
				donatable_items.insert(item.ToInt64());
			}
		}
	}
}

void LoadObjectCategories()
{
	RValue object_categories = global_instance->GetMember("__object_category__");
	RValue object_categories_length = g_ModuleInterface->CallBuiltin("array_length", { object_categories });

	for (size_t i = 0; i < object_categories_length.ToInt64(); ++i)
	{
		RValue object_category = g_ModuleInterface->CallBuiltin("array_get", { object_categories, i });
		object_category_to_id_map[object_category.ToString()] = i;
	}
}

bool ItemHasBeenDonated(int item_id)
{
	RValue museum_progress_data = global_instance->GetMember("__museum_progress_data");
	RValue item_donated = g_ModuleInterface->CallBuiltin("array_get", { museum_progress_data, item_id });
	return item_donated.ToBoolean();
}

void ObjectCallback(
	IN FWCodeEvent& CodeEvent
)
{
	auto& [self, other, code, argc, argv] = CodeEvent.Arguments();

	if (!self)
		return;

	if (!self->m_Object)
		return;

	if (configuration.highlight_fish_in_world && strstr(self->m_Object->m_Name, "obj_fish") && !StructVariableExists(self, "__donate_it__processed_fish") && StructVariableExists(self, "fish_loot"))
	{
		StructVariableSet(self, "__donate_it__processed_fish", true);

		int item_id = self->GetMember("fish_loot").GetMember("item").GetMember("item_id").ToInt64();
		if (donatable_items.contains(item_id) && !ItemHasBeenDonated(item_id))
		{
			// move_sprites
			RValue move_sprites = self->GetMember("move_sprites");
			RValue move_sprites_length = g_ModuleInterface->CallBuiltin("array_length", { move_sprites });
			for (int i = 0; i < move_sprites_length.ToInt64(); i++)
			{
				RValue original_move_sprite = g_ModuleInterface->CallBuiltin("array_get", { move_sprites, i });
				if (original_move_sprite.m_Kind == VALUE_REF)
				{
					RValue original_move_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { original_move_sprite });

					std::string replacement_move_sprite_name = original_move_sprite_name.ToString() + "_donatable";
					RValue replacement_move_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_move_sprite_name.c_str() });
					if (replacement_move_sprite.m_Kind == VALUE_REF)
						g_ModuleInterface->CallBuiltin("array_set", { move_sprites, i, replacement_move_sprite });
				}
			}

			// idle_sprites
			RValue idle_sprites = self->GetMember("idle_sprites");
			RValue idle_sprites_length = g_ModuleInterface->CallBuiltin("array_length", { idle_sprites });
			for (int i = 0; i < idle_sprites_length.ToInt64(); i++)
			{
				RValue original_idle_sprite = g_ModuleInterface->CallBuiltin("array_get", { idle_sprites, i });
				if (original_idle_sprite.m_Kind == VALUE_REF)
				{
					RValue original_idle_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { original_idle_sprite });

					std::string replacement_idle_sprite_name = original_idle_sprite_name.ToString() + "_donatable";
					RValue replacement_idle_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_idle_sprite_name.c_str() });
					if (replacement_idle_sprite.m_Kind == VALUE_REF)
						g_ModuleInterface->CallBuiltin("array_set", { idle_sprites, i, replacement_idle_sprite });
				}
			}
		}
	}
	else if (StructVariableExists(self, "__donate_it__processed_fish"))
		g_ModuleInterface->CallBuiltin("variable_instance_set", { self, "image_speed", 0.125 });

	if (configuration.highlight_bugs_in_world && strstr(self->m_Object->m_Name, "obj_bug") && !StructVariableExists(self, "__donate_it__processed_bug") && StructVariableExists(self, "item_id"))
	{
		StructVariableSet(self, "__donate_it__processed_bug", true);

		int item_id = self->GetMember("item_id").ToInt64();
		if (donatable_items.contains(item_id) && !ItemHasBeenDonated(item_id))
		{
			// move_sprite
			RValue original_move_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { self->GetMember("move_sprite") });
			std::string replacement_move_sprite_name = original_move_sprite_name.ToString() + "_donatable";
			RValue replacement_move_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_move_sprite_name.c_str() });
			if (replacement_move_sprite.m_Kind == VALUE_REF)
				StructVariableSet(self, "move_sprite", replacement_move_sprite);

			// idle_sprite
			RValue original_idle_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { self->GetMember("idle_sprite") });
			std::string replacement_idle_sprite_name = original_idle_sprite_name.ToString() + "_donatable";
			RValue replacement_idle_sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_idle_sprite_name.c_str() });
			if (replacement_idle_sprite.m_Kind == VALUE_REF)
				StructVariableSet(self, "idle_sprite", replacement_idle_sprite);
		}
	}
	else if (StructVariableExists(self, "__donate_it__processed_bug"))
		g_ModuleInterface->CallBuiltin("variable_instance_set", { self, "image_speed", 0.125 });
}

RValue& GmlScriptGetUiIconCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_ITEM_UI_ICON));
	original(
		Self,
		Other,
		Result,
		ArgumentCount,
		Arguments
	);

	if (configuration.highlight_items_in_menus && Self != nullptr)
	{
		RValue self = Self->ToRValue();
		if (StructVariableExists(self, "item_id"))
		{
			int item_id = self.GetMember("item_id").ToInt64();
			if (donatable_items.contains(item_id) && !ItemHasBeenDonated(item_id))
			{
				RValue type = g_ModuleInterface->CallBuiltin("asset_get_type", { Result });
				if (type.ToInt64() == 1) // asset_sprite
				{
					RValue original_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { Result });
					std::string replacement_sprite_name = original_sprite_name.ToString() + "_donatable";

					RValue sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
					if (sprite.m_Kind == VALUE_REF)
						Result = sprite;
				}
			}
		}
	}

	return Result;
}

RValue& GmlScriptNodeObjectSetSpriteCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (configuration.highlight_forage_in_world)
	{
		RValue node = Self->GetMember("node");
		RValue prototype = node.GetMember("prototype");
		if (StructVariableExists(prototype, "category_id"))
		{
			auto node_ref_map = node.ToRefMap(); // debug
			auto prototype_ref_map = prototype.ToRefMap(); // debug

			int category_id = prototype.GetMember("category_id").ToInt64();
			if (category_id == object_category_to_id_map["bush"] || category_id == object_category_to_id_map["crop"])
			{
				if (StructVariableExists(prototype, "harvest") && IsNumeric(prototype.GetMember("harvest")))
				{
					int item_id = prototype.GetMember("harvest").ToInt64();
					if (donatable_items.contains(item_id) && !ItemHasBeenDonated(item_id))
					{
						if (ArgumentCount > 0 && Arguments[0]->m_Kind == VALUE_REF)
						{
							RValue asset_type = g_ModuleInterface->CallBuiltin("asset_get_type", { *Arguments[0] });
							if (asset_type.ToInt64() == 1) // asset_sprite
							{
								RValue original_sprite_name = g_ModuleInterface->CallBuiltin("sprite_get_name", { *Arguments[0] });
								std::string replacement_sprite_name = original_sprite_name.ToString() + "_donatable";

								RValue sprite = g_ModuleInterface->CallBuiltin("asset_get_index", { replacement_sprite_name.c_str() });
								if (sprite.m_Kind == VALUE_REF)
									*Arguments[0] = sprite;
							}
						}
					}
				}
			}
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_NODE_OBJECT_SET_SPRITE));
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
	if (load_on_start)
	{
		g_ModuleInterface->GetGlobalInstance(&global_instance);
		LoadItemData();
		LoadDonatableItems();
		LoadObjectCategories();
		CreateOrLoadConfigFile();

		for (int item_id : donatable_items)
		{
			if (item_id_to_name_map.contains(item_id))
				donatable_item_names.insert(item_id_to_name_map[item_id]);
			else
				g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Missing item name for item id: %d", MOD_NAME, VERSION, item_id);
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

void CreateObjectCallback(AurieStatus& status)
{
	status = g_ModuleInterface->CreateCallback(
		g_ArSelfModule,
		EVENT_OBJECT_CALL,
		ObjectCallback,
		0
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook (EVENT_OBJECT_CALL)!", MOD_NAME, VERSION);
	}
}

void CreateHookGmlScriptGetUiIcon(AurieStatus& status)
{
	CScript* gml_script_get_ui_icon = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_GET_ITEM_UI_ICON,
		(PVOID*)&gml_script_get_ui_icon
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_ITEM_UI_ICON);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_GET_ITEM_UI_ICON,
		gml_script_get_ui_icon->m_Functions->m_ScriptFunction,
		GmlScriptGetUiIconCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_GET_ITEM_UI_ICON);
	}
}

void CreateHookGmlScriptNodeObjectSetSprite(AurieStatus& status)
{
	CScript* gml_script_node_object_set_sprite = nullptr;
	status = g_ModuleInterface->GetNamedRoutinePointer(
		GML_SCRIPT_NODE_OBJECT_SET_SPRITE,
		(PVOID*)&gml_script_node_object_set_sprite
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NODE_OBJECT_SET_SPRITE);
	}

	status = MmCreateHook(
		g_ArSelfModule,
		GML_SCRIPT_NODE_OBJECT_SET_SPRITE,
		gml_script_node_object_set_sprite->m_Functions->m_ScriptFunction,
		GmlScriptNodeObjectSetSpriteCallback,
		nullptr
	);

	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to hook script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_NODE_OBJECT_SET_SPRITE);
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
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Failed to get script (%s)!", MOD_NAME, VERSION, GML_SCRIPT_SETUP_MAIN_SCREEN);
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

EXPORTED AurieStatus ModuleInitialize(
	IN AurieModule* Module,
	IN const fs::path& ModulePath
)
{
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	status = ObGetInterface(
		"YYTK_Main",
		(AurieInterfaceBase*&)(g_ModuleInterface)
	);

	if (!AurieSuccess(status))
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;

	g_ModuleInterface->Print(CM_LIGHTAQUA, "[%s %s] - Plugin starting...", MOD_NAME, VERSION);

	CreateObjectCallback(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptGetUiIcon(status);
	if (!AurieSuccess(status))
	{
		g_ModuleInterface->Print(CM_LIGHTRED, "[%s %s] - Exiting due to failure on start!", MOD_NAME, VERSION);
		return status;
	}

	CreateHookGmlScriptNodeObjectSetSprite(status);
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

	g_ModuleInterface->Print(CM_LIGHTGREEN, "[%s %s] - Plugin started!", MOD_NAME, VERSION);
	return AURIE_SUCCESS;
}