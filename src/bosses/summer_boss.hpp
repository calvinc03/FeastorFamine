#pragma once

#include "common.hpp"
#include "entt.hpp"

// Mob monster that appears in all seasons
struct SummerBoss
{
public:
    std::string sprite;
    // Creates all the associated render resources and default transform
    static entt::entity createSummerBossEntt();
};
