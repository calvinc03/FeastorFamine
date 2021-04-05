#pragma once

#include "render.hpp"
#include "common.hpp"
#include "entt.hpp"
#include <vector>

struct Chain {
	entt::entity root;
	std::vector<entt::entity> chain_vector;
public:
	Chain(entt::entity root_, std::vector<entt::entity> chain_vector_) {
		root = root_;
		chain_vector = chain_vector_;
	}
};
struct Rig { 
	std::vector< Chain> chains;


	static entt::entity createPart(entt::entity root_entity, std::string name, vec2 offset = { 0,0 }, vec2 origin = { 0,0 }, float angle = 0);
	static void delete_rig(entt::entity character);
};

struct RigPart {
	entt::entity root_entity; //needed for each part to reference the Motion component 
};

struct RigSystem {
	static void animate_rig_fk(entt::entity character, float elapsed_ms);
	static void animate_rig_ik(entt::entity character, float elapsed_ms);
	
	static void update_rig(entt::entity character);
	static void ik_solve(entt::entity character, vec2 goal, int chain_idx);
};

struct KeyFrames_FK { // per joint keyframes -- might change this, kind of weird!
	// timestamp, angle
	std::map<float, float> data;
};

struct KeyFrames_IK {
	// timestamp, angle
	std::vector<std::map<float, vec2>> data;
};

struct Animations { // using string as key temporarily. 
	int anim_state = 0;
	std::map<int,KeyFrames_IK> anims;
};

struct Timeline { // per rig time
	float current_time = 0; 
	bool loop = true;
};


