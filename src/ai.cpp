// internal
#include "ai.hpp"
#include "entt.hpp"
#include "common.hpp"
#include <projectile.hpp>
#include <render_components.hpp>
#include <cstring>
#include <queue>
#include <BehaviorTree.cpp>
#include <BehaviorTree.hpp>
#include <monsters/spring_boss.hpp>
#include <monsters/summer_boss.hpp>
#include <monsters/fall_boss.hpp>
#include <monsters/winter_boss.hpp>
#include <monsters/final_boss.hpp>
#include <monsters/fireball_boss.hpp>
#include <units/unit.hpp>

const size_t BULLET_UPGRADE = 2;


AISystem::AISystem(PhysicsSystem* physics) 
{
	this->physics = physics;
	this->physics->attach(this);
}

AISystem::~AISystem() {}

void AISystem::step(float elapsed_ms)
{
	//(void)elapsed_ms; // placeholder to silence unused warning until implemented
	
	for (auto& unit : registry.view<Unit>()) {
		auto hunter = entt::to_entity(registry, unit);
		auto& placeable_unit = registry.get<Unit>(hunter);

		// TODO: scale projectile spawn with the current speed of the game 
		placeable_unit.next_projectile_spawn -= elapsed_ms;
	}

	// Attack mobs if in range of hunter
    for (auto unit : registry.view<Unit>()) {
        auto hunter = entt::to_entity(registry, unit);
        auto& motion_h = registry.get<Motion>(hunter);
        auto& placeable_unit = registry.get<Unit>(hunter);

        Motion motion_monster;
        float min_distance = INFINITY;
        for (auto monster : registry.view<Monster>()) {
            auto animal = entt::to_entity(registry, monster);
            auto& motion_m = registry.get<Motion>(animal);

            float distance_to_village = length(motion_m.position - coord_to_pixel(VILLAGE_COORD));
            float distance_to_hunter = length(motion_m.position - motion_h.position);
            if (distance_to_village < min_distance && distance_to_hunter <= placeable_unit.attack_range) {
                min_distance = length(motion_m.position - coord_to_pixel(VILLAGE_COORD));
                motion_monster = motion_m;
            }
		}
        if (min_distance == INFINITY) continue;

        float opposite = motion_monster.position.y - motion_h.position.y;
        float adjacent = motion_monster.position.x - motion_h.position.x;
        float distance = sqrt(pow(adjacent, 2) + pow(opposite, 2));
        motion_h.angle = atan2(opposite, adjacent);

        if (placeable_unit.next_projectile_spawn < 0.f) {
            placeable_unit.next_projectile_spawn = placeable_unit.attack_interval_ms;
            if (placeable_unit.upgrades >= BULLET_UPGRADE) {
                Projectile::createProjectile(motion_h.position, motion_monster.position, placeable_unit.damage);
            }
            else {
                RockProjectile::createRockProjectile(motion_h.position, motion_monster.position, placeable_unit.damage);
            }
        }
	}
}

void AISystem::updateProjectileMonsterCollision(entt::entity projectile, entt::entity monster)
{
	if (registry.has<Monster>(monster))
	{
		auto& boss = registry.get<Monster>(monster);
		if (!boss.hit)
		{
			boss.hit = true;

			if (boss.speed_multiplier > 1)
			{
				boss.sprite = boss.run_sprite;
				boss.frames = boss.run_frames;
			}

			auto& motion = registry.get<Motion>(monster);

		}
	}
    else {
		auto& hit_reaction = registry.get<HitReaction>(monster);
		hit_reaction.hit_bool = true;

		auto& motion = registry.get<Motion>(monster);
	}
}

float get_distance(ivec2 coord1, ivec2 coord2) {
    return length((vec2)(coord1 - coord2));
}

