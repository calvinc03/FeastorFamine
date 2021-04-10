// Header
#include "talky_boi.hpp"
#include "render.hpp"
#include <iostream>

const std::string WALK_SPRITE = "speakers/helge.png";
const std::string RUN_SPRITE = "NA";
const std::string ATTACK_SPRITE = "NA";
const std::string DEATH_SPRITE = "NA";
const int WALK_FRAMES = 6;
const int RUN_FRAMES = 0;
const int ATTACK_FRAMES = 0;

entt::entity TalkyBoi::createTalkyBoiEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "monster";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 60;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 180.f;
    motion.velocity = vec2(0.f, 0.f);
    motion.position = vec2(800.f, 200.f);
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, -static_cast<vec2>(resource.texture.size).y), 3.5, WALK_FRAMES);

    motion.boundingbox = { 200, 200 };

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 500;
    monster.health = monster.max_health;
    monster.damage = 100;
    monster.reward = 50;

    monster.hit = false;
    monster.type = TALKY_BOI;
    monster.speed_multiplier = 0.f;
    monster.sprite = WALK_SPRITE;
    monster.frames = WALK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES;
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;
    monster.run_frames = RUN_FRAMES;
    monster.run_sprite = RUN_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = WALK_FRAMES;
    animate.state_num = 1;

    registry.emplace<TalkyBoi>(entity);

    return entity;
}
