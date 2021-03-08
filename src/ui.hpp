#pragma once


#include "entt.hpp"
#include "common.hpp"
#include <vector>
#include "text.hpp"
//enum for ui buttons
enum Button { no_button_pressed, tower_button, green_house_button, stick_figure_button, wall_button, upgrade_button, save_button, sell_button};

std::string button_to_string(int button); // breaks if enums change
Button UI_click_system();
void UI_highlight_system(vec2 mouse_pos);

entt::entity create_ui_text(vec2 position, std::string content);

struct UI_element {
	std::string tag;
	vec2 position;
	vec2 scale;
};

struct UI_background
{
	static entt::entity createUI_background();
};

struct UI_button
{
	static entt::entity createUI_button(int pos, Button button, size_t cost = 0, std::string tag = "default_ui_button_tag", bool show = true);
};

inline std::string ui_texture_path(const std::string& name) { return data_path() + "/textures/ui/" + name; };
