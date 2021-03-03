// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "bosses/spring_boss.hpp"
#include "bosses/fall_boss.hpp"
#include "bosses/summer_boss.hpp"
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
#include "ui.hpp"
#include "ai.hpp"
// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <../ext/nlohmann/json.hpp>

// Game configuration
size_t MAX_MOBS = 20;
size_t MOB_DELAY_MS = 8000;
size_t MAX_BOSS = 2;
size_t BOSS_DELAY_MS = 20000;


const size_t ANIMATION_FPS = 12;
const size_t GREENHOUSE_PRODUCTION_DELAY = 8000;

const size_t SET_UP_TIME = 15 * 1000; // 15 seconds to setup

const size_t WATCHTOWER_COST = 200;
const size_t GREENHOUSE_COST = 300;
const size_t HUNTER_COST = 150;
const size_t WALL_COST = 100;
const std::string WATCHTOWER_NAME = "watchtower";
const std::string GREENHOUSE_NAME = "greenhouse";
const std::string HUNTER_NAME = "hunter";
const std::string WALL_NAME = "wall";

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
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

}

WorldSystem::~WorldSystem(){
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
	impact_sound = Mix_LoadWAV(audio_path("impact.wav").c_str());
	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr || impact_sound == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: "+
			audio_path("music2.wav")+
			audio_path("impact.wav") +
			audio_path("salmon_dead.wav")+
			audio_path("salmon_eat.wav"));

}

// Update our game world
void WorldSystem::step(float elapsed_ms)
{
	// Updating window title with health
	std::stringstream title_ss;
	title_ss << "Battle stage... Food: " << health << " Round: " << round_number << " fps: " << 1000.0/elapsed_ms;
	glfwSetWindowTitle(window, title_ss.str().c_str());

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
	if (num_bosses_spawned < MAX_BOSS && next_boss_spawn < 0.f)
	{
		// Reset spawn timer and spawn boss
		next_boss_spawn = (BOSS_DELAY_MS / 2) + uniform_dist(rng) * (BOSS_DELAY_MS / 2);
		//FallBoss::createFallBossEntt();
		create_boss();
		num_bosses_spawned += 1;
	}

	// Spawning new mobs
    next_mob_spawn -= elapsed_ms * current_speed;
    if (num_mobs_spawned < MAX_MOBS && next_mob_spawn < 0.f)
    {
        next_mob_spawn = (MOB_DELAY_MS / 2) + uniform_dist(rng) * (MOB_DELAY_MS / 2);
        Mob::createMobEntt();
		num_mobs_spawned += 1;
    }

    // update velocity for every monster
    for(auto entity: registry.view<Monster>()) {
        auto& monster = registry.get<Monster>(entity);
        auto& motion = registry.get<Motion>(entity);
        auto& current_path_node = monster_path.at(monster.current_path_index);

        // check that the monster is indeed within the current path node
        ivec2 coord = GridMap::pixelToCoord(motion.position);
        //assert(GridMap::pixelToCoord(motion.position) == current_path_node.coord);

        // if we are on the last node, stop the monster and remove entity
        // TODO: make disappearance fancier
        if (monster.current_path_index >= monster_path.size() - 1) {
            health -= monster.damage;
            motion.velocity *= 0;
            registry.destroy(entity);
            continue;
        }

        GridNode next_path_node = monster_path.at(monster.current_path_index + 1);
        vec2 move_direction = normalize((vec2)(next_path_node.coord - current_path_node.coord));
        motion.velocity = length(motion.velocity) * move_direction;
        motion.angle = atan(move_direction.y / move_direction.x);
        // if we will reach the next node in the next step, increase path index for next step
        ivec2 next_step_coord = GridMap::pixelToCoord(motion.position + (elapsed_ms / 1000.f) * motion.velocity);
        if (next_step_coord == next_path_node.coord) {
            monster.current_path_index++;
        }
    }

	// removes projectiles that are out of the screen
	for (auto projectile : registry.view<Projectile>()) {
		auto& pos = registry.get<Motion>(projectile);
		if (pos.position.x > WINDOW_SIZE_IN_PX.x || pos.position.y > WINDOW_SIZE_IN_PX.y || pos.position.x < 0 || pos.position.y < 0) {
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
	if (num_bosses_spawned == MAX_BOSS && num_mobs_spawned == MAX_MOBS) {
		if (registry.view<Monster>().empty() && registry.view<Projectile>().empty()) {
			round_number++;
			player_state = set_up_stage;
			num_bosses_spawned = 0;
			num_mobs_spawned = 0;
		}
	}

	// TODO polish death scene of village
	if (health < 0) // using < vs <= so we don't die if we spend all the food
	{
		auto& screen = registry.get<ScreenState>(screen_state_entity);
		//screen.darken_screen_factor = 1-elapsed_ms/3000.f;
		screen.darken_screen_factor = 0;
		restart();
	}
}

void un_highlight() {
	auto view_ui = registry.view< HighlightBool>();
	for (auto [entity, highlight] : view_ui.each()) {
		highlight.highlight = false;
	}
}

void WorldSystem::set_up_step(float elapsed_ms) {

	// Restart/End game after max rounds
	if (round_number > 16) {
		restart();
	}

	set_up_timer -= elapsed_ms;

	// Updating window title with health and setup timer 
	std::stringstream title_ss;
	title_ss << "Setup stage... Food: " << health << " Round: " << round_number << " Time left to setup: " << round(set_up_timer / 1000) << " fps: " << 1000.0 / elapsed_ms;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	if (set_up_timer <= 0) {
		player_state = battle_stage;
		set_up_timer = SET_UP_TIME;
		un_highlight();

		MAX_MOBS = round_json[round_number]["max_mobs"];
		MOB_DELAY_MS = round_json[round_number]["mob_delay_ms"];
		MAX_BOSS = round_json[round_number]["max_bosses"];
		BOSS_DELAY_MS = round_json[round_number]["boss_delay_ms"];
		
		if (round_json[round_number]["season"] == "spring") {
			std::cout << "Spring season! \n";
			create_boss = SpringBoss::createSpringBossEntt;

		}
		else if (round_json[round_number]["season"] == "summer") {
			std::cout << "Summer season! \n";
			create_boss = SummerBoss::createSummerBossEntt;

		}
		else if (round_json[round_number]["season"] == "fall") {
			std::cout << "Fall season! \n";
			create_boss = FallBoss::createFallBossEntt;

		}
		else if (round_json[round_number]["season"] == "winter") {
			std::cout << "Winter season! \n";
			create_boss = WinterBoss::createWinterBossEntt;

		}
	}
}


// Start Menu
void WorldSystem::setup_start_menu()
{
	std::cout << "In Start Menu\n";
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 1 / 5, "new_game", new_game_button);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 2 / 5, "load_game", load_game_button);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 3 / 5, "settings", settings_button);
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 4 / 5, "exit", exit_button);
	camera = Camera::createCamera();
}

