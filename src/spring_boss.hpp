#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// Mob monster that appears in all seasons
struct SpringBoss
{
    // Creates all the associated render resources and default transform
    static ECS::Entity createSpringBoss();
    int health = 1000;
    int damage = 50;
};
