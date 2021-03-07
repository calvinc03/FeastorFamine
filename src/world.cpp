// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "bosses/spring_boss.hpp"
#include "bosses/fall_boss.hpp"
#include "bosses/summer_boss.hpp"
#include "bosses/burrow_boss.hpp"
#include "bosses/winter_boss.hpp"
#include "mob.hpp"
#include <projectile.hpp>

#include "grid_map.hpp"
#include "hunter.hpp"
#include "greenhouse.hpp"
#include "watchtower.hpp"
#include "village.hpp"
#include "wall.hpp"
#include "camera.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "ui.hpp"
#include "ai.hpp"
#include <BehaviorTree.hpp>

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

#include "json.hpp"

const size_t ANIMATION_FPS = 12;
const size_t GREENHOUSE_PRODUCTION_DELAY = 8000;

const size_t SET_UP_TIME = 10 * 1000; // 15 seconds to setup

const size_t STARTING_HEALTH = 300;
const size_t WATCHTOWER_COST = 200;
const size_t GREENHOUSE_COST = 300;
const size_t HUNTER_COST = 150;
const size_t WALL_COST = 100;
const size_t HUNTER_UPGRADE_COST = 50;
void debug_path(std::vector<ivec2> monster_path_coords);
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.

const std::string NEW_GAME = "new_game";
const std::string SAVE_GAME = "save_game";
const std::string LOAD_GAME = "load_game";
const std::string SETTINGS_MENU = "settings_menu";
const std::string EXIT = "exit";
const std::string UPGRADE_BUTTON_TITLE = "upgrade_button";
const std::string SPRING_TITLE = "spring";
const std::string SUMMER_TITLE = "summer";
const std::string FALL_TITLE = "fall";
const std::string WINTER_TITLE = "winter";
const std::string INPUT_PATH = "data/monster_rounds/";
const std::string JSON_EXTENSION = ".json"; \
const std::string SAVE_PATH = "data/save_files/save_state.json";

WorldSystem::WorldSystem(ivec2 window_size_px, PhysicsSystem* physics) :
	game_state(start_menu),
	player_state(set_up_stage),
	fps_ms(1000 / ANIMATION_FPS),
	health(500),
	next_boss_spawn(2000.f),
	next_mob_spawn(3000.f),
	num_mobs_spawned(0),
	num_bosses_spawned(0),
	next_greenhouse_production(3000.f),

	set_up_timer(SET_UP_TIME),
	round_number(0) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

	///////////////////////////////////////
	// Initialize GLFW
	auto glfw_err_callback = [](int error, const char* desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
	glfwSetErrorCallback(glfw_err_callback);
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization, needs to be set before glfwCreateWindow
	// Core Opengl 3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_size_px.x, window_size_px.y, "Feast or Famine", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failed to glfwCreateWindow");


	GLFWimage images[1];
	images[0].pixels = stbi_load("data/textures/icons/ff.png", &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(this->window, 1, images);
	stbi_image_free(images[0].pixels);

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	auto scroll_redirect = [](GLFWwindow* wnd, double xoffset, double yoffset) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->scroll_callback(xoffset, yoffset); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);
	glfwSetScrollCallback(window, scroll_redirect);

	// Playing background music indefinitely
	init_audio();
	// Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";

	// Attaching World Observer to Physics observerlist
	this->physics = physics;
	this->physics->attach(this);

	BTCollision = AISystem::MonstersAI::createCollisionTree();
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	if (impact_sound != nullptr)
		Mix_FreeChunk(impact_sound);
	Mix_CloseAudio();

	// Destroy all created components
	//ECS::ContainerInterface::clear_all_components();

	registry.clear(); // this destroys all entities...
	// Close the window
	glfwDestroyWindow(window);
}

