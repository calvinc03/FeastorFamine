// Header

#include "projectile.hpp"
#include "render.hpp"
#include "common.hpp"
#include <iostream>

entt::entity Projectile::createProjectile(vec2 hunter_position, vec2 monster_position, int damage)
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

    float opposite = monster_position.y - hunter_position.y;
    float adjacent = monster_position.x - hunter_position.x;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(opposite, adjacent);
    motion.velocity = grid_to_pixel_velocity(normalize(vec2(adjacent, opposite)) * 20.f);
    motion.position = hunter_position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = { 50, 50 };

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    return entity;
}

entt::entity RockProjectile::createRockProjectile(vec2 hunter_position, vec2 monster_position, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "rock_projectile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("rock.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    float opposite = monster_position.y - hunter_position.y;
    float adjacent = monster_position.x - hunter_position.x;
    
    vec2 orthogonal = vec2(-opposite, adjacent) * 0.5f;
    if (adjacent > 0) {
        orthogonal = vec2(opposite, -adjacent) * 0.5f;
    }

    vec2 middle_point = vec2(adjacent, opposite) / 2.f + hunter_position + orthogonal;

    std::vector<vec2> points;
    points.push_back(hunter_position);
    points.push_back(middle_point);
    points.push_back(monster_position);

    std::vector<vec2> bezier = bezierVelocities(bezierCurve(points, length(hunter_position - monster_position)));

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(opposite, adjacent);
    motion.velocity = grid_to_pixel_velocity(normalize(vec2(adjacent, opposite)) * 0.f);
    motion.position = hunter_position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = { 50, 50 };

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;
    
    RockProjectile& rock = registry.emplace<RockProjectile>(entity);
    rock.bezier_points = bezier;

    return entity;
}
