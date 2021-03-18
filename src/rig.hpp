#pragma once

#include "render.hpp"
#include "common.hpp"
#include "entt.hpp"
#include <vector>


struct Rig { 
	//add initialize rig -- for segment lengths
	std::vector< std::vector<entt::entity>> chains;
	static entt::entity createPart(entt::entity root_entity, std::string name, vec2 offset = { 0,0 }, vec2 origin = { 0,0 }, float angle = 0);
};

struct RigPart {
	entt::entity root_entity;
};

struct RigSystem {
	static void animate_rig_fk(entt::entity character);
	static void animate_rig_ik(entt::entity character);

	static void update_rig(entt::entity character);
	static void ik_solve(entt::entity character, vec2 goal, int chain_idx);
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


