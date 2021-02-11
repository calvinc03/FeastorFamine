#pragma once


#include "entt.hpp"
#include "common.hpp"

//enum for ui buttons
enum Button { no_button_pressed, tower_button, green_house_button, stick_figure_button };

std::string button_to_string(int button); // breaks if enums change
Button UI_click_system();
void UI_highlight_system(vec2 mouse_pos);

struct UI_element {
	std::string tag;
};
struct UI_background
{
	static entt::entity createUI_background();
};

struct UI_button
{
	static entt::entity createUI_button(int pos, Button button);
};

