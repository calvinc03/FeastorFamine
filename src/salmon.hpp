#pragma once

#include "common.hpp"
//#include "tiny_ecs.hpp"
#include "entt.hpp"
struct Salmon
{
	// Creates all the associated render resources and default transform
	static entt::entity createSalmon(vec2 pos);
};
