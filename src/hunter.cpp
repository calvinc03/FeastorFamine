// Header
#include "hunter.hpp"
#include "render.hpp"

entt::entity Hunter::createHunter(vec2 pos)
{
    // snap to nearest grid size
    int xpos = (pos.x + GRID_CELL_SIZE / 2) / GRID_CELL_SIZE;
    xpos *= GRID_CELL_SIZE;
    int ypos = (pos.y + GRID_CELL_SIZE / 2) / GRID_CELL_SIZE;
    ypos *= GRID_CELL_SIZE;

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
    motion.position = vec2{ xpos, ypos };
    // Then we scale it to whatever size is needed
    motion.scale *= 3.f;

    registry.emplace<Hunter>(entity);

    return entity;
}
