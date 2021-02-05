#pragma once

#include "common.hpp"
//#include "tiny_ecs.hpp"
#include "entt.hpp"
// Salmon food
struct Fish
{
	// Creates all the associated render resources and default transform
	static entt::entity createFish(vec2 position);
};
