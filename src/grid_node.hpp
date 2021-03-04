
class GridNode {
public:
    ivec2 coord;
    int terran = GRID_GRASS;
    int occupancy = GRID_VACANT;

    static entt::entity createGridNode(int terran, vec2 coord);
};


