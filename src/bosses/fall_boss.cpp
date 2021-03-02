// Header
#include "render.hpp"
#include "fall_boss.hpp"

entt::entity FallBoss::createFallBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "fall_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("hawk.png"), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    //ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    registry.emplace<ShadedMeshRef>(entity, resource);
    // Initialize the position, scale, and physics components
    //auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 75.f, 0 };
    motion.position = GridMap::coordToPixel(FOREST_COORD);
    motion.scale = vec2({ 1, 1 }) * static_cast<vec2>(resource.texture.size);
    motion.boundingbox = motion.scale;

    auto& monster = registry.emplace<Monster>(entity);
    monster.health = 40;
    monster.damage = 5;

    registry.emplace<FallBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
