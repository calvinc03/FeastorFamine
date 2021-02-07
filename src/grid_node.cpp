#include "grid_node.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity GridNode::createGridNode(int type, vec2 coord)
{
    auto entity = registry.create();
    std::string key = "grid_node";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("grid_"+std::to_string(type)+".png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);
    registry.emplace<GridNode>(entity);

    return entity;
}

void GridNode::calculateFCost() {
    f_cost = g_cost + h_cost;
}