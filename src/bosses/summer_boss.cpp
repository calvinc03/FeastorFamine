// Header
#include "render.hpp"
#include "summer_boss.hpp"

const std::string WALK_SPRITE = "moose/moose_walk.png";
const std::string RUN_SPRITE = "moose/moose_run.png";
const std::string ATTACK_SPRITE = "moose/moose_eat.png";
const std::string DEATH_SPRITE = "moose/moose_death.png";
const size_t WALK_FRAMES = 10.f;
const size_t RUN_FRAMES = 8.f;
const size_t ATTACK_FRAMES = 11.f;
const size_t DEATH_FRAMES = 11.f;

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
    registry.emplace<ShadedMeshRef>(entity, resource);
    // Initialize the position, scale, and physics components
    //auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 50.f, 0 };
    motion.position = coordToPixel(FOREST_COORD);
    motion.scale = vec2({ 1, 1 }) * static_cast<vec2>(resource.texture.size);
    motion.boundingbox = vec2({ motion.scale.x * (1 / WALK_FRAMES), motion.scale.y });

    auto& monster = registry.emplace<Monster>(entity);
    monster.health = 60;
    monster.damage = 10;
    monster.reward = 50;

    auto& boss = registry.emplace<Boss>(entity);
    boss.hit = false;
    boss.speed_multiplier = 6.f;
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
    animate.frame_num = boss.frames;
    animate.state_num = 1.f;

    registry.emplace<SummerBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
