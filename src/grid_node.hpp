
class GridNode {
public:
    ivec2 coord;
    int type = GRID_DEFAULT;

    int g_cost;
    int h_cost;
    int f_cost;

    static entt::entity createGridNodeEntt(int type, vec2 coord);
    void calculateFCost();
};


