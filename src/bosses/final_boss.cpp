// Header
#include "render.hpp"
#include "final_boss.hpp"

const std::string WALK_SPRITE = "NA";
const std::string RUN_SPRITE = "NA";
const std::string ATTACK_SPRITE = "final/final_boss2.png";
const std::string DEATH_SPRITE = "NA";
const size_t WALK_FRAMES = 0.f;
const size_t RUN_FRAMES = 0.f;
const size_t ATTACK_FRAMES = 39.f;
const size_t DEATH_FRAMES = 0.f;

entt::entity FinalBoss::createFinalBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "final_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(ATTACK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 11;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = vec2(1, 0);
    motion.position = coord_to_pixel(DRAGON_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 7, ATTACK_FRAMES);
    motion.boundingbox = vec2({ motion.scale.x *.8f / ATTACK_FRAMES, motion.scale.y });

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 5000;
    monster.health = monster.max_health;
    monster.damage = 0;
    monster.reward = 10000;

    auto& boss = registry.emplace<Boss>(entity);
    boss.hit = false;
    boss.type = DRAGON_BOSS_TYPE;
    boss.speed_multiplier = 1.f;
    boss.sprite = ATTACK_SPRITE;
    boss.frames = ATTACK_FRAMES;
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
    animate.frame_num = ATTACK_FRAMES;
    animate.state_num = 1.f;

    registry.emplace<FinalBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
