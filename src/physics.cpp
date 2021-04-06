// internal
#include "physics.hpp"
#include "entt.hpp"
#include "debug.hpp"
#include <iostream>
#include "grid_node.hpp"
#include <projectile.hpp>
#include "rig.hpp"
#include <monsters/spider.hpp>

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

std::vector<vec2> get_box_vertices(entt::entity entity)
{
	auto& motion = registry.get<Motion>(entity);
	std::vector<vec2> points;

	if (registry.has<LaserBeam>(entity)) {
		float x_direction = cos(motion.angle) * 125;
		float y_direction = sin(motion.angle) * 125;

		points.push_back(motion.position + vec2(x_direction, y_direction));
		points.push_back(motion.position - vec2(x_direction, y_direction));
	}
	else if (registry.has<IceField>(entity)) {
		for (float i = 0; i < 2 * PI; i += PI / 4)
		{
			points.push_back(motion.position + 148.f * vec2(cos(i), sin(i)));
		}
	}
	else {
		points.push_back(motion.position + motion.boundingbox / 2.f);
		points.push_back(motion.position + vec2(motion.boundingbox.x, -motion.boundingbox.y) / 2.f);
		points.push_back(motion.position - motion.boundingbox / 2.f);
		points.push_back(motion.position + vec2(-motion.boundingbox.x, motion.boundingbox.y) / 2.f);
	}
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
	std::vector<vec2> polygon_a = get_box_vertices(entity1);
	std::vector<vec2> polygon_b = get_box_vertices(entity2);
	
	std::vector<vec2> poly_a_norms = get_norms(polygon_a);
	std::vector<vec2> poly_b_norms = get_norms(polygon_b);

	return checkProjection(polygon_a, polygon_b, poly_a_norms, poly_b_norms);
}

// Precise Collisions with two convex objects 
bool preciseCollides(entt::entity spider, entt::entity projectile)
{
	auto& spider_motion = registry.get<Motion>(spider);

	std::vector<vec2> projectile_vertices = get_box_vertices(projectile);
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
	// Move entities based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	update_projectiles(elapsed_ms);

	for(auto entity: registry.view<Motion>()) {
	    auto& motion = registry.get<Motion>(entity);
		if (registry.has<Monster>(entity)) {
			motion.position += step_seconds * motion.velocity * registry.get<Monster>(entity).speed_multiplier;
		}
		else {
			motion.position += step_seconds * motion.velocity;
		}
	}

	// Check for collisions between all moving entities

	auto view_motion = registry.view<Motion>();

	for (unsigned int i = 0; i < view_motion.size(); i++)
	{
		for (unsigned int j = i + 1; j < view_motion.size(); j++)
		{
			Motion& motion_i = registry.get<Motion>(view_motion[i]);
			entt::entity entity_i = view_motion[i];
			Motion& motion_j = registry.get<Motion>(view_motion[j]);
			entt::entity entity_j = view_motion[j];

			// If either entity is already dying, do not consider collisions
			if (registry.has<EntityDeath>(entity_i) || registry.has<EntityDeath>(entity_j)) continue;

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
		for (auto [entity, motion] : view_motion.each())
		{
			if (registry.has<Rig>(entity)) {
				DebugSystem::display_rig_vertices(entity);
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

void PhysicsSystem::update_projectiles(float elapsed_ms)
{
	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	for (auto entity : registry.view<Snowball>()) {
		auto& motion = registry.get<Motion>(entity);
		motion.angle += 0.2f;
		auto& rock = registry.get<Snowball>(entity);
		if (rock.current_step == rock.bezier_points.size() - 1) {
			continue;
		}
		motion.velocity = 1 / step_seconds * rock.bezier_points[rock.current_step];
		rock.current_step += 1;
	}

	for (auto entity : registry.view<Flamethrower>()) {
		auto& flamethrower = registry.get<Flamethrower>(entity);
		flamethrower.active_timer -= elapsed_ms;
		auto& motion_p = registry.get<Motion>(entity);
		auto& motion_u = registry.get<Motion>(flamethrower.e_unit);
		float x_direction = cos(motion_u.angle) * 60;
		float y_direction = sin(motion_u.angle) * 60;

		motion_p.angle = motion_u.angle + PI;
		motion_p.position = motion_u.position + vec2(x_direction, y_direction);

		if (flamethrower.active_timer < 0)
			registry.destroy(entity);
	}

	for (auto entity : registry.view<LaserBeam>()) {
		auto& laserBeam = registry.get<LaserBeam>(entity);
		laserBeam.active_timer -= elapsed_ms;
		auto& motion_p = registry.get<Motion>(entity);
		auto& motion_m = registry.get<Motion>(laserBeam.e_unit);
		vec2 direction = normalize(motion_m.position - laserBeam.unit_pos);

		motion_p.angle = atan2(direction.y, direction.x);
		motion_p.position = laserBeam.unit_pos + direction * abs(motion_p.scale.x) / 2.f;

		if (laserBeam.active_timer < 0)
			registry.destroy(entity);
	}

	for (auto entity : registry.view<Explosion>()) {
		auto& laserBeam = registry.get<Explosion>(entity);
		laserBeam.active_timer -= elapsed_ms;
		
		if (laserBeam.active_timer < 0)
			registry.destroy(entity);
	}

	for (auto entity : registry.view<IceField>()) {
		auto& icefield = registry.get<IceField>(entity);
		icefield.active_timer -= elapsed_ms;

		if (icefield.active_timer < 0)
			registry.destroy(entity);
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