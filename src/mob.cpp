// Header
#include "mob.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity Mob::createMob()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "mob";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("fish.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 380.f, 0 };
    motion.position = FOREST_POS;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);

    // Create and (empty) Fish component to be able to refer to all fish
    registry.emplace<Mob>(entity);

    return entity;
}
