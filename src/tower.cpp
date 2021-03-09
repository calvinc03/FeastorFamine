// Header
#include "tower.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity Tower::createTower(int x_pos, int y_pos)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "tower";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("tower.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = { x_pos, y_pos };
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ 0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);

    // Create and (empty) Fish component to be able to refer to all fish
    registry.emplace<Tower>(entity);

    return entity;
}
