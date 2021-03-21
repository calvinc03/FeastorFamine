// internal
#include "physics.hpp"
#include "entt.hpp"
#include "debug.hpp"
#include <iostream>
#include "grid_node.hpp"
#include <projectile.hpp>
#include "rig.hpp"
#include <spider.hpp>

const size_t POTENTIAL_COLLISION_RANGE = 200;

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

std::vector<vec2> get_box_vertices(const Motion& motion)
{
	std::vector<vec2> points;
	points.push_back(motion.position + motion.boundingbox / 2.f);
	points.push_back(motion.position + vec2(motion.boundingbox.x, -motion.boundingbox.y) / 2.f);
	points.push_back(motion.position - motion.boundingbox / 2.f);
	points.push_back(motion.position + vec2(-motion.boundingbox.x, motion.boundingbox.y) / 2.f);
	return points;
}

std::vector<vec2> get_norms(std::vector<vec2> vertices)
{
	std::vector<vec2> norms;
	for (int i = 0; i < vertices.size(); i++) {
		vec2 current_point = vertices[i];
		vec2 next_point = vertices[(i + 1) % vertices.size()];
		vec2 edge = next_point - current_point;

		vec2 norm;
		if (edge.x >= 0) {
			if (edge.y >= 0) norm = { edge.y, -edge.x };
			else norm = { -edge.y, edge.x };
		}
		else {
			if (edge.y >= 0) norm = { -edge.y, edge.x };
			else norm = { edge.y, -edge.x };
		}
		norms.push_back(norm);
	}
	return norms;
}

vec2 projectShape(std::vector<vec2> vertices, vec2 axis)
{
	float min = INFINITY;
	float max = -INFINITY;
	for (vec2 v : vertices) {
		float projection = dot(v, axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
	return vec2(min, max);
}

bool checkProjection(std::vector<vec2> poly_a_vertices, std::vector<vec2> poly_b_vertices,
	std::vector<vec2> poly_a_norms, std::vector<vec2> poly_b_norms)
{
	for (vec2 norm : poly_a_norms) {
		vec2 projection1 = projectShape(poly_a_vertices, norm);
		vec2 projection2 = projectShape(poly_b_vertices, norm);
		if ((projection1.y < projection2.x) || (projection2.y < projection1.x))
			return false;
	}

	for (vec2 norm : poly_b_norms) {
		vec2 projection1 = projectShape(poly_a_vertices, norm);
		vec2 projection2 = projectShape(poly_b_vertices, norm);
		if ((projection1.y < projection2.x) || (projection2.y < projection1.x))
			return false;
	}

	return true;
}
 
bool collidesSAT(entt::entity entity1, entt::entity entity2)
{
	auto& motion1 = registry.get<Motion>(entity1);
	auto& motion2 = registry.get<Motion>(entity2);

	std::vector<vec2> polygon_a = get_box_vertices(motion1);
	std::vector<vec2> polygon_b = get_box_vertices(motion2);
	
	std::vector<vec2> poly_a_norms = get_norms(polygon_a);
	std::vector<vec2> poly_b_norms = get_norms(polygon_b);

	return checkProjection(polygon_a, polygon_b, poly_a_norms, poly_b_norms);
}

// Precise Collisions with two convex objects 
bool preciseCollides(entt::entity spider, entt::entity projectile)
{
	auto& spider_motion = registry.get<Motion>(spider);

	auto& proj_motion = registry.get<Motion>(projectile);
	std::vector<vec2> projectile_vertices = get_box_vertices(proj_motion);
	std::vector<vec2> projectile_norms = get_norms(projectile_vertices);

	auto& spider_rig = registry.get<Rig>(spider);

	for (auto rig_vector : spider_rig.chains) {
		for (auto rig_entity : rig_vector) {
			auto& motion = registry.get<Motion>(rig_entity);
			auto& meshref = registry.get<ShadedMeshRef>(rig_entity);

			Transform transform;
			transform.translate(spider_motion.position + motion.position);
			transform.rotate(spider_motion.angle + motion.angle);
			transform.scale(spider_motion.scale + motion.scale);

			std::vector<vec2> spider_rig_vertices;
			for (auto& v : meshref.reference_to_cache->mesh.vertices) {
				vec3 global_pos = transform.mat * vec3(v.position.x, v.position.y, 1.0f);
				spider_rig_vertices.push_back(vec2(global_pos.x, global_pos.y));
			}
			std::vector<vec2> spider_rig_norms = get_norms(spider_rig_vertices);

			if (checkProjection(spider_rig_vertices, projectile_vertices, spider_rig_norms, projectile_norms)) {
				return true;
			}
		}
	}

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

			// considers collisions between only if the entities are projectiles and monsters
			if ((registry.has<Projectile>(entity_i) && registry.has<Monster>(entity_j)) || (registry.has<Projectile>(entity_j) && registry.has<Monster>(entity_i)))
			{
				// considers collisions if entities are within a certain range
				if (length(motion_i.position - motion_j.position) < POTENTIAL_COLLISION_RANGE) {

					// convex polygon precise collision
					if (registry.has<Rig>(entity_i) || registry.has<Rig>(entity_j)) {
						// notify Observers - ORDER MATTERS
						if (registry.has<Projectile>(entity_i)) {
							// precise collision between complex rig and convex object - ORDER MATTERS
							if (preciseCollides(entity_j, entity_i)) {
								notifyObservers(entity_i, entity_j);
							}
						}
						else {
							if (preciseCollides(entity_i, entity_j)) {
								notifyObservers(entity_j, entity_i);
							}
						}
					}
					
					// box-to-box polygon collision
					else {
						// checks if collides using Separating Axis Theorem
						if (collidesSAT(entity_i, entity_j))
						{
							// notify Observers - ORDER MATTERS!
							if (registry.has<Projectile>(entity_i)) {
								notifyObservers(entity_i, entity_j);
							}
							else {
								notifyObservers(entity_j, entity_i);
							}
						}
					}
				}
			}
		}
	}


	if (DebugSystem::in_debug_mode)
	{
		auto view_motion = registry.view<Motion>();
		for (auto [entity, motion] : view_motion.each())
		{
			if (registry.has<Rig>(entity)) {
				DebugSystem::display_rig_vertices(entity, camera);
			}
			if (!registry.has<GridNode>(entity) 
				&& !registry.has<HealthComponent>(entity) 
				&& !registry.has<DebugComponent>(entity)
				) {//&& !registry.has<RigPart>(entity)

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
		observerlist[i]->updateProjectileMonsterCollision(entity_i, entity_j);
	}
}

void PhysicsSystem::attach(Observer* obs) {
	this->observerlist.push_back(obs);
}