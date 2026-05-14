#include "Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Combat;
using namespace State::Maps;

static const std::map<Classes, std::unordered_set<std::string>> CLASS_NAME_TO_ARMOR_NAMES_MAP = {
	{ Classes::CLERIC, { CLERIC_HELMET_NAME, CLERIC_CHESTPIECE_NAME, CLERIC_GLOVES_NAME, CLERIC_PANTS_NAME, CLERIC_BOOTS_NAME } },
	{ Classes::DARK_KNIGHT, { DARK_KNIGHT_HELMET_NAME, DARK_KNIGHT_CHESTPIECE_NAME, DARK_KNIGHT_GLOVES_NAME, DARK_KNIGHT_PANTS_NAME, DARK_KNIGHT_BOOTS_NAME } },
	{ Classes::MAGE, { MAGE_HELMET_NAME, MAGE_CHESTPIECE_NAME, MAGE_GLOVES_NAME, MAGE_PANTS_NAME, MAGE_BOOTS_NAME } },
	{ Classes::PALADIN, { PALADIN_HELMET_NAME, PALADIN_CHESTPIECE_NAME, PALADIN_GLOVES_NAME, PALADIN_PANTS_NAME, PALADIN_BOOTS_NAME } },
	{ Classes::ROGUE, { ROGUE_HELMET_NAME, ROGUE_CHESTPIECE_NAME, ROGUE_GLOVES_NAME, ROGUE_PANTS_NAME, ROGUE_BOOTS_NAME } },
	{ Classes::ORACLE, { ORACLE_HELMET_NAME, ORACLE_CHESTPIECE_NAME, ORACLE_GLOVES_NAME, ORACLE_PANTS_NAME, ORACLE_BOOTS_NAME } }
};

std::set<std::string> GetEquippedArmor()
{
	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	std::set<std::string> equipped_armor = {};
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "prototype"))
			{
				RValue prototype = item.GetMember("prototype");
				if (StructVariableExists(prototype, "recipe_key"))
				{
					std::string recipe_key = prototype.GetMember("recipe_key").ToString();
					equipped_armor.insert(recipe_key);
				}
			}
		}
	}

	return equipped_armor;
}

std::map<Classes, int> CountEquippedClassArmor()
{
	if (is_challenge_mode)
		return {};

	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	std::map<Classes, int> class_armor_equipped = {};
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "prototype"))
			{
				RValue prototype = item.GetMember("prototype");
				if (StructVariableExists(prototype, "recipe_key"))
				{
					RValue recipe_key = prototype.GetMember("recipe_key");
					for (const auto& class_armor : CLASS_NAME_TO_ARMOR_NAMES_MAP)
					{
						if (CLASS_NAME_TO_ARMOR_NAMES_MAP.at(class_armor.first).contains(recipe_key.ToString()))
							class_armor_equipped[class_armor.first]++;
					}
				}
			}
		}
	}

	return class_armor_equipped;
}

ArmorSetBonuses GetArmorSetBonuses()
{
	auto counts = CountEquippedClassArmor();
	ArmorSetBonuses armor_set_bonuses = {};
	armor_set_bonuses.cleric.equipped = counts[Classes::CLERIC];
	armor_set_bonuses.dark_knight.equipped = counts[Classes::DARK_KNIGHT];
	armor_set_bonuses.mage.equipped = counts[Classes::MAGE];
	armor_set_bonuses.paladin.equipped = counts[Classes::PALADIN];
	armor_set_bonuses.rogue.equipped = counts[Classes::ROGUE];
	armor_set_bonuses.oracle.equipped = counts[Classes::ORACLE];
	return armor_set_bonuses;
}

std::map<int, int> GetClassArmorInfusions()
{
	std::map<int, int> class_armor_infusions = {};

	RValue ari = global_instance->GetMember("__ari");
	RValue armor = ari.GetMember("armor");
	RValue slots = armor.GetMember("slots");
	RValue buffer = slots.GetMember("__buffer");

	size_t array_length;
	g_ModuleInterface->GetArraySize(buffer, array_length);

	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_entry;
		g_ModuleInterface->GetArrayEntry(buffer, i, array_entry);

		if (StructVariableExists(*array_entry, "item"))
		{
			RValue item = array_entry->GetMember("item");
			if (item.m_Kind == VALUE_OBJECT && StructVariableExists(item, "infusion") && StructVariableExists(item, "prototype"))
			{
				RValue infusion = item.GetMember("infusion");
				RValue prototype = item.GetMember("prototype");
				if (IsNumeric(infusion) && IsObject(prototype) && StructVariableExists(prototype, "recipe_key") && CLASS_ARMOR_NAMES.contains(prototype.GetMember("recipe_key").ToString()))
					class_armor_infusions[infusion.ToInt64()]++;
			}
		}
	}

	return class_armor_infusions;
}

