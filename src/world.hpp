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
#include <../ext/nlohmann/json.hpp>

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class WorldSystem : public Observer
{
public:
	// Creates a window
	WorldSystem(ivec2 window_size_px, PhysicsSystem *physics);

	// Releases all associated resources
	~WorldSystem();

	// menu
	void setup_start_menu();

	// restart level
	void restart();

	// Check for collisions
	void updateCollisions(entt::entity entity_i, entt::entity entity_j);

	// Steps the game during monster rounds ahead by ms milliseconds
	void step(float elapsed_ms);

	// Steps the game during set up rounds
	void set_up_step(float elapsed_ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	// OpenGL window handle
	GLFWwindow* window;

	// game state
	int game_state;

	// Menu
	enum GameState { start_menu, in_game };

	// state for set_up and monster_rounds
	int player_state;
	enum PlayerState { set_up_stage, battle_stage };
	
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

	int season;

	// json object for rounds
	nlohmann::json round_json;

	// Game state
	float current_speed;
	float next_boss_spawn;
	float next_mob_spawn;

    int current_season;
    int current_weather;

	float next_greenhouse_production;
	int num_mobs_spawned;
	int num_bosses_spawned; 
	entt::entity (*create_boss)();

    // Monster path
	GridMap current_map;
    std::vector<ivec2> monster_path_coords = {};

	// round and set up
	int round_number;
	float set_up_timer;

	//UI
	entt::entity ui;

	std::string unit_selected;



	// helper for start menu mouse click and in_game mouse click
	void start_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void in_game_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);

	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk* impact_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
