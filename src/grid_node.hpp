#include "render.hpp"
#include "common.hpp"

static const std::map<int, std::string> terran_texture_path = {
        {GRID_GRASS, "grid/grass.png"},
        {GRID_PAVEMENT, "grid/pavement.png"},
        {GRID_MUD, "grid/mud.png"},
        {GRID_PUDDLE, "grid/puddle.png"}
};

static const std::map<int, std::string> terran_str = {
        {GRID_GRASS, "grass"},
        {GRID_PAVEMENT, "pavement"},
        {GRID_MUD, "mud"},
        {GRID_PUDDLE, "puddle"}
};

class GridNode {
public:
    ivec2 coord;
    int terran = GRID_GRASS;
    int occupancy = GRID_VACANT;

    static entt::entity createGridNode(int terran, vec2 coord);
    void setTerran(int terran);
    void setOccupancy(int occupancy);
};


