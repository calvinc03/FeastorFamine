#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class GridMap
{
public:
    int width = WINDOW_SIZE_IN_PX.x / GRID_CELL_SIZE;
    int height = WINDOW_SIZE_IN_PX.y / GRID_CELL_SIZE;
    int cell_size = GRID_CELL_SIZE;

    entt::entity createGridMap(std::vector<vec2> path_coords);

    std::vector<std::vector<entt::entity>> node_vector;

    vec2 gridToPixelCoord(vec2 grid_coord);
    vec2 pixelToGridCoord(vec2 pixel_coord);
    entt::entity getEntityAtCoord(vec2 grid_coord);
    GridNode& getNodeAtCoord(vec2 grid_coord);
    void setGridType(vec2 grid_coord, int type);
    void setPath(std::vector<vec2>& grid_coords);
};

