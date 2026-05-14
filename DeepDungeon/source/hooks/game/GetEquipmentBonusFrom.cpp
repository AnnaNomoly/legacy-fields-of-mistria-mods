#include "../../utils/Utils.h"

using namespace State::Floor;
using namespace State::Maps;

// MMAPI Equipment::Hooks::AfterGetEquipmentBonus callback.
// Registered in Init.cpp via MMAPI::Equipment::Hooks::AfterGetEquipmentBonus(AfterGetEquipmentBonus).
void AfterGetEquipmentBonus(MMAPI::Equipment::EquipmentBonusContext& ctx)
{
	if (!AriCurrentGmRoomIsDungeonFloor() || GameIsPaused())
		return;

	const MMAPI::Infusion::Ids infusion_id = ctx.GetInfusion();

	// Leech — force leeching infusion to always contribute.
	if (active_offerings.contains(Offerings::LEECH) && infusion_id == MMAPI::Infusion::Ids::Leeching)
		ctx.SetBonusValue(1.0);

	// Dungeon's Curse — suppress infusion bonuses from restricted equipment slots.
	if (Config::config.restrict_armor
		&& (infusion_id == MMAPI::Infusion::Ids::Fortified
			|| infusion_id == MMAPI::Infusion::Ids::Hasty
			|| infusion_id == MMAPI::Infusion::Ids::Tireless))
	{
		ctx.SetBonusValue(0.0);
	}
	if (Config::config.restrict_tools && infusion_id == MMAPI::Infusion::Ids::Lightweight)
		ctx.SetBonusValue(0.0);

	// Class armor set bonuses — override bonus value based on number of class armor pieces with this infusion.
	const auto class_armor_infusions = GetClassArmorInfusions();
	if (class_armor_infusions.contains(static_cast<int>(infusion_id)))
	{
		const int count = class_armor_infusions.at(static_cast<int>(infusion_id));
		if (infusion_id == MMAPI::Infusion::Ids::Fortified)
			ctx.SetBonusValue(count * 4.0);
		else if (infusion_id == MMAPI::Infusion::Ids::Hasty)
			ctx.SetBonusValue(count * 0.04);
		else if (infusion_id == MMAPI::Infusion::Ids::Tireless)
			ctx.SetBonusValue(count * 4.0);
	}
}
