#include "render.hpp"
#include "common.hpp"

class GridNode {
public:
    ivec2 coord;
    int terrain = TERRAIN_DEFAULT;
    int occupancy = OCCUPANCY_VACANT;

    static entt::entity createGridNode(int terrain, vec2 coord);
    void setterrain(entt::entity, int new_terrain);
    void setOccupancy(int new_occupancy);
};


