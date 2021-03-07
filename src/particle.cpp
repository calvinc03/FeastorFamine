
#include "particle.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity ParticleSystem::createParticle() {
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "particle";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("raindrop.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    registry.emplace<ShadedMeshRef>(entity, resource);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);

    motion.angle = 0;
    motion.velocity = {0, 450.0f};
    motion.position = {rand() % WINDOW_SIZE_IN_PX.x + 1 , 0};
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale *= vec2({1.0, 1.0});
    
    auto& particle = registry.emplace<ParticleSystem>(entity);
    particle.life = 1300.0f;


    return entity;
}

