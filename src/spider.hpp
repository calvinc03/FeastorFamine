#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include <vector>


struct Spider
{
	static entt::entity createSpider();
};
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

struct MotionTransform {
	Transform transform;
	Motion motion;
};
struct Rig {
	//could make jagged 2D vector to store a series of kinematic chains, generalize to rig class. 
	entt::entity root;
	std::vector< std::vector<entt::entity>> chains;
	static entt::entity createPart(std::string name, vec2 offset = { 0,0 }, vec2 origin = { 0,0 }, float angle = 0);
	static void update_rigs();
};

//apply transforms hierarchially. should be called every world step. could be in another file like animation.cpp
struct Anim {
	static void animate();
};

Transform parent(Transform parent, Motion child_motion, Motion root_motion);

