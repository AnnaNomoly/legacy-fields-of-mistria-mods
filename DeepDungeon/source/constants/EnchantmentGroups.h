#pragma once
// Requires Globals.h (FloorEnchantments enum) to be included before this header.
#include <vector>

static const std::vector<FloorEnchantments> GROUP_ONE_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::HP_PENALTY,
	FloorEnchantments::EXHAUSTION,
	FloorEnchantments::GRAVITY,
	FloorEnchantments::RESTORATION,
	FloorEnchantments::SECOND_WIND,
	FloorEnchantments::HASTE,
};

static const std::vector<FloorEnchantments> GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::HP_PENALTY,
	FloorEnchantments::EXHAUSTION,
	FloorEnchantments::GRAVITY,
	FloorEnchantments::RESTORATION,
	FloorEnchantments::SECOND_WIND,
	FloorEnchantments::HASTE,
	FloorEnchantments::FUMIGATE,
	FloorEnchantments::DEEP_WOUNDS,
	FloorEnchantments::BLINK,
	FloorEnchantments::STONESKIN,
	FloorEnchantments::PHALANX
};

static const std::vector<FloorEnchantments> GROUP_TWO_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::AMNESIA,
	FloorEnchantments::ITEM_PENALTY,
	FloorEnchantments::DISTORTION,
	FloorEnchantments::DAMAGE_DOWN,
};

static const std::vector<FloorEnchantments> GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::AMNESIA,
	FloorEnchantments::ITEM_PENALTY,
	FloorEnchantments::DISTORTION,
	FloorEnchantments::DAMAGE_DOWN,
	FloorEnchantments::FRAILTY,
	FloorEnchantments::GRUDGE
};

static const std::vector<FloorEnchantments> GROUP_THREE_FLOOR_ENCHANTMENTS = {
	FloorEnchantments::GLOOM,
	FloorEnchantments::FEY
};
