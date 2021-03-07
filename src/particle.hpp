#pragma once

#include "common.hpp"
#include "entt.hpp"

class ParticleSystem
{
public:
    
    float life;
    static entt::entity createParticle();
};
