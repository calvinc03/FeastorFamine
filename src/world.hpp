#pragma once

// internal
#include "common.hpp"
#include "grid_map.hpp"
#include "Observer.hpp"
#include "physics.hpp"
#include <BehaviorTree.hpp>
#include "text.hpp"
#include "units/unit.hpp"
#include "ui_description.hpp"
#include "tip_manager.hpp"
#include "story_card.hpp"

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
	WorldSystem(ivec2 window_size_px, PhysicsSystem* physics);

	// Releases all associated resources
	~WorldSystem();

	// menu
	void game_setup();
	void create_start_menu();
	void setup_start_menu();

	// restart level
	void restart();
	void restart_with_save();

	// start round
	void start_round();

	// helper to load game from save game path
	void load_game();

	void pause_game();

	void more_options_menu();

	void resume_game();

	// helper to save game to disk
	void save_game();

	void upgrade_unit(Unit& unit);

	void sell_unit(entt::entity& entity);

	// setup game setup stage
	void prepare_setup_stage();

	// helper for path to round jsons
	void setup_round_from_round_number(int round_number);

	// Check for collisions
	void updateProjectileMonsterCollision(entt::entity e_projectile, entt::entity e_monster);

	// animates the portrait for the story card
	void animate_speaker(float elapsed_ms);

	// Steps the game during monster rounds ahead by ms milliseconds
	void step(float elapsed_ms);

	// Steps the game during set up rounds
	void set_up_step(float elapsed_ms);
	
	// steps the game during title screen
	void title_screen_step(float elapsed_ms);

	// lost game 
	void darken_screen_step(float elapsed_ms);
	void lost_game_screen_step(float elapsed_ms);

	// End of battle phase
	void end_battle_phase();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	static void deduct_health(int num);
	static void add_health(int num);
	// OpenGL window handle
	GLFWwindow* window;

	// game state
	int game_state;
	bool game_tips;

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
		story_card,
		sandbox,
		paused,
		darken_screen,
		lost_game_screen
	};

	// state for set_up and monster_rounds
	int player_state;
	enum PlayerState
	{
		set_up_stage,
		battle_stage,
		story_stage
	};

	// health of the village
	static int health;
	static GridMap current_map;

	// tutorial tip manager
	TipManager tip_manager;

	// decrease reward at higher levels
	static float reward_multiplier;

	// speed up factor for fastforwarding time
	static float speed_up_factor;

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

	// remove game tip
	void remove_game_tip();

	void handle_game_tips();

	// village
	entt::entity village;

	// animation fps
	float fps_ms;

	// season
	std::string world_season_str;

	// Game state
	float current_speed;
	float next_boss_spawn;
	int next_fireball_spawn;
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
	// end of battle stage delay 
	float end_of_battle_stage_dealy_ms = END_OF_BATTLE_STAGE_DELAY_MS;
	// greenhouse food already increased
	bool greenhouse_food_increased = false;

	int num_mobs_spawned;
	int num_bosses_spawned;
	entt::entity(*create_boss)();

	std::shared_ptr<BTNode> BTCollision;

	// round and set up
	int world_round_number;

	// flag for selected view bottom ui
	bool selected_view_change;
	entt::entity previous_selected;
	entt::entity upgrade_button_1;
	entt::entity upgrade_button_2;
	entt::entity button_sell;

	//UI
	entt::entity round_text_entity;
	entt::entity food_text_entity;
	entt::entity stage_text_entity;
	entt::entity season_text_entity;
	entt::entity weather_text_entity;
	entt::entity season_wheel_arrow_entity;
	entt::entity weather_icon_entity;
	entt::entity pause_menu_entity;
	entt::entity help_menu_entity;
	entt::entity title_button_highlight_entity;

	unit_type placement_unit_selected;
	entt::entity entity_selected;
	entt::entity entity_range_circle;
	entt::entity selected_range_circle;
	entt::entity(*create_unit_indicator)(vec2 pos);

	// remove entities from start menu
	void remove_menu_buttons();
	void create_controls_menu();
	entt::entity create_help_menu();

	//helper methods for updateProjectileMonsterCollision() method
	void damage_monster_helper(entt::entity e_monster, int damage, bool slow = false);

	// helper for start menu mouse click and in_game mouse click
	void start_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void in_game_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	void paused_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	void settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod);
	vec2 on_click_select_unit(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	bool click_on_unit(double mouse_pos_x, double mouse_pos_y);
	void help_menu_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void story_card_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void update_look_for_selected_buttons(int action, bool unit_selected, bool sell_clicked);
	void lost_game_click_handle(double mosue_pos_x, double mouse_pos_y, int button, int action, int mod);
	void createEntityRangeIndicator(vec2 mouse_pos);

	// lost game
	void start_lost_game_screen();

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
