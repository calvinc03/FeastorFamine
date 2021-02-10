#include "grid_map.hpp"
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
        RenderSystem::createSprite(resource, textures_path("grid_"+std::to_string(type)+".png"), "textured");
    }
    registry.emplace<ShadedMeshRef>(entity, resource);

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = GridMap::coordToPixel(coord);
    // Setting initial values, scale is 1
    motion.scale = vec2({ 1, 1 }) * static_cast<vec2>(resource.texture.size);

    return entity;
}

void GridNode::calculateFCost() {
    f_cost = g_cost + h_cost;
}