// Reset the world state to its initial state
void WorldSystem::restart()
{
	std::cout << "Restarting\n";

	// Reset the game state
	current_speed = 1.f;
	health = 500; //reset health
	unit_selected = ""; // no initial selection
	round_number = 0;
	
	registry.clear();	// Remove all entities that we created

	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);
	
	//create UI	-- needs to be at the top of restart for rendering order. 
	UI_button::createUI_button(0, tower_button);
	UI_button::createUI_button(1, green_house_button);
	UI_button::createUI_button(2, stick_figure_button);
	UI_button::createUI_button(3, wall_button);
	UI_background::createUI_background();


    // create grid map
    current_map = registry.get<GridMap>(GridMap::createGridMap());

	// hardcode path
	std::vector<ivec2> path = {};
	for (int y = FOREST_COORD.y; y < VILLAGE_COORD.y; y++) {
		current_map.setGridOccupancy(current_map, vec2(FOREST_COORD.x, y), GRID_BLOCKED);
		path.emplace_back(FOREST_COORD.x, y);
	}
	for (int x = FOREST_COORD.x; x < VILLAGE_COORD.x; x++) {
		current_map.setGridOccupancy(current_map, vec2(x, VILLAGE_COORD.y), GRID_BLOCKED);
		path.emplace_back(x, VILLAGE_COORD.y);
	}

	// set path
	monster_path = GridMap::getNodesFromCoords(current_map, path);
    //monster_path = AISystem::PathFinder::find_path(current_map, FOREST_COORD, VILLAGE_COORD);

    // create village
	village = Village::createVillage();

	current_map.setGridOccupancy(current_map, VILLAGE_COORD, GRID_VILLAGE);
	current_map.setGridOccupancy(current_map, VILLAGE_COORD + ivec2(1, 0), GRID_VILLAGE);
	current_map.setGridOccupancy(current_map, VILLAGE_COORD + ivec2(0, 1), GRID_VILLAGE);
	current_map.setGridOccupancy(current_map, VILLAGE_COORD + ivec2(1, 1), GRID_VILLAGE);
	
	camera = Camera::createCamera();

	// Reading json file of rounds 
	// TODO : This uses the absolute path. I have no idea how to get it with relative path. The json file is currently in the src dir because it doesn't work elsewhere
	auto absolute_path = "C:\\Users\\calvi\\OneDrive\\Desktop\\XD\\Feast-or-Famine\\src\\rounds.json";
	std::ifstream input_stream(absolute_path);

	if (input_stream.fail()) {
		std::cout << "Not reading json file \n";
	}
	
	round_json = nlohmann::json::parse(input_stream);
}

