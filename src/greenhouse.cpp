// Header
#include "greenhouse.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity GreenHouse::createGreenHouse(vec2 position)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = GREENHOUSE_NAME;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("greenhouse.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Setting initial motion values
    Motion& motion = registry.emplace<Motion>(entity);
    motion.position = position;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = vec2({ 0.5f,0.5f }) * static_cast<vec2>(resource.texture.size);

    auto& unit = registry.emplace<Unit>(entity);
    unit.damage = 0;
    unit.attack_rate = 0;
    unit.attack_range = 0;
    unit.workers = 0;
    unit.upgrades = 0;
    unit.type = key;

    registry.emplace<GreenHouse>(entity);
    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);

    return entity;
}
