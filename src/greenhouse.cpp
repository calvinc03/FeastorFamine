// Header
#include "greenhouse.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity GreenHouse::createGreenHouse(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "greenhouse";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("greenhouse.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale *= 1.f;

    registry.emplace<GreenHouse>(entity);

    return entity;
}
