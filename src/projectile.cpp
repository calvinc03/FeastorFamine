// Header

#include "projectile.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity Projectile::createProjectile(vec2 pos, vec2 velocity, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "projectile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("projectile.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
//    motion.angle = 0.f;
//    motion.velocity = { -20.f, 0.f};
    motion.angle = atan2(velocity.y, velocity.x);
    motion.velocity = { 700 * velocity.x, 700 * velocity.y };
    motion.position = pos;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale *= vec2({ -1.f, 0.5f });
    
    auto& projectile = registry.emplace<Projectile_Dmg>(entity);
    projectile.damage = damage;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    registry.emplace<Projectile>(entity);

    return entity;
}
