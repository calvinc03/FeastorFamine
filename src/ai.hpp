#pragma once

#include <vector>

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);

//    struct Pathfinding {
//        GridMap grid = GridMap(0, 0, 0, std::vector<vec2>());
//        std::vector<entt::entity> open_list = std::vector<entt::entity>();
//        std::vector<entt::entity> closed_list = std::vector<entt::entity>();
//
//        Pathfinding(GridMap &grid) {
//            this->grid = grid;
//        }
//
//        // TODO: A* search
//        std::vector<entt::entity> findPath(vec2 start_coord, vec2 end_coord){
//            entt::entity start_node = grid.getEntityAtCoord(start_coord);
//            open_list.push_back(start_node);
//
//            for (int x = 0; x < grid.width; x++){
//                for (int y = 0; y < grid.height; y++) {
//
//                }
//            }
//        }
//    };
};
