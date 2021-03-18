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
struct Root {
	entt::entity entity;
};

struct Rig {
	static void animate_rig(entt::entity character);
	static void update_rig(entt::entity character);
	static void ik_solve(entt::entity character, entt::entity camera, int chain_idx);

	std::vector< std::vector<entt::entity>> chains;
	static Transform parent(Transform parent, Motion child_motion, Motion root_motion);
	static entt::entity createPart(entt::entity root_entity, std::string name, vec2 offset = { 0,0 }, vec2 origin = { 0,0 }, float angle = 0);

	
};

