#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

// A simple physics system that moves rigid bodies and checks for collision

class GridMap
{
public:
    int cell_size = GRID_CELL_SIZE;
    int width = WINDOW_SIZE_IN_PX.x / GRID_CELL_SIZE;
    int height = WINDOW_SIZE_IN_PX.y / GRID_CELL_SIZE;

    std::vector<std::vector<entt::entity>> node_vector = {{}};

    static entt::entity createGridMapEntt();

    vec2 gridCoordToPixel(vec2 grid_coord);
    vec2 pixelToGridCoord(vec2 pixel_coord);
    entt::entity getEntityAtCoord(vec2 grid_coord);
    GridNode& getNodeAtCoord(vec2 grid_coord);
    void setGridType(vec2 grid_coord, int type);
    void setPath(std::vector<vec2>& grid_coords);
};

