#pragma once

// internal
#include "common.hpp"
#include "grid_map.hpp"
#include "Observer.hpp"
#include "physics.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class WorldSystem : public Observer
{
public:
	// Creates a window
	WorldSystem(ivec2 window_size_px, PhysicsSystem *physics);

	// Releases all associated resources
	~WorldSystem();

	// restart level
	void restart();

	void updateCollisions(entt::entity entity_i, entt::entity entity_j);

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	// OpenGL window handle
	GLFWwindow* window;
private:
	// PhysicsSystem handle
	PhysicsSystem* physics;

	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_pos);
	void on_mouse_click(int button, int action, int mod);
	void scroll_callback(double xoffset, double yoffset);

	// Loads the audio
	void init_audio();

	// village
	entt::entity village;

	// animation fps
	float fps_ms;

	// health of the village
	int health;

	// Game state
	float current_speed;
	float next_boss_spawn;
	float next_mob_spawn;

    // Map nodes and path
	GridMap current_map;
    std::vector<GridNode> monster_path = {};

	float round_timer;
	int round_number;

	//UI
	entt::entity ui;

	std::string unit_selected;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk* impact_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
