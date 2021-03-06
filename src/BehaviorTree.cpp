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
#include <mob.hpp>
#pragma once

// A composite node that loops through all children and exits when one fails
//class BTAndSequence : public BTNode {
//public:
//	BTAndSequence(std::vector<std::shared_ptr<BTNode>> children)
//		: m_children(std::move(children)) {
//		m_index = std::map<entt::entity, int>();
//	}
//
//private:
//	void init(entt::entity e)
//	{
//		m_index[e] = 0;
//		assert(m_index[e] < m_children.size());
//		// initialize the first child
//		const auto& child = m_children[m_index[e]];
//		assert(child);
//		child->init(e);
//	}
//
//	BTState process(entt::entity e) override {
//		if (m_index[e] >= m_children.size())
//			return BTState::Success;
//
//		// process current child
//		const auto& child = m_children[m_index[e]];
//		assert(child);
//		BTState state = child->process(e);
//
//		// select a new active child and initialize its internal state
//		if (state == BTState::Success) {
//			++m_index[e];
//			if (m_index[e] >= m_children.size()) {
//				return BTState::Success;
//			}
//			else {
//				const auto& nextChild = m_children[m_index[e]];
//				assert(nextChild);
//				nextChild->init(e);
//				return BTState::Running;
//			}
//		}
//		else {
//			return state;
//		}
//	}
//
//	std::map<entt::entity, int> m_index;
//	std::vector<std::shared_ptr<BTNode>> m_children;
//};

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


class onCollisionSelector : public BTNode {
public:
	onCollisionSelector(std::vector<std::shared_ptr<BTIfCondition>> children) :
		m_children(std::move(children)) {}

	void init(entt::entity e) override {
		for (auto child : m_children) {
			child->init(e);
		}
	}

	BTState process(entt::entity e) override {
		auto& monster = registry.get<Monster>(e);
		if (monster.collided) {
			for (auto child : m_children) {
				BTState state = child->process(e);

				if (state == BTState::Failure) continue;
				else {
					return state;
				}
			}
			return BTState::Failure;
		}
		else {
			return BTState::Moving;
		}
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
		// essentially does nothing
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
			monster.health += 200;
			visited[e] = false;
		}

		return BTState::Moving;
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
		}

		return BTState::Moving;
	}
private:
	std::map<entt::entity, bool> visited;
};

class Knockback : public BTNode {
public:
	Knockback() noexcept {
		visited = std::map<entt::entity, bool>();
	}

	void init(entt::entity e) override { visited[e] = true;  }

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);
		if (visited[e]) {
			motion.position -= 0.5f * motion.velocity;
			visited[e] = false;
		}

		return BTState::Moving;
	}
private:
	std::map<entt::entity, bool> visited;
};