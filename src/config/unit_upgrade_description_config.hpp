#include <map>
#include <vector>

// description cards for hunter path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_hunter_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(HUNTER)),
    "",
    "Upgraded Attack Damage: " + std::to_string(hunter_path_1.at(1)[2]),
    "Cost: " + std::to_string(hunter_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_hunter_1 = {
    "Current Attack Damage: " + std::to_string(hunter_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(hunter_path_1.at(2)[2]),
    "Cost: " + std::to_string(hunter_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_hunter_2 = {
    "Current Attack Damage: " + std::to_string(hunter_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(hunter_path_1.at(3)[2]),
    "Cost: " + std::to_string(hunter_path_1.at(2)[0])
};

// description cards for hunter path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_hunter_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(HUNTER)),
    "",
    "Upgraded Range: " + std::to_string(hunter_path_2.at(1)[2]),
    "Cost: " + std::to_string(hunter_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_hunter_1 = {
    "Current Range: " + std::to_string(hunter_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(hunter_path_2.at(2)[2]),
    "Cost: " + std::to_string(hunter_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_hunter_2 = {
    "Current Range: " + std::to_string(hunter_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(hunter_path_2.at(3)[2]),
    "Cost: " + std::to_string(hunter_path_2.at(2)[0])
};

// description cards for greenhouse path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_greenhouse_0 = {
    "Current Production: " + std::to_string(unit_damage.at(GREENHOUSE)),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_1.at(1)[2]),
    "Cost: " + std::to_string(greenhouse_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_greenhouse_1 = {
    "Current Production: " + std::to_string(greenhouse_path_1.at(1)[2]),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_1.at(2)[2]),
    "Cost: " + std::to_string(greenhouse_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_greenhouse_2 = {
    "Current Production: " + std::to_string(greenhouse_path_1.at(2)[2]),
    "",
    "Upgraded Production: " + std::to_string(greenhouse_path_1.at(3)[2]),
    "Cost: " + std::to_string(greenhouse_path_1.at(2)[0])
};

// description cards for exterminator path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_exterminator_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(EXTERMINATOR)),
    "",
    "Upgraded Attack Damage: " + std::to_string(exterminator_path_1.at(1)[2]),
    "Cost: " + std::to_string(exterminator_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_exterminator_1 = {
    "Current Attack Damage: " + std::to_string(exterminator_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(exterminator_path_1.at(2)[2]),
    "Cost: " + std::to_string(exterminator_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_exterminator_2 = {
    "Current Attack Damage: " + std::to_string(exterminator_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(exterminator_path_1.at(3)[2]),
    "Cost: " + std::to_string(exterminator_path_1.at(2)[0])
};

// description cards for exterminator path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_exterminator_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(EXTERMINATOR)),
    "",
    "Upgraded Range: " + std::to_string(exterminator_path_2.at(1)[2]),
    "Cost: " + std::to_string(exterminator_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_exterminator_1 = {
    "Current Range: " + std::to_string(exterminator_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(exterminator_path_2.at(2)[2]),
    "Cost: " + std::to_string(exterminator_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_exterminator_2 = {
    "Current Range: " + std::to_string(exterminator_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(exterminator_path_2.at(3)[2]),
    "Cost: " + std::to_string(exterminator_path_2.at(2)[0])
};


// description cards for robot path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_robot_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(ROBOT)),
    "",
    "Upgraded Attack Damage: " + std::to_string(robot_path_1.at(1)[2]),
    "Cost: " + std::to_string(robot_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_robot_1 = {
    "Current Attack Damage: " + std::to_string(robot_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(robot_path_1.at(2)[2]),
    "Cost: " + std::to_string(robot_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_robot_2 = {
    "Current Attack Damage: " + std::to_string(robot_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(robot_path_1.at(3)[2]),
    "Cost: " + std::to_string(robot_path_1.at(2)[0])
};

// description cards for robot path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_robot_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(ROBOT)),
    "",
    "Upgraded Range: " + std::to_string(robot_path_2.at(1)[2]),
    "Cost: " + std::to_string(robot_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_robot_1 = {
    "Current Range: " + std::to_string(robot_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(robot_path_2.at(2)[2]),
    "Cost: " + std::to_string(robot_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_robot_2 = {
    "Current Range: " + std::to_string(robot_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(robot_path_2.at(3)[2]),
    "Cost: " + std::to_string(robot_path_2.at(2)[0])
};


// description cards for priestess path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_priestess_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(PRIESTESS)),
    "",
    "Upgraded Attack Damage: " + std::to_string(priestess_path_1.at(1)[2]),
    "Cost: " + std::to_string(priestess_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_priestess_1 = {
    "Current Attack Damage: " + std::to_string(priestess_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(priestess_path_1.at(2)[2]),
    "Cost: " + std::to_string(priestess_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_priestess_2 = {
    "Current Attack Damage: " + std::to_string(priestess_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(priestess_path_1.at(3)[2]),
    "Cost: " + std::to_string(priestess_path_1.at(2)[0])
};

// description cards for priestess path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_priestess_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(PRIESTESS)),
    "",
    "Upgraded Range: " + std::to_string(priestess_path_2.at(1)[2]),
    "Cost: " + std::to_string(priestess_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_priestess_1 = {
    "Current Range: " + std::to_string(priestess_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(priestess_path_2.at(2)[2]),
    "Cost: " + std::to_string(priestess_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_priestess_2 = {
    "Current Range: " + std::to_string(priestess_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(priestess_path_2.at(3)[2]),
    "Cost: " + std::to_string(priestess_path_2.at(2)[0])
};


// description cards for snowmachine path 1 upgrades
const std::vector<std::string> path_1_upgrade_button_snowmachine_0 = {
    "Current Attack Damage: " + std::to_string(unit_damage.at(SNOWMACHINE)),
    "",
    "Upgraded Attack Damage: " + std::to_string(snowmachine_path_1.at(1)[2]),
    "Cost: " + std::to_string(snowmachine_path_1.at(0)[0])
};

const std::vector<std::string> path_1_upgrade_button_snowmachine_1 = {
    "Current Attack Damage: " + std::to_string(snowmachine_path_1.at(1)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(snowmachine_path_1.at(2)[2]),
    "Cost: " + std::to_string(snowmachine_path_1.at(1)[0])
};

const std::vector<std::string> path_1_upgrade_button_snowmachine_2 = {
    "Current Attack Damage: " + std::to_string(snowmachine_path_1.at(2)[2]),
    "",
    "Upgraded Attack Damage: " + std::to_string(snowmachine_path_1.at(3)[2]),
    "Cost: " + std::to_string(snowmachine_path_1.at(2)[0])
};

// description cards for snowmachine path 2 upgrades
const std::vector<std::string> path_2_upgrade_button_snowmachine_0 = {
    "Current Range: " + std::to_string(unit_attack_range.at(SNOWMACHINE)),
    "",
    "Upgraded Range: " + std::to_string(snowmachine_path_2.at(1)[2]),
    "Cost: " + std::to_string(snowmachine_path_2.at(0)[0])
};

const std::vector<std::string> path_2_upgrade_button_snowmachine_1 = {
    "Current Range: " + std::to_string(snowmachine_path_2.at(1)[2]),
    "",
    "Upgraded Range: " + std::to_string(snowmachine_path_2.at(2)[2]),
    "Cost: " + std::to_string(snowmachine_path_2.at(1)[0])
};

const std::vector<std::string> path_2_upgrade_button_snowmachine_2 = {
    "Current Range: " + std::to_string(snowmachine_path_2.at(2)[2]),
    "",
    "Upgraded Range: " + std::to_string(snowmachine_path_2.at(3)[2]),
    "Cost: " + std::to_string(snowmachine_path_2.at(2)[0])
};