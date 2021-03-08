
#include "particle.hpp"
#include "render.hpp"
#include "common.hpp"

entt::entity ParticleSystem::createParticle(vec2 velocity, vec2 position, float life, std::string texture) {
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "particle";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(texture), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 5;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);

    motion.angle = 0;
    motion.velocity = velocity;
    motion.position = position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale *= vec2({1.0, 1.0});
    
    auto& particle = registry.emplace<ParticleSystem>(entity);
    particle.life = life;


    return entity;
}

