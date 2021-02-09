
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "common.hpp"
#include "world.hpp"

#include "entt.hpp"
//#include "tiny_ecs.hpp"

#include "render.hpp"
#include "physics.hpp"

#include "debug.hpp"

using Clock = std::chrono::high_resolution_clock;
// Note, here the window will show a width x height part of the game world, measured in px. 
// You could also define a window to show 1.5 x 1 part of your game world, where the aspect ratio depends on your window size.

struct Description {
	std::string name;
	Description(const char* str) : name(str) {};
};

// Entry point
int main()
{
	
	//entt::registry reg;
	

	// Initialize the main systems
	WorldSystem world(WINDOW_SIZE_IN_PX);
	RenderSystem renderer(*world.window);
	PhysicsSystem physics;
	//AISystem ai;

	// Set all states to default
	world.restart();
	auto t = Clock::now();
	// Variable timestep loop
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;

		DebugSystem::clearDebugComponents();
		//ai.step(elapsed_ms);
		world.step(elapsed_ms);
		physics.step(elapsed_ms);
		world.handle_collisions();

		renderer.draw();
	}
	//
	return EXIT_SUCCESS;
}
