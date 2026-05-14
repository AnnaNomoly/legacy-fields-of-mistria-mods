#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Maps;

// MMAPI Game::Hooks::AfterGameActive callback — fires once per session on the first
// get_weather call after a save load. Cleared on return-to-title.
// (DD's pre-migration GetWeather hook used `!game_is_active` and `unlock_recipes` as its own
// once-per-session triggers; both have the same lifecycle as AfterGameActive, so they collapse
// into this single callback. The DD-side flags are still set/cleared for the rest of the mod
// that reads them.)
void AfterGameActive()
{
	game_is_active = true;
	//MarkDungeonTutorialUnseen(); // TODO: Only do this once per save file.

	if (unlock_recipes)
	{
		// TODO: Unlock more recipes as added
		unlock_recipes = false;
		for (std::string armor_name : CLASS_ARMOR_NAMES)
			UnlockRecipe(item_name_to_id_map[armor_name]);
		for (std::string orb_name : ORB_NAMES)
			UnlockRecipe(item_name_to_id_map[orb_name]);
		for (auto& entry : sigil_to_item_id_map)
			UnlockRecipe(entry.second);
	}
}