void WorldSystem::updateCollisions(entt::entity entity_i, entt::entity entity_j)
{
	if (registry.has<Projectile>(entity_i)) {
		if (registry.has<Monster>(entity_j)) {
			//std::cout << "A monster was hit" << "\n";
			auto& animal = registry.get<Monster>(entity_j);
			auto& projectile = registry.get<Projectile_Dmg>(entity_i);

			Mix_PlayChannel(-1, impact_sound, 0);

			animal.health -= projectile.damage;
			registry.destroy(entity_i);
			if (animal.health <= 0)
			{
				health += 30;
				registry.destroy(entity_j);
			}
		}
	}
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return glfwWindowShouldClose(window)>0;
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{
	// if village is alive
	if (health > 0)
	{
	}

	// keys used to skip rounds; used to debug and test rounds
	if (action == GLFW_RELEASE && key == GLFW_KEY_G) {
		if (player_state == set_up_stage) {
			set_up_timer = 0;
		}
		else if (player_state == battle_stage) {
			num_bosses_spawned = MAX_BOSS;
			num_mobs_spawned = MAX_MOBS;
			for (entt::entity projectile : registry.view<Projectile>()) {
				registry.destroy(projectile);
			}
			for (entt::entity monster : registry.view<Monster>()) {
				registry.destroy(monster);
			}
		}
	}


	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		auto view = registry.view<Motion, MouseMovement>();
		auto& cam_motion = view.get<Motion>(camera);
		auto& mouse_move = view.get<MouseMovement>(camera);
		mouse_move.mouse_start = mouse_move.mouse_pos + cam_motion.position;
		mouse_move.is_pan_state = 1;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
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

bool mouse_in_game_area(vec2 mouse_pos) {
	auto view_ui = registry.view< UI_element>();
	for (auto [entity,ui_element] : view_ui.each()) {
		if ((sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f ) < 0.0f)) {
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
	if (camera_scale.y + (yoffset / temp_scale) < 1) {
		camera_scale = { 1.f, 1.f };
		camera_motion.position = { 0.f, 0.f };
		return;
	}

	camera_scale.y += yoffset / temp_scale;
	camera_scale.x = camera_scale.y;
	
	double mouse_in_world_x = abs(yoffset) * ((mouse_movement.mouse_pos.x + camera_position.x) / camera_scale.x) / temp_scale;
	double mouse_in_world_y = abs(yoffset) * ((mouse_movement.mouse_pos.y + camera_position.y) / camera_scale.y) / temp_scale;

	

	if (yoffset > 0) {
		// zoom in
		// no need to check out of border
		camera_position.x += mouse_in_world_x;
		camera_position.y += mouse_in_world_y;
	}
	else {
		// zoom out
		double new_cam_pos_x = camera_position.x - mouse_in_world_x;
		double new_cam_pos_y = camera_position.y - mouse_in_world_y;
		// check out of map border
		if (new_cam_pos_x < 0) {
			new_cam_pos_x = 0;
		}
		if (new_cam_pos_y < 0) {
			new_cam_pos_y = 0;
		}
		if ((WINDOW_SIZE_IN_PX.x * camera_motion.scale.x) - new_cam_pos_x < WINDOW_SIZE_IN_PX.x) {
		//std::cout << new_pos_x + (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) << " > " << WINDOW_SIZE_IN_PX.x << "\n";
			new_cam_pos_x = (WINDOW_SIZE_IN_PX.x * camera_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
}
		if ((WINDOW_SIZE_IN_PX.y * camera_motion.scale.y) - new_cam_pos_y < WINDOW_SIZE_IN_PX.y) {
			new_cam_pos_y = (WINDOW_SIZE_IN_PX.y * camera_motion.scale.y) - WINDOW_SIZE_IN_PX.y;
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
	
	GridNode& node = GridMap::getNodeAtCoord(current_map, GridMap::pixelToCoord(mouse_pos));
	for (auto [entity, grid_motion, highlight] : view_ui.each()) {
		if (sdBox(mouse_pos, grid_motion.position, grid_motion.scale / 2.0f) < 0.0f && node.occupancy == GRID_VACANT) {
			highlight.highlight = true;
		}
		else {
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
	if (mouse_move.is_pan_state == 1) {
		// prevent pan off map
		double new_pos_x = mouse_move.mouse_start.x - mouse_pos.x;
		double new_pos_y = mouse_move.mouse_start.y - mouse_pos.y;
		if (new_pos_x < 0 ) {
			new_pos_x = 0;
		}
		if (new_pos_y < 0) {
			new_pos_y = 0;
		}
		if ((WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - new_pos_x < WINDOW_SIZE_IN_PX.x) {
			//std::cout << new_pos_x + (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) << " > " << WINDOW_SIZE_IN_PX.x << "\n";
			new_pos_x = (WINDOW_SIZE_IN_PX.x * cam_motion.scale.x) - WINDOW_SIZE_IN_PX.x;
		}
		if ((WINDOW_SIZE_IN_PX.y * cam_motion.scale.y) - new_pos_y < WINDOW_SIZE_IN_PX.y) {
			new_pos_y = (WINDOW_SIZE_IN_PX.y * cam_motion.scale.y) - WINDOW_SIZE_IN_PX.y;
		}
		cam_motion.position = vec2(new_pos_x, new_pos_y);
	}
	
}

// mouse click callback function 
void WorldSystem::on_mouse_click(int button, int action, int mod) {
	//getting cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	switch (game_state)
	{
		case start_menu: 
			start_menu_click_handle(xpos, ypos, button, action, mod); 
			break;

		case in_game: 
			in_game_click_handle(xpos, ypos, button, action, mod);
			break;
	}
	
}

// helper for start menu mouse click
void WorldSystem::start_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	/*if (mouse_pos_x > 1000) {
		game_state = in_game;
		std::cout << "NOW IN GAME\n";
	}*/
	std::string button_tag = "";
	if (action == GLFW_PRESS)
	{
		button_tag = on_click({ mouse_pos_x , mouse_pos_y });
		std::cout << button_tag << "\n";
	}

	if (button_tag == "exit")
	{
		// close window
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (button_tag == "new_game")
	{
		game_state = in_game;
		restart();
	}
}

// helper for in game mouse click
void WorldSystem::in_game_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{	
	Motion camera_motion = registry.get<Motion>(camera);

	// cursor position in world pos
	vec2 mouse_world_pos = mouse_in_world_coord(vec2({ mouse_pos_x, mouse_pos_y }));

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
	bool in_game_area = mouse_in_game_area(vec2(mouse_pos_x, mouse_pos_y));

	//some debugging print outs
	/*if (in_game_area) {
		std::cout << "in game area" << std::endl;
	}
	else {
		std::cout << "not in game area" << std::endl;
		std::cout << button_to_string(ui_button) << " pressed " << std::endl;
	}*/

	if (player_state == set_up_stage) {
		// Mouse click for placing units 
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && unit_selected != "" && in_game_area)
		{
			GridNode& node = GridMap::getNodeAtCoord(current_map, GridMap::pixelToCoord(vec2(x, y)));

			if (node.occupancy == GRID_VACANT) {
				if (unit_selected == HUNTER_NAME && health >= HUNTER_COST)
				{
					entt::entity entity = Hunter::createHunter({ x, y });
					health -= HUNTER_COST;
					unit_selected = "";
					node.occupancy = GRID_HUNTER;
				}
				else if (unit_selected == GREENHOUSE_NAME && health >= GREENHOUSE_COST)
				{
					entt::entity entity = GreenHouse::createGreenHouse({ x, y });
					health -= GREENHOUSE_COST;
					unit_selected = "";
					node.occupancy = GRID_GREENHOUSE;
				}
				else if (unit_selected == WATCHTOWER_NAME && health >= WATCHTOWER_COST)
				{
					entt::entity entity = WatchTower::createWatchTower({ x, y });
					health -= WATCHTOWER_COST;
					unit_selected = "";
					node.occupancy = GRID_TOWER;
				}
				else if (unit_selected == WALL_NAME && health >= WALL_COST)
				{
					entt::entity entity = Wall::createWall({ x, y }, false);
					health -= WALL_COST;
					unit_selected = "";
					node.occupancy = GRID_WALL;
				}

				un_highlight();
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !in_game_area) {

			if (ui_button == Button::tower_button) {
				unit_selected = WATCHTOWER_NAME;
			}
			else if (ui_button == Button::green_house_button) {
				unit_selected = GREENHOUSE_NAME;
			}
			else if (ui_button == Button::stick_figure_button) {
				unit_selected = HUNTER_NAME;
			}
			else if (ui_button == Button::wall_button) {
				unit_selected = WALL_NAME;
			}
			else {
				unit_selected = "";
			}
		}

		//std::cout << "selected: " << unit_selected << std::endl;

		// handle clicks in the start menu
	}
}
