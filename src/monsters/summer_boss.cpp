// Header
#include "render.hpp"
#include "summer_boss.hpp"
const std::string DIR = "monsters/summer/";
const std::string WALK_SPRITE = DIR + "moose_walk.png";
const std::string ATTACK_SPRITE = DIR + "moose_attack.png";
const std::string DEATH_SPRITE = DIR + "moose_death.png";
const int WALK_FRAMES = 4;
const int ATTACK_FRAMES = 4;
const int DEATH_FRAMES = 1;

entt::entity SummerBoss::createSummerBossEntt() 
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "summer_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    //ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 11;
    // Initialize the position, scale, and physics components
    //auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(2.f, 0));
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1, WALK_FRAMES);
   // motion.boundingbox = vec2({ motion.scale.x * (1 / WALK_FRAMES), motion.scale.y });
    motion.boundingbox = vec2({ motion.scale.x * 0.85f / WALK_FRAMES, motion.scale.y });
    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 80;
    monster.health = monster.max_health;
    monster.damage = 30;
    monster.reward = 40;

    monster.hit = false;
    monster.type = SUMMER_BOSS;
    monster.speed_multiplier = 6.f;
    monster.sprite = WALK_SPRITE;
    monster.frames = WALK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES;
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;
    monster.run_frames = WALK_FRAMES;
    monster.run_sprite = WALK_SPRITE;
    monster.death_frames = DEATH_FRAMES;
    monster.death_sprite = DEATH_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = monster.frames;
    animate.state_num = 1;

    registry.emplace<SummerBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
