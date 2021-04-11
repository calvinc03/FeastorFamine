#include "entt.hpp"
#include "common.hpp"
#include "config/ai_config.hpp"
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
#include <rig.hpp>
#include <monsters/spring_boss.hpp>
#include <units/robot.hpp>
#include <units/exterminator.hpp>
#include <units/snowmachine.hpp>

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
		// do nothing
		(void)e;
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

class Attack : public BTNode {
public:
    Attack() noexcept {
        vel = std::map<entt::entity, vec2>();
        next_attack = std::map<entt::entity, int>();
        attackable_entities = std::vector<entt::entity>();
    }

    void init(entt::entity e) override {
        auto& motion = registry.get<Motion>(e);
        vel[e] = motion.velocity;
        next_attack[e] = 0;
        first_process = true;
    }

    BTState process(entt::entity e) override {
        auto& motion = registry.get<Motion>(e);

        // add all attackable units to list
        if (first_process) {
            ivec2 coord = pixel_to_coord(motion.position);
            for (ivec2 nbr : neighbor_map.at(DIRECT_NBRS)) {
                ivec2 nbr_coord = coord + nbr;
                if (!is_inbounds(nbr_coord)) {
                    continue;
                }
                auto& node = WorldSystem::current_map.getNodeAtCoord(nbr_coord);
                if (node.occupancy != NONE
                    && registry.has<Unit>(node.occupying_entity)
                    && registry.get<Unit>(node.occupying_entity).health > 0) {
                    attackable_entities.emplace_back(node.occupying_entity);
                }
            }
            first_process = false;
        }

        motion.velocity *= 0;
        if (next_attack[e] > 0) {
            next_attack[e]--;
            return BTState::Attack;
        }

        // attack a unit from attackable list
        auto& monster = registry.get<Monster>(e);
        auto& entity_to_attack = attackable_entities.back();
        auto& unit = registry.get<Unit>(entity_to_attack);
        next_attack[e] = monster.effect_interval;

        // TODO: create on hit and damaged(hp<=0) appearances for unit
        unit.health -= monster.damage;
        
        auto &hit_reaction = registry.get<HitReaction>(entity_to_attack);
        hit_reaction.counter_ms = 750;

        if (unit.health <= 0) {
            attackable_entities.pop_back();
        }

        // continue moving if no more attackables
        if (attackable_entities.empty()) {
            motion.velocity = vel[e];
            first_process = true;
            return BTState::Failure;
        }
        return BTState::Attack;
    }
private:
    std::map<entt::entity, vec2> vel;
    std::map<entt::entity, int> next_attack;
    std::vector<entt::entity> attackable_entities;
    bool first_process;
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

	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}

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

	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}

	BTState process(entt::entity e) override {
		auto& motion = registry.get<Motion>(e);

		// stop when dragon is at edge of screen
		if (motion.position.x < 46) {
			motion.position += motion.velocity;
		}

		return BTState::Dragon;
	}
};

class Fireball : public BTNode {
public:
	Fireball() noexcept {}

	void init(entt::entity e) override {
		// do nothing
		(void)e;
	}
	
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

	void init(entt::entity e) override { 
		// do nothing
		(void)e;
	}

	BTState process(entt::entity e) override {
		increment_monster_step(e);
		return BTState::Moving;
	}
};

