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

    // fill node_entity_matrix with default type grid node
    for (int x = 0; x < WINDOW_SIZE_IN_COORD.x; x++){
        for (int y = 0; y < WINDOW_SIZE_IN_COORD.y; y++){
            int terran = GRID_GRASS;
            map.node_entity_matrix[x][y] = GridNode::createGridNode(terran, vec2(x, y));
            map.node_matrix[x][y] = registry.get<GridNode>(map.node_entity_matrix[x][y]);
        }
    }
    return entity;
}

void GridMap::setGridTerran(ivec2 grid_coord, int terran) {
    if (!is_inbounds(grid_coord)) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    auto& node = getNodeAtCoord(grid_coord);
    auto& entity = getEntityAtCoord(grid_coord);
    node.setTerran(entity, terran);
}

void GridMap::setGridOccupancy(ivec2 grid_coord, int occupancy) {
    if (!is_inbounds(grid_coord)) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    auto& node = getNodeAtCoord(grid_coord);
    node.setOccupancy(occupancy);
}

GridNode& GridMap::getNodeAtCoord(ivec2 grid_coord) {
    return this->node_matrix[grid_coord.x][grid_coord.y];
}

entt::entity& GridMap::getEntityAtCoord(ivec2 grid_coord) {
    return this->node_entity_matrix[grid_coord.x][grid_coord.y];
}

std::vector<entt::entity> GridMap::getNodeEntitiesFromCoords(std::vector<ivec2>& grid_coords){
    std::vector<entt::entity>  path_nodes;
    for(vec2 grid_coord : grid_coords) {
        entt::entity node = this->node_entity_matrix[grid_coord.x][grid_coord.y];
        path_nodes.push_back(node);
    }
    return path_nodes;
}

std::vector<GridNode> GridMap::getNodesFromCoords(std::vector<ivec2>& grid_coords){
    std::vector<GridNode> path_nodes;
    for(vec2 grid_coord : grid_coords) {
        GridNode node = this->node_matrix[grid_coord.x][grid_coord.y];
        path_nodes.push_back(node);
    }
    return path_nodes;
}