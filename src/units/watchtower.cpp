// Header
#include "watchtower.hpp"
#include "render.hpp"
#include "common.hpp"
#include "units/unit.hpp"

entt::entity WatchTower::createWatchTower(vec2 pos)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = unit_str.at(WATCHTOWER);
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
    unit.attack_interval_ms = 800;
    unit.attack_range = 400;
    unit.workers = 0;
    unit.upgrades = 0;
    unit.type = WATCHTOWER;
    unit.cost = WATCHTOWER_COST;
    unit.upgrade_cost = WATCHTOWER_UPGRADE_COST;
    unit.sell_price = WATCHTOWER_SELL_PRICE;

    registry.emplace<WatchTower>(entity);
    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);
    registry.emplace<HitReaction>(entity);
    return entity;
}