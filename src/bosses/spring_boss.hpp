#pragma once

#include "common.hpp"
#include "entt.hpp"

// Mob monster that appears in all seasons
struct SpringBoss
{
    // Creates all the associated render resources and default transform
    static entt::entity createSpringBossEntt();
};