void WorldSystem::init_audio()
{
	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("Failed to initialize SDL Audio");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
		throw std::runtime_error("Failed to open audio device");

	background_music = Mix_LoadMUS(audio_path("music2.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
	ui_sound_bottle_pop = Mix_LoadWAV(audio_path("bottle_pop.wav").c_str());
	ui_sound_tick = Mix_LoadWAV(audio_path("tick.wav").c_str());
	ui_sound_hollow_tick = Mix_LoadWAV(audio_path("hollow_tick.wav").c_str());
	ui_sound_negative_tick = Mix_LoadWAV(audio_path("negative_tick.wav").c_str());
	impact_sound = Mix_LoadWAV(audio_path("impact.wav").c_str());
	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr || impact_sound == nullptr || ui_sound_bottle_pop == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: " +
			audio_path("music2.wav") +
			audio_path("impact.wav") +
			audio_path("salmon_dead.wav") +
			audio_path("salmon_eat.wav") +
			audio_path("ui_sound_bottle_pop.wav"));
}


// Update our game world
void WorldSystem::step(float elapsed_ms) {
	// Updating window title with health
	//std::stringstream title_ss;
	//title_ss << "Battle stage... Food: " << health << " Round: " << round_number << " fps: " << 1000.0 / elapsed_ms;
	//glfwSetWindowTitle(window, title_ss.str().c_str());

	// animation
	fps_ms -= elapsed_ms;
	if (fps_ms < 0.f) {
		for (auto entity : registry.view<Animate>()) {
			auto& animate = registry.get<Animate>(entity);
			animate.frame += 1;
			animate.frame = (int)animate.frame % (int)animate.frame_num;
		}
		fps_ms = 1000 / ANIMATION_FPS;
	}



	//Spawning new boss
	next_boss_spawn -= elapsed_ms * current_speed;
	if (num_bosses_spawned < max_boss && next_boss_spawn < 0.f) {
		// Reset spawn timer and spawn boss
		next_boss_spawn = (boss_delay_ms / 2) + uniform_dist(rng) * (boss_delay_ms / 2);
		entt::entity boss = create_boss();
		num_bosses_spawned += 1;
		BTCollision->init(boss);
	}

	// Spawning new mobs
	next_mob_spawn -= elapsed_ms * current_speed;
	if (num_mobs_spawned < max_mobs && next_mob_spawn < 0.f) {
		next_mob_spawn = (mob_delay_ms / 2) + uniform_dist(rng) * (mob_delay_ms / 2);
		entt::entity mob = Mob::createMobEntt();
		num_mobs_spawned += 1;
		BTCollision->init(mob);
	}


	// update velocity for every monster
	for (auto entity : registry.view<Monster>()) {
		auto state = BTCollision->process(entity);
		if (state == BTState::Stopped) continue;

		auto& monster = registry.get<Monster>(entity);
		auto& motion = registry.get<Motion>(entity);
		auto& current_path_coord = monster_path_coords.at(monster.current_path_index);

		// check that the monster is indeed within the current path node
		ivec2 coord = pixelToCoord(motion.position);

		// if we are on the last node, stop the monster and remove entity
		// TODO: make disappearance fancier
		if (pixelToCoord(motion.position) == VILLAGE_COORD
			|| monster.current_path_index >= monster_path_coords.size() - 1) {
			health -= monster.damage;
			motion.velocity *= 0;
			registry.destroy(entity);
			continue;
		}

        assert(monster_path_coords[monster.current_path_index] == current_path_coord);

        ivec2 next_path_coord = monster_path_coords.at(monster.current_path_index + 1);
        vec2 next_step_position = motion.position + (elapsed_ms / 1000.f) * motion.velocity;
        ivec2 next_step_coord = pixelToCoord(next_step_position);

        // change direction if reached the middle of the this node
        if (abs(length(coordToPixel(current_path_coord) - motion.position)) < length(motion.velocity) * elapsed_ms / 1000.f) {
            vec2 move_direction = normalize((vec2)(next_path_coord - current_path_coord));
            motion.velocity = length(motion.velocity) * move_direction;
            motion.angle = atan(move_direction.y / move_direction.x);
        }

        if (next_step_coord == next_path_coord) {
            monster.current_path_index++;
            // update monster count (for terrain distortion effects)
            current_map.getNodeAtCoord(current_path_coord).num_monsters--;
            current_map.getNodeAtCoord(next_path_coord).num_monsters++;
        }

        if (DebugSystem::in_debug_mode)
		{
			DebugSystem::createDirectedLine(coordToPixel(current_path_coord), coordToPixel(next_path_coord), 5);
		}
	}

	// removes projectiles that are out of the screen
	for (auto projectile : registry.view<Projectile>()) {
		auto& pos = registry.get<Motion>(projectile);
		if (pos.position.x > WINDOW_SIZE_IN_PX.x || pos.position.y > WINDOW_SIZE_IN_PX.y || pos.position.x < 0 ||
			pos.position.y < 0) {
			registry.destroy(projectile);
		}
	}

	// greenhouse food production
	next_greenhouse_production -= elapsed_ms * current_speed;
	if (next_greenhouse_production < 0.f) {
		health += registry.view<GreenHouse>().size() * 20;
		next_greenhouse_production = GREENHOUSE_PRODUCTION_DELAY;
	}

	// Increment round number if all enemies are not on the map and projectiles are removed
	if (num_bosses_spawned == max_boss && num_mobs_spawned == max_mobs) {
		if (registry.view<Monster>().empty() && registry.view<Projectile>().empty()) {
			round_number++;

			setup_round_from_round_number(round_number);
            // re-roll some weather terrains
            AISystem::MapAI::setRandomGridsWeatherTerrain(current_map, 10);
			player_state = set_up_stage;
			num_bosses_spawned = 0;
			num_mobs_spawned = 0;
		}
	}

	//stage text is set once per step... 
	auto& stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "BATTLE";
	stage_text.colour = { 1.0f, 0.1f, 0.1f };

	registry.get<Text>(round_text_entity).content = "round: " + std::to_string(round_number);
	registry.get<Text>(food_text_entity).content =  "food: " + std::to_string(health);
}


void un_highlight()
{
	auto view_ui = registry.view<HighlightBool>();
	for (auto [entity, highlight] : view_ui.each())
	{
		highlight.highlight = false;
	}
}
// set path
bool is_walkable(GridMap& current_map, ivec2 coord)
{
    if (is_inbounds(coord)) {
        int occupancy = current_map.getNodeAtCoord(coord).occupancy;
        return occupancy == OCCUPANCY_VACANT || occupancy == OCCUPANCY_FOREST || occupancy == OCCUPANCY_VILLAGE;
    }
    return false;
}


void WorldSystem::set_up_step(float elapsed_ms)
{
	
	// Restart/End game after max rounds
	if (round_number > 16)
	{
		restart();
	}

	set_up_timer -= elapsed_ms;

	// Updating window title with health and setup timer
	//std::stringstream title_ss;
	//title_ss << "Setup stage... Food: " << health << " Round: " << round_number << " Time left to setup: " << round(set_up_timer / 1000) << " fps: " << 1000.0 / elapsed_ms;
	//glfwSetWindowTitle(window, title_ss.str().c_str());

	if (set_up_timer <= 0)
	{
		player_state = battle_stage;
		set_up_timer = SET_UP_TIME;
		un_highlight();
		// set path at start of battle phase
		monster_path_coords = AISystem::MapAI::findPathBFS(current_map, FOREST_COORD, VILLAGE_COORD, is_walkable);

		std::cout << season_str << " season! \n";

		if (season_str == SPRING_TITLE) {
			season = SPRING;
			// Uncomment when done with weather testing
//            int weather_int = rand() % 5 + 1;
//            if (weather_int % 5 == 1)
//            {
//                weather = RAIN;
//            } else {
//                weather = CLEAR;
//            }
			// comment out when done testing
			weather = RAIN;
			create_boss = SpringBoss::createSpringBossEntt;
		}
		else if (season_str == SUMMER_TITLE) {
			season = SUMMER;
			//            int weather_int = rand() % 5 + 1;
			//            if (weather_int % 5 == 1)
			//            {
			//                weather = DROUGHT;
			//            } else {
			//                weather = CLEAR;
			//            }
			weather = DROUGHT;
			create_boss = SummerBoss::createSummerBossEntt;
		}
		else if (season_str == FALL_TITLE) {
			season = FALL;
			int weather_int = rand() % 5 + 1;
			//            if (weather_int % 5 == 1)
			//            {
			//                weather = FOG;
			//            } else {
			//                weather = CLEAR;
			//            }
			weather = FOG;
			create_boss = FallBoss::createFallBossEntt;
		}
		else if (season_str == WINTER_TITLE) {
			season = WINTER;
			//            int weather_int = rand() % 5 + 1;
			//            if (weather_int % 5 == 1)
			//            {
			//                weather = SNOW;
			//            } else {
			//                weather = CLEAR;
			//            }
			weather = SNOW;
			create_boss = WinterBoss::createWinterBossEntt;
		}
		std::cout << season_str << " \n";
		std::cout << "weather " << weather << " \n";
	}
	auto& stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "PREPARE: " + std::to_string((int)round(set_up_timer / 1000));
	stage_text.colour = {1.0f, 1.0f, 1.0f};
	registry.get<Text>(round_text_entity).content = "round: " + std::to_string(round_number);
	registry.get<Text>(food_text_entity).content =  "food: " + std::to_string(health);
}

// Start Menu
void WorldSystem::setup_start_menu()
{
	
	std::cout << "In Start Menu\n";
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	create_start_menu();
	camera = Camera::createCamera();
	
	
}

// Reset the world state to its initial state
void WorldSystem::restart()
{
	
	std::cout << "Restarting\n";

	// Reset the game state
	current_speed = 1.f;
	health = 500;		//reset health
	unit_selected = ""; // no initial selection
	round_number = 0;

	registry.clear(); // Remove all entities that we created

	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	//create UI	-- needs to be at the top of restart for rendering order.
	UI_button::createUI_button(0, tower_button, WATCHTOWER_COST );
	UI_button::createUI_button(1, green_house_button, GREENHOUSE_COST);
	UI_button::createUI_button(2, stick_figure_button, HUNTER_COST);
	UI_button::createUI_button(4, wall_button, WALL_COST);
	UI_button::createUI_button(7, upgrade_button, HUNTER_COST, "upgrade_button"); 
	UI_button::createUI_button(10, save_button,0, "save_button");
	UI_background::createUI_background();

	stage_text_entity = create_ui_text(vec2(5, 65), "PREPARE");
	round_text_entity = create_ui_text(vec2(5,50), "");
	food_text_entity = create_ui_text(vec2(5, 35), "");
	

	// create grid map
	current_map = registry.get<GridMap>(GridMap::createGridMap());

    // create village
	village = Village::createVillage();
	current_map.setGridOccupancy(VILLAGE_COORD, OCCUPANCY_VILLAGE);
	current_map.setGridOccupancy(VILLAGE_COORD + ivec2(1, 0), OCCUPANCY_VILLAGE);
	current_map.setGridOccupancy(VILLAGE_COORD + ivec2(0, 1), OCCUPANCY_VILLAGE);
	current_map.setGridOccupancy(VILLAGE_COORD + ivec2(1, 1), OCCUPANCY_VILLAGE);

	// TODO: create forest
	current_map.setGridOccupancy(FOREST_COORD, OCCUPANCY_FOREST);
	camera = Camera::createCamera();

	// set up variables for first round
	setup_round_from_round_number(0);

}

nlohmann::json WorldSystem::get_json(std::string json_path)
{
	std::ifstream input_stream(json_path);

	if (input_stream.fail())
	{
		std::cout << "Not reading json file for path \"" + json_path + "\" \n";
	}

	return nlohmann::json::parse(input_stream);
}

void WorldSystem::setup_round_from_round_number(int round_number)
{
	nlohmann::json round_json = get_json(INPUT_PATH + std::to_string(round_number) + JSON_EXTENSION);
	max_mobs = round_json["max_mobs"];
	mob_delay_ms = round_json["mob_delay_ms"];
	max_boss = round_json["max_bosses"];
	boss_delay_ms = round_json["boss_delay_ms"];
	season_str = round_json["season"];
}

void WorldSystem::updateCollisions(entt::entity entity_i, entt::entity entity_j)
{
	if (registry.has<Projectile>(entity_i)) {
		if (registry.has<Monster>(entity_j)) {

			auto& animal = registry.get<Monster>(entity_j);
			auto& projectile = registry.get<Projectile_Dmg>(entity_i);

			Mix_PlayChannel(-1, impact_sound, 0);

			animal.health -= projectile.damage;
			animal.collided = true;

			auto& hit_reaction = registry.get<HitReaction>(entity_j);
			hit_reaction.counter_ms = 750; //ms duration used by health bar

			registry.destroy(entity_i);
			if (animal.health <= 0)
			{
				if (season == 3) {
					health += 30 * 2;
				}
				else if (season == 4) {
					health += 30 / 2;
				}
				else {
					health += 30;
				}
				registry.destroy(entity_j);
			}
		}
	}
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return glfwWindowShouldClose(window) > 0;
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{
	// if village is alive
	if (health > 0)
	{
	}

	// keys used to skip rounds; used to debug and test rounds
	if (action == GLFW_RELEASE && key == GLFW_KEY_G)
	{
		if (player_state == set_up_stage)
		{
			set_up_timer = 0;
		}
		else if (player_state == battle_stage)
		{
			num_bosses_spawned = max_boss;
			num_mobs_spawned = max_mobs;
			for (entt::entity projectile : registry.view<Projectile>())
			{
				registry.destroy(projectile);
			}
			for (entt::entity monster : registry.view<Monster>())
			{
				registry.destroy(monster);
			}
		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
	{
		auto view = registry.view<Motion, MouseMovement>();
		auto& cam_motion = view.get<Motion>(camera);
		auto& mouse_move = view.get<MouseMovement>(camera);
		mouse_move.mouse_start = mouse_move.mouse_pos + cam_motion.position;
		mouse_move.is_pan_state = 1;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE)
	{
		auto view = registry.view<Motion, MouseMovement>();
		auto& motion = view.get<Motion>(camera);
		auto& mouse_move = view.get<MouseMovement>(camera);
		mouse_move.is_pan_state = 0;
	}

	// Hot keys for selecting placeable units
	else if (action == GLFW_PRESS && key == GLFW_KEY_1)
	{
		unit_selected = "watchtower";
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_2)
	{
		unit_selected = "greenhouse";
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_3)
	{
		unit_selected = "hunter";
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart();
	}

	// Debugging
	if (key == GLFW_KEY_D)
		DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		current_speed -= 0.1f;
		std::cout << "Current speed = " << current_speed << std::endl;
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		std::cout << "Current speed = " << current_speed << std::endl;
	}
	current_speed = std::max(0.f, current_speed);
}

bool mouse_in_game_area(vec2 mouse_pos)
{
	auto view_ui = registry.view<UI_element>();
	for (auto [entity, ui_element] : view_ui.each())
	{
		if ((sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f))
		{
			return false;
		}
	}
	return true;
}

void WorldSystem::scroll_callback(double xoffset, double yoffset)
{
	auto view = registry.view<Motion, MouseMovement>();
	auto& camera_motion = view.get<Motion>(camera);
	auto& camera_scale = camera_motion.scale;
	auto& camera_position = camera_motion.position;

	auto& mouse_movement = view.get<MouseMovement>(camera);

	double temp_scale = 20.0f;

	// zoom out limit
	if (camera_scale.y + (yoffset / temp_scale) < 1)
	{
		camera_scale = { 1.f, 1.f };
		camera_motion.position = { 0.f, 0.f };
		return;
	}

	camera_scale.y += yoffset / temp_scale;
	camera_scale.x = camera_scale.y;

	double mouse_in_world_x = abs(yoffset) * ((mouse_movement.mouse_pos.x + camera_position.x) / camera_scale.x) / temp_scale;
	double mouse_in_world_y = abs(yoffset) * ((mouse_movement.mouse_pos.y + camera_position.y) / camera_scale.y) / temp_scale;

	if (yoffset > 0)
	{
		// zoom in
		// no need to check out of border
		camera_position.x += mouse_in_world_x;
		camera_position.y += mouse_in_world_y;
	}
	else
	{
		// zoom out
		double new_cam_pos_x = camera_position.x - mouse_in_world_x;
		double new_cam_pos_y = camera_position.y - mouse_in_world_y;
		// check out of map border
		if (new_cam_pos_x < 0)
		{
			new_cam_pos_x = 0;
		}
		if (new_cam_pos_y < 0)
		{
			new_cam_pos_y = 0;
		}
		if ((WINDOW_SIZE_IN_PX.x * camera_motion.scale.x) - new_cam_pos_x < WINDOW_SIZE_IN_PX.x)
		{
			//std::cout << new_pos_x + (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) << " > " << WINDOW_SIZE_IN_PX.x << "\n";
			new_cam_pos_x = (WINDOW_SIZE_IN_PX.x * camera_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
		}
		// to compensate the ui background blocking the map, increase the threshold in the y position
		auto view_ui = registry.view<UI_element>();
		float ui_element_background_height = 0;
		for (auto entity : view_ui)
		{
			UI_element ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "in_game_ui_background") {
				ui_element_background_height = ui_element.scale.y;
			}
		}
		if ((WINDOW_SIZE_IN_PX.y * camera_motion.scale.y) - new_cam_pos_y < WINDOW_SIZE_IN_PX.y - ui_element_background_height)
		{
			new_cam_pos_y = (WINDOW_SIZE_IN_PX.y * camera_motion.scale.y) - WINDOW_SIZE_IN_PX.y + ui_element_background_height;
		}

		camera_position = vec2(new_cam_pos_x, new_cam_pos_y);
	}
	//
	//double new_pos_x = mouse_move.mouse_start.x - mouse_pos.x;
	//double new_pos_y = mouse_move.mouse_start.y - mouse_pos.y;
	//if (new_pos_x < 0) {
	//	new_pos_x = 0;
	//}
	//if (new_pos_y < 0) {
	//	new_pos_y = 0;
	//}
	//if ((WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - new_pos_x < WINDOW_SIZE_IN_PX.x) {
	//	//std::cout << new_pos_x + (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) << " > " << WINDOW_SIZE_IN_PX.x << "\n";
	//	new_pos_x = (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
	//}
	//if ((WINDOW_SIZE_IN_PX.y * cam_motion.scale.y) - new_pos_y < WINDOW_SIZE_IN_PX.y) {
	//	new_pos_y = (WINDOW_SIZE_IN_PX.y * cam_motion.scale.y) - WINDOW_SIZE_IN_PX.y;
	//}
	//cam_motion.position = vec2(new_pos_x, new_pos_y);

	// std::cout << camera_scale.x << ", " << camera_position.y << "\n";
}

//will move this eventually
//atm this is repeated code because ui uses a different position/scale than gridnode 
void grid_highlight_system(vec2 mouse_pos, std::string unit_selected, GridMap current_map) {
	auto view_ui = registry.view<Motion, HighlightBool>();

	auto& node = current_map.getNodeAtCoord(pixelToCoord(mouse_pos));
	for (auto [entity, grid_motion, highlight] : view_ui.each()) {
		if (sdBox(mouse_pos, grid_motion.position, grid_motion.scale / 2.0f) < 0.0f
		        && node.occupancy == OCCUPANCY_VACANT && node.terrain >= TERRAIN_DEFAULT) {
			highlight.highlight = true;
		}
		else
		{
			highlight.highlight = false;
		}
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_pos)
{
	//if mouse is hovering over a button, then highlight
	UI_highlight_system(mouse_pos);

	bool in_game_area = mouse_in_game_area(mouse_pos);
	if (in_game_area && unit_selected != "" && player_state == set_up_stage)
		grid_highlight_system(mouse_pos, unit_selected, current_map);

	// if village is alive
	if (health > 0)
	{
	}

	// camera control
	auto view = registry.view<Motion, MouseMovement>();
	auto& cam_motion = view.get<Motion>(camera);
	auto& mouse_move = view.get<MouseMovement>(camera);
	mouse_move.mouse_pos = mouse_pos;
	if (mouse_move.is_pan_state == 1)
	{
		// prevent pan off map
		double new_pos_x = mouse_move.mouse_start.x - mouse_pos.x;
		double new_pos_y = mouse_move.mouse_start.y - mouse_pos.y;
		if (new_pos_x < 0)
		{
			new_pos_x = 0;
		}
		if (new_pos_y < 0)
		{
			new_pos_y = 0;
		}
		if ((WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - new_pos_x < WINDOW_SIZE_IN_PX.x)
		{
			new_pos_x = (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
		}
		// to compensate the ui background blocking the map, increase the threshold in the y position
		auto view_ui = registry.view<UI_element>();
		float ui_element_background_height = 0;
		for (auto entity : view_ui)
		{
			UI_element ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "in_game_ui_background") {
				ui_element_background_height = ui_element.scale.y;
			}
		}

		if ((WINDOW_SIZE_IN_PX.y * cam_motion.scale.y) - new_pos_y < WINDOW_SIZE_IN_PX.y - ui_element_background_height)
		{
			new_pos_y = (WINDOW_SIZE_IN_PX.y * cam_motion.scale.y) - WINDOW_SIZE_IN_PX.y + ui_element_background_height;
		}
		cam_motion.position = vec2(new_pos_x, new_pos_y);
	}
}

// mouse click callback function
void WorldSystem::on_mouse_click(int button, int action, int mod)
{
	//getting cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	//some debugging print outs
	/*if (in_game_area) {
		std::cout << "in game area" << std::endl;
	}
	else {
		std::cout << "not in game area" << std::endl;
		std::cout << button_to_string(ui_button) << " pressed " << std::endl;
	}*/

	switch (game_state)
	{
	case start_menu:
		start_menu_click_handle(xpos, ypos, button, action, mod);
		break;
	case settings_menu:
		settings_menu_click_handle(xpos, ypos, button, action, mod);
		break;
	case in_game:
		unit_upgrade_click_handle(xpos, ypos, button, action, mod);
		in_game_click_handle(xpos, ypos, button, action, mod);
		break;
	}

	//std::cout << "selected: " << unit_selected << std::endl;

	// handle clicks in the start menu
	//std::cout << "Game State: " << game_state << "\n";
}

void WorldSystem::unit_upgrade_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{

	if (action == GLFW_PRESS)
	{
		bool upgrading = false;
		// get upgrade button position
		auto view_ui = registry.view<UI_element>();
		UI_element ui_element;
		for (auto entity : view_ui)
		{
			ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "upgrade_button") {
				break;
			}
		}
		auto view_unit = registry.view<Unit>();
		auto view_highlight = registry.view<HighlightBool>();
		auto view_selectable = registry.view<Selectable, Motion>();
		vec2 mouse_pos = mouse_in_world_coord({ mouse_pos_x, mouse_pos_y });
		for (auto [entity, selectable, motion] : view_selectable.each())
		{
			// check click on units
			if (sdBox(mouse_pos, motion.position, motion.scale / 2.0f) < 0.0f)
			{
				selectable.selected = true;
				view_highlight.get<HighlightBool>(entity).highlight = true;
				auto unit_stats = view_unit.get<Unit>(entity);
				std::cout << "=== Unit stats ===\n";
				std::cout << "attack damage: " << unit_stats.damage << "\n";
				std::cout << "attack rate: " << unit_stats.attack_rate << "\n";
				std::cout << "attack range: " << unit_stats.attack_range << "\n";

				if (registry.has<Unit>(entity)) {
					upgrading = true;
				}
			}
			// click on upgrade button
			else if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f)
			{
				upgrading = true;
			}
			else
			{
				view_highlight.get<HighlightBool>(entity).highlight = false;
				selectable.selected = false;
			}
		}
		auto view_ui_mesh = registry.view<UI_element, ShadedMeshRef>();
		if (upgrading)
		{
			for (auto entity : view_ui_mesh)
			{
				auto& shaded_mesh_ref = view_ui_mesh.get<ShadedMeshRef>(entity);
				if (view_ui_mesh.get<UI_element>(entity).tag == "upgrade_button")
				{
					shaded_mesh_ref.show = true;
				}
			}
		}
		else
		{
			for (auto entity : view_ui_mesh)
			{
				auto& shaded_mesh_ref = view_ui_mesh.get<ShadedMeshRef>(entity);
				if (view_ui_mesh.get<UI_element>(entity).tag == "upgrade_button")
				{
					//std::cout << "not upgrading\n";
					shaded_mesh_ref.show = false;
				}
			}
		}
	}
}

// helper for start menu mouse click
void WorldSystem::start_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	std::string button_tag = "";
	if (action == GLFW_PRESS)
	{
		button_tag = on_click_button({ mouse_pos_x, mouse_pos_y });
		//std::cout << button_tag << "\n";
	}

	if (button_tag == "exit")
	{
		// close window
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (button_tag == "new_game")
	{
		remove_menu_buttons();
		game_state = in_game;
		restart();
	}
	else if (button_tag == "settings_menu")
	{
		remove_menu_buttons();
		game_state = settings_menu;
		create_settings_menu();
	}
	else if (button_tag == "load_game")
	{
		restart();
		remove_menu_buttons();
		load_game();
		game_state = in_game;
	}
}

void WorldSystem::settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	std::string button_tag = "";
	if (action == GLFW_PRESS)
	{
		button_tag = on_click_button({ mouse_pos_x, mouse_pos_y });
		//std::cout << button_tag << "\n";
	}

	if (button_tag == "back")
	{
		// close window
		remove_menu_buttons();
		auto view = registry.view<Menu>();
		for (auto entity : view)
		{
			registry.destroy(entity);
		}
		game_state = start_menu;
		create_start_menu();
	}
}

void WorldSystem::remove_menu_buttons()
{
	auto view = registry.view<MenuButton>();
	for (auto entity : view)
	{
		registry.destroy(entity);
	}
}

//
void WorldSystem::menu_setup()
{
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);
	camera = Camera::createCamera();
}

// Start Menu
void WorldSystem::create_start_menu()
{
	std::cout << "In Start Menu\n";
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 1 / 5, "new_game", new_game_button);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 2 / 5, "load_game", load_game_button);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 3 / 5, "settings_menu", settings_button);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 4 / 5, "exit", exit_button);
}