std::vector<ivec2> AISystem::MapAI::findPathBFS(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2), int neighbor_type) {
    std::vector<ivec2> neighbors = neighbor_map.at(neighbor_type);
    std::vector<std::vector<bool>> visited(MAP_SIZE_IN_COORD.x, std::vector<bool> (MAP_SIZE_IN_COORD.y, false));
    std::vector<std::vector<ivec2>> parent(MAP_SIZE_IN_COORD.x,std::vector<ivec2> (MAP_SIZE_IN_COORD.y, vec2(-1, -1)));

    std::queue<ivec2> queue;
    visited[start_coord.x][start_coord.y] = true;
    queue.push(start_coord);

    while (!queue.empty()) {
        ivec2 current_coord = queue.front();

        // current node is the goal node, return path
         if (current_coord == goal_coord) {
             std::vector<ivec2> path_nodes = {current_coord};
             while(current_coord != start_coord) {
                 current_coord = parent[current_coord.x][current_coord.y];
                 path_nodes.emplace_back(current_coord);
             }
             std::reverse(path_nodes.begin(), path_nodes.end());
             return path_nodes;
        }
        queue.pop();
        // check neighbors
        for (int i = 0; i < neighbors.size(); i++) {
            ivec2 nbr_coord = current_coord + neighbors.at(i);
            if (!is_valid(current_map, nbr_coord) || visited[nbr_coord.x][nbr_coord.y]) {
                continue;
            }
            visited[nbr_coord.x][nbr_coord.y] = true;
            queue.emplace(nbr_coord);
            parent[nbr_coord.x][nbr_coord.y] = current_coord;
        }
    }
    // a path does not exist between start and end.
    // should NOT reach this point for monster travel path with random map generation is in place
    return std::vector<ivec2>();
}

struct search_node {
    ivec2 coord;
    float c;
    float h;
    float f;
    search_node(ivec2 coord, float c, float h) {
        this->coord = coord;
        this->c = c;
        this->h = h;
        this->f = c + h;
    }
};

// diagonal distance
float heuristic_diagonal_dist(GridMap& current_map, int monster_type, ivec2 from_coord, ivec2 to_coord) {
    float dx = abs(from_coord.x - to_coord.x);
    float dy = abs(from_coord.y - to_coord.y);
    float unit_move_cost = 1;
    // if calculating unit move (as opposed to heuristic over multiple grids), get the corresponding cost of that terrain
    if (length((vec2)(from_coord - to_coord)) > sqrt(2)) {
        unit_move_cost = monster_move_cost.at({monster_type, current_map.getNodeAtCoord(to_coord).terrain});
    }
    float diag_cost = sqrt(2 * unit_move_cost);
    return unit_move_cost * (dx + dy) + (diag_cost - 2 * unit_move_cost) * min(dx, dy);
}

std::vector<ivec2> AISystem::MapAI::findPathAStar(GridMap& current_map, int monster_type, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2), int neighbor_type) {
    std::vector<ivec2> neighbors = neighbor_map.at(neighbor_type);
    std::vector<std::vector<search_node>> parent(MAP_SIZE_IN_COORD.x,std::vector<search_node> (MAP_SIZE_IN_COORD.y, {ivec2(-1, -1), INFINITY, INFINITY}));
    std::vector<search_node> open;
    std::vector<search_node> closed;

    search_node start_node = {start_coord, 0, heuristic_diagonal_dist(current_map, monster_type, start_coord, goal_coord)};
    open.emplace_back(start_node);

    while (!open.empty()) {
        // get node with smallest f on open list
        auto min_iter = open.begin();
        search_node current_node = open.front();
        for (auto iter = open.begin(); iter != open.end(); iter++) {
            if ((*iter).f < min_iter->f) {
                min_iter = iter;
            }
        }
        current_node = *min_iter;
        open.erase(min_iter);

        // check neighbors
        for (ivec2 neighbor : neighbors) {
            ivec2 nbr_coord = current_node.coord + neighbor;
            if (!is_valid(current_map, nbr_coord)) {
                continue;
            }
            // current node is the goal node, return path
            if (nbr_coord == goal_coord) {
                std::vector<ivec2> path_nodes = {nbr_coord, current_node.coord};
                while(current_node.coord != start_coord) {
                    current_node = parent[current_node.coord.x][current_node.coord.y];
                    path_nodes.emplace_back(current_node.coord);
                }
                std::reverse(path_nodes.begin(), path_nodes.end());
                return path_nodes;
            }

            search_node nbr_node = {nbr_coord, current_node.c + heuristic_diagonal_dist(current_map, monster_type, current_node.coord, nbr_coord),
                                    heuristic_diagonal_dist(current_map, monster_type, nbr_coord, goal_coord)};

            // skip nbr if already exists in open or closed list with lower f = c + h
            bool exists_lower = false;
            for (search_node node : open) {
                if (node.coord == nbr_coord && node.f < nbr_node.f) {
                    exists_lower = true;
                    break;
                }
            }
            for (search_node node : closed) {
                if (node.coord == nbr_coord && node.f < nbr_node.f) {
                    exists_lower = true;
                    break;
                }
            }
            if (exists_lower) {
                continue;
            }
            parent[nbr_coord.x][nbr_coord.y] = {current_node.coord, current_node.c, current_node.h};
            open.emplace_back(nbr_node);
        }
        closed.emplace_back(current_node);
    }
    // a path does not exist between start and end.
    // should NOT reach this point for monster travel path with random map generation is in place
    return std::vector<ivec2>();
}

