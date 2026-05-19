#pragma once
#include "../utils/Utils.h"

std::vector<std::vector<double>> generate_circle(const std::vector<int>& Ns);
std::vector<std::vector<double>> generate_cross(int threshold, int lower_bound, int upper_bound, int increment, bool rotate);
std::vector<std::vector<double>> generate_checkerboard();
std::vector<std::vector<double>> generate_inverted_checkerboard();

void LoadStalagmiteAttackData();

void ModifyRockClodAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster);
void ModifyStalagmiteAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster);
void ModifySaplingAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id);
void ModifyShroomAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster);
void ModifyEnchanternAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster);
void ModifySpiritAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id);
void ModifyCatAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id);
void ModifyBatAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster);
void ModifyTomeAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster);
void ModifyRockStackAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster);
void ModifyGriffinStatueAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster);
void ModifyDreadBeastAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster);
