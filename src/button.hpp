#pragma once
#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

enum Button_texture {new_game_button, load_game_button, settings_button, exit_button, empty_button, back_button};

struct MenuButton {
	static entt::entity create_button(double x, double y, std::string button_name, Button_texture button_texture, std::string button_text = "");
};

std::string on_click_button(vec2 mouse_pos);

inline std::string menu_button_texture_path(const std::string& name) { return data_path() + "/textures/menu_button/" + name; };
