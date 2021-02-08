#include "grid_node.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity GridNode::createGridNodeEntt(int type, vec2 coord)
{
    // get up node components
    auto entity = registry.create();
    auto& node = registry.emplace<GridNode>(entity);
    node.type = type;
    node.coord = coord;

    // set up mesh components
    std::string key = "grid_node";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("grid_0.png"), "textured");
    }
    registry.emplace<ShadedMeshRef>(entity, resource);

    return entity;
}

void GridNode::calculateFCost() {
    f_cost = g_cost + h_cost;
}