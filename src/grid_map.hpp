#pragma once

#include <iostream>
#include "common.hpp"
#include "grid_node.hpp"

// A simple physics system that moves rigid bodies and checks for collision

class GridMap
{
public:
    std::vector<std::vector<entt::entity>> node_entt_matrix = {{}};
    std::vector<entt::entity> path_entt = {};

    static entt::entity createGridMapEntt();

    static ivec2 coordToPixel(ivec2 grid_coord);
    static ivec2 pixelToCoord(ivec2 pixel_coord);
    static entt::entity getEntityAtCoord(std::vector<std::vector<entt::entity>>& node_matrix, ivec2 grid_coord);
    static GridNode& getNodeAtCoord(std::vector<std::vector<entt::entity>>& node_matrix, vec2 grid_coord);
    void setGridType(std::vector<std::vector<entt::entity>>& node_matrix, vec2 grid_coord, int type);
    // coordinates must be in order from starting coord to end coord
    void setPathFromCoords(std::vector<vec2>& grid_coords);
};

