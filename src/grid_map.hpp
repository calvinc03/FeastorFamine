#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

// A simple physics system that moves rigid bodies and checks for collision

class GridMap
{
public:
    std::vector<std::vector<entt::entity>> node_entity_matrix = std::vector<std::vector<entt::entity>>(WINDOW_SIZE_IN_COORD.x, std::vector<entt::entity>(WINDOW_SIZE_IN_COORD.y));
    std::vector<std::vector<GridNode>> node_matrix = std::vector<std::vector<GridNode>>(WINDOW_SIZE_IN_COORD.x, std::vector<GridNode>(WINDOW_SIZE_IN_COORD.y));;

    static entt::entity createGridMap();

    entt::entity& getEntityAtCoord(ivec2 grid_coord);
    GridNode& getNodeAtCoord(ivec2 grid_coord);
    void setGridTerran(ivec2 grid_coord, int terran);
    void setGridOccupancy(ivec2 grid_coord, int occupancy);
    std::vector<entt::entity> getNodeEntitiesFromCoords(std::vector<ivec2>& grid_coords);
    std::vector<GridNode> getNodesFromCoords(std::vector<ivec2>& grid_coords);
};