// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "spring_boss.hpp"
#include "mob.hpp"

#include "grid_map.hpp"
#include "hunter.hpp"
#include "greenhouse.hpp"
#include "watchtower.hpp"
#include "village.hpp"
#include "wall.hpp"
#include "camera.hpp"

#include "ui.hpp"
// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <projectile.hpp>

// Game configuration
const size_t MAX_MOBS = 20;
const size_t MOB_DELAY_MS = 10000;
const size_t MAX_BOSS = 2;
const size_t BOSS_DELAY_MS = 20000;
const size_t ANIMATION_FPS = 12;

const size_t ROUND_TIME = 30 * 1000; // 30 seconds?

const size_t WATCHTOWER_COST = 300;
const size_t GREENHOUSE_COST = 500;
const size_t HUNTER_COST = 100;
const size_t WALL_COST = 100;
const std::string WATCHTOWER_NAME = "watchtower";
const std::string GREENHOUSE_NAME = "greenhouse";
const std::string HUNTER_NAME = "hunter";
const std::string WALL_NAME = "wall";

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
WorldSystem::WorldSystem(ivec2 window_size_px) :
		fps_ms(1000 / ANIMATION_FPS),
        health(500),
        next_boss_spawn(2000.f),
        next_mob_spawn(3000.f),
		round_timer(ROUND_TIME), round_number(0){
    // Seeding rng with random device
    rng = std::default_random_engine(std::random_device()());

    ///////////////////////////////////////
    // Initialize GLFW
    auto glfw_err_callback = [](int error, const char *desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
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
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);

	// Playing background music indefinitely
	init_audio();
	Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";

}

WorldSystem::~WorldSystem(){
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
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

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: "+
			audio_path("music2.wav")+
			audio_path("salmon_dead.wav")+
			audio_path("salmon_eat.wav"));

}



