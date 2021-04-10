#include <string>
#include <map>

const std::map<int, vec2> monster_velocities = {
	{ MOB, {2.5, 0}},
	{ SPIDER, {1.5, 0}},
	{ SPRING_BOSS, {1.5, 0}},
	{ SUMMER_BOSS, {2, 0}},
	{ FALL_BOSS, {1, 0}},
	{ WINTER_BOSS, {1, 0}},
	{ BURROW_BOSS, {3, 0}},
	{ FINAL_BOSS, {1, 0}},
	{ FIREBALL_BOSS, {1.0, 0.1}}
};

const std::map<int, int> monster_health = {
	{ MOB, 30},
	{ SPIDER, 80},
	{ SPRING_BOSS, 60},
	{ SUMMER_BOSS, 80},
	{ FALL_BOSS, 150},
	{ WINTER_BOSS, 200},
	{ BURROW_BOSS, 20},
	{ FINAL_BOSS, 5000},
	{ FIREBALL_BOSS, 500}
};

const std::map<int, int> monster_damage = {
	{ MOB, 10},
	{ SPIDER, 80},
	{ SPRING_BOSS, 20},
	{ SUMMER_BOSS, 30},
	{ FALL_BOSS, 100},
	{ WINTER_BOSS, 40},
	{ BURROW_BOSS, 10},
	{ FINAL_BOSS, 0},
	{ FIREBALL_BOSS, 100}
};

const std::map<int, int> monster_reward = {
	{ MOB, 10},
	{ SPIDER, 30},
	{ SPRING_BOSS, 30},
	{ SUMMER_BOSS, 40},
	{ FALL_BOSS, 50},
	{ WINTER_BOSS, 40},
	{ BURROW_BOSS, 10},
	{ FINAL_BOSS, 10000},
	{ FIREBALL_BOSS, 50}
};