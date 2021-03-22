#pragma once

#include <vector>
#include <queue>

#include "common.hpp"
#include "entt.hpp"
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
        static bool is_walkable(GridMap& current_map, ivec2 coord)
        {
            if (is_inbounds(coord))
            {
                int occupancy = current_map.getNodeAtCoord(coord).occupancy;
                return occupancy == OCCUPANCY_VACANT || occupancy == OCCUPANCY_FOREST || occupancy == OCCUPANCY_VILLAGE;
            }
            return false;
        }
        static std::vector<ivec2> findPathBFS(GridMap& current_map,
                                              ivec2 start_coord = FOREST_COORD,
                                              ivec2 goal_coord = VILLAGE_COORD,
                                              bool is_valid(GridMap&, ivec2) = is_walkable,
                                              int neighbors = ALL_NBRS);
        static std::vector<ivec2> findPathAStar(GridMap& current_map, int monster_type,
                                                ivec2 start_coord = FOREST_COORD,
                                                ivec2 goal_coord = VILLAGE_COORD,
                                                bool is_valid(GridMap&, ivec2) = is_walkable,
                                                int neighbors = ALL_NBRS);
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
