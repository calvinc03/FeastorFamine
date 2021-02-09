// internal
#include "physics.hpp"
//#include "tiny_ecs.hpp"
#include "entt.hpp"
#include "debug.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity 
vec2 get_bounding_box(const Motion& motion)
{
	// fabs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool collides(const Motion& motion1, const Motion& motion2)
{
	auto dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	float other_r = std::sqrt(std::pow(get_bounding_box(motion1).x/2.0f, 2.f) + std::pow(get_bounding_box(motion1).y/2.0f, 2.f));
	float my_r = std::sqrt(std::pow(get_bounding_box(motion2).x/2.0f, 2.f) + std::pow(get_bounding_box(motion2).y/2.0f, 2.f));
	float r = max(other_r, my_r);
	if (dist_squared < r * r)
		return true;
	return false;
}

void PhysicsSystem::step(float elapsed_ms)
{
	auto view_motion = registry.view<Motion>();
	

	// Move entities based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	for(auto entity: registry.view<Motion>()) {
	    auto& motion = registry.get<Motion>(entity);
        float step_seconds = 1.0f * (elapsed_ms / 1000.f);
        motion.position.x += step_seconds * motion.velocity.x;
        motion.position.y += step_seconds * motion.velocity.y;
	}


	// Check for collisions between all moving entities
	//TODO: needs further testing. See if this avoids duplicates
	for (auto [entity_i, motion_i] : view_motion.each()) {
		for (auto [entity_j, motion_j] : view_motion.each()) {
			if (collides(motion_i, motion_j) && entity_i != entity_j)
			{
				registry.emplace_or_replace<Collision>(entity_i, entity_j);
				registry.emplace_or_replace<Collision>(entity_j, entity_i);
			}
		}
	}

}

PhysicsSystem::Collision::Collision(entt::entity& other)
{
	this->other = other;
}
