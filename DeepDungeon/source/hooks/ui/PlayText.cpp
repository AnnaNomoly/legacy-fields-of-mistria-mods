#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Maps;

RValue& GmlScriptPlayTextCallback(
	IN CInstance* Self,
	IN CInstance* Other,
	OUT RValue& Result,
	IN int ArgumentCount,
	IN RValue** Arguments
)
{
	if (game_is_active)
	{
		std::string localization_key = Arguments[0]->ToString();

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

				// Pick a random offering effect
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
				int count = InventoryCountItem(item_name_to_id_map[cursed_armor_name], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
				total_cursed_armor_inventory_count += count;
				cursed_armor_name_to_inventory_count[cursed_armor_name] = count;
			}

			if (total_cursed_armor_inventory_count == 0)
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/insufficient";
			else
			{
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/cursed_armor";

				for (size_t i = 0; i < total_cursed_armor_inventory_count * 3; i++)
					DropItem(item_name_to_id_map["sigil_parchment"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);

				for (auto& entry : cursed_armor_name_to_inventory_count)
					if (entry.second > 0)
						InventoryRemoveItem(item_name_to_id_map[entry.first], entry.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]);
			}
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/placeholders/sigil_parchment_exchange/soul_stone/result")
		{
			int total_soul_stone_inventory_count = 0;
			std::map<std::string, int> soul_stone_name_to_inventory_count = {};

			for (std::string soul_stone_name : SOUL_STONE_NAMES)
			{
				int count = InventoryCountItem(item_name_to_id_map[soul_stone_name], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]).ToInt64();
				total_soul_stone_inventory_count += count;
				soul_stone_name_to_inventory_count[soul_stone_name] = count;
			}

			if (total_soul_stone_inventory_count == 0)
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/insufficient";
			else
			{
				*Arguments[0] = "Conversations/Mods/Deep Dungeon/sigil_parchment_exchange/soul_stone";

				for (size_t i = 0; i < total_soul_stone_inventory_count * 3; i++)
					DropItem(item_name_to_id_map["sigil_parchment"], ari_x, ari_y, script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][0], script_name_to_reference_map[GML_SCRIPT_DROP_ITEM][1]);

				for (auto& entry : soul_stone_name_to_inventory_count)
					if (entry.second > 0)
						InventoryRemoveItem(item_name_to_id_map[entry.first], entry.second, script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][0], script_name_to_reference_map[GML_SCRIPT_DESERIALIZE_INVENTORY][1]);
			}
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines/1" || localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods/1")
		{
			CloseTextbox(Self, Other);
			TeleportAriToRoom(
				script_name_to_reference_map["obj_ari"][0],
				script_name_to_reference_map["obj_ari"][1],
				location_name_to_id_map["mines_entry"],
				216,
				198
			);
			return Result;
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods/2")
		{
			CloseTextbox(Self, Other);
			TeleportAriToRoom(
				script_name_to_reference_map["obj_ari"][0],
				script_name_to_reference_map["obj_ari"][1],
				location_name_to_id_map["caldarus_house"],
				312,
				328
			);
			return Result;
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines/2" || localization_key == "Conversations/Mods/Deep Dungeon/teleport_to_mines_or_deep_woods/3")
		{
			CloseTextbox(Self, Other);
			return Result;
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/challenge_mode/1")
		{
			CloseTextbox(Self, Other);

			time_stopped = true;
			is_challenge_mode = true;
			Config::OverrideWithDefaultsForChallengeMode();
			RemoveItemsFromInventoryForChallengeMode();

			if (challenge_mode_progress.highest_floor_reached == 0)
			{
				floor_number = 0;
				EnterDungeon(0, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
			}
			else if (challenge_mode_progress.highest_floor_reached == 20)
			{
				floor_number = 20;
				EnterDungeon(20, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
			}
			else if (challenge_mode_progress.highest_floor_reached == 40)
			{
				floor_number = 40;
				EnterDungeon(40, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
			}
			else if (challenge_mode_progress.highest_floor_reached == 60)
			{
				floor_number = 60;
				EnterDungeon(60, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
			}
			else if (challenge_mode_progress.highest_floor_reached == 80)
			{
				floor_number = 80;
				EnterDungeon(80, script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][0], script_name_to_reference_map[GML_SCRIPT_STATUS_EFFECT_MANAGER_UPDATE][1]);
			}
			else
			{
				// CreateNotification(...)
			}

			return Result;
		}
		else if (localization_key == "Conversations/Mods/Deep Dungeon/challenge_mode/2")
		{
			CloseTextbox(Self, Other);
			return Result;
		}
	}

	const PFUNC_YYGMLScript original = reinterpret_cast<PFUNC_YYGMLScript>(MmGetHookTrampoline(g_ArSelfModule, GML_SCRIPT_PLAY_TEXT));
	original(Self, Other, Result, ArgumentCount, Arguments);

	return Result;
}
