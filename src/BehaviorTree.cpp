#include "entt.hpp"
#include "common.hpp"

#include <cassert>
#include <iostream>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <debug.hpp>
#include <BehaviorTree.hpp>
#include <monsters/mob.hpp>
#include <world.hpp>
#pragma once

// A general decorator with lambda condition
class BTIfCondition : public BTNode
{
public:
	BTIfCondition(std::shared_ptr<BTNode> child, std::function<bool(entt::entity)> condition)
		: m_child(std::move(child)), m_condition(condition) {
	}

	virtual void init(entt::entity e) override {
		m_child->init(e);
	}

	virtual BTState process(entt::entity e) override {
		if (m_condition(e))
			return m_child->process(e);
		else
			return BTState::Failure;
	}
private:
	std::shared_ptr<BTNode> m_child;
	std::function<bool(entt::entity)> m_condition;
};

class BTSelector : public BTNode {
public:
	BTSelector(std::vector<std::shared_ptr<BTIfCondition>> children) :
		m_children(std::move(children)) {}

	void init(entt::entity e) override {
		for (auto child : m_children) {
			child->init(e);
		}
	}

	BTState process(entt::entity e) override {
		for (auto child : m_children) {
			BTState state = child->process(e);
			if (state == BTState::Failure) continue;
			else {
				return state;
			}
		}
		return BTState::Failure;
	}
private:
	std::vector<std::shared_ptr<BTIfCondition>> m_children;
};

// Leaf nodes
class DoNothing : public BTNode {
public:
	void init(entt::entity e) override {
	}

	BTState process(entt::entity e) override {
		auto& monster = registry.get<Monster>(e);
		monster.collided = false;
		return BTState::Moving;
	}
};

class Grow : public BTNode {
public:
	Grow() noexcept {
		visited = std::map<entt::entity, bool>();
	}

	void init(entt::entity e) override {
		visited[e] = true;
	}

	BTState process(entt::entity e) {
		auto& motion = registry.get<Motion>(e);
		auto& monster = registry.get<Monster>(e);

		if (visited[e]) {
			motion.scale *= 1.3;
			monster.damage *= 2;
			visited[e] = false;
			auto& monster = registry.get<Monster>(e);
			monster.collided = false;
			return BTState::Moving;
		}
		return BTState::Failure;
	}
private:
	std::map<entt::entity, bool> visited;
};

class Stop : public BTNode {
public:
	Stop() noexcept {
		frames_to_stop = std::map<entt::entity, int>();
		vel = std::map<entt::entity, vec2>();
	}

	void init(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		frames_to_stop[e] = 40;
		vel[e] = motion.velocity;
	}

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);

		if (frames_to_stop[e] > 0) {
			frames_to_stop[e] -= 1;
			motion.velocity *= 0;
			return BTState::Stopped;
		}

		else {
			motion.velocity = vel[e];
			return BTState::Moving;
		}
	}
private:
	std::map<entt::entity, int> frames_to_stop;
	std::map<entt::entity, vec2> vel;
};

class Run : public BTNode {
public:
	Run() noexcept {
		visited = std::map<entt::entity, bool>();
	}

	void init(entt::entity e) {
		visited[e] = true;
	} 

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		if (visited[e]) {
			motion.velocity *= 2;
			visited[e] = false;
			auto& monster = registry.get<Monster>(e);
			monster.collided = false;
			return BTState::Moving;
		}
		return BTState::Failure;
	}
private:
	std::map<entt::entity, bool> visited;
};

class Knockback : public BTNode {
public:
	Knockback() noexcept { }

	void init(entt::entity e) override { }

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		auto& monster = registry.get<Monster>(e);
		if (monster.collided) {
			motion.position -= 0.25f * motion.velocity;
			monster.collided = false;
		}
		return BTState::Failure;
	}
};

class Dragon : public BTNode { // todo probably delete from here and ai
public:
	Dragon() noexcept {}

	void init(entt::entity e) override { }

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);

		motion.position += motion.velocity;

		// stop when dragon is at edge of screen
		if (motion.position.x > 245) {
			// if velocity is 0 it faces upwards??
			motion.velocity.x = 0.01f;
		}

		return BTState::Dragon;
	}
};

class Fireball : public BTNode {
public:
	Fireball() noexcept {}

	void init(entt::entity e) override { }
	
	BTState process(entt::entity e) override {
		auto& monster = registry.get<Monster>(e);
		auto& motion = registry.get<Motion>(e);

		motion.position += motion.velocity;

		if (motion.position.x + 100 >= coord_to_pixel(VILLAGE_COORD).x) {
			std::cout << "hit the village with a fireball" << std::endl;
			WorldSystem::health -= monster.damage;
			motion.velocity *= 0;
			registry.destroy(e);
		}

		return BTState::Fireball;
	}
};



class Walk : public BTNode {
public:
	Walk() noexcept {}

	void init(entt::entity e) override { }

	BTState process(entt::entity e) override {
		increment_monster_step(e);
		return BTState::Moving;
	}
};

void increment_monster_step(entt::entity entity) {
	auto& monster = registry.get<Monster>(entity);
	auto& motion = registry.get<Motion>(entity);
	auto& current_path_coord = monster.path_coords.at(monster.current_path_index);

	// check that the monster is indeed within the current path node
	ivec2 coord = pixel_to_coord(motion.position);

	// if we are on the last node, stop the monster and remove entity
	// TODO: make disappearance fancier
	if (pixel_to_coord(motion.position) == VILLAGE_COORD
		|| monster.current_path_index >= monster.path_coords.size() - 1) {
		WorldSystem::health -= monster.damage;
		motion.velocity *= 0;
		registry.destroy(entity);
		return;
	}

	assert(monster.path_coords[monster.current_path_index] == current_path_coord);

	ivec2 next_path_coord = monster.path_coords.at(monster.current_path_index + 1);
	vec2 next_step_position = motion.position + (15 / 1000.f) * motion.velocity;
	ivec2 next_step_coord = pixel_to_coord(next_step_position);

	// change direction if reached the middle of the this node
	if (abs(length(coord_to_pixel(current_path_coord) - motion.position)) <= length(motion.velocity) * ELAPSED_MS / 1000.f) {
		vec2 move_direction = normalize((vec2)(next_path_coord - current_path_coord));
		motion.velocity = length(motion.velocity) * move_direction;
		motion.angle = atan(move_direction.y / move_direction.x);
		monster.current_path_index++;
	}

	/*if (next_step_coord == next_path_coord) {
		monster.current_path_index++;
	}*/

	if (DebugSystem::in_debug_mode)
	{
		DebugSystem::createDirectedLine(motion.position, coord_to_pixel(current_path_coord), 5);
		DebugSystem::createDirectedLine(coord_to_pixel(current_path_coord), coord_to_pixel(next_path_coord), 5);
	}
}