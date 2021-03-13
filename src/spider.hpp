#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include <vector>
// Creates all the associated render resources and default transform

struct Spider
{
	static entt::entity createSpider();
	static entt::entity createSpiderPart(std::string name, vec2 offset = {0,0}); //, vec2 offset, vec2 scale

	entt::entity body;
	entt::entity L_upper_leg;
	entt::entity L_lower_leg;
	entt::entity R_upper_leg;
	entt::entity R_lower_leg;

	//apply transforms hierarchially. should be called every world step. could be in another file like animation.cpp
	static void update_rigs();
};
