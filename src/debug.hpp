#pragma once

#include "common.hpp"

// Data structure for pebble-specific information
namespace DebugSystem {
	extern bool in_debug_mode;

	// draw a red line for debugging purposes
	void createLine(vec2 position, vec2 size);

	// !!! TODO A2: implement debugging of bounding boxes
	void createBox(vec2 position, vec2 size);
	void createDirectedLine(vec2 position1, vec2 position2, float width);
	void createPoint(vec2 position, float size);
	void display_rig_vertices(entt::entity character, entt::entity camera);

	void clearDebugComponents();
};
