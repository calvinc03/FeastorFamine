#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

// A simple physics system that moves rigid bodies and checks for collision

class GridMap
{
public:
    std::vector<std::vector<entt::entity>> node_matrix = {{}};

    static entt::entity createGridMap();

    static vec2 coordToPixel(ivec2 grid_coord);
    static ivec2 pixelToCoord(vec2 pixel_coord);
    static entt::entity getEntityAtCoord(GridMap current_map, ivec2 grid_coord);
    static GridNode& getNodeAtCoord(GridMap current_map, ivec2 grid_coord);
    void setGridType(GridMap current_map, vec2 grid_coord, int type);
    static std::vector<entt::entity> getNodesFromCoords(GridMap current_map, std::vector<ivec2>& grid_coords);
};