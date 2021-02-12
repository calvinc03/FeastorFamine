#include "grid_map.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>


// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
entt::entity GridMap::createGridMap()
{
    auto entity = registry.create();
    // maintain a GridMap registry (we might want to have multiple maps later)
    auto& map = registry.emplace<GridMap>(entity);
    map.node_matrix.resize(WINDOW_SIZE_IN_COORD.x);

    // fill node_matrix with default type grid node
    for (int x = 0; x < WINDOW_SIZE_IN_COORD.x; x++){
        map.node_matrix[x].resize(WINDOW_SIZE_IN_COORD.y);
        for (int y = 0; y < WINDOW_SIZE_IN_COORD.y; y++){
            int type = GRID_VACANT;
            map.node_matrix[x][y] = GridNode::createGridNode(type, vec2(x, y));
        }
    }
    return entity;
}

// add offset so that pixel is centered on grid
vec2 GridMap::coordToPixel(ivec2 grid_coord) {
    return grid_coord * GRID_CELL_SIZE + GRID_OFFSET;
}

ivec2 GridMap::pixelToCoord(vec2 pixel_coord) {
    return (ivec2)pixel_coord / GRID_CELL_SIZE;
}

void GridMap::setGridType(GridMap current_map, vec2 grid_coord, int type) {
    if (grid_coord.x < 0 || grid_coord.y < 0 || grid_coord.x > WINDOW_SIZE_IN_COORD.x || grid_coord.y > WINDOW_SIZE_IN_COORD.y) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    getNodeAtCoord(current_map, grid_coord).type = type;
}

entt::entity GridMap::getEntityAtCoord(GridMap current_map, ivec2 grid_coord) {
    return current_map.node_matrix[grid_coord.x][grid_coord.y];
}

GridNode& GridMap::getNodeAtCoord(GridMap current_map, ivec2 grid_coord) {
    return registry.get<GridNode>(getEntityAtCoord(current_map, grid_coord));
}

std::vector<entt::entity> GridMap::getNodesFromCoords(GridMap current_map, std::vector<ivec2>& grid_coords){
    std::vector<entt::entity> path_nodes = {};
    for(vec2 grid_coord : grid_coords) {
        entt::entity node = current_map.node_matrix[grid_coord.x][grid_coord.y];
        path_nodes.push_back(node);
    }
    return path_nodes;
}