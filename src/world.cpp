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


// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

// Game configuration
const size_t MAX_MOBS = 20;
const size_t MOB_DELAY_MS = 1000;
const size_t MAX_BOSS = 2;
const size_t BOSS_DELAY_MS = 5000;

const size_t ROUND_TIME = 30 * 1000; // 30 seconds?

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
WorldSystem::WorldSystem(ivec2 window_size_px) :
        health(500),
        next_boss_spawn(0.f),
        next_mob_spawn(0.f),
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
	title_ss << "Food: " << health << " Round: " << round_number;
	glfwSetWindowTitle(window, title_ss.str().c_str());
	//
	// Removing out of screen entities
	//auto& registry = ECS::registry<Motion>; // TODO

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current upon delete)
	//for (int i = static_cast<int>(registry.components.size())-1; i >= 0; --i)
	//{
	//	auto& motion = registry.components[i];
	//	if (motion.position.x + abs(motion.scale.x) < 0.f)
	//	{
	//		ECS::ContainerInterface::remove_all_components_of(registry.entities[i]);
	//	}
	//}

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
	if (round_timer < 0.0f) {
		std::stringstream title_ss;
		//TODO: only increment round number if certain conditions are met (no enemies left)
		round_number++;
		round_timer = ROUND_TIME; // no delay between rounds
	}

//    // TODO follow the path on the grid
//    auto& path = current_map.path_entt;
//    // update velocity for every monster
//    for(auto entity: registry.view<Monster>()) {
//        auto& motion = registry.get<Motion>(entity);
//        auto& current_path_node = registry.get<GridNode>(path.at(motion.current_path_index));
//        // check that the monster is indeed within the current path node
//        assert(GridMap::pixelToCoord(motion.position) == current_path_node.coord);
//        ivec2 next_position = motion.position + elapsed_ms * motion.velocity;
//        // if the next position of monster is on the same grid
//        if (GridMap::pixelToCoord(next_position) == current_path_node.coord){
//            ivec2 target_position = GridMap::coordToPixel(current_path_node.coord + ivec2(1,1));
//        }
//
//    }


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
	// Debugging for memory/component leaks
	//ECS::ContainerInterface::list_all_components(); //TODO 
	std::cout << "Restarting\n";

	// Reset the game speed
	current_speed = 1.f;
	
	// Remove all entities that we created
	
	registry.clear();

	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);
	
	
	
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	//while (ECS::registry<Motion>.entities.size()>0)
	//	ECS::ContainerInterface::remove_all_components_of(ECS::registry<Motion>.entities.back());

	// Debugging for memory/component leaks
	//ECS::ContainerInterface::list_all_components();

	// Create a new salmon
	//player_salmon = Salmon::createSalmon({ 100, 200 });

    // create grid map
    current_map = registry.get<GridMap>(GridMap::createGridMapEntt());
    // hardcode path
    std::vector<vec2> path = {};
    for (int y = FOREST_COORD.y; y < VILLAGE_COORD.y; y++) {
        path.emplace_back(FOREST_COORD.x, y);
    }
    for (int x = FOREST_COORD.x; x < VILLAGE_COORD.x; x++) {
        path.emplace_back(x, VILLAGE_COORD.y);
    }
    // set path
    current_map.setPathFromCoords(path);
}

// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	//// Loop over all collisions detected by the physics system

	auto view_collision = registry.view<PhysicsSystem::Collision>();
	for (auto [entity, collision] : view_collision.each()) {
		auto entity_other = collision.other;

		// TODO
		// check projectile and monster collision

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


	// Hot keys for selecting placeable units
	if (action == GLFW_PRESS && key == GLFW_KEY_1)
	{
		unit_selected = "hunter";
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_2)
	{
		unit_selected = "watchtower";
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_3)
	{
		unit_selected = "greenhouse"; 
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

void WorldSystem::on_mouse_move(vec2 mouse_pos)
{
    // if village is alive
    if (health > 0)
    {
    }
    (void)mouse_pos;
}

// mouse click callback function 
void WorldSystem::on_mouse_click(int button, int action, int mod) {

	// Mouse click for placing units 
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && unit_selected != "")
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);

		if (unit_selected == "hunter")
		{
			entt::entity entity = Hunter::createHunter({ xpos, ypos });
		}
		if (unit_selected == "greenhouse")
		{
			entt::entity entity = GreenHouse::createGreenHouse({ xpos, ypos });
		}
		if (unit_selected == "watchtower")
		{
			entt::entity entity = WatchTower::createWatchTower({ xpos, ypos });
		}
	}
}