#pragma once
#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

enum Button_texture {new_game_button, load_game_button, settings_button, exit_button};

struct MenuButton {
	static entt::entity create_button(double x, double y, std::string button_name, Button_texture button_texture);
};

std::string on_click(vec2 mouse_pos);