#pragma once


#include "common.hpp"

// Data structure for pebble-specific information
namespace HealthSystem {
	void createHealthBar(vec2 position, int health, int max_health);

	void updateHealthComponents(float elapsed_ms);


};
