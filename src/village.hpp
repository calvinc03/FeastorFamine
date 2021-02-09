#pragma once

#include "common.hpp"
#include "entt.hpp"

struct Village
{
    // Creates all the associated render resources and default transform
    static entt::entity createVillage();
    int health = 1000;
    int damage = 50;
};
