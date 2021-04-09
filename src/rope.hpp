#pragma once

#include "entt.hpp"
#include "render.hpp"
#include "common.hpp"

struct RopeRig {
	std::vector<entt::entity> chain;
	static entt::entity createRope(vec2 start, int length);
	static entt::entity createRopePart(vec2 pos);
	static void update_rig(entt::entity rope_rig);
	static void update_physics(entt::entity rope_rig);
};