#pragma once
#include <map>
#include "config/unit_config.hpp"
#include "projectile.hpp"

struct Unit {
	unit_type type;
	int damage;
	size_t attack_interval_ms;
	float next_projectile_spawn;
	int attack_range;
	int path_1_upgrade;
	int path_2_upgrade;

	int num_projectiles;
	entt::entity(*create_projectile)(entt::entity e_unit, entt::entity e_monster, int damage);

	bool rotate;
	unsigned int upgrade_path_1_cost = 0;
	unsigned int upgrade_path_2_cost = 0;
	unsigned int cost = 0;
	unsigned int sell_price = 0;
	unsigned int health = 100;
	unsigned int max_health = 100;
};

const Unit hunter_unit = {
	HUNTER, //type
	10,		//damage
	1500,	//attack_interval_ms
	0,		//next_projectile_spawn
	200,	//attack_range
	0,		//path_1_upgrade
	0,      //path_2_upgrade
	1,      //num_projectiles
	Projectile::createProjectile, //create projectile
	false,	//rotate
	10,		//upgrade_path_1_cost
	10,     //upgrade_path_2_cost
	150,	//cost
	100,	//sell_price
	100,	//health
	100,	//max_health
};

const Unit watchtower_unit = {
	WATCHTOWER, //type
	10,			//damage
	800,		//attack_interval_ms
	0,			//next_projectile_spawn
	250,		//attack_range
	0,		    //path_1_upgrade
	0,          //path_2_upgrade
	1,          //num_projectiles
	LaserBeam::createLaserBeam, //create projectile
	false,		//rotate
	10,		    //upgrade_path_1_cost
	10,         //upgrade_path_2_cost
	200,		//cost
	150,		//sell_price
	100,		//health
	100,		//max_health
};

const Unit greenhouse_unit = {
	GREENHOUSE, //type
	0,			//damage
	0,			//attack_interval_ms
	0,			//next_projectile_spawn
	0,			//attack_range
	0,		    //path_1_upgrade
	0,          //path_2_upgrade
	0,          //num_projectiles
	NULL,       //create projectile
	false,		//rotate
	50,		    //upgrade_path_1_cost
	10,         //upgrade_path_2_cost
	300,		//cost
	200,		//sell_price
	100,		//health
	100,		//max_health
};

const Unit exterminator_unit = {
	EXTERMINATOR, //type
	10,		//damage
	2000,	//attack_interval_ms
	0,		//next_projectile_spawn
	300,	//attack_range
	0,		//path_1_upgrade
	0,      //path_2_upgrade
	1,      //num_projectiles
	Flamethrower::createFlamethrower, //create projectile
	false,	//rotate
	10,		//upgrade_path_1_cost
	10,     //upgrade_path_2_cost
	150,	//cost
	100,	//sell_price
	100,	//health
	100,	//max_health
};

const Unit priestess_unit = {
	PRIESTESS, //type
	10,		//damage
	0,	    //attack_interval_ms
	0,		//next_projectile_spawn
	300,	//attack_range
	0,		//path_1_upgrade
	0,      //path_2_upgrade
	1,      //num_projectiles
	NULL, //create projectile
	false,	//rotate
	10,		//upgrade_path_1_cost
	10,     //upgrade_path_2_cost
	150,	//cost
	100,	//sell_price
	100,	//health
	100,	//max_health
};

const Unit snowmachine_unit = {
	SNOWMACHINE, //type
	10,		//damage
	0,	    //attack_interval_ms
	0,		//next_projectile_spawn
	300,	//attack_range
	0,		//path_1_upgrade
	0,      //path_2_upgrade
	1,      //num_projectiles
	RockProjectile::createRockProjectile, //create projectile
	false,	//rotate
	10,		//upgrade_path_1_cost
	10,     //upgrade_path_2_cost
	150,	//cost
	100,	//sell_price
	100,	//health
	100,	//max_health
};

const Unit robot_unit = {
	ROBOT, //type
	10,		//damage
	2000,	//attack_interval_ms
	0,		//next_projectile_spawn
	250,	//attack_range
	0,		//path_1_upgrade
	0,      //path_2_upgrade
	1,      //num_projectiles
	LaserBeam::createLaserBeam, //create projectile
	false,	//rotate
	10,		//upgrade_path_1_cost
	10,     //upgrade_path_2_cost
	150,	//cost
	100,	//sell_price
	100,	//health
	100,	//max_health
};

const Unit wall_unit = {
	WALL,		//type
	0,			//damage
	0,			//attack_interval_ms
	0,			//next_projectile_spawn
	0,			//attack_range
	0,		    //path_1_upgrade
	0,          //path_2_upgrade
	0,          //num_projectiles
	NULL,       //create projectile
	false,		//rotate
	50,		    //upgrade_path_1_cost
	10,         //upgrade_path_2_cost
	50, 		//cost
	50, 		//sell_price
	100,		//health
	100,		//max_health
};

const std::map<unit_type, Unit> unit_configs = {
	{HUNTER, hunter_unit},
	{WATCHTOWER, watchtower_unit},
	{GREENHOUSE, greenhouse_unit},
	{WALL, wall_unit},
	{EXTERMINATOR, exterminator_unit}
};

void upgrade_unit_path_1(entt::entity e_unit);

void upgrade_unit_path_2(entt::entity e_unit);
