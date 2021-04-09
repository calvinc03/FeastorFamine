#pragma once

#include "entt.hpp"
#include "render.hpp"
#include "common.hpp"

struct RopeRig {
	static entt::entity createRope(vec2 start, int length);
	static entt::entity createRopePart(vec2 pos);
};