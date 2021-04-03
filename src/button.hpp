#pragma once
#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

struct MenuButton {
	static entt::entity create_button(float x, float y, MenuButtonType button_type, std::string button_text = "");
	MenuButtonType button_type;
};

MenuButtonType on_click_button(vec2 mouse_pos);

inline std::string menu_button_texture_path(const std::string& name) { return data_path() + "/textures/menu_button/" + name; };
