#pragma once

// stlib
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>
#include <map>

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
static const int GRID_CELL_SIZE = 100;
static const ivec2 GRID_OFFSET =  ivec2(GRID_CELL_SIZE/2 , GRID_CELL_SIZE/2);
static const ivec2 WINDOW_SIZE_IN_PX = {1200, 900};
static const ivec2 WINDOW_SIZE_IN_COORD = WINDOW_SIZE_IN_PX / GRID_CELL_SIZE;
static const ivec2 FOREST_COORD = ivec2(0, 0);
// bottom right position (TODO offset this by village size)
static const ivec2 VILLAGE_COORD = WINDOW_SIZE_IN_COORD - ivec2(2, 2);

enum grid_terran
{
    GRID_GRASS = 0,
    GRID_PAVEMENT = 1,
    GRID_MUD = 2,
    GRID_WATER = 3,
};

enum grid_occupancy
{
    GRID_BLOCKED = -1,
    GRID_VACANT = 0,
    GRID_FOREST = 1,
    GRID_VILLAGE = 2,
    GRID_GREENHOUSE = 3,
    GRID_TOWER = 4,
    GRID_WALL = 5,
    GRID_HUNTER = 6,
};

enum season
{
    SPRING = 0,
    SUMMER = 1,
    FALL = 2,
    WINTER = 3
};

static const std::map<grid_terran, std::string> terran_str_map = {
        {GRID_GRASS, "grass"},
        {GRID_PAVEMENT, "pavement"},
        {GRID_MUD, "mud"},
        {GRID_WATER, "water"}
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
    int health;
    int damage;
    int current_path_index = 0;
	int reward;
};

struct Unit {
	int damage;
	size_t attack_rate;
	float next_projectile_spawn;
	int attack_range;
	int workers;
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


//detects if mouse is within the a rectangle of size scale at position entity_pos
float sdBox(vec2 mouse_pos_grid, vec2 entity_pos, vec2 scale);

//TODO: temporary soln
#include "entt.hpp"
extern entt::registry registry;
extern entt::entity screen_state_entity;
// for camera view; zoom & pan
extern entt::entity camera;
vec2 mouse_in_world_coord(vec2 mouse_pos);