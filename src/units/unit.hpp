#pragma once
// build cost
const int WATCHTOWER_COST = 200;
const int GREENHOUSE_COST = 300;
const int HUNTER_COST = 150;
const int WALL_COST = 50;

// upgrade cost
const int WATCHTOWER_UPGRADE_COST = 50;
const int GREENHOUSE_UPGRADE_COST = 200;
const int HUNTER_UPGRADE_COST = 50;
const int WALL_UPGRADE_COST = 50;

// sell price
const int WATCHTOWER_SELL_PRICE = 150;
const int GREENHOUSE_SELL_PRICE = 250;
const int HUNTER_SELL_PRICE = 100;
const int WALL_SELL_PRICE = 80;

struct Unit {
	int type;
	int damage;
	size_t attack_interval_ms;
	float next_projectile_spawn;
	int attack_range;
	int workers;
	int upgrades;
	bool rotate;
	unsigned int upgrade_cost = 0;
	unsigned int cost = 0;
	unsigned int sell_price = 0;
	unsigned int health = 200;
};