// Header
#include "fireball_boss.hpp"
#include "render.hpp"

const std::string WALK_SPRITE = "projectile.png";
const std::string RUN_SPRITE = "NA";
const std::string ATTACK_SPRITE = "NA";
const std::string DEATH_SPRITE = "NA";
const size_t WALK_FRAMES = 1.f;
const size_t RUN_FRAMES = 0.f;
const size_t ATTACK_FRAMES = 0;
const size_t DEATH_FRAMES = 0.f;

entt::entity FireballBoss::createFireballBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "fireball";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 12;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 200.f;
    motion.velocity = vec2(1, 0);
    motion.position = coord_to_pixel(vec2(7, DRAGON_COORD.y + 1.5));
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), .75f);
    motion.boundingbox = { 75, 75 };

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 500;
    monster.health = monster.max_health;
    monster.damage = 100;
    monster.reward = 50;

    monster.hit = false;
    monster.type = FIREBALL_BOSS;
    monster.speed_multiplier = 1.f;
    monster.sprite = WALK_SPRITE;
    monster.frames = WALK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES;
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;
    monster.run_frames = RUN_FRAMES;
    monster.run_sprite = RUN_SPRITE;
    monster.death_frames = DEATH_FRAMES;
    monster.death_sprite = DEATH_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = WALK_FRAMES;
    animate.state_num = 1.f;

    registry.emplace<FireballBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
