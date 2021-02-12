// Header
#include "hunter.hpp"
#include "render.hpp"

entt::entity Hunter::createHunter(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "hunter";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("circle.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale *= 3.f;

    auto& unit = registry.emplace<Unit>(entity);
    unit.damage = 10;
    unit.attack_rate = 10000;
    unit.attack_range = 400;

    registry.emplace<Hunter>(entity);

    return entity;
}
