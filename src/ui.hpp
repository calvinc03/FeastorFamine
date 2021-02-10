#pragma once


#include "entt.hpp"
#include "common.hpp"

struct UI
{
	static entt::entity createUI();
	
};



struct UI_element
{
	static entt::entity createUI_button(int pos, Button button);
};

