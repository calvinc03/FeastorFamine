#pragma once

// stlib
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>
#include <map>
#include <random>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>
#include <entt.hpp>
// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3

using namespace glm;
static const float PI = 3.14159265359f;
static const size_t FIRING_RATE = 3000;

static const int GRID_CELL_SIZE = 70;
static const ivec2 GRID_OFFSET =  ivec2(GRID_CELL_SIZE/2 , GRID_CELL_SIZE/2);
static const int UI_TAB_HEIGHT = 88;

static const ivec2 WINDOW_SIZE_IN_PX = {GRID_CELL_SIZE * 15, GRID_CELL_SIZE * 10 + UI_TAB_HEIGHT };
static const ivec2 MAP_SIZE_IN_PX = {WINDOW_SIZE_IN_PX.x, WINDOW_SIZE_IN_PX.y - UI_TAB_HEIGHT};
static const ivec2 MAP_SIZE_IN_COORD = MAP_SIZE_IN_PX / GRID_CELL_SIZE;

static const ivec2 FOREST_COORD = ivec2(0, 0);

static const int MAX_PARTICLES = 1000;
static int PARTICLE_COUNT = 0;
static GLfloat* g_particule_position_size_data;

static const ivec2 VILLAGE_COORD = MAP_SIZE_IN_COORD - ivec2(2, 2);

static int season;
static int weather;

const std::string WATCHTOWER_NAME = "watchtower";
const std::string GREENHOUSE_NAME = "greenhouse";
const std::string HUNTER_NAME = "hunter";
const std::string WALL_NAME = "wall";

// C++ random number generator
static std::default_random_engine rng;
static std::uniform_real_distribution<float> uniform_dist; // number between 0..1

// Terrains with neg value are ones that cannot be placed on
// May be refactored later if too hard to keep track
enum grid_terrain
{
    TERRAIN_PAVEMENT = -1,
    TERRAIN_DEFAULT = 0,
    TERRAIN_MUD = 1,
    TERRAIN_ICE = 2,
    TERRAIN_PUDDLE = 3,
};

enum grid_occupancy
{
    OCCUPANCY_BLOCKED = -1,
    OCCUPANCY_VACANT = 0,
    OCCUPANCY_FOREST = 1,
    OCCUPANCY_VILLAGE = 2,
    OCCUPANCY_GREENHOUSE = 3,
    OCCUPANCY_TOWER = 4,
    OCCUPANCY_WALL = 5,
    OCCUPANCY_HUNTER = 6,
};

enum season
{
    SPRING = 0,
    SUMMER = 1,
    FALL = 2,
    WINTER = 3
};

// Weather
enum weather
{
    CLEAR = 0,
    RAIN = 1,
    DROUGHT = 2,
    FOG = 3,
    SNOW = 4,
};

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

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
	vec2 boundingbox = { 10, 10 };;
};

struct Boss {
	bool hit;
	float speed_multiplier;
	std::string walk_sprite;
	std::string run_sprite;
	std::string attack_sprite;
	std::string death_sprite;
	std::string sprite;
	size_t walk_frames;
	size_t run_frames;
	size_t attack_frames;
	size_t death_frames;
	size_t frames;
};

struct Monster {
	int max_health; //useful for displaying health bars
    int health;
    int damage;
    int current_path_index = 0;
	int reward;
	bool collided = false;
	std::vector<ivec2> path_coords;
};

struct Unit {
	std::string type;
	int damage;
	size_t attack_interval_ms;
	float next_projectile_spawn;
	int attack_range;
	int workers;
	int upgrades;
	bool rotate;
};

struct Food {
	unsigned int food = 100;
	float food_production_speed = 0;
};

struct Projectile_Dmg {
	int damage;
};

struct Animate {
	float state = 0.f;
	float frame = 0.f;
	float state_num = 1.f;
	float frame_num = 1.f;
};


// id for entity
struct Tag {
	std::string tag;
};

struct Selectable {
	bool selected = false;
};

struct Upgradeable {
	int rank = 0;
};

//detects if mouse is within the a rectangle of size scale at position entity_pos
float sdBox(vec2 mouse_pos_grid, vec2 entity_pos, vec2 scale);

//TODO: temporary soln
#include "entt.hpp"
extern entt::registry registry;
extern entt::entity screen_state_entity;
// for camera view; zoom & pan
extern entt::entity camera;

vec2 mouse_in_world_coord(vec2 mouse_pos);
// add offset so that pixel is centered on grid

vec2 coord_to_pixel(ivec2 grid_coord);

ivec2 pixel_to_coord(vec2 pixel_position);

vec2 scale_to_grid_units(vec2 original_scale, float cell_units, int frames = 1);

vec2 grid_to_pixel_velocity(vec2 unit_velocity);

bool is_inbounds(ivec2 coord);
