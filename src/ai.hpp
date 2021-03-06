#pragma once

#include <vector>
#include <queue>

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include "Observer.hpp"
#include "physics.hpp"
#include <BehaviorTree.cpp>
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem : public Observer
{
public:
	AISystem(PhysicsSystem* physics);
	~AISystem();

	void step(float elapsed_ms);
	void updateCollisions(entt::entity entity_i, entt::entity entity_j);

	struct MapAI {
        static std::vector<ivec2> findPathBFS(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2));
        static void setRandomWeatherTerrain(GridMap& map);
        static void setRandomPathTerran(GridMap& map, ivec2 start_coord, ivec2 end_coord, int terrain = TERRAIN_PAVEMENT);
	};

	struct MonstersAI {
		static std::shared_ptr<onCollisionSelector> createCollisionTree();
	};

private:
	// PhysicsSystem handle
	PhysicsSystem* physics;
};
