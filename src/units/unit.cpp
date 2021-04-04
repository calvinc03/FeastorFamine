#include "common.hpp"
#include "unit.hpp"
#include <iostream>
#include <units/hunter.hpp>
#include <units/greenhouse.hpp>
#include <world.hpp>
#include <units/exterminator.hpp>
#include <units/robot.hpp>
#include <units/priestess.hpp>
#include <units/snowmachine.hpp>


void upgrade_unit_path_1(entt::entity e_unit)
{
	auto& unit = registry.get<Unit>(e_unit);
	unit.path_1_upgrade++;
	WorldSystem::health -= unit.upgrade_path_1_cost;

	if (registry.has<Hunter>(e_unit)) {
		std::vector<int> upgrades = hunter_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];
	}

	else if (registry.has<GreenHouse>(e_unit)) {
		std::vector<int> upgrades = greenhouse_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];
	}

	else if (registry.has<Exterminator>(e_unit)) {
		std::vector<int> upgrades = exterminator_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];
	}

	else if (registry.has<Robot>(e_unit)) {
		std::vector<int> upgrades = robot_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];
	}
	else if (registry.has<Priestess>(e_unit)) {
		std::vector<int> upgrades = priestess_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];
	}
	else if (registry.has<SnowMachine>(e_unit)) {
		std::vector<int> upgrades = snowmachine_path_1.at(unit.path_1_upgrade);
		unit.upgrade_path_1_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.num_projectiles = upgrades[2];
	}
}

void upgrade_unit_path_2(entt::entity e_unit)
{
	auto& unit = registry.get<Unit>(e_unit);
	unit.path_2_upgrade++;
	WorldSystem::health -= unit.upgrade_path_2_cost;

	if (registry.has<Hunter>(e_unit)) {
		std::vector<int> upgrades = hunter_path_2.at(unit.path_2_upgrade);
		unit.upgrade_path_2_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.attack_range = upgrades[2];
	}

	else if (registry.has<Exterminator>(e_unit)) {
		std::vector<int> upgrades = exterminator_path_2.at(unit.path_2_upgrade);
		unit.upgrade_path_2_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];
		unit.create_projectile = Missile::createMissile;
		unit.attack_range = 400;
	}

	else if (registry.has<Robot>(e_unit)) {
		std::vector<int> upgrades = robot_path_2.at(unit.path_2_upgrade);
		unit.upgrade_path_2_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.num_projectiles = upgrades[2];
	}
	else if (registry.has<Priestess>(e_unit)) {
		std::vector<int> upgrades = priestess_path_2.at(unit.path_2_upgrade);
		unit.upgrade_path_2_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.attack_interval_ms = upgrades[2];
	}
	else if (registry.has<SnowMachine>(e_unit)) {
		std::vector<int> upgrades = snowmachine_path_2.at(unit.path_2_upgrade);
		unit.upgrade_path_2_cost = upgrades[0];
		unit.sell_price += upgrades[1];
		unit.damage = upgrades[2];

		unit.create_projectile = IceField::createIceField;
		unit.attack_range = 150;
	}
}