bool CanAffordSpell(MMAPI::Spell::Ids spell)
{
	int cost = spell_id_to_default_cost_map[static_cast<int>(spell)];
	int mana = ari_resource_to_value_map[AriResources::MANA];
	bool fey_active = active_floor_enchantments.contains(FloorEnchantments::FEY);
	return mana >= (fey_active ? cost / 2 : cost);
}

int GetClericAutoRegenPotency()
{
	auto armor_set_bonuses = GetArmorSetBonuses();
	if (armor_set_bonuses.cleric.equipped == 0) return 0;
	if (armor_set_bonuses.cleric.equipped < 3)  return 1;
	if (armor_set_bonuses.cleric.equipped < 5)  return 2;
	return 3;
}

double GetDarkKnightDrainPotency()
{
	auto armor_set_bonuses = GetArmorSetBonuses();
	if (armor_set_bonuses.dark_knight.equipped == 0) return 0;
	if (armor_set_bonuses.dark_knight.equipped < 3)  return 0.03;
	if (armor_set_bonuses.dark_knight.equipped < 5)  return 0.05;
	return 0.08;
}

double GetPaladinHolyCirclePotency()
{
	auto armor_set_bonuses = GetArmorSetBonuses();
	if (armor_set_bonuses.paladin.equipped == 0) return 0;
	if (armor_set_bonuses.paladin.equipped < 3)  return 0.05;
	if (armor_set_bonuses.paladin.equipped < 5)  return 0.10;
	return 0.15;
}

int ScaleTemperanceDamage(int current_health, int max_health, int damage)
{
	if (max_health <= 0)
		return damage;

	current_health = std::clamp(current_health, 0, max_health);

	int health_pct = (current_health * 100) / max_health;

	static const struct {
		int health;
		int multiplier;
	} breakpoints[] = {
		{100,  20},  // -80%
		{95,   40},
		{90,   60},
		{85,   80},
		{80,  100},  // 0%
		{75,  110},
		{70,  120},
		{65,  130},
		{60,  140},
		{55,  150},
		{40,  160},
		{35,  170},
		{30,  180},
		{25,  190},
		{20,  200},
		{15,  210},
		{10,  220},
		{5,   230},
		{0,   240}
	};

	constexpr int count = sizeof(breakpoints) / sizeof(breakpoints[0]);

	for (int i = 0; i < count - 1; ++i)
	{
		int h1 = breakpoints[i].health;
		int m1 = breakpoints[i].multiplier;
		int h2 = breakpoints[i + 1].health;
		int m2 = breakpoints[i + 1].multiplier;

		if (health_pct <= h1 && health_pct >= h2)
		{
			int numerator = (health_pct - h2) * (m1 - m2);
			int denominator = (h1 - h2);
			int multiplier = m2 + numerator / denominator;

			return (damage * multiplier) / 100;
		}
	}

	return (damage * breakpoints[count - 1].multiplier) / 100;
}

ElementalSealEffects GetRandomElementalSealEffect()
{
	static thread_local pcg32 random_generator([] {
		std::random_device rd;
		return pcg32(
			(static_cast<uint64_t>(rd()) << 32) | rd(),
			(static_cast<uint64_t>(rd()) << 32) | rd()
		);
	}());
	std::uniform_int_distribution<size_t> random_elemental_seal_effect_distribution(0, magic_enum::enum_count<ElementalSealEffects>() - 1);
	return magic_enum::enum_value<ElementalSealEffects>(random_elemental_seal_effect_distribution(random_generator));
}

void ModifySpellCosts(bool reset_cost, bool in_dungeon) {
	size_t array_length = 0;
	RValue spells = global_instance->GetMember("__spells");
	g_ModuleInterface->GetArraySize(spells, array_length);
	auto armor_set_bonuses = GetArmorSetBonuses();
	for (size_t i = 0; i < array_length; i++)
	{
		RValue* array_element;
		g_ModuleInterface->GetArrayEntry(spells, i, array_element);

		const auto spell = static_cast<MMAPI::Spell::Ids>(i);
		int cost = reset_cost ? spell_id_to_default_cost_map[i] : spell_id_to_default_cost_map[i] / 2;
		if (in_dungeon && spell == MMAPI::Spell::Ids::Growth)
			cost = reset_cost ? spell_id_to_default_cost_map[i] / 2 : spell_id_to_default_cost_map[i] / 4;
		if (active_greater_sigils.contains(GreaterSigils::CHAIN_SPELL))
			cost = 0;
		if (spell == MMAPI::Spell::Ids::FullRestore && armor_set_bonuses.oracle.FullSet())
			cost = 0;
		if (spell == MMAPI::Spell::Ids::Growth && armor_set_bonuses.oracle.FullSet())
			cost = 0;

		*array_element->GetRefMember("cost") = cost;
	}
}
