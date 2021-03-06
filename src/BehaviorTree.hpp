#include <entt.hpp>
#pragma once

// The return type of behaviour tree processing
enum class BTState {
    Moving,
    Stopped,
    Failure,
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
    virtual ~BTNode() noexcept = default; // Needed for deletion-through-pointer

    virtual void init(entt::entity e) {};

    virtual BTState process(entt::entity e) = 0;
};