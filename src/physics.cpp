// internal
#include "physics.hpp"
//#include "tiny_ecs.hpp"
#include "entt.hpp"
#include "debug.hpp"
#include <iostream>
#include "grid_node.hpp"
#include <projectile.hpp>
// Returns the local bounding coordinates scaled by the current size of the entity 
vec2 get_bounding_box(const Motion& motion)
{
	// fabs is to avoid negative scale due to the facing direction.
	
	return { abs(motion.boundingbox.x), abs(motion.boundingbox.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool collides(const Motion& motion1, const Motion& motion2, float elapsed_ms)
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

	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	for (auto entity : registry.view<RockProjectile>()) {
		auto& motion = registry.get<Motion>(entity);
		auto& rock = registry.get<RockProjectile>(entity);
		motion.velocity = 1 / step_seconds * rock.bezier_points[rock.current_step];
		if (rock.current_step < rock.bezier_points.size() - 1) {
			rock.current_step += 1;
		}
	}


	for(auto entity: registry.view<Motion>()) {
	    auto& motion = registry.get<Motion>(entity);
        motion.position += step_seconds * motion.velocity;
	}

	// Check for collisions between all moving entities

	auto entity = registry.view<Motion>();

	for (unsigned int i = 0; i < entity.size(); i++)
	{
		for (unsigned int j = i + 1; j < entity.size(); j++)
		{
			Motion& motion_i = registry.get<Motion>(entity[i]);
			entt::entity entity_i = entity[i];
			Motion& motion_j = registry.get<Motion>(entity[j]);
			entt::entity entity_j = entity[j];

			if (collides(motion_i, motion_j, elapsed_ms))
			{
				notifyObservers(entity_i, entity_j);
			}
		}
	}


	if (DebugSystem::in_debug_mode)
	{
		//bounding boxes and scale of objects
		auto view_motion = registry.view<Motion>();
		for (auto [entity, motion] : view_motion.each())
		{
			//TODO: add a boudning box to projectiles
			if (!registry.has<GridNode>(entity) && registry.has<Motion>(entity) && !registry.has<HealthComponent>(entity) && !registry.has<DebugComponent>(entity)) {

				DebugSystem::createBox(motion.position, motion.boundingbox);
			}
		}
	}
	
}

PhysicsSystem::Collision::Collision(entt::entity& other)
{
	this->other = other;
}

void PhysicsSystem::notifyObservers(entt::entity entity_i, entt::entity entity_j) {
	for (int i = 0; i < observerlist.size(); i++) {
		if (registry.has<Projectile>(entity_i) && registry.has<Monster>(entity_j)) {
			observerlist[i]->updateProjectileMonsterCollision(entity_i, entity_j);
		}
		else if (registry.has<Projectile>(entity_j) && registry.has<Monster>(entity_i)) {
			observerlist[i]->updateProjectileMonsterCollision(entity_j, entity_i);
		}
	}
}

void PhysicsSystem::attach(Observer* obs) {
	this->observerlist.push_back(obs);
}