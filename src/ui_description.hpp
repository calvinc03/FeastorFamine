#include "entt.hpp"
#include "common.hpp"
#include <vector>
#include "text.hpp"
#include "ui.hpp"
#include "render.hpp"
#include "units/unit.hpp"

struct UI_unit_description_card
{
	static entt::entity createUI_unit_description_card(entt::entity button_entity);
};

const std::map<int, std::string> unit_name_str = {
    {hunter_button, "Hunter"},
    {watchtower_button, "Watchtower" },
    {green_house_button, "Greenhouse"},
    {wall_button, "Wall"}
};

const std::vector<std::string> hunter_description = {
    "Slow attack speed and short range.",
    "",
    "Attack: " + std::to_string(unit_damage.at(HUNTER)),
    "Attack speed:" + std::to_string(1000.f / unit_attack_interval_ms.at(HUNTER)),
    "Attack range:" + std::to_string(unit_attack_range.at(HUNTER))
};

const std::vector<std::string> watchtower_description = {
    "Fast attack speed and long range.",
    "",
    "Attack: " + std::to_string(watchtower_unit.damage),
    "Attack speed:" + std::to_string(1000.f / watchtower_unit.attack_interval_ms),
    "Attack range:" + std::to_string(watchtower_unit.attack_range)
};

const std::vector<std::string> greenhouse_description = {
    "Produce extra food each round."
};

const std::vector<std::string> wall_description = {
    "Blocks the path."
};

const std::map<int, std::vector<std::string>> unit_description_str = {
    {hunter_button, hunter_description },
    {watchtower_button, watchtower_description },
    {green_house_button, greenhouse_description},
    {wall_button, wall_description}
};