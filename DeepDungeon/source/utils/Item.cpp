#include "Utils.h"

void ModifyMistpoolWeaponSprites()
{
	// Sprite indexes for the "Scrap Metal Sword" which is used as the Mistpool Sword.
	RValue spr_weapon_sword_scrap_metal_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo1_s" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo2_s" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_e" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_n" });
	RValue spr_weapon_sword_scrap_metal_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_combo3_s" });
	RValue spr_weapon_sword_scrap_metal_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_scrap_metal_tool_down_attack_e" });
	RValue spr_ui_item_tool_scrap_metal_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_scrap_metal_sword" });

	if (floor_number < 20)
	{
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t0_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t0_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t0_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t0_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t0_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t0_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t0_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t0_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t0_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t0_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t0_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t0_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t0_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t0_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t0_sword });
	}
	else if (floor_number < 40)
	{
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t1_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t1_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t1_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t1_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t1_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t1_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t1_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t1_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t1_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t1_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t1_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t1_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t1_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t1_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t1_sword });
	}
	else if (floor_number < 60)
	{
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t2_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t2_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t2_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t2_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t2_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t2_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t2_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t2_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t2_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t2_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t2_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t2_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t2_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t2_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t2_sword });
	}
	else
	{
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo1_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo1_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo2_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo2_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_e" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_n" });
		RValue spr_weapon_sword_mistpool_t3_tool_combo3_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_combo3_s" });
		RValue spr_weapon_sword_mistpool_t3_tool_down_attack_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_weapon_sword_mistpool_t3_tool_down_attack_e" });
		RValue spr_ui_item_tool_mistpool_t3_sword = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_e, spr_weapon_sword_mistpool_t3_tool_combo1_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_n, spr_weapon_sword_mistpool_t3_tool_combo1_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo1_s, spr_weapon_sword_mistpool_t3_tool_combo1_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_e, spr_weapon_sword_mistpool_t3_tool_combo2_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_n, spr_weapon_sword_mistpool_t3_tool_combo2_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo2_s, spr_weapon_sword_mistpool_t3_tool_combo2_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_e, spr_weapon_sword_mistpool_t3_tool_combo3_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_n, spr_weapon_sword_mistpool_t3_tool_combo3_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_combo3_s, spr_weapon_sword_mistpool_t3_tool_combo3_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_weapon_sword_scrap_metal_tool_down_attack_e, spr_weapon_sword_mistpool_t3_tool_down_attack_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_scrap_metal_sword, spr_ui_item_tool_mistpool_t3_sword });
	}
}

void ModifyMistpoolPickaxeSprites()
{
	// Sprite indexes for the "Worn Pickaxe" which is used as the Mistpool Pickaxe.
	RValue spr_tool_pick_axe_worn_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_e" });
	RValue spr_tool_pick_axe_worn_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_n" });
	RValue spr_tool_pick_axe_worn_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_worn_tool_axe_s" });
	RValue spr_ui_item_tool_rusty_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_rusty_pickaxe" });

	if (floor_number < 20)
	{
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t0_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t0_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t0_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t0_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t0_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t0_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t0_pickaxe });
	}
	else if (floor_number < 40)
	{
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t1_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t1_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t1_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t1_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t1_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t1_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t1_pickaxe });
	}
	else if (floor_number < 60)
	{
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t2_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t2_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t2_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t2_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t2_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t2_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t2_pickaxe });
	}
	else
	{
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_e = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_e" });
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_n = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_n" });
		RValue spr_tool_pick_axe_mistpool_t3_tool_axe_s = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_tool_pick_axe_mistpool_t3_tool_axe_s" });
		RValue spr_ui_item_tool_mistpool_t3_pickaxe = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_pickaxe" });

		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_e, spr_tool_pick_axe_mistpool_t3_tool_axe_e });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_n, spr_tool_pick_axe_mistpool_t3_tool_axe_n });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_tool_pick_axe_worn_tool_axe_s, spr_tool_pick_axe_mistpool_t3_tool_axe_s });
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_item_tool_rusty_pickaxe, spr_ui_item_tool_mistpool_t3_pickaxe });
	}
}

