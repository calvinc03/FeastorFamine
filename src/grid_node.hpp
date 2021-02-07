#include "common.hpp"
class GridNode {
public:
    vec2 coord;
    int type;

    int g_cost;
    int h_cost;
    int f_cost;

    static entt::entity createGridNode(int type, vec2 coord);
    void calculateFCost();
};


