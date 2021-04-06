#include "entt.hpp"
#include "common.hpp"
#include <vector>
#include "text.hpp"
#include "ui.hpp"
#include "render.hpp"
#include "units/unit.hpp"
#include "config/unit_upgrade_description_config.hpp"

struct UI_unit_description_card
{
	static entt::entity createUI_unit_description_card(entt::entity button_entity);
};

const std::map<int, std::string> unit_name_str = {
    {hunter_button, "Hunter"},
    {watchtower_button, "Watchtower" },
    {green_house_button, "Greenhouse"},
    {wall_button, "Wall"},
    {exterminator_button, "Exterminator" },
    {robot_button, "Robot" },
    {priestess_button, "Priestess" },
    {snowmachine_button, "Snowmachine" }
};

const std::vector<std::string> hunter_description = {
    "Slow attack speed and short range.",
    "",
    "Attack: " + std::to_string(unit_damage.at(HUNTER)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(HUNTER)),
    "Attack range:" + std::to_string(unit_attack_range.at(HUNTER))
};

//const std::vector<std::string> watchtower_description = {
//    "Fast attack speed and long range.",
//    "",
//    "Attack: " + std::to_string(watchtower_unit.damage),
//    "Attack speed:" + std::to_string(1000.f / watchtower_unit.attack_interval_ms),
//    "Attack range:" + std::to_string(watchtower_unit.attack_range)
//};

const std::vector<std::string> greenhouse_description = {
    "Produce extra food each round."
};

const std::vector<std::string> wall_description = {
    "Blocks the path."
};

const std::vector<std::string> exterminator_description = {
    "Flamethrower to burn enemies down",
    "",
    "Attack: " + std::to_string(unit_damage.at(EXTERMINATOR)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(EXTERMINATOR)),
    "Attack range:" + std::to_string(unit_attack_range.at(EXTERMINATOR))
};

const std::vector<std::string> robot_description = {
    "Laserbeams. Enough said.",
    "",
    "Attack: " + std::to_string(unit_damage.at(ROBOT)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(ROBOT)),
    "Attack range:" + std::to_string(unit_attack_range.at(ROBOT))
};

const std::vector<std::string> priestess_description = {
    "Allies gain attack damage and speed",
    "",
    "Attack granted: " + std::to_string(unit_damage.at(PRIESTESS)),
    "Attack speed granted: " + std::to_string(unit_attack_interval_ms.at(PRIESTESS)) + "x",
    "Buff range:" + std::to_string(unit_attack_range.at(PRIESTESS))
};

const std::vector<std::string> snowmachine_description = {
    "Slow enemies down with snowballs",
    "",
    "Slow percentage: " + std::to_string(unit_damage.at(SNOWMACHINE)) + "%",
    "Attack speed: " + std::to_string(1000.f / unit_attack_interval_ms.at(SNOWMACHINE)),
    "Attack range:" + std::to_string(unit_attack_range.at(SNOWMACHINE))
};

const std::map<int, std::vector<std::string>> unit_description_str = {
    {hunter_button, hunter_description },
    //{watchtower_button, watchtower_description },
    {green_house_button, greenhouse_description},
    {wall_button, wall_description},
    {exterminator_button, exterminator_description },
    {robot_button, robot_description },
    {priestess_button, priestess_description},
    {snowmachine_button, snowmachine_description}
};

struct UI_selected_description_card
{
    static entt::entity createUI_selected_description_card(entt::entity button_entity);
};

