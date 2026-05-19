#include "MonsterPatterns.h"

using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

std::vector<std::vector<double>> generate_circle(const std::vector<int>& Ns)
{
	static constexpr double PI = 3.14159265358979323846;
	std::vector<std::vector<double>> points = {};

	for (int N : Ns)
	{
		auto ts = linspace(0.0, 1.0, static_cast<std::size_t>(N));

		for (double t : ts)
		{
			std::complex<double> z = static_cast<double>(N - 1) * std::exp(std::complex<double>(0.0, 2.0 * PI * t));
			std::complex<double> c = round_complex(z, 2);
			points.push_back({ c.real(), c.imag() });
		}
	}

	return points;
}

std::vector<std::vector<double>> generate_cross(int threshold, int lower_bound, int upper_bound, int increment, bool rotate)
{
	static const std::complex<double> SQRT_I = std::sqrt(std::complex<double>(0.0, 1.0));

	std::vector<std::vector<double>> points;
	for (int y = lower_bound; y <= upper_bound; y += increment)
	{
		for (int x = lower_bound; x <= upper_bound; x += increment)
		{
			if (std::min(std::abs(x), std::abs(y)) < threshold && (std::abs(x) + std::abs(y)) != 0)
			{
				std::complex<double> z(x, y);
				std::complex<double> c;

				if (!rotate)
					c = round_complex(z, 2); // Cross
				else
					c = round_complex(z * SQRT_I, 2); // X

				points.push_back({ c.real(), c.imag() });
			}
		}
	}

	return points;
}

std::vector<std::vector<double>> generate_checkerboard()
{
	std::vector<std::vector<double>> positions;

	for (int y = -24; y <= 24; ++y)
	{
		for (int x = -24; x <= 24; ++x)
		{
			int xm = euclidean_modulo(x, 10);
			int ym = euclidean_modulo(y, 10);

			if ((xm < 5 && ym < 5) || (xm >= 5 && ym >= 5))
				positions.push_back({ 8.0 * (x + 0.5), 8.0 * (y + 0.5) });
		}
	}

	return positions;
}

std::vector<std::vector<double>> generate_inverted_checkerboard()
{
	std::vector<std::vector<double>> positions;

	for (int y = -24; y <= 24; ++y)
	{
		for (int x = -24; x <= 24; ++x)
		{
			int xm = euclidean_modulo(x, 10);
			int ym = euclidean_modulo(y, 10);

			if (!((xm < 5 && ym < 5) || (xm >= 5 && ym >= 5)))
				positions.push_back({ 8.0 * (x + 0.5), 8.0 * (y + 0.5) });
		}
	}

	return positions;
}

void LoadStalagmiteAttackData()
{
	const std::vector<int> smallNs = { 9, 17, 25, 33, 41, 49 };
	const std::vector<int> bigNs = { 57, 65, 73, 81, 89, 97 };

	pb_aoe_points = generate_circle(smallNs);
	donut_aoe_points = generate_circle(bigNs);
	cross_aoe_points = generate_cross(18, -96, 96, 8, false);
	x_aoe_points = generate_cross(18, -96, 96, 8, true);
	checkerboard_room_wide_a_points = generate_checkerboard();
	checkerboard_room_wide_b_points = generate_inverted_checkerboard();
}

void ModifyRockClodAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster)
{
	const enum class Patterns {
		WALL, // Shoots a wall of 10 pellets repeatedly 5 times
		SPIN, // Rotates 18-degrees at a time while shooting 5 pellets in a line at various angles
		SPLIT // Shoots a single pellet that then splits into many that repeatedly split
	};

	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> random_pattern_distribution(0, magic_enum::enum_count<Patterns>() - 1);

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			Patterns pattern = magic_enum::enum_value<Patterns>(random_pattern_distribution(random_generator));
			if (is_boss_battle)
			{
				pattern = Patterns::WALL;
				boss_monsters_configured++;
			}

			if (pattern == Patterns::WALL)
			{
				// Shoots a wall of 10 pellets repeatedly 5 times
				if(!is_outbreak)
					StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

				StructVariableSet(config_clone, "launcher", false);
				StructVariableSet(config_clone, "attack_sequence", 5.0);
				StructVariableSet(config_clone, "attack_legion", 10.0);
				StructVariableSet(config_clone, "attack_sequence_turn", -1.0);
				StructVariableSet(config_clone, "attack_sequence_image_speed", -1.0);
				StructVariableSet(config_clone, "projectile_speed", 3.0);
				StructVariableSet(config_clone, "split_distance", -1.0);
				StructVariableSet(config_clone, "split_depth", -1.0);
				StructVariableSet(config_clone, "split_angle", -1.0);
				StructVariableSet(monster, "config", config_clone);
				StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
			}
			else if (pattern == Patterns::SPIN)
			{
				// Rotates 18-degrees at a time while shooting 5 pellets in a small cone
				double attack_sequence = 20;
				double attack_legion = 3; // 5
				double attack_sequence_turn = 18;
				double split_distance = 20; // 5
				double split_depth = 2;
				double split_angle = 40;

				if (!is_outbreak)
					StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

				StructVariableSet(config_clone, "launcher", false);
				StructVariableSet(config_clone, "attack_sequence", attack_sequence);
				StructVariableSet(config_clone, "attack_legion", attack_legion);
				StructVariableSet(config_clone, "attack_sequence_turn", attack_sequence_turn);
				StructVariableSet(config_clone, "attack_sequence_image_speed", 3.0);
				StructVariableSet(config_clone, "projectile_speed", 3.0);
				StructVariableSet(config_clone, "split_distance", split_distance);
				StructVariableSet(config_clone, "split_depth", split_depth);
				StructVariableSet(config_clone, "split_angle", split_angle);
				StructVariableSet(monster, "config", config_clone);
				StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
			}
			else if (pattern == Patterns::SPLIT)
			{
				// Shoots a single pellet that then splits into many that repeatedly split
				double attack_sequence = 3;
				double attack_legion = 1;
				double attack_sequence_turn = -1;
				double split_distance = 20;
				double split_depth = 5;
				double split_angle = 20;

				if (!is_outbreak)
					StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

				StructVariableSet(config_clone, "launcher", false);
				StructVariableSet(config_clone, "attack_sequence", attack_sequence);
				StructVariableSet(config_clone, "attack_legion", attack_legion);
				StructVariableSet(config_clone, "attack_sequence_turn", attack_sequence_turn);
				StructVariableSet(config_clone, "attack_sequence_image_speed", -1.0);
				StructVariableSet(config_clone, "projectile_speed", 3.0);
				StructVariableSet(config_clone, "split_distance", split_distance);
				StructVariableSet(config_clone, "split_depth", split_depth);
				StructVariableSet(config_clone, "split_angle", split_angle);
				StructVariableSet(monster, "config", config_clone);
				StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
			}
		}
	}
}

void ModifyStalagmiteAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster)
{
	const enum class Modes {
		DONUT_PB,
		CROSS_X,
		CHECKERBOARD
	};

	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> zero_to_one_distribution(0, 1);
	std::uniform_int_distribution<size_t> random_mode_distribution(0, magic_enum::enum_count<Modes>() - 1);

	// Setup the state tracker variable.
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	// Setup the attack pattern mode.
	RValue attack_pattern_mode_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__attack_pattern_mode" });
	if (!attack_pattern_mode_exists.ToBoolean())
	{
		if (is_boss_battle)
		{
			Modes mode = Modes::CHECKERBOARD;
			StructVariableSet(monster, "__deep_dungeon__attack_pattern_mode", magic_enum::enum_name(mode));
			boss_monsters_configured++;
		}
		else
		{
			Modes mode = magic_enum::enum_value<Modes>(zero_to_one_distribution(random_generator)); // Only use DONUT_PB and CROSS_X modes for Dread Beasts.
			StructVariableSet(monster, "__deep_dungeon__attack_pattern_mode", magic_enum::enum_name(mode));
		}
	}
	Modes attack_pattern_mode = magic_enum::enum_cast<Modes>(monster.GetMember("__deep_dungeon__attack_pattern_mode").ToString()).value();

	// Control the attack patterns.
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			// Randomly choose starting attack. Each pattern has two alternating attacks.
			int starting_pattern = zero_to_one_distribution(random_generator);

			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (attack_pattern_mode == Modes::DONUT_PB)
			{
				if (starting_pattern == 0) // Donut AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { donut_aoe_points.size() });
					for (int i = 0; i < donut_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, donut_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, donut_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
				else if (starting_pattern == 1) // PB AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { pb_aoe_points.size() });
					for (int i = 0; i < pb_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, pb_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, pb_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
			}
			else if (attack_pattern_mode == Modes::CROSS_X)
			{
				if (starting_pattern == 0) // Cross AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { cross_aoe_points.size() });
					for (int i = 0; i < cross_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, cross_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, cross_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
				else if (starting_pattern == 1) // X AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { x_aoe_points.size() });
					for (int i = 0; i < x_aoe_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, x_aoe_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, x_aoe_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
			}
			else if (attack_pattern_mode == Modes::CHECKERBOARD)
			{
				if (starting_pattern == 0) // Checkerboard AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_a_points.size() });
					for (int i = 0; i < checkerboard_room_wide_a_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_a_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_a_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
				else if (starting_pattern == 1) // Checkerboard Inverted AOE
				{
					RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_b_points.size() });
					for (int i = 0; i < checkerboard_room_wide_b_points.size(); i++)
					{
						RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_b_points[i][0] });
						g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_b_points[i][1] });
						g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
					}

					if (!is_outbreak)
						StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

					StructVariableSet(config_clone, "secondary_spikes", secondary_spikes);
					StructVariableSet(monster, "config", config_clone);
					StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", starting_pattern);
				}
			}
		}
	}

	if (custom_attack_pattern_exists.ToBoolean() && StructVariableExists(monster, "fsm"))
	{
		RValue state_id = monster.GetMember("fsm").GetMember("state").GetMember("state_id");
		if (state_id.ToInt64() == monster_category_to_state_id_map["mite"]["attack"])
			*monster.GetRefMember("__deep_dungeon__wait_to_change_attack_pattern") = false;

		if (state_id.ToInt64() == monster_category_to_state_id_map["mite"]["tired"] && !wait_to_change_attack_pattern.ToBoolean())
		{
			*monster.GetRefMember("__deep_dungeon__wait_to_change_attack_pattern") = true;

			int custom_attack_pattern = monster.GetMember("__deep_dungeon__custom_attack_pattern").ToInt64() + 1;
			if (custom_attack_pattern > 1)
				custom_attack_pattern = 0;

			if (attack_pattern_mode == Modes::DONUT_PB)
			{
				if (custom_attack_pattern == 0) // Donut AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { donut_aoe_points.size() });
						for (int i = 0; i < donut_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, donut_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, donut_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
				else if (custom_attack_pattern == 1) // PB AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { pb_aoe_points.size() });
						for (int i = 0; i < pb_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, pb_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, pb_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
			}
			else if (attack_pattern_mode == Modes::CROSS_X)
			{
				if (custom_attack_pattern == 0) // Cross AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { cross_aoe_points.size() });
						for (int i = 0; i < cross_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, cross_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, cross_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
				else if (custom_attack_pattern == 1) // X AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { x_aoe_points.size() });
						for (int i = 0; i < x_aoe_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, x_aoe_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, x_aoe_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
			}
			else if (attack_pattern_mode == Modes::CHECKERBOARD)
			{
				if (custom_attack_pattern == 0) // Checkerboard AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_a_points.size() });
						for (int i = 0; i < checkerboard_room_wide_a_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_a_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_a_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
				else if (custom_attack_pattern == 1) // Checkerboard Inverted AOE
				{
					if (StructVariableExists(monster, "config"))
					{
						RValue config = *monster.GetRefMember("config");
						RValue secondary_spikes = g_ModuleInterface->CallBuiltin("array_create", { checkerboard_room_wide_b_points.size() });
						for (int i = 0; i < checkerboard_room_wide_b_points.size(); i++)
						{
							RValue pair = g_ModuleInterface->CallBuiltin("array_create", { 2 });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 0, checkerboard_room_wide_b_points[i][0] });
							g_ModuleInterface->CallBuiltin("array_set", { pair, 1, checkerboard_room_wide_b_points[i][1] });
							g_ModuleInterface->CallBuiltin("array_set", { secondary_spikes, i, pair });
						}

						*config.GetRefMember("secondary_spikes") = secondary_spikes;
					}
				}
			}

			*monster.GetRefMember("__deep_dungeon__custom_attack_pattern") = custom_attack_pattern;
		}
	}
}

void ModifySaplingAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id)
{
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "sticky", true);
			StructVariableSet(config_clone, "free_fly", true);
			StructVariableSet(config_clone, "air_speed_modifier", 0.6);
			StructVariableSet(config_clone, "use_circle", true);
			StructVariableSet(config_clone, "speed", 0.75);
			// "jump_speed": -2.0,
			StructVariableSet(config_clone, "attack_radius", 624);
			StructVariableSet(config_clone, "max_jump_radius", 624);
			StructVariableSet(config_clone, "aggro_radius", 624);
			StructVariableSet(config_clone, "sap_children_birth_timer", 30);
			StructVariableSet(config_clone, "sap_children_birth_distance", 15);
			StructVariableSet(config_clone, "sap_children", 3);
			StructVariableSet(config_clone, "sap_children_species", RValue(monster_id_to_name_map[monster_id]));

			RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 15 }); // 45
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 25 }); // 55
			StructVariableSet(config_clone, "acknowledgment", acknowledgment);

			RValue tired = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { tired, 0, 10 }); // 90
			g_ModuleInterface->CallBuiltin("array_set", { tired, 1, 30 }); // 110
			StructVariableSet(config_clone, "tired", tired);

			RValue windup = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup, 0, 1 }); // 35
			g_ModuleInterface->CallBuiltin("array_set", { windup, 1, 1 }); // 55
			StructVariableSet(config_clone, "windup", windup);

			if (monster_id == monster_name_to_id_map["sapling_pink"])
			{
				StructVariableSet(config_clone, "hyper_armor", 3);
			}

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyShroomAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "spew_lava", true);
			StructVariableSet(config_clone, "lava_damage", 1);
			StructVariableSet(config_clone, "lava_angle", 90);
			StructVariableSet(config_clone, "lava_distance", 16);
			StructVariableSet(config_clone, "lava_timer", 500); // 300
			StructVariableSet(config_clone, "lava_count", 4);
			StructVariableSet(config_clone, "attack_radius", 704);
			StructVariableSet(config_clone, "hide_radius", 360);
			StructVariableSet(config_clone, "shadow_threshold", 0.18); // 0.2
			StructVariableSet(config_clone, "fade_in_rate", 0.18); // 0.2
			StructVariableSet(config_clone, "fade_out_rate", 0.18); // 0.2

			StructVariableSet(config_clone, "windup_friction", 0.93); // 0.93
			StructVariableSet(config_clone, "push_force", 500);
			StructVariableSet(config_clone, "ari_bounce_distance", 500);

			RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 1 });
			StructVariableSet(config_clone, "acknowledgment", acknowledgment);

			RValue tired = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { tired, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { tired, 1, 1 });
			StructVariableSet(config_clone, "tired", tired);

			RValue windup = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup, 0, 10 });
			g_ModuleInterface->CallBuiltin("array_set", { windup, 1, 20 });
			StructVariableSet(config_clone, "windup", windup);

			RValue wiggle = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { wiggle, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { wiggle, 1, 1 });
			StructVariableSet(config_clone, "wiggle", wiggle);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyEnchanternAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue wait_to_change_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__wait_to_change_attack_pattern" });
	if (!wait_to_change_attack_pattern_exists.ToBoolean())
		StructVariableSet(monster, "__deep_dungeon__wait_to_change_attack_pattern", false);
	RValue wait_to_change_attack_pattern = monster.GetMember("__deep_dungeon__wait_to_change_attack_pattern");

	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "charge_speed", 3);
			StructVariableSet(config_clone, "flee_speed", 2);
			StructVariableSet(config_clone, "attack_radius", 384);
			StructVariableSet(config_clone, "drops_balls", true);

			RValue charge_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { charge_timer, 0, 360 }); // 540
			g_ModuleInterface->CallBuiltin("array_set", { charge_timer, 1, 400 }); // 600
			StructVariableSet(config_clone, "charge_timer", charge_timer);

			RValue projectile_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { projectile_timer, 0, 200 }); // 400
			g_ModuleInterface->CallBuiltin("array_set", { projectile_timer, 1, 400 }); // 800
			StructVariableSet(config_clone, "projectile_timer", projectile_timer);

			RValue flee_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 0, 180 }); // 120
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 1, 200 }); // 160
			StructVariableSet(config_clone, "flee_timer", flee_timer);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifySpiritAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			if (!is_outbreak)
				StructVariableSet(config_clone, "projectile_damage", static_cast<int>(config_clone.GetMember("projectile_damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			RValue idle_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { idle_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { idle_duration, 1, 2 });
			StructVariableSet(config_clone, "idle_duration", idle_duration);

			RValue tired_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { tired_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { tired_duration, 1, 2 });
			StructVariableSet(config_clone, "tired_duration", tired_duration);

			RValue teleport_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_duration, 1, 2 });
			StructVariableSet(config_clone, "teleport_duration", teleport_duration);

			RValue teleport_distance_from_player = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_distance_from_player, 0, 10 });
			g_ModuleInterface->CallBuiltin("array_set", { teleport_distance_from_player, 1, 30 });
			StructVariableSet(config_clone, "teleport_distance_from_player", teleport_distance_from_player);

			if (monster_id == monster_name_to_id_map["spirit_purple"])
			{
				//StructVariableSet(config_clone, "belt_size", 4);
				StructVariableSet(config_clone, "projectile_speed", 4.5);
				StructVariableSet(config_clone, "projectile_turn_rate", 0.4);
				StructVariableSet(config_clone, "projectile_distance", 24); // Distance they rotate at from the spirit
				StructVariableSet(config_clone, "rotation_speed", 8);
				StructVariableSet(config_clone, "shot_rate", 60);
				StructVariableSet(config_clone, "pre_attack_wait", 60);

				RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
				g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 10 });
				g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 20 });
				StructVariableSet(config_clone, "acknowledgment", acknowledgment);
			}

			if (monster_id == monster_name_to_id_map["spirit"])
			{
				StructVariableSet(config_clone, "projectile_turn_rate", 0.8);

				RValue projectile_life_time = g_ModuleInterface->CallBuiltin("array_create", { 2 });
				g_ModuleInterface->CallBuiltin("array_set", { projectile_life_time, 0, 240 });
				g_ModuleInterface->CallBuiltin("array_set", { projectile_life_time, 1, 240 });
				StructVariableSet(config_clone, "projectile_life_time", projectile_life_time);
			}

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyCatAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster, int monster_id)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "charge_range", 192);
			StructVariableSet(config_clone, "attack_movement_speed", 8);

			RValue windup_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 0, 15 });
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 1, 30 });
			StructVariableSet(config_clone, "windup_duration", windup_duration);

			RValue attack_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 0, 180 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 1, 180 });
			StructVariableSet(config_clone, "attack_duration", attack_duration);

			RValue attack_stall_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_stall_duration, 0, 15 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_stall_duration, 1, 30 });
			StructVariableSet(config_clone, "attack_stall_duration", attack_stall_duration);

			if (monster_id == monster_name_to_id_map["cat"])
			{
				StructVariableSet(config_clone, "petrified_duration", 60);
				StructVariableSet(config_clone, "petrified_shakes", 10);
			}

			if (monster_id == monster_name_to_id_map["cat_void"])
			{
				*config_clone.GetRefMember("light_hater")->GetRefMember("light_health") = 99999;
			}

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyBatAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "speed", 3);
			StructVariableSet(config_clone, "flee_speed", 3); // -1
			StructVariableSet(config_clone, "attack_radius", 144);
			StructVariableSet(config_clone, "variant_attack", true);

			RValue acknowledgment = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 0, 9 }); // 45
			g_ModuleInterface->CallBuiltin("array_set", { acknowledgment, 1, 11 }); // 55
			StructVariableSet(config_clone, "acknowledgment", acknowledgment);

			RValue flee_timer = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 0, 90 }); // 30
			g_ModuleInterface->CallBuiltin("array_set", { flee_timer, 1, 120 }); // 45
			StructVariableSet(config_clone, "flee_timer", flee_timer);

			RValue attack_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 0, 1 });
			g_ModuleInterface->CallBuiltin("array_set", { attack_duration, 1, 2 });
			StructVariableSet(config_clone, "attack_duration", attack_duration);

			RValue windup_duration = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 0, 5 }); // 15
			g_ModuleInterface->CallBuiltin("array_set", { windup_duration, 1, 10 }); // 30
			StructVariableSet(config_clone, "windup_duration", windup_duration);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyTomeAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "acknowledgment", 15); // 32
			StructVariableSet(config_clone, "flying_speed", 6); // 2.4
			StructVariableSet(config_clone, "flying_timeout", 450); // 220
			StructVariableSet(config_clone, "steering", 0.8); // 0.6
			StructVariableSet(config_clone, "charge_up_time", 1); // 100
			StructVariableSet(config_clone, "idle_override_len", 1); // 60
			StructVariableSet(config_clone, "wind_attack_duration", 60); // 135
			StructVariableSet(config_clone, "stun_star_duration", 1); // 60
			StructVariableSet(config_clone, "stun_blink_duration", 1); // 180
			StructVariableSet(config_clone, "blink_timing", 1); // 60
			StructVariableSet(config_clone, "blink_timing_decrement", 1); // 10
			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyRockStackAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "speed", 10); // 1.5
			StructVariableSet(config_clone, "aggro_radius", 624); // 256
			StructVariableSet(config_clone, "air_speed", 10); // 1.25
			StructVariableSet(config_clone, "air_speed_starting", 10); // 0
			StructVariableSet(config_clone, "air_speed_max", 10); // 0.02
			StructVariableSet(config_clone, "rising_duration", 10); // 30
			StructVariableSet(config_clone, "fall_speed", 15); // 5 // 1
			StructVariableSet(config_clone, "hop_threshold", 9999); // 48

			RValue fell_stall = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { fell_stall, 0, 1 }); // 30 // 60
			g_ModuleInterface->CallBuiltin("array_set", { fell_stall, 1, 2 }); // 45 // 75
			StructVariableSet(config_clone, "fell_stall", fell_stall);

			RValue air_stall = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { air_stall, 0, 75 }); // 150
			g_ModuleInterface->CallBuiltin("array_set", { air_stall, 1, 150 }); // 250
			StructVariableSet(config_clone, "air_stall", air_stall);

			RValue air_wait = g_ModuleInterface->CallBuiltin("array_create", { 2 });
			g_ModuleInterface->CallBuiltin("array_set", { air_wait, 0, 1 }); // 15 // 30
			g_ModuleInterface->CallBuiltin("array_set", { air_wait, 1, 2 }); // 30 // 60
			StructVariableSet(config_clone, "air_wait", air_wait);

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyGriffinStatueAttackPatterns(bool is_boss_battle, RValue is_outbreak, RValue monster)
{
	RValue custom_attack_pattern_exists = g_ModuleInterface->CallBuiltin("struct_exists", { monster, "__deep_dungeon__custom_attack_pattern" });
	if (!custom_attack_pattern_exists.ToBoolean())
	{
		if (StructVariableExists(monster, "config"))
		{
			RValue config = monster.GetMember("config");
			RValue config_clone = g_ModuleInterface->CallBuiltin("variable_clone", { config });

			if (is_boss_battle)
				boss_monsters_configured++;

			if (!is_outbreak)
				StructVariableSet(config_clone, "damage", static_cast<int>(config_clone.GetMember("damage").ToDouble() * Config::config.dread_beast_damage_modifier));

			StructVariableSet(config_clone, "aggro_radius", 624); // 360
			StructVariableSet(config_clone, "speed", 10); // 0.6
			StructVariableSet(config_clone, "stomp_frames", 24); // 6
			StructVariableSet(config_clone, "bonus_damage", 13); // 5
			StructVariableSet(config_clone, "tumble_hit_reduction", 0.4); // 0.75
			StructVariableSet(config_clone, "tumble_hit_speed_reduction", 10); // 1
			StructVariableSet(config_clone, "tumble_hit_speed", 10); // 6
			StructVariableSet(config_clone, "tumble_spd_reduction", 0.4); // 0.2
			StructVariableSet(config_clone, "move_speed", is_boss_battle ? 6 : 3);
			StructVariableSet(config_clone, "chase_rate", 5); // 20
			StructVariableSet(config_clone, "jump_speed_gain", 1); // 0.25
			StructVariableSet(config_clone, "gravity_gain", 1); // 0.25

			StructVariableSet(monster, "config", config_clone);
			StructVariableSet(monster, "__deep_dungeon__custom_attack_pattern", 0);
		}
	}
}

