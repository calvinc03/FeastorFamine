#pragma once
#include <map>
#include "config/unit_config.hpp"

struct Unit {
	unit_type type;
	int damage;
	size_t attack_interval_ms;
	float next_projectile_spawn;
	int attack_range;
	int upgrades;
	bool rotate;
	unsigned int upgrade_cost = 0;
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
	300,	//attack_range
	0,		//upgrades
	false,	//rotate
	50,		//upgrade_cost
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
	400,		//attack_range
	0,			//upgrades
	false,		//rotate
	50,			//upgrade_cost
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
	0,			//upgrades
	false,		//rotate
	200,		//upgrade_cost
	300,		//cost
	200,		//sell_price
	100,		//health
	100,		//max_health
};

const Unit wall_unit = {
	WALL,		//type
	0,			//damage
	0,			//attack_interval_ms
	0,			//next_projectile_spawn
	0,			//attack_range
	0,			//upgrades
	false,		//rotate
	50,   		//upgrade_cost
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
};
