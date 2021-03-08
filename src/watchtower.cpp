// Header
#include "watchtower.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity WatchTower::createWatchTower(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = WATCHTOWER_NAME;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("units/watchtower.png"), "unit");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 50;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1);

    auto& unit = registry.emplace<Unit>(entity);
    unit.damage = 10;
    unit.attack_rate = 10000;
    unit.attack_range = 400;
    unit.workers = 0;
    unit.upgrades = 0;
    unit.type = key;

    registry.emplace<WatchTower>(entity);
    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);

    return entity;
}
