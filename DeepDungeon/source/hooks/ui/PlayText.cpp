#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Maps;

static void StartChallengeMode(int starting_floor)
{
	is_challenge_mode = true;
	challenge_mode_progress.run_in_progress = true;
	WriteChallengeModeFile();
	Config::OverrideWithDefaultsForChallengeMode();
	RefreshPrototypes();
	RemoveItemsFromInventoryForChallengeMode();
	floor_number = starting_floor;
	MMAPI::Dungeon::EnterDungeon(starting_floor);
}

// MMAPI Text::Hooks::BeforePlayText callback.
void BeforePlayText(MMAPI::Text::PlayTextContext& ctx)
{
	if (!game_is_active)
		return;

	std::string localization_key{ ctx.GetKey() };

	if (localization_key.contains("Conversations/Mods/Deep Dungeon/offering"))
	{
		bool is_offering = false;
		if (localization_key == "Conversations/Mods/Deep Dungeon/offering/2/health")
		{
			is_offering = true;
			ari_resource_to_penalty_map[AriResources::HEALTH] = true;
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/offering/2/stamina")
		{
			is_offering = true;
			ari_resource_to_penalty_map[AriResources::STAMINA] = true;
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/offering/2/mana")
		{
			is_offering = true;
			ari_resource_to_penalty_map[AriResources::MANA] = true;
		}

		if (is_offering)
		{
			std::vector<Offerings> possible_offerings = {};
			if (!Config::config.experimental_extra_floor_enchantments_and_offerings)
				possible_offerings = { Offerings::DREAD, Offerings::INNER_FIRE, Offerings::LEECH, Offerings::PERIL, Offerings::RECKONING };
			else
				possible_offerings = { Offerings::DREAD, Offerings::INNER_FIRE, Offerings::LEECH, Offerings::PERIL, Offerings::RECKONING, Offerings::OUTBREAK, Offerings::SPIRIT_LINK, Offerings::SPIKES, Offerings::REFLECT };

			static thread_local pcg32 random_generator([] {
				std::random_device rd;
				return pcg32(
					(static_cast<uint64_t>(rd()) << 32) | rd(),
					(static_cast<uint64_t>(rd()) << 32) | rd()
				);
			}());
			std::uniform_int_distribution<size_t> random_offering_distribution(0, possible_offerings.size() - 1);
			Offerings offering = magic_enum::enum_value<Offerings>(random_offering_distribution(random_generator));
			queued_offerings.insert(offering);
		}
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/placeholders/sigil_parchment_exchange/cursed_armor/result")
	{
		int total_cursed_armor_inventory_count = 0;
		std::map<std::string, int> cursed_armor_name_to_inventory_count = {};
		std::unordered_set<std::string> cursed_armor = { CURSED_HELMET_NAME, CURSED_CHESTPIECE_NAME, CURSED_PANTS_NAME, CURSED_BOOTS_NAME, CURSED_GLOVES_NAME };

		for (std::string cursed_armor_name : cursed_armor)
		{
			int count = MMAPI::Inventory::CountItem(item_name_to_id_map[cursed_armor_name]).ToInt64();
			total_cursed_armor_inventory_count += count;
			cursed_armor_name_to_inventory_count[cursed_armor_name] = count;
		}

		if (total_cursed_armor_inventory_count == 0)
			ctx.SetKey("Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/insufficient");
		else
		{
			ctx.SetKey("Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/cursed_armor");

			for (size_t i = 0; i < total_cursed_armor_inventory_count * 3; i++)
				MMAPI::Item::Drop(item_name_to_id_map["sigil_parchment"], ari_x, ari_y);

			for (auto& entry : cursed_armor_name_to_inventory_count)
				if (entry.second > 0)
					MMAPI::Inventory::RemoveItem(item_name_to_id_map[entry.first], entry.second);
		}
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/placeholders/sigil_parchment_exchange/soul_stone/result")
	{
		int total_soul_stone_inventory_count = 0;
		std::map<std::string, int> soul_stone_name_to_inventory_count = {};

		for (std::string soul_stone_name : SOUL_STONE_NAMES)
		{
			int count = MMAPI::Inventory::CountItem(item_name_to_id_map[soul_stone_name]).ToInt64();
			total_soul_stone_inventory_count += count;
			soul_stone_name_to_inventory_count[soul_stone_name] = count;
		}

		if (total_soul_stone_inventory_count == 0)
			ctx.SetKey("Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/insufficient");
		else
		{
			ctx.SetKey("Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/soul_stone");

			for (size_t i = 0; i < total_soul_stone_inventory_count * 3; i++)
				MMAPI::Item::Drop(item_name_to_id_map["sigil_parchment"], ari_x, ari_y);

			for (auto& entry : soul_stone_name_to_inventory_count)
				if (entry.second > 0)
					MMAPI::Inventory::RemoveItem(item_name_to_id_map[entry.first], entry.second);
		}
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines/1" || localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods/1")
	{
		MMAPI::Text::CloseTextbox();
		MMAPI::Location::TeleportAri(MMAPI::Location::Ids::MinesEntry, 216, 198);
		ctx.Cancel();
		return;
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods/2")
	{
		MMAPI::Text::CloseTextbox();
		MMAPI::Location::TeleportAri(MMAPI::Location::Ids::CaldarusHouse, 312, 328);
		ctx.Cancel();
		return;
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines/2" || localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods/3")
	{
		MMAPI::Text::CloseTextbox();
		ctx.Cancel();
		return;
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/challenge_mode/start")
	{
		MMAPI::Text::CloseTextbox();

		if (challenge_mode_progress.highest_floor_reached == 0)
			StartChallengeMode(0);
		else if (challenge_mode_progress.highest_floor_reached == 20)
			StartChallengeMode(20);
		else if (challenge_mode_progress.highest_floor_reached == 40)
			StartChallengeMode(40);
		else if (challenge_mode_progress.highest_floor_reached == 60)
			StartChallengeMode(60);
		else if (challenge_mode_progress.highest_floor_reached == 80)
			StartChallengeMode(80);
		else
			MMAPI::Game::CreateNotification(false, CHALLENGE_MODE_ALREADY_COMPLETED_NOTIFICATION_KEY);

		ctx.Cancel();
		return;
	}
	else if (localization_key == "Conversations/Mods/Deep Dungeon/challenge_mode/cancel")
	{
		MMAPI::Text::CloseTextbox();
		ctx.Cancel();
		return;
	}
}
