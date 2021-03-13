#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include <vector>

struct Spider
{
	static entt::entity createSpider();
	static entt::entity createSpiderPart(std::string name, vec2 offset = {0,0}); //, vec2 offset, vec2 scale

	//could make jagged 2D vector to store a series of kinematic chains, generalize to rig class. 
	entt::entity body;
	entt::entity L_upper_leg;
	entt::entity L_lower_leg;
	entt::entity R_upper_leg;
	entt::entity R_lower_leg;

	//apply transforms hierarchially. should be called every world step. could be in another file like animation.cpp
	static void update_rigs();
};

Transform parent(Transform parent, Motion child_motion, vec2 origin_offset);

