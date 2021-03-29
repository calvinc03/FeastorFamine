#include "common.hpp"
#include "unit.hpp"
#include <iostream>
#include <units/hunter.hpp>
#include <units/greenhouse.hpp>
#include <world.hpp>
#include <config/unit_upgrade_configs.hpp>


void upgrade_unit_path_1(entt::entity e_unit)
{
	auto& unit = registry.get<Unit>(e_unit);
	unit.path_1_upgrade++;
	WorldSystem::health -= unit.upgrade_path_1_cost;

	if (registry.has<Hunter>(e_unit)) {
		std::vector<int> upgrades = hunter_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.cost = upgrades[1];
		unit.damage = upgrades[2];
	}

	else if (registry.has<GreenHouse>(e_unit)) {

	}

	/*else if (registry.has<Greenhouse>(e_unit)) {

	}
	else if (registry.has<Greenhouse>(e_unit)) {

	}
	else if (registry.has<Greenhouse>(e_unit)) {

	}
	else if (registry.has<Greenhouse>(e_unit)) {

	}*/
}

void upgrade_unit_path_2(entt::entity e_unit)
{
	auto& unit = registry.get<Unit>(e_unit);
	unit.path_2_upgrade++;
	WorldSystem::health -= unit.upgrade_path_2_cost;

	if (registry.has<Hunter>(e_unit)) {
		std::vector<int> upgrades = hunter_path_2.at(unit.path_2_upgrade);
		unit.upgrade_path_2_cost = upgrades[0];
		unit.cost = upgrades[1];
		unit.attack_range = upgrades[2];
	}

	else if (registry.has<GreenHouse>(e_unit)) {

	}

	/*else if (registry.has<Greenhouse>(e_unit)) {

	}
	else if (registry.has<Greenhouse>(e_unit)) {

	}
	else if (registry.has<Greenhouse>(e_unit)) {

	}
	else if (registry.has<Greenhouse>(e_unit)) {

	}*/
}
