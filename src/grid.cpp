#include "common.hpp"
#include "grid.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>


// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
GridSystem::GridSystem(int width, int height, float cell_size) :
        width(width),
        height(height),
        cell_size(cell_size),
        grid_vector(width, std::vector<int>(height, GRID_DEFAULT))
{
    // set L shaped path
    for (int y = 0; y < width; y++){
        std::cout<<grid_vector[0][y];
    }
    for (int x = 0; x < width; x++){
        std::cout<<grid_vector[x][height];
    }

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            std::cout<<grid_vector[x][y];
        }
    }
}

vec2 GridSystem::getPixelPosition(vec2 grid_coord) {
    return grid_coord * cell_size;
}

vec2 GridSystem::getGridPosition(vec2 pixel_coord) {
    return floor(pixel_coord/cell_size);
}

void GridSystem::setGridValue(vec2 grid_coord, int value) {
    if (grid_coord.x < 0 || grid_coord.y < 0 || grid_coord.x > width || grid_coord.y > height) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    grid_vector[grid_coord.x][grid_coord.y] = value;
}