#include <map>
#include <vector>

// each unit has a map for upgrade paths. the key is used to represent the level and the value is a vector
// vector[0] = cost to upgrade to next level 
// vector[1] = health to be added to sell_price 
// vector[2] = value to be modified
// each unit will have potential 3 upgrades on both paths for simplicity sake
// the first entree will only consist of the upgrade cost

// damage upgrade 
const std::map<int, std::vector<int>> hunter_path_1 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {30, 10, 30}},
	{3, {70, 10, 40}}
};

// range upgrade
const std::map<int, std::vector<int>> hunter_path_2 = {
	{0, {10}},
	{1, {20, 10, 350}},
	{2, {50, 10, 400}},
	{3, {80, 10, 450}}
};

// food upgrade 
const std::map<int, std::vector<int>> greenhouse_path_1 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};

// damage upgrade 
const std::map<int, std::vector<int>> exterminator_path_1 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 30}},
	{3, {10, 10, 40}}
};

// range upgrade
const std::map<int, std::vector<int>> exterminator_path_2 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 30}},
	{3, {10, 10, 40}}
};

// number of projectile upgrade 
const std::map<int, std::vector<int>> robot_path_1 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};

// range upgrade
const std::map<int, std::vector<int>> robot_path_2 = {
	{0, {10}},
	{1, {10, 10, 2}},
	{2, {10, 10, 3}},
	{3, {10, 10, 4}}
};

// damage upgrade 
const std::map<int, std::vector<int>> priestess_path_1 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};

// attack speed upgrade
const std::map<int, std::vector<int>> priestess_path_2 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};

// number of projectile upgrade  
const std::map<int, std::vector<int>> snowmachine_path_1 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};

// range upgrade
const std::map<int, std::vector<int>> snowmachine_path_2 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};