// Header
#include "hunter.hpp"
#include "render.hpp"
#include "menu.hpp"
#include "button.hpp"

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
        RenderSystem::createSprite(resource, textures_path("hunter.png"), "unit");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 50;

    // Initialize the position component
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = pos;
    // Then we scale it to whatever size is needed
    motion.scale *= vec2({ 1.f, 0.8f }) ;

    auto& unit = registry.emplace<Unit>(entity);
    unit.damage = 10;
    unit.attack_rate = 3000;
    unit.attack_range = 400;

    registry.emplace<Hunter>(entity);

    auto view_shaded_mesh = registry.view<ShadedMeshRef>();

    registry.emplace<Selectable>(entity);
    registry.emplace<HighlightBool>(entity);

    return entity;
}