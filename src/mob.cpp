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
        RenderSystem::createSprite(resource, textures_path("rabbit_animate.png"), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 10;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(1, 0) * 3.f);
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 0.5);
    // temporary fix
    //motion.boundingbox = vec2({ motion.scale.x , motion.scale.y });
    motion.boundingbox = vec2({ motion.scale.x*0.13f , motion.scale.y });
    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 30;
    monster.health = monster.max_health;
    monster.damage = 5;
    monster.reward = 20;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = 6.f;
    animate.state_num = 1.f;

    registry.emplace<Mob>(entity);
    registry.emplace<HitReaction>(entity);
    return entity;
}
