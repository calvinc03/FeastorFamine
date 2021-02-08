#include "common.hpp"
#include "grid_map.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>


// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
entt::entity GridMap::createGridMapEntt()
{
    auto entity = registry.create();
    // maintain a GridMap registry (we might want to have multiple maps later)
    auto& map = registry.emplace<GridMap>(entity);
    map.node_vector.resize(map.width);

    // fill node_vector with default type grid node
    for (int x = 0; x < map.width; x++){
        map.node_vector[x].resize(map.height);
        for (int y = 0; y < map.height; y++){
            map.node_vector[x][y] = GridNode::createGridNodeEntt(GRID_DEFAULT,vec2(x,y));
        }
    }
    return entity;
}

vec2 GridMap::gridCoordToPixel(vec2 grid_coord) {
    return grid_coord * (float)cell_size;
}

vec2 GridMap::pixelToGridCoord(vec2 pixel_coord) {
    return floor(pixel_coord / (float)cell_size);
}

void GridMap::setGridType(vec2 grid_coord, int type) {
    if (grid_coord.x < 0 || grid_coord.y < 0 || grid_coord.x > width || grid_coord.y > height) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    getNodeAtCoord(grid_coord).type = type;
}

entt::entity GridMap::getEntityAtCoord(vec2 grid_coord) {
    return node_vector[grid_coord.x][grid_coord.y];
}

GridNode& GridMap::getNodeAtCoord(vec2 grid_coord) {
    return registry.get<GridNode>(getEntityAtCoord(grid_coord));
}

void GridMap::setPath(std::vector<vec2>& grid_coords) {
    for(vec2 grid_coord : grid_coords) {
        registry.get<GridNode>(node_vector[grid_coord.x][grid_coord.y]).type = GRID_PATH;
    }
}