// Header
#include "mob.hpp"
#include "render.hpp"

entt::entity Mob::createMobEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "mob";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("rabbit_animate.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 3.f, 0 };
    motion.position = FOREST_COORD;
    // Setting initial values, scale is 1
    motion.scale = vec2({ 0.3f, 0.3f }) * static_cast<vec2>(resource.texture.size);

    auto& monster = registry.emplace<Monster>(entity);
    monster.health = 20;
    monster.damage = 5;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = 6.f;
    animate.state_num = 1.f;

    registry.emplace<Mob>(entity);

    return entity;
}
