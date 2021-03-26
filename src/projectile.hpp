#pragma once

#include "common.hpp"
#include "entt.hpp"

// Projectile from
struct Projectile
{
// Creates all the associated render resources and default transform
    static entt::entity createProjectile(vec2 hunter_position, vec2 monster_position, int damage);
    int damage;
};

struct RockProjectile
{
    static entt::entity createRockProjectile(vec2 hunter_position, vec2 monster_position, int damage);
    std::vector<vec2> bezier_points;
    int current_step = 0;
};

struct Flamethrower
{
    static entt::entity createFlamethrower(entt::entity e_unit, vec2 monster_position, int damage);
    float active_timer = 1500;
    entt::entity e_unit; 
};