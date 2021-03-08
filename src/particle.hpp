#pragma once

#include "common.hpp"
#include "entt.hpp"

class ParticleSystem
{
public:
    
    float life;
    static entt::entity createParticle(vec2 velocity, vec2 position, float life, std::string texture);
};
