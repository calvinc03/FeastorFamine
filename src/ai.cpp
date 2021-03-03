// internal
#include "ai.hpp"
#include "entt.hpp"
#include "common.hpp"
#include <projectile.hpp>
#include <render_components.hpp>
#include <cstring>
#include <queue>

AISystem::AISystem(PhysicsSystem* physics) 
{
	this->physics = physics;
	this->physics->attach(this);
}

AISystem::~AISystem() {}

void AISystem::step(float elapsed_ms)
{
	//(void)elapsed_ms; // placeholder to silence unused warning until implemented
	
	// Attack mobs if in range of hunter
	for (auto monster : registry.view<Monster>()) {
		auto animal = entt::to_entity(registry, monster);
		auto& motion_m = registry.get<Motion>(animal);
		for (auto unit : registry.view<Unit>()) {
			auto hunter = entt::to_entity(registry, unit);
			auto& motion_h = registry.get<Motion>(hunter);
			auto& placeable_unit = registry.get<Unit>(hunter);

			float opposite = motion_m.position.y - motion_h.position.y;
			float adjacent = motion_m.position.x - motion_h.position.x;
			float distance = sqrt(pow(adjacent, 2) + pow(opposite, 2));

			if (distance <= placeable_unit.attack_range) {
				// TODO: scale projectile spawn with the current speed of the game 
				placeable_unit.next_projectile_spawn -= elapsed_ms;
				if (placeable_unit.next_projectile_spawn < 0.f) {
					placeable_unit.next_projectile_spawn = FIRING_RATE;
					Projectile::createProjectile(motion_h.position, vec2(adjacent, opposite) / distance, placeable_unit.damage);
				}

			}
		}
	}
}

void AISystem::updateCollisions(entt::entity entity_i, entt::entity entity_j)
{
	if (registry.has<Projectile>(entity_i)) {
		if (registry.has<Boss>(entity_j))
		{
			auto& boss = registry.get<Boss>(entity_j);
			if (!boss.hit)
			{
				boss.hit = true;

				if (boss.speed_multiplier > 1)
				{
					boss.sprite = boss.run_sprite;
					boss.frames = boss.run_frames;
				}

				auto& motion = registry.get<Motion>(entity_j);
				motion.velocity *= boss.speed_multiplier;

			}
		}
		if (registry.has<Monster>(entity_j)) {
			auto& hit_reaction = registry.get<HitReaction>(entity_j);
			hit_reaction.hit_bool = true;

			// increase velocity of monsters that are hit - removed
			auto& motion = registry.get<Motion>(entity_j);
			//motion.velocity.x += motion.velocity.x > 0 ? 100.f : 0;
			//motion.velocity.y += motion.velocity.y > 0 ? 100.f : 0;
		}
	}
}

bool isValidPosition(ivec2 coord)
{
    return (coord.x >= 0) && (coord.x < WINDOW_SIZE_IN_COORD.y) &&
           (coord.y >= 0) && (coord.y < WINDOW_SIZE_IN_COORD.x);
}

float get_distance(GridNode node1, GridNode node2) {
    return length((vec2)(node1.coord - node2.coord));
}

int col_neighbor[] = {1, 1, 1, 0, 0, -1, -1, -1};
int row_neighbor[] = {1, 0, -1, 1, -1, 1, 0, -1};

std::vector<GridNode> AISystem::PathFinder::find_path(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord) {
    bool visited[WINDOW_SIZE_IN_COORD.x][WINDOW_SIZE_IN_COORD.y] = {};
    GridNode start_node = current_map.node_matrix[start_coord.x][start_coord.y];
    QueueNode min_path_qnode = {start_node, nullptr, INFINITY};
    std::vector<QueueNode> qnode_storage = {};
    qnode_storage.reserve(current_map.node_matrix.size());

    // visit start node and add it to queue
    visited[start_coord.x][start_coord.y] = true;
    QueueNode s = {start_node, nullptr, 0};
    std::queue<QueueNode> queue;
    queue.push(s);

    while (!queue.empty()) {
        QueueNode current_qnode = queue.front();
        qnode_storage.emplace_back(current_qnode);

        // current node is the goal node, check and update shortest path
         if (current_qnode.node.coord == goal_coord && current_qnode.cost < min_path_qnode.cost) {
             min_path_qnode = current_qnode;
        }

        queue.pop();

        // check neighbors
        for (int i = 0; i < 8; i++) {
            ivec2 next_coord = current_qnode.node.coord + ivec2(row_neighbor[i], col_neighbor[i]);
            if (!isValidPosition(next_coord) || visited[next_coord.x][next_coord.y]) {
                continue;
            }

            visited[next_coord.x][next_coord.y] = true;
            GridNode next_node = current_map.node_matrix[next_coord.x][next_coord.y];

            QueueNode next_qnode = {next_node, &qnode_storage.back(), current_qnode.cost + get_distance(next_node, current_qnode.node)};
            queue.push(next_qnode);
        }
    }
    // verify that a path can be found
    assert(min_path_qnode.cost != INFINITY);

    // trace backward to start node and return
    std::vector<GridNode> path_nodes = {};
    while(min_path_qnode.node.coord != start_coord) {
        path_nodes.emplace_back(min_path_qnode.node);
        min_path_qnode = *min_path_qnode.parent;
    }
    path_nodes.emplace_back(start_node);
    std::reverse(path_nodes.begin(), path_nodes.end());
    return path_nodes;
}