void ModifyBarkSprites()
{
	// TODO: Update as more custom bark sprites are implemented
	RValue spr_ui_bark_icon_no_coin = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_bark_icon_no_coin" });
	RValue spr_ui_bark_icon_no_coin_copy = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_bark_icon_no_coin_copy" });
	RValue spr_ui_bark_icon_inhibiting_trap = g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_bark_icon_inhibiting_trap" });

	if (floor_number != 0)
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_bark_icon_no_coin, spr_ui_bark_icon_inhibiting_trap });
	else
		g_ModuleInterface->CallBuiltin("sprite_assign", { spr_ui_bark_icon_no_coin, spr_ui_bark_icon_no_coin_copy });
}

void ScaleMistpoolWeapon(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");

	RValue* sword_scrap_metal;
	g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map["sword_scrap_metal"], sword_scrap_metal);

	if (in_dungeon)
	{
		int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
		int damage = (modified_floor_number / 4) + 3;
		*sword_scrap_metal->GetRefMember("damage") = damage;
	}
	else
		*sword_scrap_metal->GetRefMember("damage") = 1;
}

void ScaleMistpoolArmor(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");
	for (std::string mistpool_armor_name : MISTPOOL_ARMOR_NAMES)
	{
		RValue* mistpool_armor_piece;
		g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[mistpool_armor_name], mistpool_armor_piece);

		if (in_dungeon)
		{
			int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
			int defense = modified_floor_number / 20;
			*mistpool_armor_piece->GetRefMember("defense") = defense;
		}
		else
			*mistpool_armor_piece->GetRefMember("defense") = 0;
	}
}

void ScaleMistpoolPickaxe(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");

	RValue* pick_axe_worn;
	g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[MISTPOOL_PICK_AXE_NAME], pick_axe_worn);

	if (in_dungeon)
	{
		if (floor_number < 20)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 2 : 1;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 1 : 0;
		}
		else if (floor_number < 40)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 3 : 2;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 2 : 1;
		}
		else if (floor_number < 60)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 5 : 3;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 3 : 2;
		}

		else if (floor_number < 80)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 6 : 4;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 4 : 3;
		}

		else if (floor_number < 100)
		{
			*pick_axe_worn->GetRefMember("damage") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 8 : 5;
			*pick_axe_worn->GetRefMember("quality") = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? 5 : 4;
		}
	}
	else
	{
		*pick_axe_worn->GetRefMember("damage") = 1;
		*pick_axe_worn->GetRefMember("quality") = 0;
	}
}

void ScaleClassArmor(bool in_dungeon)
{
	RValue item_data = *global_instance->GetRefMember("__item_data");
	for (std::string class_armor_name : CLASS_ARMOR_NAMES)
	{
		RValue* class_armor_piece;
		g_ModuleInterface->GetArrayEntry(item_data, item_name_to_id_map[class_armor_name], class_armor_piece);

		if (in_dungeon)
		{
			int modified_floor_number = active_greater_sigils.contains(GreaterSigils::MEIKYO_SHISUI) ? floor_number + 40 : floor_number;
			int defense = modified_floor_number / 20;
			*class_armor_piece->GetRefMember("defense") = defense;
		}
		else
			*class_armor_piece->GetRefMember("defense") = 0;
	}
}

int GetRandomSoulStone()
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> random_soul_stone_distribution(0, SOUL_STONE_NAMES.size() - 1);
	return item_name_to_id_map[SOUL_STONE_NAMES[random_soul_stone_distribution(random_generator)]];
}

