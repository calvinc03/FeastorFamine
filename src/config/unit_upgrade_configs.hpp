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
	{0, {50}},
	{1, {100, 40, 20}},
	{2, {150, 90, 30}},
	{3, {70, 140, 40}}
};

// range upgrade
const std::map<int, std::vector<int>> hunter_path_2 = {
	{0, {40}},
	{1, {80, 30, 350}},
	{2, {120, 70, 400}},
	{3, {80, 110, 450}}
};

// food upgrade 
const std::map<int, std::vector<int>> greenhouse_path_1 = {
	{0, {60}},
	{1, {80, 40, 90}},
	{2, {100, 50, 120}},
	{3, {80, 50, 150}}
};

// I have no idea if this is going to be used
const std::map<int, std::vector<int>> greenhouse_path_2 = {
	{0, {60}},
	{1, {60, 10, 90}},
	{2, {60, 10, 120}},
	{3, {60, 10, 150}}
};

// damage upgrade 
const std::map<int, std::vector<int>> exterminator_path_1 = {
	{0, {100}},
	{1, {150, 80, 20}},
	{2, {200, 100, 30}},
	{3, {10, 150, 40}}
};

// range upgrade
const std::map<int, std::vector<int>> exterminator_path_2 = {
	{0, {100}},
	{1, {150, 80, 10}},
	{2, {200, 120, 20}},
	{3, {10, 150, 30}}
};

// number of projectile upgrade 
const std::map<int, std::vector<int>> robot_path_1 = {
	{0, {200}},
	{1, {300, 150, 20}},
	{2, {400, 250, 30}},
	{3, {10, 350, 40}}
};

// range upgrade
const std::map<int, std::vector<int>> robot_path_2 = {
	{0, {200}},
	{1, {300, 150, 2}},
	{2, {400, 250, 3}},
	{3, {10, 350, 4}}
};

// damage upgrade 
const std::map<int, std::vector<int>> priestess_path_1 = {
	{0, {200}},
	{1, {400, 150, 10}},
	{2, {600, 350, 15}},
	{3, {10, 550, 20}}
};

// attack speed upgrade
const std::map<int, std::vector<float>> priestess_path_2 = {
	{0, {300}},
	{1, {500, 200, 1.5f}},
	{2, {700, 400, 1.75f}},
	{3, {10, 600, 2.0f}}
};

// number of projectile upgrade  
const std::map<int, std::vector<int>> snowmachine_path_1 = {
	{0, {100}},
	{1, {200, 100, 2}},
	{2, {300, 200, 3}},
	{3, {10, 300, 4}}
};

// range upgrade
const std::map<int, std::vector<int>> snowmachine_path_2 = {
	{0, {250}},
	{1, {200, 250, 30}},
	{2, {300, 150, 50}},
	{3, {10, 150, 70}}
};

// damage upgrade  
const std::map<int, std::vector<int>> wall_path_1 = {
	{0, {100}},
	{1, {150, 50, 150}},
	{2, {250, 100, 250}},
	{3, {10, 200, 400}}
};

// health upgrade
const std::map<int, std::vector<int>> wall_path_2 = {
	{0, {10}},
	{1, {10, 10, 20}},
	{2, {10, 10, 20}},
	{3, {10, 10, 20}}
};