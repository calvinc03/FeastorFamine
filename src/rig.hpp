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
struct Timeline {    //TODO: refactor timeline
	int current_frame = 0;
	std::vector<Frame> frame;
};

struct Rig {
	static void animate_rigs();
	static void update_rigs(entt::entity camera);
	entt::entity root;
	std::vector< std::vector<entt::entity>> chains;
	static Transform parent(Transform parent, Motion child_motion, Motion root_motion);
	static entt::entity createPart( std::string name, vec2 offset = { 0,0 }, vec2 origin = { 0,0 }, float angle = 0);
};

