#include "BehaviorTree.hpp"
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

// A composite node that loops through all children and exits when one fails
class BTAndSequence : public BTNode {
public:
	BTAndSequence(std::vector<std::shared_ptr<BTNode>> children)
		: m_children(std::move(children)) {
		m_index = std::map<entt::entity, int>();
	}

private:
	void init(entt::entity e)
	{
		m_index[e] = 0;
		assert(m_index[e] < m_children.size());
		// initialize the first child
		const auto& child = m_children[m_index[e]];
		assert(child);
		child->init(e);
	}

	BTState process(entt::entity e) override {
		if (m_index[e] >= m_children.size())
			return BTState::Success;

		// process current child
		const auto& child = m_children[m_index[e]];
		assert(child);
		BTState state = child->process(e);

		// select a new active child and initialize its internal state
		if (state == BTState::Success) {
			++m_index[e];
			if (m_index[e] >= m_children.size()) {
				return BTState::Success;
			}
			else {
				const auto& nextChild = m_children[m_index[e]];
				assert(nextChild);
				nextChild->init(e);
				return BTState::Running;
			}
		}
		else {
			return state;
		}
	}

	std::map<entt::entity, int> m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

// A general decorator with lambda condition
class BTIfCondition : public BTNode
{
public:
	BTIfCondition(std::shared_ptr<BTNode> child, std::function<bool(entt::entity e)> condition)
		: m_child(std::move(child)), m_condition(condition) {
	}

	virtual void init(entt::entity e) {
		m_child->init(e);
	}

	virtual BTState process(entt::entity e) {
		if (m_condition(e))
			return m_child->process(e);
		else
			return BTState::Success;
	}
private:
	std::shared_ptr<BTNode> m_child;
	std::function<bool(entt::entity e)> m_condition;
};

// Leaf node examples
class TakeNextStep : public BTNode {
public:
	TakeNextStep() noexcept {
		
	}

private:
	void init(entt::entity e) override {
		
	}

	BTState process(entt::entity e) override {
		auto& monster = registry.get<Monster>(e);
		auto& motion = registry.get<Motion>(e);
		auto& current_path_coord = monster_path_coords.at(monster.current_path_index);

		// check that the monster is indeed within the current path node
		ivec2 coord = pixelToCoord(motion.position);

		// if we are on the last node, stop the monster and remove entity
		// TODO: make disappearance fancier
		if (pixelToCoord(motion.position) == VILLAGE_COORD
			|| monster.current_path_index >= monster_path_coords.size() - 1) {
			// notify health
			// health -= monster.damage;
			motion.velocity *= 0;
			registry.destroy(e);
		}

		ivec2 next_path_coord = monster_path_coords.at(monster.current_path_index + 1);
		vec2 move_direction = normalize((vec2)(next_path_coord - current_path_coord));
		motion.velocity = length(motion.velocity) * move_direction;
		motion.angle = atan(move_direction.y / move_direction.x);

		//// if we will reach the next node in the next step, increase path index for next step
		//ivec2 next_step_coord = pixelToCoord(motion.position + (elapsed_ms / 1000.f) * motion.velocity);
		//if (next_step_coord == next_path_coord) {
		//	monster.current_path_index++;
		//}

		// if we will reach the middle of next node in the next step, increase path index for next step
		vec2 next_step_pos = motion.position + (elapsed_ms / 1000.f) * motion.velocity;
		vec2 next_node_pos = coordToPixel(next_path_coord);
		if (abs(next_step_pos.x - next_node_pos.x) < 10 && abs(next_step_pos.y - next_node_pos.y) < 10) {
			monster.current_path_index++;
		}

		if (DebugSystem::in_debug_mode)
		{
			DebugSystem::createDirectedLine(coordToPixel(current_path_coord), coordToPixel(next_path_coord), 5);
		}
	}
};