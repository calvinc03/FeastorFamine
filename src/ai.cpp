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
#include <bosses/spring_boss.hpp>
#include <bosses/summer_boss.hpp>
#include <bosses/fall_boss.hpp>
#include <bosses/winter_boss.hpp>

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

			if (distance <= placeable_unit.attack_range && placeable_unit.next_projectile_spawn < 0.f) {
				placeable_unit.next_projectile_spawn = FIRING_RATE;
				Projectile::createProjectile(motion_h.position, vec2(adjacent, opposite) / distance, placeable_unit.damage);
				motion_h.angle = atan2(opposite, adjacent);
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

// with diagonals
std::vector<ivec2> nbr_walk = {ivec2(1,0), ivec2(1,-1),ivec2(1,1),
                  ivec2(0,-1),ivec2(0,1),
                  ivec2(-1,0),ivec2(-1,1),ivec2(-1,-1)};

float get_distance(ivec2 coord1, ivec2 coord2) {
    return length((vec2)(coord1 - coord2));
}

std::vector<ivec2> AISystem::MapAI::findPathBFS(GridMap& current_map, ivec2 start_coord, ivec2 goal_coord, bool is_valid(GridMap&, ivec2)) {
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
            ivec2 nbr_coord = std::get<0>(current_qnode) + nbr_walk.at(i);
            if (!is_valid(current_map, nbr_coord) || visited[nbr_coord.x][nbr_coord.y]) {
                continue;
            }
            std::tuple<ivec2, float> next_qnode = std::make_tuple(nbr_coord, std::get<1>(current_qnode) + get_distance(nbr_coord, std::get<0>(current_qnode)));
            visited[nbr_coord.x][nbr_coord.y] = true;
            queue.emplace(next_qnode);
            parent[nbr_coord.x][nbr_coord.y] = current_qnode;
        }
    }
    // a path does not exist between start and end.
    // should NOT reach this point for monster travel path with random map generation is in place
    std::cout<<"Debug: no path exist \n";
    return std::vector<ivec2>();
}

std::map<int, float> weather_tile_prob = {
        {TERRAIN_MUD,      1},
        {TERRAIN_PUDDLE,   1}
};

int get_random_weather_terrain() {
    // grid_change_prob is the probability to set grid to special terrain
    float grid_change_prob = 0.7;
    // start with <rand * grid_change prob> chance for each happening
    for (auto& [terrain, prob] : weather_tile_prob) {
        prob = uniform_dist(rng) * grid_change_prob;
    }
    if (weather == RAIN) {
        weather_tile_prob[TERRAIN_PUDDLE] *= 1.2;
        weather_tile_prob[TERRAIN_MUD] *= 1.1;
    }
    else if (weather == DROUGHT) {
        weather_tile_prob[TERRAIN_PUDDLE] *= 0.75;
        weather_tile_prob[TERRAIN_MUD] *= 0.8;
    }
    else if (weather == FOG) {

    }
    else if (weather == SNOW) {

    }

    vec2 max_prob(-1, -1);

    for (auto& [terrain, prob] : weather_tile_prob) {
        if (prob > max_prob.y){
            max_prob.x = terrain;
            max_prob.y = prob;
        }
    }

    if (max_prob.y > 0.5) {
        return max_prob.x;
    }

    return TERRAIN_DEFAULT;
}

void AISystem::MapAI::setRandomMapWeatherTerrain(GridMap& map) {
    for (int i = 0; i < WINDOW_SIZE_IN_COORD.x; i++) {
        for (int j = 0; j < WINDOW_SIZE_IN_COORD.y; j++) {
            int weather_terrain = get_random_weather_terrain();
            if (weather_terrain != TERRAIN_DEFAULT && map.getNodeAtCoord(ivec2(i,j)).terrain != TERRAIN_PAVEMENT) {
                map.setGridterrain(ivec2(i,j), weather_terrain);
            }
        }
    }
}

void AISystem::MapAI::setRandomGridsWeatherTerrain(GridMap &map, int max_grids) {
    for (int i = 0; i < max_grids; i++) {
     ivec2 random_coord(uniform_dist(rng)*WINDOW_SIZE_IN_COORD.x,  uniform_dist(rng)*WINDOW_SIZE_IN_COORD.y);
     auto& node = map.getNodeAtCoord(random_coord);
     if (node.terrain != TERRAIN_PAVEMENT) {
         map.setGridterrain(random_coord, get_random_weather_terrain());
     }
    }
}

