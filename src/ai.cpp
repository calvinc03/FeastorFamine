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
					motion_h.angle = atan2(opposite, adjacent);
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

bool isWalkable(GridMap& current_map, ivec2 coord)
{
    if (is_inbounds(coord)) {
        int occupancy = current_map.node_matrix[coord.x][coord.y].occupancy;
        return occupancy == GRID_VACANT || occupancy == GRID_FOREST || occupancy == GRID_VILLAGE;
    }
    return false;
}

float get_distance(ivec2 coord1, ivec2 coord2) {
    return length((vec2)(coord1 - coord2));
}

// with diagonals
//int col_neighbor[] = {1, 1, 1, 0, 0, -1, -1, -1};
//int row_neighbor[] = {0, -1, 1, -1, 1, 0, 1, -1};

// no diagonal version
int col_neighbor[] = {1, -1, 0, 0, };
int row_neighbor[] = {0, 0, 1, -1,};

std::vector<ivec2> AISystem::PathFinder::find_path(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord) {
    std::vector<std::vector<bool>> visited(WINDOW_SIZE_IN_COORD.x, std::vector<bool> (WINDOW_SIZE_IN_COORD.y, false));
    std::vector<std::vector<std::tuple<ivec2, float>>> parent(WINDOW_SIZE_IN_COORD.x,
                                                              std::vector<std::tuple<ivec2, float>> (WINDOW_SIZE_IN_COORD.y, std::make_tuple(vec2(-1, -1), -1)));
    std::tuple<ivec2, float> start_qnode = std::make_tuple(start_coord, 0.f);

    std::queue<std::tuple<ivec2, float>> queue;
    visited[start_coord.x][start_coord.y] = true;
    queue.push(start_qnode);

    while (!queue.empty()) {
        std::tuple<ivec2, float> current_qnode = queue.front();

        // current node is the goal node, return path
         if (std::get<0>(current_qnode) == goal_coord) {
             ivec2 coord_pointer = std::get<0>(current_qnode);
             std::vector<ivec2> path_nodes = {coord_pointer};
             while(std::get<1>(parent[coord_pointer.x][coord_pointer.y]) != -1) {
                 coord_pointer = std::get<0>(parent[coord_pointer.x][coord_pointer.y]);
                 path_nodes.emplace_back(coord_pointer);
             }
             std::reverse(path_nodes.begin(), path_nodes.end());
             return path_nodes;
        }
        queue.pop();
        // check neighbors
        for (int i = 0; i < 8; i++) {
            ivec2 next_coord = std::get<0>(current_qnode) + ivec2(row_neighbor[i], col_neighbor[i]);
            if (!isWalkable(current_map, next_coord) || visited[next_coord.x][next_coord.y]) {
                continue;
            }
            std::tuple<ivec2, float> next_qnode = std::make_tuple(next_coord, std::get<1>(current_qnode) + get_distance(next_coord, std::get<0>(current_qnode)));
            visited[next_coord.x][next_coord.y] = true;
            queue.emplace(next_qnode);
            parent[next_coord.x][next_coord.y] = current_qnode;
        }
    }
    // a path does not exist between start and end. should not reach this point
    assert(false);
    return std::vector<ivec2>({start_coord});
}