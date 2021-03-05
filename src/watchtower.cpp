// Header
#include "watchtower.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity WatchTower::createWatchTower(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "watchtower";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("watchtower.png"), "unit");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 50;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale *= 1.f;

    auto& unit = registry.emplace<Unit>(entity);
    unit.damage = 10;
    unit.attack_rate = 10000;
    unit.attack_range = 400;
    unit.workers = 0;

    registry.emplace<WatchTower>(entity);
    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);

    return entity;
}