// no diagonal version
// with diagonals
std::vector<ivec2> nbr_path = {ivec2(0,-1),ivec2(-1,0),
                               ivec2(1,0), ivec2(0,1)};

int get_random_index () {
    float num = uniform_dist(rng);
    // 10% chance for UP and LEFT
    if (num < 0.1) {
        return 0;
    } else if (num < 0.2) {
        return 1;
    }
        // 40% chance for DOWN and RIGHT
    else if (num < 0.6) {
        return 2;
    }
    return 3;
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


ivec2 get_random_neighbor(GridMap& map, ivec2 current_coord, ivec2 end_coord) {
    bool visited[4] = {false, false, false, false};
    int count = 0;

    int index = get_random_index();
    while (count < 4) {
        // randomly roll a direction until we get an unvisited direction
        while (visited[index]) {
            index = get_random_index();;
        }
        count++;
        // return this neighbor if it's in bounds and has an open path to village (don't want to block itself in)
        ivec2 nbr_coord = current_coord + nbr_path.at(index);
        visited[index] = true;
        if (nbr_coord == end_coord ||
            (is_inbounds(nbr_coord) && map.getNodeAtCoord(nbr_coord).terrain != TERRAIN_PAVEMENT
             && !AISystem::MapAI::findPathBFS(map, nbr_coord, VILLAGE_COORD, is_valid_terrain_path).empty())) {
            return nbr_coord;
        }
    }
    // cannot find a valid neighbor; should NOT happen
    assert(false);
    return ivec2(0, 0);
}

void AISystem::MapAI::setRandomMapPathTerran(GridMap& map, ivec2 start_coord, ivec2 end_coord, int terrain) {
    ivec2 rand_nbr = get_random_neighbor(map, start_coord, end_coord);
    map.setGridterrain(start_coord, terrain);
    // randomly step toward end_coord
    while (rand_nbr != end_coord) {
        map.setGridterrain(rand_nbr, terrain);
        rand_nbr = get_random_neighbor(map, rand_nbr, end_coord);
    }
    map.setGridterrain(end_coord, terrain);
}

std::shared_ptr<onCollisionSelector> AISystem::MonstersAI::createCollisionTree() {
	std::shared_ptr <BTNode> donothing = std::make_unique<DoNothing>();
	std::shared_ptr <BTNode> grow = std::make_unique<Grow>();
	std::shared_ptr <BTNode> stop = std::make_unique<Stop>();
	std::shared_ptr <BTNode> run = std::make_unique<Run>();
	std::shared_ptr <BTNode> knockback = std::make_unique<Knockback>();

	std::shared_ptr <BTIfCondition> conditional_donothing = std::make_unique<BTIfCondition>(
		donothing,
		[](entt::entity e) {return registry.has<Mob>(e); }
	);
	std::shared_ptr <BTIfCondition> conditional_grow = std::make_unique<BTIfCondition>(
		grow,
		[](entt::entity e) {return registry.has<SpringBoss>(e); }
	);
	std::shared_ptr <BTIfCondition> conditional_stop = std::make_unique<BTIfCondition>(
		stop,
		[](entt::entity e) {return registry.has<SummerBoss>(e); }
	);
	std::shared_ptr <BTIfCondition> conditional_run = std::make_unique<BTIfCondition>(
		run,
		[](entt::entity e) {return registry.has<FallBoss>(e); }
	);
	std::shared_ptr <BTIfCondition> conditional_knockback = std::make_unique<BTIfCondition>(
		knockback,
		[](entt::entity e) {return registry.has<WinterBoss>(e); }
	);

	std::vector<std::shared_ptr<BTIfCondition>> cond_nodes;
	cond_nodes.push_back(conditional_donothing);
	cond_nodes.push_back(conditional_grow);
	cond_nodes.push_back(conditional_stop);
	cond_nodes.push_back(conditional_run);
	cond_nodes.push_back(conditional_knockback);

	std::shared_ptr<onCollisionSelector> root = std::make_unique<onCollisionSelector>(cond_nodes);

	return root;
}