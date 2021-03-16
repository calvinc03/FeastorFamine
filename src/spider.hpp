#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include <vector>

struct Frame {
	std::vector<float> angle;

	Frame(std::vector<float> angles) {
		angle = angles;
	}
};

struct Timeline {
	int current_frame = 0;
	std::vector<Frame> frame;
};

struct Rig {
	//could make jagged 2D vector to store a series of kinematic chains, generalize to rig class. 
	std::vector<entt::entity> parts;
};
struct Spider
{
	static entt::entity createSpider();
	static entt::entity createSpiderPart(std::string name, vec2 offset = { 0,0 }, vec2 origin = {0,0}, float angle = 0);



	//apply transforms hierarchially. should be called every world step. could be in another file like animation.cpp
	static void update_rigs();
	static void animate();
};

Transform parent(Transform parent, Motion child_motion, Motion root_motion);