RValue GetDynamicItemSprite(int item_id)
{
	if (item_id == item_name_to_id_map[MISTPOOL_SWORD_NAME])
	{
		if (active_traps.contains(Traps::INHIBITING) || !AriCurrentGmRoomIsDungeonFloor())
		{
			if (floor_number < 20)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword_disabled" });
			else if (floor_number < 40)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword_disabled" });
			else if (floor_number < 60)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword_disabled" });
		}
		else
		{
			if (floor_number < 20)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_sword" });
			else if (floor_number < 40)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_sword" });
			else if (floor_number < 60)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_sword" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_sword" });
		}
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_PICK_AXE_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t0_pickaxe" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t1_pickaxe" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t2_pickaxe" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_tool_mistpool_t3_pickaxe" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_HELMET_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_helmet_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_CHESTPIECE_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_chestpiece_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_GLOVES_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_gloves_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_PANTS_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_pants_tier_4" });
	}
	else if (item_id == item_name_to_id_map[MISTPOOL_BOOTS_NAME])
	{
		if (floor_number < 20)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_1" });
		else if (floor_number < 40)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_2" });
		else if (floor_number < 60)
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_3" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_equipment_mistpool_boots_tier_4" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::ALTERATION])
	{
		if (active_sigils.contains(Sigils::ALTERATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_alteration_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_alteration" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::CONCEALMENT])
	{
		if (active_sigils.contains(Sigils::CONCEALMENT) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_concealment_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_concealment" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::FORTIFICATION])
	{
		if (active_sigils.contains(Sigils::FORTIFICATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortification_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortification" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::FORTUNE])
	{
		if (active_sigils.contains(Sigils::FORTUNE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortune_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_fortune" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::PROTECTION])
	{
		if (active_sigils.contains(Sigils::PROTECTION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || GetInvulnerabilityHits() > 0 || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_protection_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_protection" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::RAGE])
	{
		if (active_sigils.contains(Sigils::RAGE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_rage_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_rage" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::REDEMPTION])
	{
		if (active_sigils.contains(Sigils::REDEMPTION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || FairyBuffIsActive() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_redemption_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_redemption" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SAFETY])
	{
		if (active_sigils.contains(Sigils::SAFETY) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_safety_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_safety" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SERENITY])
	{
		if (active_sigils.contains(Sigils::SERENITY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_serenity_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_serenity" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SILENCE])
	{
		if (active_sigils.contains(Sigils::SILENCE) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_silence_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_silence" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::STRENGTH])
	{
		if (active_sigils.contains(Sigils::STRENGTH) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_strength_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_strength" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::TEMPTATION])
	{
		if (active_sigils.contains(Sigils::TEMPTATION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0 || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_temptation_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_temptation" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::SIGHT])
	{
		if (active_sigils.contains(Sigils::SIGHT) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_sight_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_sight" });
	}
	else if (item_id == sigil_to_item_id_map[Sigils::INTUITION])
	{
		if (active_sigils.contains(Sigils::INTUITION) || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_intuition_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_sigil_of_intuition" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::BENEDICTION])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_benediction_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_benediction" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::ASTRAL_FLOW])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_astral_flow_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_astral_flow" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::CHAIN_SPELL])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_chain_spell_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_chain_spell" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::SPIRIT_SURGE])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_spirit_surge_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_spirit_surge" });
	}
	else if (item_id == greater_sigil_to_item_id_map[GreaterSigils::MEIKYO_SHISUI])
	{
		if (!active_greater_sigils.empty() || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_meikyo_shisui_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_greater_sigil_of_meikyo_shisui" });
	}
	else if (item_id == salve_name_to_id_map[SUSTAINING_POTION_NAME])
	{
		if (active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_potion_sustain_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_potion_sustain" });
	}
	else if (item_id == salve_name_to_id_map[HEALTH_SALVE_NAME])
	{
		if (salves_used[HEALTH_SALVE_NAME] >= Config::config.health_salve_limit || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_health_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_health" });
	}
	else if (item_id == salve_name_to_id_map[STAMINA_SALVE_NAME])
	{
		if (salves_used[STAMINA_SALVE_NAME] >= Config::config.stamina_salve_limit || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_stamina_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_stamina" });
	}
	else if (item_id == salve_name_to_id_map[MANA_SALVE_NAME])
	{
		if (salves_used[MANA_SALVE_NAME] >= Config::config.mana_salve_limit || active_floor_enchantments.contains(FloorEnchantments::ITEM_PENALTY) || !AriCurrentGmRoomIsDungeonFloor())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_mana_disabled" });
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_item_salve_mana" });
	}
}

RValue GetDynamicUiSprite(std::string sprite_name)
{
	auto armor_set_bonuses = GetArmorSetBonuses();

	// OnDungeonRoomStart Conversation Sprite Overrides
	if (!journal_menu_open) // floor_start_time == current_time_in_seconds
	{
		// Priestess Portrait Replacement
		if (sprite_name.contains("spr_portrait_seridia") && !sprite_name.contains("flashback"))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_portrait_seridia_flashback_priestess_sad" });
		// Heart Insert Icon Conversation Replacement
		else if (sprite_name == "spr_ui_dialogue_namebar_heartinsert")
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_dialogue_namebar_circleinsert" });
		// Heart & Circle Icon Conversation Replacement
		else if (sprite_name.contains("spr_ui_dialogue_heart_") || sprite_name.contains("spr_ui_dialogue_circle_"))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_dialogue_circle_purple" });
	}

	// Full Restore (Spell Icon)
	if (sprite_name == "spr_ui_journal_magic_restore_spell_icon_main")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (armor_set_bonuses.dark_knight.DarkSeal())
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::DARK_KNIGHT][ManagedSetBonuses::DARK_SEAL] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_spell_icon_main" });
		}
		// Elemental Seal (Mage Set Bonus)
		else if (armor_set_bonuses.mage.ElementalSeal() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENFIRE] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_spell_icon_main" });
			}
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENBLIZZARD] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_spell_icon_main" });
			}
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
			{
				if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ENPOISON] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_spell_icon_disabled" });
				else
					return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_spell_icon_main" });
			}
		}
		// Predict (Oracle Set Bonus)
		else if (armor_set_bonuses.oracle.FullSet())
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::PREDICT] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_spell_icon_main" });
		}
		// Full Restore Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_restore_spell_icon_disabled" });
	}
	// Summon Rain (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_rain_spell_icon_main")
	{
		// Flood (Mage Set Bonus)
		if (armor_set_bonuses.mage.Flood())
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::FLOOD] >= 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_spell_icon_main" });
		}
		// Summon Rain Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_rain_spell_icon_disabled" });
	}
	// Growth (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_growth_spell_icon_main")
	{
		// Quake (Mage Set Bonus)
		if (armor_set_bonuses.mage.Quake())
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::QUAKE] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_spell_icon_main" });
		}
		// Condemn (Oracle Set Bonus)
		else if (armor_set_bonuses.oracle.FullSet())
		{
			if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || class_name_to_set_bonus_effect_value_map[Classes::ORACLE][ManagedSetBonuses::CONDEMN] > 0 || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_spell_icon_disabled" });
			else
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_spell_icon_main" });
		}
		// Growth Disabled
		else if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_growth_spell_icon_disabled" });
	}
	// Fire Breath (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_fire_spell_icon_main")
	{
		// Fire Breath Disabled
		if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_fire_spell_icon_disabled" });
	}
	// Sacred Light (Spell Icon)
	else if (sprite_name == "spr_ui_journal_magic_sacred_light_spell_icon_main")
	{
		// Sacred Light Disabled
		if (active_floor_enchantments.contains(FloorEnchantments::AMNESIA) || (Config::config.enable_boss_fight_restrictions && boss_battle != BossBattle::NONE))
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_sacred_light_spell_icon_disabled" });
	}
	// Dungeon Backplate
	else if (sprite_name == "spr_ui_dungeon_backplate")
	{
		// Floor Enchantments & Offerings
		if (!active_floor_enchantments.empty() || !active_offerings.empty())
		{
			std::string sprite_name = "backplate";
			std::string group_one_enchantment_str = "";
			std::string group_two_enchantment_str = "";
			std::string group_three_enchantment_str = "";
			std::string offering_str = "";

			for (FloorEnchantments floor_enchantment : active_floor_enchantments)
			{
				auto group_one_enchantment = std::find(GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.begin(), GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_one_enchantment != GROUP_ONE_PREDICT_FLOOR_ENCHANTMENTS.end())
					group_one_enchantment_str += magic_enum::enum_name(floor_enchantment);

				auto group_two_enchantment = std::find(GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.begin(), GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_two_enchantment != GROUP_TWO_PREDICT_FLOOR_ENCHANTMENTS.end())
					group_two_enchantment_str += magic_enum::enum_name(floor_enchantment);

				auto group_three_enchantment = std::find(GROUP_THREE_FLOOR_ENCHANTMENTS.begin(), GROUP_THREE_FLOOR_ENCHANTMENTS.end(), floor_enchantment);
				if (group_three_enchantment != GROUP_THREE_FLOOR_ENCHANTMENTS.end())
					group_three_enchantment_str += magic_enum::enum_name(floor_enchantment);
			}

			for (Offerings offering : active_offerings)
				offering_str += magic_enum::enum_name(offering);

			if (!group_one_enchantment_str.empty())
				sprite_name += "_" + group_one_enchantment_str;
			if (!group_two_enchantment_str.empty())
				sprite_name += "_" + group_two_enchantment_str;
			if (!group_three_enchantment_str.empty())
				sprite_name += "_" + group_three_enchantment_str;
			if (!offering_str.empty())
				sprite_name += "_" + offering_str;

			std::transform(sprite_name.begin(), sprite_name.end(), sprite_name.begin(), [](unsigned char c) { return std::tolower(c); });
			return g_ModuleInterface->CallBuiltin("asset_get_index", { RValue(sprite_name) });
		}
		// Empty
		else
			return g_ModuleInterface->CallBuiltin("asset_get_index", { RValue("backplate_empty") });
	}
	// Full Restore (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_restore_card_icon")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (armor_set_bonuses.dark_knight.DarkSeal())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_siphon_life_card_icon" });
		// Elemental Seal (Mage Set Bonus)
		else if (armor_set_bonuses.mage.ElementalSeal() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enfire_card_icon" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enblizzard_card_icon" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_enpoison_card_icon" });
		}
		// Predict (Oracle Set Bonus)
		else if (armor_set_bonuses.oracle.FullSet())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_predict_card_icon" });
	}
	// Summon Rain (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_rain_card_icon")
	{
		// Flood (Mage Set Bonus)
		if (armor_set_bonuses.mage.Flood())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_flood_card_icon" });
	}
	// Growth (Card Icon)
	else if (sprite_name == "spr_ui_journal_magic_growth_card_icon")
	{
		// Quake (Mage Set Bonus)
		if (armor_set_bonuses.mage.Quake())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_quake_card_icon" });
		// Condemn (Oracle Set Bonus)
		else if (armor_set_bonuses.oracle.FullSet())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_condemn_card_icon" });
	}
	// Full Restore (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_restore")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (armor_set_bonuses.dark_knight.DarkSeal())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_siphon_life" });
		// Elemental Seal (Mage Set Bonus)
		else if (armor_set_bonuses.mage.ElementalSeal() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enfire" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enblizzard" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_enpoison" });
		}
		// Predict (Oracle Set Bonus)
		else if (armor_set_bonuses.oracle.FullSet())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_predict" });
	}
	// Summon Rain (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_rain")
	{
		// Flood (Mage Set Bonus)
		if (armor_set_bonuses.mage.Flood())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_flood" });
	}
	// Growth (Card Ribbon)
	else if (sprite_name == "spr_ui_journal_magic_card_ribbon_growth")
	{
		// Quake (Mage Set Bonus)
		if (armor_set_bonuses.mage.Quake())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_quake" });
		// Condemn (Oracle Set Bonus)
		else if (armor_set_bonuses.oracle.FullSet())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_magic_card_ribbon_condemn" });
	}
	// Spell Card Backplate
	else if (sprite_name == "spr_ui_journal_magic_card_backplate")
	{
		// Dark Seal (Dark Knight Set Bonus)
		if (armor_set_bonuses.dark_knight.DarkSeal())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_sipon_life_card_backplate" });
		// Flood (Mage Set Bonus)
		else if (armor_set_bonuses.mage.Flood())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_flood_card_backplate" });
		// Elemental Seal (Mage Set Bonus)
		else if (armor_set_bonuses.mage.ElementalSeal() && class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL] > 0)
		{
			ElementalSealEffects elemental_seal_effect = *magic_enum::enum_cast<ElementalSealEffects>(class_name_to_set_bonus_effect_value_map[Classes::MAGE][ManagedSetBonuses::ELEMENTAL_SEAL]);
			if (elemental_seal_effect == ElementalSealEffects::FIRE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enfire_card_backplate" });
			else if (elemental_seal_effect == ElementalSealEffects::ICE)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enblizzard_card_backplate" });
			else if (elemental_seal_effect == ElementalSealEffects::VENOM)
				return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_enpoison_card_backplate" });
		}
		// Quake (Mage Set Bonus)
		else if (armor_set_bonuses.mage.Quake())
			return g_ModuleInterface->CallBuiltin("asset_get_index", { "spr_ui_journal_quake_card_backplate" });
	}
	return RValue();
}
