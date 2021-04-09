#pragma once

#include "common.hpp"

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
    "Health: 30",
    "Speed: 3.5",
    "Attack: 10",
    "Path Color: Red",
    "Bounty: 10"
};

const std::vector<std::string> spring_boss_text = {
    "Health: 30",
    "Speed: 3.5",
    "Attack: 10",
    "Path Color: Orange",
    "Bounty: 10"
};

const std::vector<std::string> summer_boss_text = {
    "Health: 30",
    "Speed: 3.5",
    "Attack: 10",
    "Path Color: Yellow",
    "Bounty: 10"
};

const std::vector<std::string> fall_boss_text = {
    "Health: 30",
    "Speed: 3.5",
    "Attack: 10",
    "Path Color: Lime",
    "Bounty: 10"
};

const std::vector<std::string> winter_boss_text = {
    "Health: 30",
    "Speed: 3.5",
    "Attack: 10",
    "Path Color: Green",
    "Bounty: 10"
};

const std::vector<std::string> spider_boss_text = {
    "Health: 30",
    "Speed: 3.5",
    "Attack: 10",
    "Path Color: Purple",
    "Bounty: 10"
};


const std::map<int, std::vector<std::string>> wanted_descriptions = {
    {MOB, mob_text},
    {SPRING_BOSS, spring_boss_text },
    {SUMMER_BOSS, summer_boss_text},
    {FALL_BOSS, fall_boss_text},
    {WINTER_BOSS, winter_boss_text },
    {SPIDER, spider_boss_text },
    /*{FINAL_BOSS, "Dragon Path" },
    {FIREBALL_BOSS, "Fireball Path" },
    {BURROW_BOSS, "Mole Path" },
    {TALKY_BOI, "Helge Path" }*/
};