void WorldSystem::create_settings_menu()
{
	std::cout << "In Settings Menu\n";
	Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "settings", Menu_texture::settings, 98, { 0.5, 0.5 });
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 4 / 5, "back", back_button);
}

// helper for in game mouse click
void WorldSystem::in_game_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	Motion camera_motion = registry.get<Motion>(camera);

	// cursor position in world pos
	vec2 mouse_world_pos = mouse_in_world_coord(vec2({ xpos, ypos }));

	int x_grid = mouse_world_pos.x;
	int y_grid = mouse_world_pos.y;

	// snap to nearest grid size
	float x = (x_grid) / GRID_CELL_SIZE; //+ GRID_CELL_SIZE / 2
	x *= GRID_CELL_SIZE;
	float y = (y_grid) / GRID_CELL_SIZE; //+ GRID_CELL_SIZE / 2
	y *= GRID_CELL_SIZE;

	x += GRID_CELL_SIZE / 2.0;
	y += GRID_CELL_SIZE / 2.0;

	Button ui_button = UI_click_system(); // returns enum of button pressed or no_button_pressed enum

	bool in_game_area = mouse_in_game_area(vec2(xpos, ypos));

	//un_highlight(); // turn off highlights for grid node on click

	if (player_state == set_up_stage)
	{
		// Mouse click for placing units
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && unit_selected != "" && in_game_area)
		{
			auto& node = current_map.getNodeAtCoord(pixelToCoord(vec2(x, y)));

			if (node.occupancy == OCCUPANCY_VACANT && node.terrain >= TERRAIN_DEFAULT)
               {
				if (unit_selected == HUNTER_NAME && health >= HUNTER_COST)
				{
					entt::entity entity = Hunter::createHunter({ x, y });
					health -= HUNTER_COST;
					node.occupancy = OCCUPANCY_HUNTER;
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (unit_selected == GREENHOUSE_NAME && health >= GREENHOUSE_COST)
				{
					entt::entity entity = GreenHouse::createGreenHouse({ x, y });
					health -= GREENHOUSE_COST;
					node.occupancy = OCCUPANCY_GREENHOUSE;
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (unit_selected == WATCHTOWER_NAME && health >= WATCHTOWER_COST)
				{
					entt::entity entity = WatchTower::createWatchTower({ x, y });
					health -= WATCHTOWER_COST;
					node.occupancy = OCCUPANCY_TOWER;
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (unit_selected == WALL_NAME && health >= WALL_COST)
				{
					entt::entity entity = Wall::createWall({ x, y }, false);
					health -= WALL_COST;
					node.occupancy = OCCUPANCY_WALL;
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else {
					//insufficent funds -- should feedback be given here, or when the button is pressed?
					Mix_PlayChannel(-1, ui_sound_negative_tick, 0);
				}
                unit_selected = "";
				un_highlight();
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !in_game_area)
		{

			if (ui_button == Button::tower_button)
			{
				
				unit_selected = WATCHTOWER_NAME;
			}
			else if (ui_button == Button::green_house_button)
			{
				
				unit_selected = GREENHOUSE_NAME;
			}
			else if (ui_button == Button::stick_figure_button)
			{
				
				unit_selected = HUNTER_NAME;
			}
			else if (ui_button == Button::wall_button)
			{
				unit_selected = WALL_NAME;
			}
			else if (ui_button == Button::upgrade_button && health >= HUNTER_UPGRADE_COST)
			{
			
				// upgrade button is hit
				auto view_selectable = registry.view<Selectable>();
				auto view_unit = registry.view<Unit>();
				for (auto entity : view_selectable)
				{
					if (view_selectable.get<Selectable>(entity).selected)
					{
						auto& unit = view_unit.get<Unit>(entity);
						upgrade_unit(unit);
						health -= HUNTER_UPGRADE_COST;
						std::cout << "damage x2\n";
					}
				}
			}
			else if (ui_button == Button::save_button)
			{
				save_game();
			}
			else 
			{
				
				unit_selected = "";
			}
		}

		//std::cout << "selected: " << unit_selected << std::endl;

		// handle clicks in the start menu
	}

}

void WorldSystem::upgrade_unit(Unit& unit)
{
	unit.damage *= 2;
	unit.upgrades++;
}

void WorldSystem::save_game()
{
	nlohmann::json save_json;
	save_json["round_number"] = round_number;
	save_json["health"] = health;
	
	// TODO finish implementing, may need to edit unit struct
	auto view_unit = registry.view<Unit>();
	auto view_motion = registry.view<Motion>();
	auto view_selectable = registry.view<Selectable>();
	std::vector<nlohmann::json> unit_list(view_selectable.size());

	int i = 0;
	for (auto& entity : view_selectable)
	{
		nlohmann::json curr_unit;
		auto unit = view_unit.get<Unit>(entity);
		auto motion = view_motion.get<Motion>(entity);

		curr_unit["type"] = unit.type;
		curr_unit["x_coord"] = motion.position.x;
		curr_unit["y_coord"] = motion.position.y;
		curr_unit["upgrades"] = unit.upgrades;
		curr_unit["rotate"] = unit.rotate;

		unit_list[i++] = curr_unit;
	}

	save_json["units"] = unit_list;
	std::ofstream file(SAVE_PATH);
	file << save_json.dump(4);
	file.close();

	if (!file.fail()) 
	{
		std::cout << "Game saved!" << std::endl;
	}
	else
	{
		std::cout << "Game failed to save" << std::endl;
	}
}

void WorldSystem::load_game()
{
	// hardcoded for now
	nlohmann::json save_json = get_json(SAVE_PATH);

	health = save_json["health"];
	round_number = save_json["round_number"];

	setup_round_from_round_number(round_number);

	for (nlohmann::json unit : save_json["units"])
	{
		int x = unit["x_coord"];
		int y = unit["y_coord"];
		auto& node = current_map.getNodeAtCoord(pixelToCoord(vec2(x, y)));
		std::string type = unit["type"];
		entt::entity entity;
		if (type == WATCHTOWER_NAME)
		{
			entity = WatchTower::createWatchTower({ x, y });
			node.occupancy = OCCUPANCY_TOWER;
		}
		else if (type == GREENHOUSE_NAME)
		{
			entity = GreenHouse::createGreenHouse({ x, y });
			node.occupancy = OCCUPANCY_TOWER;
		}
		else if (type == WALL_NAME)
		{
			entity = Wall::createWall({ x, y }, unit["rotate"]);
			node.occupancy = OCCUPANCY_TOWER;
		}
		else if (type == HUNTER_NAME)
		{
			entity = Hunter::createHunter({ x, y });
			node.occupancy = OCCUPANCY_TOWER;
		}

		auto view_unit = registry.view<Unit>();
		auto& curr_unit = view_unit.get<Unit>(entity);
		for (int i = 0; i < unit["upgrades"]; i++)
		{
			upgrade_unit(curr_unit);
		}
	}
}
