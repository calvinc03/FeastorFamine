#pragma once
#include "config/enums.hpp"
#include "config/strings.hpp"
#include "config/game_config.hpp"
#include "config/ui_config.hpp"
#include "projectile.hpp"
// stlib
#include <string>
#include <tuple>
#include <vector>
#include <queue>
#include <stdexcept>
#include <map>
#include <random>
#include <fstream>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>
#include <entt.hpp>
// The glm library provides vector and matrix operations as in GLSL
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
#include <json.hpp>					// json

using namespace glm;
static const float PI = 3.14159265359f;

static int season;

// C++ random number generator
static std::default_random_engine rng;
static std::uniform_real_distribution<float> uniform_dist; // number between 0..1

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
struct Transform {
	vec2 camera_position = { 0.f, 0.f };
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0.f, 0.f };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { GRID_CELL_SIZE, GRID_CELL_SIZE };
	vec2 boundingbox = { 10, 10 };
	vec2 acceleration = { 0, 0 };
	vec2 origin = { 0,0 }; // this is useful for setting the point of rotation for parent/child transforms.
};

struct Monster {
    int max_health; //useful for displaying health bars
    int health;
    int damage;
    int attack_interval = 20;
    int current_path_index = 0;
	bool current_node_visited = false;
    int reward;
    bool collided = false;
    std::vector<ivec2> path_coords;

	bool hit;
	float speed_multiplier = 1.0;
	monster_type type;
	std::string walk_sprite;
	std::string run_sprite;
	std::string attack_sprite;
	std::string death_sprite;
	std::string sprite;
	int walk_frames;
	int run_frames;
	int attack_frames;
	int death_frames;
	int frames;
};

struct Food {
	unsigned int food = 100;
	float food_production_speed = 0;
};

struct Animate {
	int state = 0;
	int frame = 0;
	int state_num = 1;
	int frame_num = 1;
};

struct EntityDeath {
	float timer;
};

struct compare_slow
{
	bool operator()(std::pair<float, float> e1, std::pair<float, float> e2)
	{
		return e1.first > e2.first;
	}
};

struct DamageProperties {
	std::map<entt::entity, float> dot_map;
	std::priority_queue<
		std::pair<float, float>,
		std::vector<std::pair<float, float>>,
		compare_slow> slow_queue;
	bool slowed = false;
	float current_slow = 0.f;
};

// id for entity
struct Tag {
	std::string tag;
};

struct Timer{
	float counter_ms = 0.0f;
};

struct Selectable {
	bool selected = false;
};

//detects if mouse is within the a rectangle of size scale at position entity_pos
float sdBox(vec2 mouse_pos_grid, vec2 entity_pos, vec2 scale);

vec2 mouse_in_world_coord(vec2 mouse_pos);
// add offset so that pixel is centered on grid

vec2 coord_to_pixel(ivec2 grid_coord);

ivec2 pixel_to_coord(vec2 pixel_position);

vec2 scale_to_grid_units(vec2 original_scale, float cell_units, int frames = 1);

vec2 grid_to_pixel_velocity(vec2 unit_velocity);

void create_hit_points_text(int hit_points, entt::entity e_damaged);

bool is_inbounds(ivec2 coord);

std::vector<vec2> bezierVelocities(std::vector<vec2> points);

std::vector<vec2> bezierCurve(std::vector<vec2> points, float total_time);

std::vector<float> pascalNRow(int n);

//TODO: temporary soln
#include "entt.hpp"
extern entt::registry registry;
extern entt::entity screen_state_entity;
// for camera view; zoom & pan
extern entt::entity camera;

nlohmann::json get_json(std::string json_path);