#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

RValue& GmlScriptGetWeatherCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (!game_is_active)
	{
		game_is_active = true;
		//MarkDungeonTutorialUnseen(); // TODO: Only do this once per save file.
	}

	if (unlock_recipes)
	{
		// TODO: Unlock more recipes as added
		unlock_recipes = false;
		for (std::string armor_name : CLASS_ARMOR_NAMES)
			UnlockRecipe(item_name_to_id_map[armor_name], Self, Other);
		for (std::string orb_name : ORB_NAMES)
			UnlockRecipe(item_name_to_id_map[orb_name], Self, Other);
		for (auto& entry : sigil_to_item_id_map)
			UnlockRecipe(entry.second, Self, Other);
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_GET_WEATHER));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
