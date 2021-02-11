#pragma once


#include "entt.hpp"
#include "common.hpp"


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

