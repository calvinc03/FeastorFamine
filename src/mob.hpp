#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// Mob monster that appears in all seasons
struct Mob
{
    // Creates all the associated render resources and default transform
    static ECS::Entity createMob();
    int health = 100;
    int damage = 5;
};
