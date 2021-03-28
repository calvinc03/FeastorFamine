#include <map>

const std::map<unit_type, int> unit_damage = {
	{HUNTER, 15},
	{WATCHTOWER, 10},
};

const std::map<unit_type, size_t> unit_attack_interval_ms = {
	{HUNTER, 1500},
	{WATCHTOWER, 800},
};

const std::map<unit_type, int> unit_attack_range = {
	{HUNTER, 300},
	{WATCHTOWER, 400},
};


const std::map<unit_type, unsigned int> upgrade_cost = {
	{HUNTER, 50},
	{WATCHTOWER, 150},
};

const std::map<unit_type, int> unit_cost = {
	{HUNTER, 150},
	{WATCHTOWER, 200},
};

const std::map<unit_type, int> unit_sell_price = {
	{HUNTER, 100},
	{WATCHTOWER, 150},
};