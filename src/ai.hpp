#pragma once

#include <vector>
#include <queue>

#include "common.hpp"
#include "config/ai_config.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
#include "Observer.hpp"
#include "physics.hpp"
#include <BehaviorTree.cpp>

class AISystem : public Observer
{
public:
	AISystem(PhysicsSystem* physics);
	~AISystem();

	void step(float elapsed_ms);
	void updateProjectileMonsterCollision(entt::entity projectile, entt::entity monster);

	struct MapAI {
        static std::vector<ivec2> findPathBFS(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2),
                                              const std::vector<ivec2>& neighbors = all_neighbors);
        static std::vector<ivec2> findPathAStar(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2),
                                                const std::vector<ivec2>& neighbors = all_neighbors);
        static void setRandomMapWeatherTerrain(GridMap& map);
        static void setRandomGridsWeatherTerrain(GridMap& map, int max_rerolls);
        static void setRandomMapPathTerran(GridMap& map, ivec2 start_coord, ivec2 end_coord, int terrain = TERRAIN_PAVEMENT);
	};

	struct MonstersAI {
		static std::shared_ptr<BTSelector> createBehaviorTree();
	};

private:
	// PhysicsSystem handle
	PhysicsSystem* physics;
};
