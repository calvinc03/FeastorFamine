// Header
#include "village.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity Village::createVillage()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "village";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("village.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = VILLAGE_POS;
    motion.scale = vec2({ 0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);

    registry.emplace<Village>(entity);

    return entity;
}
