// Header
#include "mob.hpp"
#include "render.hpp"
#include "common.hpp"
#include "spring_boss.hpp"

entt::entity SpringBoss::createSpringBoss()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "spring_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("turtle.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    //ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    registry.emplace<ShadedMeshRef>(entity, resource);
    // Initialize the position, scale, and physics components
    //auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 380.f, 0 };
    motion.position = FOREST_POS;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);

   registry.emplace<SpringBoss>(entity);

    return entity;
}
