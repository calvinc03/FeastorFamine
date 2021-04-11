#include <map>
#include "projectile.hpp"

// this is for initial values; when the entity is created

const std::map<unit_type, int> unit_damage = {
	{HUNTER, 15},
	//{WATCHTOWER, 10},
	{GREENHOUSE, 60},
	{EXTERMINATOR, 10},
	{ROBOT, 10},
	{PRIESTESS, 5},
	{SNOWMACHINE, 50},
	{WALL, 0}
};

const std::map<unit_type, float> unit_attack_interval_ms = {
	{HUNTER, 1500.f},
	//{WATCHTOWER, 800.f},
	{GREENHOUSE, 0.f},
	{EXTERMINATOR, 2000.f},
	{ROBOT, 2000.f},
	{PRIESTESS, 1.25f},
	{SNOWMACHINE, 1500.f},
	{WALL, 0.f}
};

const std::map<unit_type, int> unit_attack_range = {
	{HUNTER, 300},
	//{WATCHTOWER, 400},
	{GREENHOUSE, 0},
	{EXTERMINATOR, 125},
	{ROBOT, 250},
	{PRIESTESS, 300},
	{SNOWMACHINE, 300},
	{WALL, 0}
};

const std::map<unit_type, int> unit_num_projectiles = {
	{HUNTER, 1},
	//{WATCHTOWER, 1},
	{GREENHOUSE, 0},
	{EXTERMINATOR, 1},
	{ROBOT, 1},
	{PRIESTESS, 0},
	{SNOWMACHINE, 1},
	{WALL, 0}
};

typedef entt::entity(*fnc_pointer)(entt::entity e_unit, entt::entity e_monster, int damage);

const std::map<unit_type, fnc_pointer> unit_create_projectiles = {
	{HUNTER, Projectile::createProjectile},
	//{WATCHTOWER, Projectile::createProjectile},
	{GREENHOUSE, NULL},
	{EXTERMINATOR, Flamethrower::createFlamethrower},
	{ROBOT, LaserBeam::createLaserBeam},
	{PRIESTESS, NULL},
	{SNOWMACHINE, Snowball::createSnowball},
	{WALL, NULL}
};

const std::map<unit_type, bool> unit_rotate = {
	{HUNTER, false},
	//{WATCHTOWER, false},
	{GREENHOUSE, false},
	{EXTERMINATOR, false},
	{ROBOT, false},
	{PRIESTESS, false},
	{SNOWMACHINE, false},
	{WALL, false}
};


const std::map<unit_type, int> unit_cost = {
	{HUNTER, 150},
	//{WATCHTOWER, 200},
	{GREENHOUSE, 250},
	{EXTERMINATOR, 300},
	{ROBOT, 400},
	{PRIESTESS, 400},
	{SNOWMACHINE, 150},
	{WALL, 50}
};

const std::map<unit_type, int> unit_sell_price = {
	{HUNTER, 150},
	//{WATCHTOWER, 150},
	{GREENHOUSE, 250},
	{EXTERMINATOR, 300},
	{ROBOT, 400},
	{PRIESTESS, 400},
	{SNOWMACHINE, 150},
	{WALL, 50}
};

const std::map<unit_type, int> unit_health = {
	{HUNTER, 100},
	//{WATCHTOWER, 100},
	{GREENHOUSE, 100},
	{EXTERMINATOR, 100},
	{ROBOT, 100},
	{PRIESTESS, 100},
	{SNOWMACHINE, 100},
	{WALL, 100}
};