#pragma once

#include "common.hpp"
#include "entt.hpp"

// Projectile from
struct Projectile
{
// Creates all the associated render resources and default transform
    static entt::entity createProjectile(entt::entity e_unit, entt::entity e_monster, int damage);
    int damage;
};

struct RockProjectile
{
    static entt::entity createRockProjectile(entt::entity e_unit, entt::entity e_monster, int damage);
    std::vector<vec2> bezier_points;
    int current_step = 0;
};

struct Flamethrower
{
    static entt::entity createFlamethrower(entt::entity e_unit, entt::entity e_monster, int damage);
    float active_timer = 1500;
    entt::entity e_unit; 
};

struct LaserBeam
{
    static entt::entity createLaserBeam(entt::entity e_unit, entt::entity e_monster, int damage);
    float active_timer = 1500;
    entt::entity e_unit;
};

struct Missile
{
    static entt::entity createMissile(entt::entity e_unit, entt::entity e_monster, int damage);
};

struct Explosion
{
    static entt::entity createExplosion(entt::entity e_projectile, int damage);
    float active_timer = 1500;
    entt::entity e_unit;
};