int get_random_weather_terrain() {
    std::map<int, float> weather_terrain_default_prob = {
            {TERRAIN_MUD,      1},
            {TERRAIN_PUDDLE,   1},
            {TERRAIN_DRY,      1},
            {TERRAIN_FIRE,     1},
            {TERRAIN_ICE,      1},
    };

    // multiply each prob with a rand number and weather multiplier
    for (auto& [terrain, prob] : weather_terrain_default_prob) {
        prob = uniform_dist(rng) * weather_terrain_prob_multiplier.at(std::pair(weather,terrain));
    }

    vec2 max_prob(-1, -1);

    for (auto& [terrain, prob] : weather_terrain_default_prob) {
        if (prob > max_prob.y){
            max_prob.x = terrain;
            max_prob.y = prob;
        }
    }

    // approx 30% tiles will be weather tiles
    if (max_prob.y > 0.7) {
        return max_prob.x;
    }

    return TERRAIN_DEFAULT;
}

void AISystem::MapAI::setRandomMapWeatherTerrain(GridMap& map) {
    for (int i = 0; i < MAP_SIZE_IN_COORD.x; i++) {
        for (int j = 0; j < MAP_SIZE_IN_COORD.y; j++) {
            int weather_terrain = get_random_weather_terrain();
            if (weather_terrain != TERRAIN_DEFAULT && map.getNodeAtCoord(ivec2(i,j)).terrain != TERRAIN_PAVEMENT) {
                map.setGridTerrain(ivec2(i, j), weather_terrain);
            }
        }
    }
}

void AISystem::MapAI::setRandomWeatherTerrain(GridMap &map, int max_rerolls) {
    for (int i = 0; i < max_rerolls; i++) {
        ivec2 random_coord(uniform_dist(rng)*MAP_SIZE_IN_COORD.x,  uniform_dist(rng)*MAP_SIZE_IN_COORD.y);
        auto& node = map.getNodeAtCoord(random_coord);
        if (node.terrain != TERRAIN_PAVEMENT) {
            map.setGridTerrain(random_coord, get_random_weather_terrain());
        }
    }
}

// set path
bool is_valid_terrain_path(GridMap& current_map, ivec2 coord)
{
    if (is_inbounds(coord)) {
        int terrain = current_map.getNodeAtCoord(coord).terrain;
        int occupancy = current_map.getNodeAtCoord(coord).occupancy;
        return terrain != TERRAIN_PAVEMENT && occupancy == OCCUPANCY_VACANT;
    }
    return false;
}


int get_random_direct_index () {
    float num = uniform_dist(rng);
    // 10% chance for UP and LEFT
    if (num < 0.1) {
        return 0;
    }
    if (num < 0.2) {
        return 1;
    }
    // 40% chance for DOWN and RIGHT
    if (num < 0.6) {
        return 2;
    }
    return 3;
}

ivec2 get_random_neighbor(GridMap& map, ivec2 current_coord, ivec2 end_coord, int neighbor_type) {
    std::vector<ivec2> neighbors = neighbor_map.at(neighbor_type);
    bool visited[4] = {false, false, false, false};
    int count = 0;

    int index = get_random_direct_index();
    while (count < 4) {
        // randomly roll a direction until we get an unvisited direction
        while (visited[index]) {
            index = get_random_direct_index();;
        }
        count++;
        // return this neighbor if it's in bounds and has an open path to village (don't want to block itself in)
        ivec2 nbr_coord = current_coord + neighbors.at(index);
        visited[index] = true;
        if (nbr_coord == end_coord ||
            (is_inbounds(nbr_coord) && map.getNodeAtCoord(nbr_coord).terrain != TERRAIN_PAVEMENT
             && !AISystem::MapAI::findPathBFS(map, nbr_coord, VILLAGE_COORD, is_valid_terrain_path, neighbor_type).empty())) {
            return nbr_coord;
        }
    }
    // cannot find a valid neighbor; should NOT happen
    assert(false);
    return ivec2(0,0);
}

