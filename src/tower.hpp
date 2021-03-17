#pragma once

#include "common.hpp"
#include "entt.hpp"

// Mob monster that appears in all seasons
struct Tower
{
    // Creates all the associated render resources and default transform
    static entt::entity createTower(int x_pos, int y_pos);
};