void ModifyDreadBeastAttackPatterns(bool is_boss_battle, bool is_outbreak, RValue monster)
{
	int monster_id = monster.GetMember("monster_id").ToInt64();
	if (monster_id == monster_name_to_id_map["rockclod"] || monster_id == monster_name_to_id_map["rockclod_blue"] || monster_id == monster_name_to_id_map["rockclod_green"] || monster_id == monster_name_to_id_map["rockclod_red"]) // TODO: "rockclod_purple" if/when implemented
		ModifyRockClodAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["stalagmite"] || monster_id == monster_name_to_id_map["stalagmite_green"] || monster_id == monster_name_to_id_map["stalagmite_purple"])
		ModifyStalagmiteAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["sapling"] || monster_id == monster_name_to_id_map["sapling_cool"] || monster_id == monster_name_to_id_map["sapling_blue"] || monster_id == monster_name_to_id_map["sapling_purple"] || monster_id == monster_name_to_id_map["sapling_orange"] || monster_id == monster_name_to_id_map["sapling_pink"])
		ModifySaplingAttackPatterns(is_boss_battle, is_outbreak, monster, monster_id);
	if (monster_id == monster_name_to_id_map["mushroom"] || monster_id == monster_name_to_id_map["mushroom_green"] || monster_id == monster_name_to_id_map["mushroom_blue"] || monster_id == monster_name_to_id_map["mushroom_purple"])
		ModifyShroomAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["enchantern"] || monster_id == monster_name_to_id_map["enchantern_blue"])
		ModifyEnchanternAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["spirit"] || monster_id == monster_name_to_id_map["spirit_purple"])
		ModifySpiritAttackPatterns(is_boss_battle, is_outbreak, monster, monster_id);
	if (monster_id == monster_name_to_id_map["cat"] || monster_id == monster_name_to_id_map["cat_void"])
		ModifyCatAttackPatterns(is_boss_battle, is_outbreak, monster, monster_id);
	if (monster_id == monster_name_to_id_map["bat"] || monster_id == monster_name_to_id_map["bat_blue"])
		ModifyBatAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["tome"])
		ModifyTomeAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["rock_stack"])
		ModifyRockStackAttackPatterns(is_boss_battle, is_outbreak, monster);
	if (monster_id == monster_name_to_id_map["griffin_statue"])
		ModifyGriffinStatueAttackPatterns(is_boss_battle, is_outbreak, monster);
}
