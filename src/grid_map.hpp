#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class GridMap
{
public:
    int width;
    int height;
    int cell_size;

    GridMap(int width, int height, int cell_size){
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    }

    entt::entity createGridMap(std::vector<vec2> path_coords);

    std::vector<std::vector<entt::entity>> node_vector;

    vec2 gridToPixelCoord(vec2 grid_coord);
    vec2 pixelToGridCoord(vec2 pixel_coord);
    entt::entity getEntityAtCoord(vec2 grid_coord);
    GridNode& getNodeAtCoord(vec2 grid_coord);
    void setGridType(vec2 grid_coord, int type);
    void setPath(std::vector<vec2>& grid_coords);
};

