#pragma once

#include "common.hpp"
//#include "tiny_ecs.hpp"
#include "entt.hpp"
// Salmon enemy 
struct Turtle
{
	// Creates all the associated render resources and default transform
	static entt::entity createTurtle(vec2 position);
};