// Update our game world
void WorldSystem::step(float elapsed_ms)
{
	// Updating window title with health
	std::stringstream title_ss;
	title_ss << "Food: " << health << " Round: " << round_number << " fps: " << 1000.0/elapsed_ms;
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
	if (registry.view<SpringBoss>().size() <= MAX_BOSS && next_boss_spawn < 0.f)
	{
		// Reset spawn timer and spawn boss
        next_boss_spawn = (BOSS_DELAY_MS / 2) + uniform_dist(rng) * (BOSS_DELAY_MS / 2);
        SpringBoss::createSpringBossEntt();
	}

	// Spawning new mobs
    next_mob_spawn -= elapsed_ms * current_speed;
    if (registry.view<Mob>().size() <= MAX_MOBS && next_mob_spawn < 0.f)
    {
        next_mob_spawn = (MOB_DELAY_MS / 2) + uniform_dist(rng) * (MOB_DELAY_MS / 2);
        Mob::createMobEntt();
    }

	round_timer -= elapsed_ms;
	//TODO: only increment round number if certain conditions are met (no enemies left)
	if (round_timer < 0.0f) {
		
		round_number++;
		round_timer = ROUND_TIME; // no delay between rounds
	}

    // update velocity for every monster
    for(auto entity: registry.view<Monster>()) {
        auto& monster = registry.get<Monster>(entity);
        auto& motion = registry.get<Motion>(entity);
        auto& current_path_node = registry.get<GridNode>(monster_path.at(monster.current_path_index));

        // check that the monster is indeed within the current path node
        ivec2 coord = GridMap::pixelToCoord(motion.position);
        assert(GridMap::pixelToCoord(motion.position) == current_path_node.coord);

        // if we are on the last node, stop the monster and remove entity
        // TODO: make disappearance fancier
        if (monster.current_path_index >= monster_path.size() - 1) {
            health -= monster.damage;
            motion.velocity *= 0;
            registry.destroy(entity);
            continue;
        }

        GridNode next_path_node = registry.get<GridNode>(monster_path.at(monster.current_path_index + 1));
        motion.velocity = length(motion.velocity) * normalize((vec2)(next_path_node.coord - current_path_node.coord));
        // if we will reach the next node in the next step, increase path index for next step
        ivec2 next_step_coord = GridMap::pixelToCoord(motion.position + (elapsed_ms / 1000.f) * motion.velocity);
        if (next_step_coord == next_path_node.coord) {
            monster.current_path_index++;
        }
    }

	// Attack mobs if in range of hunter
	for (auto monster : registry.view<Monster>()) {
		auto animal = entt::to_entity(registry, monster);
		auto& motion_m = registry.get<Motion>(animal);
		for (auto unit : registry.view<Unit>()) {
			auto hunter = entt::to_entity(registry, unit);
			auto& motion_h = registry.get<Motion>(hunter);
			auto& placeable_unit = registry.get<Unit>(hunter);

			float opposite = motion_m.position.y - motion_h.position.y;
			float adjacent = motion_m.position.x - motion_h.position.x;
			float distance = sqrt(pow(adjacent, 2) + pow(opposite, 2));

			if (distance <= placeable_unit.attack_range) {
				placeable_unit.next_projectile_spawn -= elapsed_ms * current_speed;
				if (placeable_unit.next_projectile_spawn < 0.f) {
					placeable_unit.next_projectile_spawn = FIRING_RATE;
					Projectile::createProjectile(motion_h.position, vec2(adjacent, opposite) / distance, placeable_unit.damage);
				}

			}
		}
	}

	for (auto projectile : registry.view<Projectile>()) {
		auto& pos = registry.get<Motion>(projectile);
		if (pos.position.x == WINDOW_SIZE_IN_PX.x || pos.position.y == WINDOW_SIZE_IN_PX.y) {
			registry.destroy(projectile);
		}
	}


	//// Processing the salmon state
	//assert(ECS::registry<ScreenState>.components.size() <= 1);
	//auto& screen = ECS::registry<ScreenState>.components[0];

 //   // TODO polish death scene of village
	//if (health <= 0)
	//{
	//	// Reduce window brightness
	//	screen.darken_screen_factor = 1-elapsed_ms/3000.f;

	//	// Restart the game once some time have passed
	//	if (elapsed_ms > 1000)
	//	{
	//		screen.darken_screen_factor = 0;
	//		restart();
	//		return;
	//	}
	//}
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
	round_timer = ROUND_TIME;
	
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
    current_map = registry.get<GridMap>(GridMap::createGridMapEntt());
    // hardcode path
    std::vector<ivec2> path = {};
    for (int y = FOREST_COORD.y; y < VILLAGE_COORD.y; y++) {
        path.emplace_back(FOREST_COORD.x, y);
    }
    for (int x = FOREST_COORD.x; x < VILLAGE_COORD.x; x++) {
        path.emplace_back(x, VILLAGE_COORD.y);
    }
    // set path
    monster_path = GridMap::getNodesFromCoords(current_map, path);

    // create village
	village = Village::createVillage();
	
	camera = Camera::createCamera();
}

// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	//// Loop over all collisions detected by the physics system
	auto collision = registry.view<PhysicsSystem::Collision>();
	for (unsigned int i = 0; i < collision.size(); i++)
	{
		auto entity = collision[i];
		auto entity_other = registry.get<PhysicsSystem::Collision>(collision[i]);
		if (registry.has<Projectile>(entity)) {
			if (registry.has<Monster>(entity_other.other)) {
				std::cout << "A monster was hit" << "\n";
				auto& animal = registry.get<Monster>(entity_other.other);
				auto& projectile = registry.get<Projectile_Dmg>(entity);
				animal.health -= projectile.damage;
				registry.destroy(entity);
				if (animal.health <= 0)
				{
					registry.destroy(entity_other.other);
					health += 20;
				}
			}
			// TODO else - village health
		}
		//registry.remove<PhysicsSystem::Collision>(entity);
	}
	registry.clear<PhysicsSystem::Collision>();
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


	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		auto view = registry.view<Motion, MouseMovement>();
		auto& motion = view.get<Motion>(camera);
		auto& mouse_move = view.get<MouseMovement>(camera);
		mouse_move.mouse_start = mouse_move.mouse_pos + motion.position;
		mouse_move.state = 1;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
		auto view = registry.view<Motion, MouseMovement>();
		auto& motion = view.get<Motion>(camera);
		auto& mouse_move = view.get<MouseMovement>(camera);
		mouse_move.state = 0;
	}

	// Hot keys for changing sprite appearance
	
	//if (action == GLFW_PRESS && key == GLFW_KEY_7)
	//{
	//	registry.get<Animate>(village).frame = 0;
	//}
	//else if (action == GLFW_PRESS && key == GLFW_KEY_8)
	//{
	//	registry.get<Animate>(village).frame = 1;
	//}
	//else if (action == GLFW_PRESS && key == GLFW_KEY_9)
	//{
	//	registry.get<Animate>(village).frame = 2;
	//}



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

void WorldSystem::on_mouse_move(vec2 mouse_pos)
{	
	//if mouse is hovering over a button, then highlight
	UI_highlight_system(mouse_pos);
    // if village is alive
    if (health > 0)
    {
    }

	// camera control 
	auto view = registry.view<Motion, MouseMovement>();
	auto& motion = view.get<Motion>(camera);
	auto& mouse_move = view.get<MouseMovement>(camera);
	mouse_move.mouse_pos = mouse_pos;
	if (mouse_move.state == 1) {
		motion.position = vec2(mouse_move.mouse_start.x - mouse_pos.x, mouse_move.mouse_start.y - mouse_pos.y);
	}
	
}

// mouse click callback function 
void WorldSystem::on_mouse_click(int button, int action, int mod) {
	//getting cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	Motion camera_motion = registry.get<Motion>(camera);

	// cursor position in grid units
	int x_grid = (xpos + camera_motion.position.x) ;
	int y_grid = (ypos  + camera_motion.position.y);

	// snap to nearest grid size
	int x = (x_grid + GRID_CELL_SIZE / 2) / GRID_CELL_SIZE;
	x *= GRID_CELL_SIZE;
	int y = (y_grid + GRID_CELL_SIZE / 2) / GRID_CELL_SIZE;
	y *= GRID_CELL_SIZE;

	Button ui_button = UI_click_system(); // returns enum of button pressed or no_button_pressed enum
	bool in_game_area = mouse_in_game_area(vec2(xpos, ypos));

	//some debugging print outs
	if (in_game_area) { 
		std::cout << "in game area" << std::endl;
	}
	else {
		std::cout << "not in game area" << std::endl;
		std::cout << button_to_string(ui_button) << " pressed " << std::endl;
	}

	// Mouse click for placing units 
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && unit_selected != "" && in_game_area)
	{
		if (unit_selected == HUNTER_NAME && health >= HUNTER_COST)
		{
			entt::entity entity = Hunter::createHunter({ x, y });
			health -= HUNTER_COST;
			unit_selected = "";
		}
		else if (unit_selected == GREENHOUSE_NAME && health >= GREENHOUSE_COST)
		{
			entt::entity entity = GreenHouse::createGreenHouse({ x, y });
			health -= GREENHOUSE_COST;
			unit_selected = "";
		}
		else if (unit_selected == WATCHTOWER_NAME && health >= WATCHTOWER_COST)
		{
			entt::entity entity = WatchTower::createWatchTower({ x, y });
			health -= WATCHTOWER_COST;
			unit_selected = "";
		}
		else if (unit_selected ==WALL_NAME && health >= WALL_COST)
		{
			entt::entity entity = Wall::createWall({ x, y }, false);
			health -= WALL_COST;
			unit_selected = "";
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
	
}
