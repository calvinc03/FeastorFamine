#pragma once

#include "common.hpp"
#include "entt.hpp"
#include "grid_map.hpp"

// Mob monster that appears in all seasons
struct WinterBoss
{
    // Creates all the associated render resources and default transform
    static entt::entity createWinterBossEntt();
};