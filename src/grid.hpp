#pragma once

#include "common.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class GridSystem
{
public:
    GridSystem(int width, int height, float cell_size);
private:
    int width;
    int height;
    float cell_size;
    std::vector<std::vector<int>> grid_vector;

    vec2 getPixelPosition(vec2 grid_coord);
    vec2 getGridPosition(vec2 pixel_coord);
    void setGridValue(vec2 grid_coord, int value);
};