const std::map<std::string, std::string> upgrade_short_descriptions = {
    {"path_1_upgrade_button_hunter_0", "Damage" },
    {"path_1_upgrade_button_hunter_1", "Damage" },
    {"path_1_upgrade_button_hunter_2", "Damage" },
    {"path_2_upgrade_button_hunter_0", "Range" },
    {"path_2_upgrade_button_hunter_1", "Range" },
    {"path_2_upgrade_button_hunter_2", "Range" },

    {"path_1_upgrade_button_greenhouse_0", "Food" },
    {"path_1_upgrade_button_greenhouse_1", "Food" },
    {"path_1_upgrade_button_greenhouse_2", "Food" },
    {"path_2_upgrade_button_greenhouse_0", "Food" },
    {"path_2_upgrade_button_greenhouse_1", "Food" },
    {"path_2_upgrade_button_greenhouse_2", "Food" },

    {"path_1_upgrade_button_exterminator_0", "Damage" },
    {"path_1_upgrade_button_exterminator_1", "Damage" },
    {"path_1_upgrade_button_exterminator_2", "Damage" },
    {"path_2_upgrade_button_exterminator_0", "Missile" },
    {"path_2_upgrade_button_exterminator_1", "Damage" },
    {"path_2_upgrade_button_exterminator_2", "Damage" },

    {"path_1_upgrade_button_robot_0", "Damage" },
    {"path_1_upgrade_button_robot_1", "Damage" },
    {"path_1_upgrade_button_robot_2", "Damage" },
    {"path_2_upgrade_button_robot_0", "Laser" },
    {"path_2_upgrade_button_robot_1", "Laser" },
    {"path_2_upgrade_button_robot_2", "Laser" },

    {"path_1_upgrade_button_priestess_0", "Stronger" },
    {"path_1_upgrade_button_priestess_1", "Stronger" },
    {"path_1_upgrade_button_priestess_2", "Stronger" },
    {"path_2_upgrade_button_priestess_0", "Faster" },
    {"path_2_upgrade_button_priestess_1", "Faster" },
    {"path_2_upgrade_button_priestess_2", "Faster" },

    {"path_1_upgrade_button_snowmachine_0", "Colder" },
    {"path_1_upgrade_button_snowmachine_1", "Colder" },
    {"path_1_upgrade_button_snowmachine_2", "Colder" },
    {"path_2_upgrade_button_snowmachine_0", "Icier" },
    {"path_2_upgrade_button_snowmachine_1", "Icier" },
    {"path_2_upgrade_button_snowmachine_2", "Icier" },

    {"path_1_upgrade_button_wall_0", "Tank" },
    {"path_1_upgrade_button_wall_1", "Tank" },
    {"path_1_upgrade_button_wall_2", "Tank" },
    {"path_2_upgrade_button_wall_0", "Tank" },
    {"path_2_upgrade_button_wall_1", "Tank" },
    {"path_2_upgrade_button_wall_2", "Tank" }
};

const std::map<std::string, std::string> selected_upgrade_title = {
    {"path_1_upgrade_button_hunter_0", "Damage Increase" },
    {"path_1_upgrade_button_hunter_1", "Damage Increase" },
    {"path_1_upgrade_button_hunter_2", "Damage Increase" },
    {"path_2_upgrade_button_hunter_0", "Range Increase" },
    {"path_2_upgrade_button_hunter_1", "Range Increase" },
    {"path_2_upgrade_button_hunter_2", "Range Increase" },

    {"path_1_upgrade_button_greenhouse_0", "Production Increase" },
    {"path_1_upgrade_button_greenhouse_1", "Production Increase" },
    {"path_1_upgrade_button_greenhouse_2", "Production Increase" },
    {"path_2_upgrade_button_greenhouse_0", "Production Increase" },
    {"path_2_upgrade_button_greenhouse_1", "Production Increase" },
    {"path_2_upgrade_button_greenhouse_2", "Production Increase" },

    {"path_1_upgrade_button_exterminator_0", "Damage Increase" },
    {"path_1_upgrade_button_exterminator_1", "Damage Increase" },
    {"path_1_upgrade_button_exterminator_2", "Damage Increase" },
    {"path_2_upgrade_button_exterminator_0", "Weapon Change" },
    {"path_2_upgrade_button_exterminator_1", "Weapon Change" },
    {"path_2_upgrade_button_exterminator_2", "Weapon Change" },

    {"path_1_upgrade_button_robot_0", "Damage Increase" },
    {"path_1_upgrade_button_robot_1", "Damage Increase" },
    {"path_1_upgrade_button_robot_2", "Damage Increase" },
    {"path_2_upgrade_button_robot_0", "More Lasers!" },
    {"path_2_upgrade_button_robot_1", "More Lasers!" },
    {"path_2_upgrade_button_robot_2", "More Lasers!" },

    {"path_1_upgrade_button_priestess_0", "Damage Increase" },
    {"path_1_upgrade_button_priestess_1", "Damage Increase" },
    {"path_1_upgrade_button_priestess_2", "Damage Increase" },
    {"path_2_upgrade_button_priestess_0", "Attack Speed Increase" },
    {"path_2_upgrade_button_priestess_1", "Attack Speed Increase" },
    {"path_2_upgrade_button_priestess_2", "Attack Speed Increase" },

    {"path_1_upgrade_button_snowmachine_0", "More Snowballs!" },
    {"path_1_upgrade_button_snowmachine_1", "More Snowballs!" },
    {"path_1_upgrade_button_snowmachine_2", "More Snowballs!" },
    {"path_2_upgrade_button_snowmachine_0", "Range Increase" },
    {"path_2_upgrade_button_snowmachine_1", "Range Increase" },
    {"path_2_upgrade_button_snowmachine_2", "Range Increase" }, 

    {"path_1_upgrade_button_wall_0", "Damage Increase" },
    {"path_1_upgrade_button_wall_1", "Damage Increase" },
    {"path_1_upgrade_button_wall_2", "Damage Increase" },
    {"path_2_upgrade_button_wall_0", "Attack Speed Increase" },
    {"path_2_upgrade_button_wall_1", "Attack Speed Increase" },
    {"path_2_upgrade_button_wall_2", "Attack Speed Increase" }
};

