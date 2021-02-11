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
static const int GRID_CELL_SIZE = 100;
static const ivec2 GRID_OFFSET =  ivec2(GRID_CELL_SIZE/2 , GRID_CELL_SIZE/2);
static const ivec2 WINDOW_SIZE_IN_PX = {1500, 900};
static const ivec2 WINDOW_SIZE_IN_COORD = WINDOW_SIZE_IN_PX / GRID_CELL_SIZE;
static const ivec2 FOREST_COORD = ivec2(0, 0) / GRID_CELL_SIZE;
// bottom right position in 0 based indexing, integers are truncated so no need to floor
static const ivec2 VILLAGE_COORD = (WINDOW_SIZE_IN_PX - ivec2(1, 1)) / GRID_CELL_SIZE;

enum grid_type
{
    GRID_BLOCKED = -1,
    GRID_DEFAULT = 0,
    GRID_FOREST = 1,
    GRID_VILLAGE = 2,
    GRID_PATH = 3,
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
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
    vec2 scale = { 10, 10 };
};

struct Monster {
    int health;
    int damage;
    int current_path_index = 0;
};

//TODO: temporary soln
#include "entt.hpp"
extern entt::registry registry;
extern entt::entity screen_state_entity;