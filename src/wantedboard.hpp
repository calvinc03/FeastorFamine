#pragma once

#include "common.hpp"
#include "config/monster_config.hpp"

struct WantedBoard {
	static entt::entity createWantedBoard();
    static void updateWantedEntries(entt::entity wanted_board, std::vector<int> current_round_monster_types);
    static void updateWantedBoardDisplay(entt::entity wanted_board_entity, bool show);
    entt::entity wanted_title;
    std::vector<entt::entity> wanted_entries;
};

struct WantedTitle {
    static entt::entity createWantedSign();
};

struct WantedEntry {
	static entt::entity createWantedEntry(vec2 position, int monster_type);
	static void createMonsterIcon(entt::entity entry, vec2 position, vec2 scale, int monster_type);
	static void createMonsterText(entt::entity entry, vec2 position, vec2 scale, int monster_type);
	std::vector<entt::entity> monster_info;
};

inline std::string wanted_board_texture_path(const std::string& name) { return data_path() + "/textures/wanted_board/" + name; };


const std::vector<std::string> mob_text = {
    "Health: " + std::to_string(monster_health.at(MOB)),
    "Speed: " + std::to_string((int) length(monster_velocities.at(MOB)) * GRID_CELL_SIZE),
    "Attack: " + std::to_string(monster_damage.at(MOB)),
    "Path Color: RED",
    "Bounty: " + std::to_string(monster_reward.at(MOB))
};

const std::vector<std::string> spring_boss_text = {
    "Health: " + std::to_string(monster_health.at(SPRING_BOSS)),
    "Speed: " + std::to_string((int) length(monster_velocities.at(SPRING_BOSS)) * GRID_CELL_SIZE),
    "Attack: " + std::to_string(monster_damage.at(SPRING_BOSS)),
    "Path Color: ORANGE",
    "Bounty: " + std::to_string(monster_reward.at(SPRING_BOSS))
};

const std::vector<std::string> summer_boss_text = {
    "Health: " + std::to_string(monster_health.at(SUMMER_BOSS)),
    "Speed: " + std::to_string((int) length(monster_velocities.at(SUMMER_BOSS)) * GRID_CELL_SIZE),
    "Attack: " + std::to_string(monster_damage.at(SUMMER_BOSS)),
    "Path Color: YELLOW",
    "Bounty: " + std::to_string(monster_reward.at(SUMMER_BOSS))
};

const std::vector<std::string> fall_boss_text = {
    "Health: " + std::to_string(monster_health.at(FALL_BOSS)),
    "Speed: " + std::to_string((int) length(monster_velocities.at(FALL_BOSS)) * GRID_CELL_SIZE),
    "Attack: " + std::to_string(monster_damage.at(FALL_BOSS)),
    "Path Color: LIME",
    "Bounty: " + std::to_string((int) monster_reward.at(FALL_BOSS))
};

const std::vector<std::string> winter_boss_text = {
    "Health: " + std::to_string(monster_health.at(WINTER_BOSS)),
    "Speed: " + std::to_string((int) length(monster_velocities.at(WINTER_BOSS)) * GRID_CELL_SIZE),
    "Attack: " + std::to_string(monster_damage.at(WINTER_BOSS)),
    "Path Color: GREEN",
    "Bounty: " + std::to_string(monster_reward.at(WINTER_BOSS))
};

const std::vector<std::string> spider_text = {
    "Health: " + std::to_string(monster_health.at(SPIDER)),
    "Speed: " + std::to_string((int) length(monster_velocities.at(SPIDER)) * GRID_CELL_SIZE),
    "Attack: " + std::to_string(monster_damage.at(SPIDER)),
    "Path Color: PURPLE",
    "Bounty: " + std::to_string(monster_reward.at(SPIDER))
};


const std::map<int, std::vector<std::string>> wanted_descriptions = {
    {MOB, mob_text},
    {SPRING_BOSS, spring_boss_text },
    {SUMMER_BOSS, summer_boss_text},
    {FALL_BOSS, fall_boss_text},
    {WINTER_BOSS, winter_boss_text },
    {SPIDER, spider_text },
    /*{FINAL_BOSS, "Dragon Path" },
    {FIREBALL_BOSS, "Fireball Path" },
    {BURROW_BOSS, "Mole Path" },
    {TALKY_BOI, "Helge Path" }*/
};