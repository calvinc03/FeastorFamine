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
        static std::vector<ivec2> find_path_BFS(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2));
	};

	struct MonstersAI {
		static std::shared_ptr<onCollisionSelector> create_collision_tree();
	};

private:
	// PhysicsSystem handle
	PhysicsSystem* physics;
};
