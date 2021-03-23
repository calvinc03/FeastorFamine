#pragma once

// internal
#include "common.hpp"
#include "grid_map.hpp"
#include "Observer.hpp"
#include "physics.hpp"
#include <BehaviorTree.hpp>
#include "text.hpp"
#include "units/unit.hpp"
// stlib
#include <vector>


#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <SDL_mixer.h>
#include <../ext/nlohmann/json.hpp>
#include "stb_image.h"
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
	void game_setup();
	void create_start_menu();
	void setup_start_menu();

	// restart level
	void restart();

	// helper to load json from disk
	nlohmann::json get_json(std::string json_path);

	// helper to load game from save game path
	void load_game();

	// helper to save game to disk
	void save_game();

	void upgrade_unit(Unit& unit);

	void sell_unit(entt::entity& entity);

	// setup game setup stage
	void setup_game_setup_stage();

	// helper for path to round jsons
	void setup_round_from_round_number(int round_number);

	// Check for collisions
	void updateProjectileMonsterCollision(entt::entity e_projectile, entt::entity e_monster);

	// Steps the game during monster rounds ahead by ms milliseconds
	void step(float elapsed_ms);

	// Steps the game during set up rounds
	void set_up_step(float elapsed_ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	static void deduct_health(int num);

	// OpenGL window handle
	GLFWwindow *window;

	// game state
	int game_state;
    
    // Particle System
//    GLuint billboard_vertex_buffer;
//    GLuint particles_position_buffer;

	// Menu
	enum GameState
	{
		start_menu,
		in_game,
		settings_menu,
		help_menu,
		story_card
	};

	// state for set_up and monster_rounds
	int player_state;
	enum PlayerState
	{
		set_up_stage,
		battle_stage,
		pause_stage,
		story_stage
	};

	// health of the village
	static int health;
    static GridMap current_map;

	// decrease reward at higher levels
	static float reward_multiplier;

private:
	// PhysicsSystem handle
	PhysicsSystem *physics;

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

	// json object for rounds
	std::string season_str;

	// Game state
	float current_speed;
	float next_boss_spawn;
	float next_fireball_spawn;
	float next_mob_spawn;
    float next_particle_spawn;
    
    // Season
    int season;
    
    enum season
    {
        SPRING = 0,
        SUMMER = 1,
        FALL = 2,
        WINTER = 3
    };
        
    // Weather
    int weather;

    
    enum weather
    {
        CLEAR = 0,
        RAIN = 1,
        DROUGHT = 2,
        FOG = 3,
        SNOW = 4,
    };
    
	int mob_delay_ms;
	int max_mobs;
	int boss_delay_ms;
	int max_boss;
	int fireball_delay_ms;
    

	int num_mobs_spawned;
	int num_bosses_spawned;
	entt::entity (*create_boss)();

    std::shared_ptr<BTNode> BTCollision;

	// round and set up
	int round_number;
	float set_up_timer;

	//UI
	entt::entity round_text_entity;
	entt::entity food_text_entity;
	entt::entity stage_text_entity;
	unit_type placement_unit_selected;


	// remove entities from start menu
	void remove_menu_buttons();
	void create_controls_menu();
	entt::entity create_help_menu();

	// helper for start menu mouse click and in_game mouse click
	void start_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void in_game_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	void settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	vec2 unit_select_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void help_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void story_card_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void sell_unit_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);

	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk* impact_sound;
	Mix_Chunk* ui_sound_bottle_pop;
	Mix_Chunk* ui_sound_tick;
	Mix_Chunk* ui_sound_negative_tick;
	Mix_Chunk* ui_sound_hollow_tick;
};
