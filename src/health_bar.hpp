#pragma once


#include "common.hpp"

// Data structure for pebble-specific information
namespace HealthSystem {

	// draw a red line for debugging purposes
	void createHealthBar(vec2 *position, int *health);

	void updateHealthComponents(float elapsed_ms);


};
