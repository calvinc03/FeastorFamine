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
        RenderSystem::createSprite(resource, textures_path("rabbit.png"), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 25.f, 0 };
    motion.position = GridMap::coordToPixel(FOREST_COORD);
    motion.scale = vec2({ 0.25f, 0.25f }) * static_cast<vec2>(resource.texture.size);

    auto& monster = registry.emplace<Monster>(entity);
    monster.health = 20;
    monster.damage = 5;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = 6.f;
    animate.state_num = 1.f;

    registry.emplace<Mob>(entity);
    registry.emplace<HitReaction>(entity);
    return entity;
}
