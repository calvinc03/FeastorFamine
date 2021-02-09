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
    map.node_entt_matrix.resize(WINDOW_SIZE_IN_COORD.x);

    // fill node_entt_matrix with default type grid node
    for (int x = 0; x < WINDOW_SIZE_IN_COORD.x; x++){
        map.node_entt_matrix[x].resize(WINDOW_SIZE_IN_COORD.y);
        for (int y = 0; y < WINDOW_SIZE_IN_COORD.y; y++){
            map.node_entt_matrix[x][y] = GridNode::createGridNodeEntt(GRID_DEFAULT, vec2(x, y));
        }
    }
    return entity;
}

ivec2 GridMap::coordToPixel(ivec2 grid_coord) {
    return grid_coord * GRID_CELL_SIZE;
}

ivec2 GridMap::pixelToCoord(ivec2 pixel_coord) {
    return pixel_coord / GRID_CELL_SIZE;
}

void GridMap::setGridType(std::vector<std::vector<entt::entity>>& node_matrix, vec2 grid_coord, int type) {
    if (grid_coord.x < 0 || grid_coord.y < 0 || grid_coord.x > WINDOW_SIZE_IN_COORD.x || grid_coord.y > WINDOW_SIZE_IN_COORD.y) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    getNodeAtCoord(node_matrix, grid_coord).type = type;
}

entt::entity GridMap::getEntityAtCoord(std::vector<std::vector<entt::entity>>& node_matrix, ivec2 grid_coord) {
    return node_matrix[grid_coord.x][grid_coord.y];
}

GridNode& GridMap::getNodeAtCoord(std::vector<std::vector<entt::entity>>& node_matrix, vec2 grid_coord) {
    return registry.get<GridNode>(getEntityAtCoord(node_matrix, grid_coord));
}

void GridMap::setPathFromCoords(std::vector<vec2>& grid_coords) {
    path_entt.clear();
    path_entt.resize(grid_coords.size());
    for(vec2 grid_coord : grid_coords) {
        auto& node = node_entt_matrix[grid_coord.x][grid_coord.y];
        registry.get<GridNode>(node).type = GRID_PATH;
        path_entt.push_back(node);
    }
}