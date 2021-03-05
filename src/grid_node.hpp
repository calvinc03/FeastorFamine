#include "render.hpp"
#include "common.hpp"

class GridNode {
public:
    ivec2 coord;
    int terran = TERRAIN_DEFAULT;
    int occupancy = OCCUPANCY_VACANT;

    static entt::entity createGridNode(int terran, vec2 coord);
    void setTerran(entt::entity, int new_terran);
    void setOccupancy(int new_occupancy);
};


