// Header

#include "projectile.hpp"
#include "render.hpp"
#include "common.hpp"
#include <iostream>
#include <ai.hpp>

float EXPLOSION_FRAMES = 4.f;
float FLAMETHROWER_FRAMES = 4.f;

entt::entity Projectile::createProjectile(entt::entity e_unit, entt::entity e_monster, int damage)
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

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    vec2 direction = normalize(monster_motion.position - hunter_motion.position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(direction.y, direction.x);
    motion.velocity = grid_to_pixel_velocity(direction * 20.f);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    return entity;
}

entt::entity RockProjectile::createRockProjectile(entt::entity e_unit, entt::entity e_monster, int damage)
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

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    float dist = length(monster_motion.position - hunter_motion.position);
    vec2 monster_pos = AISystem::calculate_position(e_monster, dist);

    vec2 distance = (monster_pos - hunter_motion.position);
    
    vec2 orthogonal = vec2(-distance.y, distance.x) * 0.5f;
    if (distance.x > 0) {
        orthogonal = vec2(distance.y, -distance.x) * 0.5f;
    }

    vec2 middle_point = vec2(distance.x, distance.y) / 2.f + hunter_motion.position + orthogonal;

    std::vector<vec2> points;
    points.push_back(hunter_motion.position);
    points.push_back(middle_point);
    points.push_back(monster_pos);

    std::vector<vec2> bezier = bezierVelocities(bezierCurve(points, length(distance)));

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;
    
    RockProjectile& rock = registry.emplace<RockProjectile>(entity);
    rock.bezier_points = bezier;

    return entity;
}

entt::entity Flamethrower::createFlamethrower(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "flamethrower";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("flamethrower.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    auto& motion_h = registry.get<Motion>(e_unit);
    auto& motion_m = registry.get<Motion>(e_monster);
    vec2 direction = normalize(motion_m.position - motion_h.position) * 60.f;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = motion_h.position + direction;
    motion.angle = atan2(-direction.y, -direction.x);
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y * 1.5), 1);
    motion.boundingbox = vec2({ motion.scale.x * 0.22f , motion.scale.y });

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = FLAMETHROWER_FRAMES;
    animate.state_num = 1.f;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& flame = registry.emplace<Flamethrower>(entity);
    flame.e_unit = e_unit;

    return entity;
}


entt::entity LaserBeam::createLaserBeam(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "laserbeam";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0) {

        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("laserbeam.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    auto& hunter_position = registry.get<Motion>(e_unit).position;
    auto& monster_position = registry.get<Motion>(e_monster).position;

    vec2 direction = normalize(monster_position - hunter_position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = hunter_position + direction * static_cast<vec2>(resource.texture.size).x / 2.f;
    motion.angle = atan2(direction.y, direction.x);
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& beam = registry.emplace<LaserBeam>(entity);
    beam.e_unit = e_unit;

    return entity;
}


entt::entity Missile::createMissile(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "missile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("missile.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    vec2 direction = normalize(monster_motion.position - hunter_motion.position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(direction.y, direction.x);
    motion.velocity = grid_to_pixel_velocity(direction * 20.f);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    registry.emplace<Missile>(entity);

    return entity;
}

entt::entity Explosion::createExplosion(entt::entity e_projectile, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "Explosion";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("explosion_animate.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    auto hunter_motion = registry.get<Motion>(e_projectile);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 1);
    motion.boundingbox = vec2({ motion.scale.x * 0.22f , motion.scale.y });

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = EXPLOSION_FRAMES;
    animate.state_num = 1.f;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& explosion = registry.emplace<Explosion>(entity);
    explosion.e_unit = e_projectile;

    return entity;
}