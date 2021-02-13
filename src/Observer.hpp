#pragma once

#include "common.hpp"
#include "entt.hpp"

// Placeable Hunter unit 
class Observer
{
    // Creates all the associated render resources and default transform
public:
    virtual void updateCollisions(entt::entity entity_i, entt::entity entity_j) {}
};
