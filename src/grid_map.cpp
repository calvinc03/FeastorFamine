#include "grid_map.hpp"
#include "ai.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

// no diagonal version
// with diagonals
std::vector<ivec2> nbr_path = {ivec2(0,-1),ivec2(-1,0),
                               ivec2(1,0), ivec2(0,1)};

int get_random_index () {
    float num = uniform_dist(rng);
    // 10% chance for UP and LEFT
    if (num < 0.1) {
        return 0;
    } else if (num < 0.2) {
        return 1;
    }
    // 40% chance for DOWN and RIGHT
    else if (num < 0.6) {
        return 2;
    }
    return 3;
}

// set path
bool is_valid_terrain_path(GridMap& current_map, ivec2 coord)
{
    if (is_inbounds(coord)) {
        int terrain = current_map.node_matrix[coord.x][coord.y].terrain;
        int occupancy = current_map.node_matrix[coord.x][coord.y].occupancy;
        return terrain != TERRAIN_PAVEMENT && occupancy == OCCUPANCY_VACANT;
    }
    return false;
}

ivec2 get_random_neighbor(GridMap& map, ivec2 current_coord, ivec2 end_coord) {
    bool visited[4] = {false, false, false, false};
    int count = 0;

    int index = get_random_index();
    while (count < 4) {
        // randomly roll a direction until we get an unvisited direction
        while (visited[index]) {
            index = get_random_index();;
        }
        count++;
        // return this neighbor if it's in bounds and has an open path to village (don't want to block itself in)
        ivec2 nbr_coord = current_coord + nbr_path.at(index);
        visited[index] = true;
        if (nbr_coord == end_coord ||
                (is_inbounds(nbr_coord) && map.node_matrix[nbr_coord.x][nbr_coord.y].terrain != TERRAIN_PAVEMENT
                && !AISystem::MapAI::find_path_BFS(map, nbr_coord, VILLAGE_COORD, is_valid_terrain_path).empty())) {
            return nbr_coord;
        }
    }
    // cannot find a valid neighbor; should NOT happen
    assert(false);
    return ivec2(0, 0);
}

void set_random_terrain_path(GridMap& map, ivec2 start_coord, ivec2 end_coord, int terrain = TERRAIN_PAVEMENT) {
    ivec2 rand_nbr = get_random_neighbor(map, start_coord, end_coord);
    map.setGridterrain(start_coord, terrain);
    // randomly step toward end_coord
    while (rand_nbr != end_coord) {
        map.setGridterrain(rand_nbr, terrain);
        rand_nbr = get_random_neighbor(map, rand_nbr, end_coord);
    }
    map.setGridterrain(end_coord, terrain);
}

std::map<int, float> weather_tile_prob = {
        {TERRAIN_MUD,      1},
        {TERRAIN_PUDDLE,   1}
};

int get_weather_terrain() {
    // grid_change_prob is the probability to set grid to special terrain
    float grid_change_prob = 0.7;
    // start with <rand * grid_change prob> chance for each happening
    for (auto& [terrain, prob] : weather_tile_prob) {
        prob = uniform_dist(rng) * grid_change_prob;
    }
    if (weather == RAIN) {
        weather_tile_prob[TERRAIN_PUDDLE] *= 1.2;
        weather_tile_prob[TERRAIN_MUD] *= 1.1;
    }
    else if (weather == DROUGHT) {
        weather_tile_prob[TERRAIN_PUDDLE] *= 0.75;
        weather_tile_prob[TERRAIN_MUD] *= 0.8;
    }
    else if (weather == FOG) {

    }
    else if (weather == SNOW) {

    }

    vec2 max_prob(-1, -1);

    for (auto& [terrain, prob] : weather_tile_prob) {
        if (prob > max_prob.y){
            max_prob.x = terrain;
            max_prob.y = prob;
        }
    }

    if (max_prob.y > 0.5) {
        return max_prob.x;
    }
    return TERRAIN_DEFAULT;
}

void set_random_weather_terrain(GridMap& map) {
    for (int i = 0; i < WINDOW_SIZE_IN_COORD.x; i++) {
        for (int j = 0; j < WINDOW_SIZE_IN_COORD.y; j++) {
            int weather_terrain = get_weather_terrain();
            if (weather_terrain != TERRAIN_DEFAULT && map.node_matrix[i][j].terrain != TERRAIN_PAVEMENT) {
                map.setGridterrain(ivec2(i,j), weather_terrain);
            }
        }
    }
}

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
entt::entity GridMap::createGridMap()
{
    auto entity = registry.create();
    // maintain a GridMap registry (we might want to have multiple maps later)
    auto& map = registry.emplace<GridMap>(entity);

    // fill node_entity_matrix with default type grid node
    for (int x = 0; x < WINDOW_SIZE_IN_COORD.x; x++){
        for (int y = 0; y < WINDOW_SIZE_IN_COORD.y; y++){
            int terrain = TERRAIN_DEFAULT;
            map.node_entity_matrix[x][y] = GridNode::createGridNode(terrain, vec2(x, y));
            map.node_matrix[x][y] = registry.get<GridNode>(map.node_entity_matrix[x][y]);
        }
    }

    set_random_terrain_path(map, FOREST_COORD, VILLAGE_COORD, TERRAIN_PAVEMENT);
    set_random_weather_terrain(map);
    return entity;
}

void GridMap::setGridterrain(ivec2 grid_coord, int terrain) {
    if (!is_inbounds(grid_coord)) {
        std::cout<<"Debug: out of bounds"<< std::endl;
        return;
    }
    auto& node = getNodeAtCoord(grid_coord);
    auto& entity = getEntityAtCoord(grid_coord);
    node.setterrain(entity, terrain);
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