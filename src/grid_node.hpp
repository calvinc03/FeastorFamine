
class GridNode {
public:
    ivec2 coord;
    int type = GRID_VACANT;

    int g_cost;
    int h_cost;
    int f_cost;

    static entt::entity createGridNode(int type, vec2 coord);
    void calculateFCost();
};