// helper for increment_monster_step
void remove_unit_entity(entt::entity e_unit)
{
	if (registry.has<Robot>(e_unit))
	{
		auto& robot = registry.get<Robot>(e_unit);
		for (auto projectile : robot.lasers) 
			registry.destroy(projectile);
	}
	else if (registry.has<Exterminator>(e_unit))
	{
		auto& exterminator = registry.get<Exterminator>(e_unit);
		for (auto projectile : exterminator.flamethrowers)
			registry.destroy(projectile);
	}
	else if (registry.has<SnowMachine>(e_unit))
	{
		auto& snowmachine = registry.get<SnowMachine>(e_unit);
		for (auto projectile : snowmachine.snowfields)
			registry.destroy(projectile);
	}

	registry.destroy(e_unit);
}

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
		WorldSystem::deduct_health(monster.damage);
		motion.velocity *= 0;

		if (registry.has<Rig>(entity)) {
			Rig::delete_rig(entity); //rigs have multiple pieces to be deleted
		}
		else {
			registry.destroy(entity);
		}
		return;
	}

	assert(monster.path_coords[monster.current_path_index] == current_path_coord);
	float time_step = (ELAPSED_MS / 1000.f) * WorldSystem::speed_up_factor;
	ivec2 next_path_coord = monster.path_coords.at(monster.current_path_index + 1);
	vec2 next_step_position = motion.position + time_step * motion.velocity * monster.speed_multiplier;
	ivec2 next_step_coord = pixel_to_coord(next_step_position);
    auto& next_node = WorldSystem::current_map.getNodeAtCoord(next_path_coord);
	// change direction if reached the middle of the this node
	if (!monster.current_node_visited) {
		if (length(coord_to_pixel(current_path_coord) - motion.position) <= length(motion.velocity * time_step * monster.speed_multiplier)) {
			vec2 move_direction = normalize((vec2)(next_path_coord - current_path_coord));
			motion.position = coord_to_pixel(current_path_coord);
			motion.velocity = length(motion.velocity) * move_direction;
			motion.angle = atan(move_direction.y / move_direction.x);
			monster.current_node_visited = true;

			if (next_node.occupancy != NONE && next_node.occupancy != FOREST && next_node.occupancy != VILLAGE) {
                if (!registry.has<SpringBoss>(entity) || next_node.occupancy != WALL) {
                    monster.state = ATTACK;
                    monster.sprite = monster.attack_sprite;
                    monster.frames = monster.attack_frames;
                    monster.setSprite(entity);
                }
			}
			//std::cout << "distance to center: " << length(coord_to_pixel(current_path_coord) - motion.position) << "\n\n";
		}
		/*else {
			std::cout << "distance to center: "<< length(coord_to_pixel(current_path_coord) - motion.position) << "\n";
			std::cout << "step size: " << length(motion.velocity * time_step) << "\n\n";
		}*/
	}

    if (monster.state == ATTACK) {
        auto atk_entity = next_node.occupying_entity;
        if (next_node.occupancy == NONE) {
            monster.state = WALK;
            monster.sprite = monster.walk_sprite;
            monster.frames = monster.walk_frames;
            monster.setSprite(entity);
            return;
        }
        auto& atk_unit = registry.get<Unit>(atk_entity);
        if (atk_unit.health <= 0) {
            next_node.setOccupancy(NONE, atk_entity);
			remove_unit_entity(atk_entity);
            return;
        }
        monster.next_attack -= 1;
        if (monster.next_attack < 0) {
            monster.next_attack = monster.effect_interval;

            auto& hit_reaction = registry.get<HitReaction>(atk_entity);
            hit_reaction.counter_ms = hit_reaction.counter_interval;
            hit_reaction.hit_bool = true;
            atk_unit.health -= monster.damage;
            HitPointsText::create_hit_points_text(monster.damage, atk_entity, { 1.f, 0.8, 0.f });
        }
    }

	// increment path index and apply terrain speed multiplier
	if (next_step_coord == next_path_coord) {
		monster.current_path_index++;
		monster.current_node_visited = false;
		terrain_type current_terran = WorldSystem::current_map.getNodeAtCoord(current_path_coord).terrain;
		terrain_type next_terran = next_node.terrain;
		monster.speed_multiplier /= monster_move_speed_multiplier.at({monster.type, current_terran});
		monster.speed_multiplier *= monster_move_speed_multiplier.at({monster.type, next_terran});

        if (monster.type != TALKY_BOI) {
            if (next_node.terrain == TERRAIN_FIRE) {
                monster.next_damage --;
                if (monster.next_damage < 0) {
                    monster.next_damage = monster.effect_interval;
                    int fire_damage = 10;
                    monster.health -= fire_damage;
                    HitPointsText::create_hit_points_text(fire_damage, entity, { 1.f, 0.3, 0.f });
                    auto& hit_reaction = registry.get<HitReaction>(entity);
                    hit_reaction.counter_ms = hit_reaction.counter_interval;
                    hit_reaction.hit_bool = true;
                }
            }
            if (next_node.terrain == TERRAIN_PUDDLE) {
                SlowHitText::create_slow_hit_text(0, entity, {0.f, 1.f, 1.f});
            }
        }
	}

	if (DebugSystem::in_debug_mode)
	{
		DebugSystem::createDirectedLine(motion.position, coord_to_pixel(current_path_coord), 5);
		DebugSystem::createDirectedLine(coord_to_pixel(current_path_coord), coord_to_pixel(next_path_coord), 5);
	}
}