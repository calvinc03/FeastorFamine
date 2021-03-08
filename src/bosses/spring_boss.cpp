// Header
#include "render.hpp"
#include "spring_boss.hpp"

const std::string WALK_SPRITE = "eagle/eagle_walk.png";
const std::string RUN_SPRITE = "NA";
const std::string ATTACK_SPRITE = "eagle/eagle_attack.png";
const std::string DEATH_SPRITE = "eagle/eagle_death.png";
const size_t WALK_FRAMES = 4.f;
const size_t RUN_FRAMES = 0.f;
const size_t ATTACK_FRAMES = 0.f;
const size_t DEATH_FRAMES = 0.f;

entt::entity SpringBoss::createSpringBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "spring_boss";
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
    motion.velocity = grid_to_pixel_velocity(vec2(1, 0));
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 0.5);
    // scale down bounding box from .png file based on number of frames
   // motion.boundingbox = vec2({ motion.scale.x * (1 / WALK_FRAMES), motion.scale.y });
    motion.boundingbox = vec2({ motion.scale.x * 0.85f/WALK_FRAMES, motion.scale.y });

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 120;
    monster.health = monster.max_health;
    monster.damage = 20;
    monster.reward = 50;

    auto& boss = registry.emplace<Boss>(entity);
    boss.hit = false;
    boss.speed_multiplier = 1.f;
    boss.sprite = WALK_SPRITE;
    boss.frames = WALK_FRAMES;
    boss.attack_frames = ATTACK_FRAMES;
    boss.attack_sprite = ATTACK_SPRITE;
    boss.walk_frames = WALK_FRAMES;
    boss.walk_sprite = WALK_SPRITE;
    boss.run_frames = RUN_FRAMES;
    boss.run_sprite = RUN_SPRITE;
    boss.death_frames = DEATH_FRAMES;
    boss.death_sprite = DEATH_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = WALK_FRAMES;
    animate.state_num = 1.f;

    registry.emplace<SpringBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
