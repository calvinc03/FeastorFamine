#include "render.hpp"
#include "common.hpp"

class GridNode {
public:
    ivec2 coord;
    int terrain = TERRAIN_DEFAULT;
    int occupancy = OCCUPANCY_VACANT;
    int num_monsters = 0;

    static entt::entity createGridNode(int terrain, vec2 coord);
    void setTerrain(entt::entity entity, int new_terrain);
    void setOccupancy(int new_occupancy);
};


