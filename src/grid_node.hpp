
class GridNode {
public:
    ivec2 coord;
    int terran = GRID_DEFAULT;
    int occupancy = GRID_VACANT;

    int g_cost;
    int h_cost;
    int f_cost;

    static entt::entity createGridNode(int terran, vec2 coord);
    void calculateFCost();
};


