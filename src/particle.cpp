
#include "particle.hpp"
#include "render.hpp"
#include "common.hpp"
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

std::array<GLfloat, MAX_PARTICLES*2>ParticleSystem::g_particule_position_size_data;
int ParticleSystem::PARTICLE_COUNT = 0;

entt::entity ParticleSystem::createParticle(vec2 velocity, vec2 position, float life, std::string texture) {
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "particle";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(texture), "particle");
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
    motion.scale *= vec2({0.5, 0.5});
    
    auto& particle = registry.emplace<ParticleSystem>(entity);
    particle.life = life;

    return entity;
}

void ParticleSystem::updateParticle() {
//    g_particule_position_size_data = new GLfloat[MAX_PARTICLES * 2]; // TODO initialize outside
    PARTICLE_COUNT = 0;
    for (auto particle_view : registry.view<ParticleSystem>()) {
        auto& particle_m = registry.get<Motion>(particle_view);
        g_particule_position_size_data[2*PARTICLE_COUNT+0] = particle_m.position.x;
        g_particule_position_size_data[2*PARTICLE_COUNT+1] = particle_m.position.y;
        PARTICLE_COUNT++;

//        for (int i = 0; i < PARTICLE_COUNT; i++) {
//            std::cout << "x: " << g_particule_position_size_data[i] << "\n";
//            std::cout << "y: " << g_particule_position_size_data[i+1] << "\n";
//        }

    }
}

//void ParticleSystem::initParticle() {
//    static const GLfloat g_vertex_buffer_data[] = {
//         -0.5f, -0.5f, 0.0f,
//          0.5f, -0.5f, 0.0f,
//         -0.5f,  0.5f, 0.0f,
//          0.5f,  0.5f, 0.0f,
//    };
//    glGenBuffers(1, &billboard_vertex_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//    
//    glGenBuffers(1, &particles_position_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
//
//    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
//    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
//}
