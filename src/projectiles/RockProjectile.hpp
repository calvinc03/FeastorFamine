#pragma once

#include "common.hpp"
#include "entt.hpp"

// Projectile from
struct RockProjectile
{
// Creates all the associated render resources and default transform
    static entt::entity createRockProjectile(vec2 pos, vec2 velocity, int damage);
};