const std::map<std::string, std::vector<std::string>> selected_upgrade_descriptions = {
    {"path_1_upgrade_button_hunter_0", path_1_upgrade_button_hunter_0 },
    {"path_1_upgrade_button_hunter_1", path_1_upgrade_button_hunter_1 },
    {"path_1_upgrade_button_hunter_2", path_1_upgrade_button_hunter_2 },
    {"path_2_upgrade_button_hunter_0", path_2_upgrade_button_hunter_0 },
    {"path_2_upgrade_button_hunter_1", path_2_upgrade_button_hunter_1 },
    {"path_2_upgrade_button_hunter_2", path_2_upgrade_button_hunter_2 },

    {"path_1_upgrade_button_greenhouse_0", path_1_upgrade_button_greenhouse_0 },
    {"path_1_upgrade_button_greenhouse_1", path_1_upgrade_button_greenhouse_1 },
    {"path_1_upgrade_button_greenhouse_2", path_1_upgrade_button_greenhouse_2 },
    {"path_2_upgrade_button_greenhouse_0", path_2_upgrade_button_greenhouse_0 },
    {"path_2_upgrade_button_greenhouse_1", path_2_upgrade_button_greenhouse_1 },
    {"path_2_upgrade_button_greenhouse_2", path_2_upgrade_button_greenhouse_2 },

    {"path_1_upgrade_button_exterminator_0", path_1_upgrade_button_exterminator_0 },
    {"path_1_upgrade_button_exterminator_1", path_1_upgrade_button_exterminator_1 },
    {"path_1_upgrade_button_exterminator_2", path_1_upgrade_button_exterminator_2 },
    {"path_2_upgrade_button_exterminator_0", path_2_upgrade_button_exterminator_0 },
    {"path_2_upgrade_button_exterminator_1", path_2_upgrade_button_exterminator_1 },
    {"path_2_upgrade_button_exterminator_2", path_2_upgrade_button_exterminator_2 },

    {"path_1_upgrade_button_robot_0", path_1_upgrade_button_robot_0 },
    {"path_1_upgrade_button_robot_1", path_1_upgrade_button_robot_1 },
    {"path_1_upgrade_button_robot_2", path_1_upgrade_button_robot_2 },
    {"path_2_upgrade_button_robot_0", path_2_upgrade_button_robot_0 },
    {"path_2_upgrade_button_robot_1", path_2_upgrade_button_robot_1 },
    {"path_2_upgrade_button_robot_2", path_2_upgrade_button_robot_2 },

    {"path_1_upgrade_button_priestess_0", path_1_upgrade_button_priestess_0 },
    {"path_1_upgrade_button_priestess_1", path_1_upgrade_button_priestess_1 },
    {"path_1_upgrade_button_priestess_2", path_1_upgrade_button_priestess_2 },
    {"path_2_upgrade_button_priestess_0", path_2_upgrade_button_priestess_0 },
    {"path_2_upgrade_button_priestess_1", path_2_upgrade_button_priestess_1 },
    {"path_2_upgrade_button_priestess_2", path_2_upgrade_button_priestess_2 },

    {"path_1_upgrade_button_wall_0", path_1_upgrade_button_wall_0 },
    {"path_1_upgrade_button_wall_1", path_1_upgrade_button_wall_1 },
    {"path_1_upgrade_button_wall_2", path_1_upgrade_button_wall_2 },
    {"path_2_upgrade_button_wall_0", path_2_upgrade_button_wall_0 },
    {"path_2_upgrade_button_wall_1", path_2_upgrade_button_wall_1 },
    {"path_2_upgrade_button_wall_2", path_2_upgrade_button_wall_2 }
};