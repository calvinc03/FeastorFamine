#include <entt.hpp>
#pragma once

// The return type of behaviour tree processing
enum class BTState {
    Moving,
    Stopped,
    Attack,
    Failure,
    Dragon,
    Fireball
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
    virtual ~BTNode() noexcept = default; // Needed for deletion-through-pointer

    virtual void init(entt::entity e) {};

    virtual BTState process(entt::entity e) = 0;
};

static void remove_unit_entity(entt::entity e_unit);
static void increment_monster_step(entt::entity entity);