void AISystem::MapAI::setRandomMapPathTerran(GridMap& map, ivec2 start_coord, ivec2 end_coord, int terrain) {


    map.setGridTerrain(start_coord, terrain);

    // randomly step toward end_coord
    while (start_coord != end_coord) {
        start_coord = get_random_neighbor(map, start_coord, end_coord, DIRECT_NBRS);
        map.setGridTerrain(start_coord, terrain);
    }
}

std::shared_ptr<BTSelector> AISystem::MonstersAI::createBehaviorTree() {
	std::shared_ptr <BTNode> donothing = std::make_unique<DoNothing>();
	std::shared_ptr <BTNode> grow = std::make_unique<Grow>();
	//std::shared_ptr <BTNode> stop = std::make_unique<Stop>();
	std::shared_ptr <BTNode> run = std::make_unique<Run>();
    std::shared_ptr <BTNode> knockback = std::make_unique<Knockback>();

	std::shared_ptr <BTIfCondition> conditional_donothing = std::make_unique<BTIfCondition>(
		donothing,
		[](entt::entity e) {return registry.has<Mob>(e); }
	);
	std::shared_ptr <BTIfCondition> conditional_grow = std::make_unique<BTIfCondition>(
		grow,
		[](entt::entity e) {return registry.has<FallBoss>(e); }
	);
	/*std::shared_ptr <BTIfCondition> conditional_stop = std::make_unique<BTIfCondition>(
		stop,
		[](entt::entity e) {return registry.has<SummerBoss>(e); }
	);*/
	std::shared_ptr <BTIfCondition> conditional_run = std::make_unique<BTIfCondition>(
		run,
		[](entt::entity e) {return registry.has<SpringBoss>(e); }
	);
    std::shared_ptr <BTIfCondition> conditional_run_sum = std::make_unique<BTIfCondition>(
        run,
        [](entt::entity e) {return registry.has<SummerBoss>(e); }
    );
    std::shared_ptr <BTIfCondition> conditional_knockback = std::make_unique<BTIfCondition>(
        knockback,
        [](entt::entity e) {return registry.has<WinterBoss>(e); }
    );

	std::vector<std::shared_ptr<BTIfCondition>> cond_nodes;
	cond_nodes.push_back(conditional_donothing);
	cond_nodes.push_back(conditional_grow);
	//cond_nodes.push_back(conditional_stop);
	cond_nodes.push_back(conditional_run);
    cond_nodes.push_back(conditional_run_sum);
    cond_nodes.push_back(conditional_knockback);

	std::shared_ptr<BTSelector> selector = std::make_unique<BTSelector>(cond_nodes);
    
    std::shared_ptr <BTIfCondition> conditional_collided = std::make_unique<BTIfCondition>(
        selector,
        [](entt::entity e) {return registry.get<Monster>(e).collided; }
    );

    std::shared_ptr<BTNode> walk = std::make_unique<Walk>();
    std::shared_ptr <BTIfCondition> conditional_walk = std::make_unique<BTIfCondition>(
        walk,
        // return true for all monsters except the final boss and its fireballs
        [](entt::entity e) {return !(registry.has<FinalBoss>(e) || registry.has<FireballBoss>(e)); }
    );

    std::shared_ptr<BTNode> final_boss_walk = std::make_unique<Dragon>();
    std::shared_ptr <BTIfCondition> conditional_final_boss_walk = std::make_unique<BTIfCondition>(
        final_boss_walk,
        [](entt::entity e) {return registry.has<FinalBoss>(e); }
    );

    std::shared_ptr<BTNode> fireball_walk = std::make_unique<Fireball>();
    std::shared_ptr <BTIfCondition> conditional_fireball_walk = std::make_unique<BTIfCondition>(
        fireball_walk,
        [](entt::entity e) {return registry.has<FireballBoss>(e); }
    );

    std::vector<std::shared_ptr<BTIfCondition>> walk_or_collide;
    walk_or_collide.push_back(conditional_collided);
    walk_or_collide.push_back(conditional_walk);
    walk_or_collide.push_back(conditional_final_boss_walk);
    walk_or_collide.push_back(conditional_fireball_walk);

    std::shared_ptr<BTSelector> root = std::make_unique<BTSelector>(walk_or_collide);

	return root;
}