#pragma once
// Created by Ray 

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

enum Menu_texture { settings, default_menu };

struct Menu
{
	std::vector<entt::entity> buttons = {};
	static entt::entity createMenu(double x, double y, std::string menu_name, Menu_texture texture, int layer = 0, vec2 scale = { 1.f, 1.f });
};
