#pragma once
// Created by Ray 

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

enum Menu_texture { title_screen, controls, default_menu, help_menu, pause_menu};

struct Menu
{
	std::vector<entt::entity> buttons = {};
	static entt::entity createMenu(float x, float y, std::string menu_name, Menu_texture texture, int layer = 0, vec2 scale = { 1.f, 1.f });
};

inline std::string menu_texture_path(const std::string& name) { return data_path() + "/textures/menu/" + name; };

struct MenuText
{
	std::string menu_name;
};