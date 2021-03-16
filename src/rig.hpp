#pragma once

#include "render.hpp"
#include "common.hpp"
#include "entt.hpp"
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
	static void animate_rigs();
	entt::entity root;
	std::vector< std::vector<entt::entity>> chains;
	static entt::entity createPart(std::string name, vec2 offset = { 0,0 }, vec2 origin = { 0,0 }, float angle = 0);
	static void update_rigs();
	static Transform parent(Transform parent, Motion child_motion, Motion root_motion);
};

