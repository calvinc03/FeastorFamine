
// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "bosses/spring_boss.hpp"
#include "bosses/fall_boss.hpp"
#include "bosses/summer_boss.hpp"
#include "bosses/winter_boss.hpp"
#include "bosses/burrow_boss.hpp"
#include "bosses/final_boss.hpp"
#include "bosses/fireball_boss.hpp"
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
#include "story_card.hpp"
#include "ui.hpp"
#include "ai.hpp"
#include "particle.hpp"

#include "rig.hpp"
#include "spider.hpp"
#include <BehaviorTree.hpp>

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

#include "json.hpp"

const size_t ANIMATION_FPS = 20;
const size_t GREENHOUSE_PRODUCTION_DELAY = 8000;
const int STARTING_HEALTH = 600;
const int MAX_ROUNDS = 18;

const size_t SET_UP_TIME = 15 * 1000; // 15 seconds to setup
int WorldSystem::health = 1000;
float WorldSystem::reward_multiplier = 1.f;

void debug_path(std::vector<ivec2> monster_path_coords);
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.

const std::string NEW_GAME = "new_game";
const std::string SAVE_GAME = "save_game";
const std::string LOAD_GAME = "load_game";
const std::string HELP_MENU = "help_menu";
const std::string SETTINGS_MENU = "settings_menu";
const std::string EXIT = "exit";
const std::string UPGRADE_BUTTON_TITLE = "upgrade_button";
const std::string SELL_BUTTON_TITLE = "sell_button";
const std::string SAVE_BUTTON_TITLE = "save_button";
const std::string SPRING_TITLE = "spring";
const std::string SUMMER_TITLE = "summer";
const std::string FALL_TITLE = "fall";
const std::string WINTER_TITLE = "winter";
const std::string FINAL_TITLE = "final";
const std::string INPUT_PATH = "data/monster_rounds/";
const std::string JSON_EXTENSION = ".json";
const std::string SAVE_PATH = "data/save_files/save_state.json";

WorldSystem::WorldSystem(ivec2 window_size_px, PhysicsSystem *physics) : game_state(start_menu),
    player_state(set_up_stage),
    fps_ms(1000 / ANIMATION_FPS),
    next_boss_spawn(0),
	next_fireball_spawn(0),
    next_mob_spawn(0),
    num_mobs_spawned(0),
    next_particle_spawn(0),
    num_bosses_spawned(0),
    next_greenhouse_production(3000.f),
    set_up_timer(SET_UP_TIME),
    round_number(0)
{
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

	GLFWimage images[1];
	images[0].pixels = stbi_load("data/textures/icons/ff.png", &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(this->window, 1, images);
	stbi_image_free(images[0].pixels);

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1}); };
	auto mouse_button_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	auto scroll_redirect = [](GLFWwindow *wnd, double xoffset, double yoffset) { ((WorldSystem *)glfwGetWindowUserPointer(wnd))->scroll_callback(xoffset, yoffset); };
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

	BTCollision = AISystem::MonstersAI::createBehaviorTree();
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

// set path
bool is_walkable(GridMap& current_map, ivec2 coord)
{
	if (is_inbounds(coord))
	{
		int occupancy = current_map.getNodeAtCoord(coord).occupancy;
		return occupancy == OCCUPANCY_VACANT || occupancy == OCCUPANCY_FOREST || occupancy == OCCUPANCY_VILLAGE;
	}
	return false;
}

// Update our game world
void WorldSystem::step(float elapsed_ms)
{
	if (player_state != pause_stage && player_state != story_stage) {
		//rig animation
		auto view_rigs = registry.view<Timeline>();
		for (auto entity : view_rigs) {
			auto& motion = registry.get<Motion>(entity);
			RigSystem::animate_rig_ik(entity, 15); // constant 15ms/frame
		}
		// animation
		fps_ms -= elapsed_ms;
		if (fps_ms < 0.f)
		{
			for (auto entity : registry.view<Animate>())
			{
				auto& animate = registry.get<Animate>(entity);
				animate.frame += 1;
				animate.frame = (int)animate.frame % (int)animate.frame_num;
			}
			fps_ms = 1000 / ANIMATION_FPS;
		}

		//Spawning new boss
		next_boss_spawn -= elapsed_ms * current_speed;
		if (num_bosses_spawned < max_boss && next_boss_spawn < 0.f)
		{
			// Reset spawn timer and spawn boss
			next_boss_spawn = (boss_delay_ms / 2) + uniform_dist(rng) * (boss_delay_ms / 2);
			entt::entity boss = create_boss();
		
			auto& monster = registry.get<Monster>(boss);
			monster.path_coords = AISystem::MapAI::findPathAStar(current_map, FOREST_COORD, VILLAGE_COORD, is_walkable);

			num_bosses_spawned += 1;
			BTCollision->init(boss);
		}

		// Spawning new mobs
		next_mob_spawn -= elapsed_ms * current_speed;
		if (num_mobs_spawned < max_mobs && next_mob_spawn < 0.f)
		{
			next_mob_spawn = (mob_delay_ms / 2) + uniform_dist(rng) * (mob_delay_ms / 2);
			entt::entity mob = Mob::createMobEntt();
			//entt::entity mob = Spider::createSpider();
			auto& monster = registry.get<Monster>(mob);
			monster.path_coords = AISystem::MapAI::findPathAStar(current_map, FOREST_COORD, VILLAGE_COORD, is_walkable);

			num_mobs_spawned += 1;
			BTCollision->init(mob);
		}

		// spawn new fireballs for the final boss
		next_fireball_spawn -= elapsed_ms * current_speed;
		if (!registry.empty<FinalBoss>() && next_fireball_spawn < 0.f)
		{
			std::cout << "fireball" << std::endl;
			next_fireball_spawn = FIREBALL_DELAY_MS;
			entt::entity fireball = FireballBoss::createFireballBossEntt();

			auto& monster = registry.get<Monster>(fireball);
			monster.path_coords = AISystem::MapAI::findPathAStar(current_map, FOREST_COORD, VILLAGE_COORD, is_walkable);

			BTCollision->init(fireball);
		}

		// update velocity for every monster
		for (auto entity : registry.view<Monster>())
		{
			auto state = BTCollision->process(entity);
			if (health < 0) {
				restart();
				return;
			}
		}

		// greenhouse food production
		next_greenhouse_production -= elapsed_ms * current_speed;
		if (next_greenhouse_production < 0.f)
		{
			health += registry.view<GreenHouse>().size() * 20;
			next_greenhouse_production = GREENHOUSE_PRODUCTION_DELAY;
		}

		// Increment round number if all enemies are not on the map and projectiles are removed
		if (num_bosses_spawned == max_boss && num_mobs_spawned == max_mobs)
		{
			//std::cout << "got here" << std::endl;
			if (registry.view<Monster>().empty() && registry.view<Projectile>().empty())
			{
				round_number++;

				if (round_number == MAX_ROUNDS)
				{
					restart();
				}

				setup_round_from_round_number(round_number);
				// re-roll some weather terrains
				AISystem::MapAI::setRandomGridsWeatherTerrain(current_map, 10);
				player_state = set_up_stage;
				num_bosses_spawned = 0;
				num_mobs_spawned = 0;
			}
		}

		// removes projectiles that are out of the screen
		for (auto projectile : registry.view<Projectile>())
		{
			auto& pos = registry.get<Motion>(projectile);
			if (pos.position.x > WINDOW_SIZE_IN_PX.x + 200 || pos.position.y > WINDOW_SIZE_IN_PX.y + 200 || pos.position.x < -200 ||
				pos.position.y < -200)
			{
				registry.destroy(projectile);
				continue;
			}

			if (registry.has<EntityDeath>(projectile)) {
				auto& death = registry.get<EntityDeath>(projectile);
				death.timer -= elapsed_ms;
				if (death.timer <= 0) {
					registry.destroy(projectile);
					continue;
				}
			}
		}

		auto particle_view = registry.view<ParticleSystem>();
		if (particle_view.size() < MAX_PARTICLES) {
			for (auto particle_entity : particle_view) {
				auto& particle = registry.get<ParticleSystem>(particle_entity);
				particle.life -= elapsed_ms;
				if (particle.life <= 0) {
					registry.destroy(particle_entity);
				}
			}
			ParticleSystem::updateParticle();

			next_particle_spawn -= elapsed_ms;

			if (weather == RAIN && next_particle_spawn < 0.f)
			{
				next_particle_spawn = 60;
				vec2 velocity = { 0.f, 450.0f };
				vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , 0 };
				float life = 1150.0f;
				std::string texture = "raindrop.png";
				std::string shader = "rain";
				ParticleSystem::createParticle(velocity, position, life, texture, shader);
			}
			else if (weather == DROUGHT) {
				// TODO
			}
			else if (weather == FOG && next_particle_spawn < 0.f) {
				next_particle_spawn = 3000;
				vec2 velocity = { -100.f, 0.f };
				vec2 position = { WINDOW_SIZE_IN_PX.x, rand() % (WINDOW_SIZE_IN_PX.y - 230) };
				float life = 13500.f;
				std::string texture = "cloud.png";
				std::string shader = "fog";
				ParticleSystem::createParticle(velocity, position, life, texture, shader);
			}
			else if (weather == SNOW && next_particle_spawn < 0.f)
			{
				next_particle_spawn = 40;
				vec2 velocity = { rand() % 400 + (-200), 300.0f };
				vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , 0 };
				float life = 1800.0f;
				std::string texture = "snow.png";
				std::string shader = "snow";
				ParticleSystem::createParticle(velocity, position, life, texture, shader);
			}
		}

		//stage text is set once per step...
		auto& stage_text = registry.get<Text>(stage_text_entity);
		stage_text.content = "BATTLE";
		stage_text.colour = { 1.0f, 0.1f, 0.1f };

		registry.get<Text>(round_text_entity).content = "round: " + std::to_string(round_number);
		registry.get<Text>(food_text_entity).content = "food: " + std::to_string(health);
	}
	
}

void WorldSystem::deduct_health(int num) {
	
}

void un_highlight()
{
	auto view_ui = registry.view<HighlightBool>();
	for (auto [entity, highlight] : view_ui.each())
	{
		highlight.highlight = false;
	}
}


void WorldSystem::set_up_step(float elapsed_ms)
{
	// Restart/End game after max rounds
	

	if (round_number >= 9) {
		reward_multiplier = .5f;
	}

	set_up_timer -= elapsed_ms;

	// Updating window title with health and setup timer
	//std::stringstream title_ss;
	//title_ss << "Setup stage... Food: " << health << " Round: " << round_number << " Time left to setup: " << round(set_up_timer / 1000) << " fps: " << 1000.0 / elapsed_ms;
	//glfwSetWindowTitle(window, title_ss.str().c_str());
    
    auto particle_view = registry.view<ParticleSystem>();
    if (particle_view.size() < MAX_PARTICLES) {
        for (auto particle_entity : particle_view) {
            auto& particle = registry.get<ParticleSystem>(particle_entity);
            particle.life -= elapsed_ms;
            if (particle.life <= 0) {
                registry.destroy(particle_entity);
            }
        }
    }
    ParticleSystem::updateParticle();

    next_particle_spawn -= elapsed_ms;

    if (weather == RAIN && next_particle_spawn < 0.f)
    {
        next_particle_spawn = 60;
        vec2 velocity = {0.f, 450.0f};
        vec2 position = {rand() % WINDOW_SIZE_IN_PX.x + 1 , 0};
        float life = 1150.0f;
        std::string texture = "raindrop.png";
        std::string shader = "rain";
        ParticleSystem::createParticle(velocity, position, life, texture, shader);
    }
    else if (weather == DROUGHT) {
        // TODO
    }
    else if (weather == FOG && next_particle_spawn < 0.f) {
        next_particle_spawn = 3000;
        vec2 velocity = {-100.f, 0.f};
        vec2 position = {WINDOW_SIZE_IN_PX.x, rand() % (WINDOW_SIZE_IN_PX.y - 230)};
        float life = 13500.f;
        std::string texture = "cloud.png";
        std::string shader = "fog";
        ParticleSystem::createParticle(velocity, position, life, texture, shader);
    }
    else if (weather == SNOW && next_particle_spawn < 0.f)
    {
        next_particle_spawn = 40;
        vec2 velocity = {rand() % 400 + (-200), 300.0f};
        vec2 position = {rand() % WINDOW_SIZE_IN_PX.x + 1 , 0};
        float life = 1800.0f;
        std::string texture = "snow.png";
        std::string shader = "snow";
        ParticleSystem::createParticle(velocity, position, life, texture, shader);
    }

	if (set_up_timer <= 0)
	{
		player_state = battle_stage;
		set_up_timer = SET_UP_TIME;
		next_mob_spawn = 0;
		next_boss_spawn = 0;
		un_highlight();
		// set path at start of battle phase
		monster_path_coords = AISystem::MapAI::findPathAStar(current_map, FOREST_COORD, VILLAGE_COORD, is_walkable);

		std::cout << season_str << " season! \n";
		std::cout << "weather " << weather << " \n";
	}
	auto &stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "PREPARE: " + std::to_string((int)round(set_up_timer / 1000));
	stage_text.colour = {1.0f, 1.0f, 1.0f};
	registry.get<Text>(round_text_entity).content = "round: " + std::to_string(round_number);
	registry.get<Text>(food_text_entity).content = "food: " + std::to_string(health);
}

void WorldSystem::setup_start_menu()
{

	std::cout << "In Start Menu\n";
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	create_start_menu();
	camera = Camera::createCamera();
}

void WorldSystem::restart()
{
		
	std::cout << "Restarting\n";

	// Reset the game state
	current_speed = 1.f;
	health = STARTING_HEALTH;				  //reset health
	placement_unit_selected = ""; // no initial selection
	round_number = 0;
	reward_multiplier = 1;
	num_bosses_spawned = 0;
	num_mobs_spawned = 0;
	player_state = set_up_stage;
	set_up_timer = SET_UP_TIME;

	registry.clear(); // Remove all entities that we created

	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	//create UI	-- needs to be at the top of restart for rendering order.
	UI_button::createUI_button(0, tower_button, WATCHTOWER_COST);
	UI_button::createUI_button(1, green_house_button, GREENHOUSE_COST);
	UI_button::createUI_button(2, stick_figure_button, HUNTER_COST);
	UI_button::createUI_button(4, wall_button, WALL_COST);
	UI_button::createUI_button(7, upgrade_button, HUNTER_UPGRADE_COST, UPGRADE_BUTTON_TITLE, false);
	UI_button::createUI_button(8, sell_button, SELL_BUTTON_TITLE, false);
	UI_button::createUI_button(9, save_button, 0, SAVE_BUTTON_TITLE);
	UI_background::createUI_background();

	stage_text_entity = create_ui_text(vec2(5, 65), "PREPARE");
	round_text_entity = create_ui_text(vec2(5, 50), "");
	food_text_entity = create_ui_text(vec2(5, 35), "");

	// create grid map
	current_map = registry.get<GridMap>(GridMap::createGridMap());

	// create village
	village = Village::createVillage(current_map);

	// TODO: create forest
	current_map.setGridOccupancy(FOREST_COORD, OCCUPANCY_FOREST);
	camera = Camera::createCamera();

	// set up variables for first round
	setup_round_from_round_number(0);

	//Spider::createSpider(vec2(300,100), vec2(20,20));
}

nlohmann::json WorldSystem::get_json(std::string json_path)
{
	std::ifstream input_stream(json_path);

	if (input_stream.fail())
	{
		std::cout << "Not reading json file for path \"" + json_path + "\" \n";
		return NULL;
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

	game_state = story_card;
	StoryCard::createStoryCard(STORY_TEXT_PER_LEVEL[round_number], std::to_string(round_number));
    
    if (season_str == SPRING_TITLE)
    {
        season = SPRING;
        int weather_int = rand() % 2 + 1;
        if (weather_int % 2 == 1)
        {
            weather = RAIN;
        } else {
            weather = CLEAR;
        }
        create_boss = SpringBoss::createSpringBossEntt;
    }
    else if (season_str == SUMMER_TITLE)
    {
        season = SUMMER;
        int weather_int = rand() % 5 + 1;
        if (weather_int % 2 == 1)
        {
            weather = DROUGHT;
        } else {
            weather = CLEAR;
        }
        //create_boss = SummerBoss::createSummerBossEntt;
		create_boss = Spider::createSpider;
    }
    else if (season_str == FALL_TITLE)
    {
        season = FALL;
        int weather_int = rand() % 5 + 1;
        if (weather_int % 2 == 1)
        {
            weather = FOG;
        } else {
            weather = CLEAR;
        }
        create_boss = FallBoss::createFallBossEntt;
    }
    else if (season_str == WINTER_TITLE)
    {
        season = WINTER;
        int weather_int = rand() % 2 + 1;
        if (weather_int % 2 == 1)
        {
            weather = SNOW;
        } else {
            weather = CLEAR;
        }
        create_boss = WinterBoss::createWinterBossEntt;
    }
	else if (season_str == FINAL_TITLE)
	{
		season = SUMMER;

		fireball_delay_ms = 5100;
		next_fireball_spawn = fireball_delay_ms;
;
		int weather_int = rand() % 5 + 1;
		if (weather_int % 2 == 1)
		{
			weather = DROUGHT;
		}
		else {
			weather = CLEAR;
		}
		std::cout << "SPAWNING FINAL BOSS" << std::endl;
		create_boss = FinalBoss::createFinalBossEntt;
	}
}

void WorldSystem::updateProjectileMonsterCollision(entt::entity projectile, entt::entity monster)
{
	auto &animal = registry.get<Monster>(monster);
	auto &prj = registry.get<Projectile>(projectile);

	Mix_PlayChannel(-1, impact_sound, 0);

	animal.health -= prj.damage;
	animal.collided = true;

	auto &hit_reaction = registry.get<HitReaction>(monster);
	hit_reaction.counter_ms = 750; //ms duration used by health bar

	if (registry.has<RockProjectile>(projectile)) {
		if (!registry.has<EntityDeath>(projectile)) {
			auto& death = registry.emplace<EntityDeath>(projectile);
			death.timer = 1000;

			auto& rock = registry.get<RockProjectile>(projectile);
			auto& prj_motion = registry.get<Motion>(projectile);
			auto& monster_motion = registry.get<Motion>(monster);

			vec2 vector = (monster_motion.position - prj_motion.position);
			vec2 norm;
			if ((vector.x >= 0 && vector.y >= 0) || (vector.x < 0 && vector.y < 0)) norm = { vector.y, -vector.x };
			else norm = { vector.y, vector.x };

			std::vector<vec2> points;
			points.push_back(prj_motion.position);
			points.push_back(prj_motion.position + norm);
			points.push_back(prj_motion.position + norm - vector);

			rock.bezier_points = bezierVelocities(bezierCurve(points, 1000));
		}
	}
	else {
		registry.destroy(projectile);
	}

	if (animal.health <= 0)
	{
		health += animal.reward * reward_multiplier;

		if (registry.has<Rig>(monster)) {
			Rig::delete_rig(monster); //rigs have multiple pieces to be deleted
		}
		else {
			registry.destroy(monster);
		}

	}

}

bool WorldSystem::is_over() const
{
	return glfwWindowShouldClose(window) > 0;
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
	// if village is alive
	if (health > 0)
	{
	}
	if (key == GLFW_KEY_T) // for testing rigs
	{
		//auto view_rigs = registry.view<Timeline>();
		//for (auto entity : view_rigs) {
		//	auto& motion = registry.get<Motion>(entity);
		//	RigSystem::animate_rig_ik(entity, 15);
		//}
	}
	if (key == GLFW_KEY_Y) // for testing rigs
	{
		//auto& mouse = registry.get <MouseMovement>(camera);
		//auto view_rigs = registry.view<Rig>();
		//for (auto entity : view_rigs) {
		//	RigSystem::ik_solve(entity, mouse.mouse_pos, 1);
		//}
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
				if (registry.has<Rig>(monster)) {
					Rig::delete_rig(monster); //rigs have multiple pieces to be deleted
				}
				else {
					registry.destroy(monster);
				}
			}
		}
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_P && player_state == battle_stage) {
		std::cout << "Paused" << std::endl;
		player_state = pause_stage;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_P && player_state == pause_stage) {
		std::cout << "Game Resumed" << std::endl;
		player_state = battle_stage;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE)
	{
		game_setup();
		create_start_menu();
		player_state = set_up_stage;
		game_state = start_menu;
	}

	// hotkey for controls
	if (action == GLFW_RELEASE && key == GLFW_KEY_H)
	{
	    if (game_state == in_game) {
            // help menu
            auto help_menu_entity = create_help_menu();
            ShadedMeshRef &shaded_mesh_ref = registry.view<ShadedMeshRef>().get<ShadedMeshRef>(help_menu_entity);
            game_state = help_menu;
	    }
	    else if (game_state == help_menu) {
            auto view = registry.view<Menu, ShadedMeshRef>();
            for (auto entity : view)
            {
                auto &shaded_mesh_ref = view.get<ShadedMeshRef>(entity);
                shaded_mesh_ref.show = false;
            }
            game_state = in_game;
	    }
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
	{
		auto view = registry.view<Motion, MouseMovement>();
		auto &cam_motion = view.get<Motion>(camera);
		auto &mouse_move = view.get<MouseMovement>(camera);
		mouse_move.mouse_start = mouse_move.mouse_pos + cam_motion.position;
		mouse_move.is_pan_state = 1;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE)
	{
		auto view = registry.view<Motion, MouseMovement>();
		auto &motion = view.get<Motion>(camera);
		auto &mouse_move = view.get<MouseMovement>(camera);
		mouse_move.is_pan_state = 0;
	}

	// Hot keys for selecting placeable units
	else if (action == GLFW_PRESS && key == GLFW_KEY_1)
	{
		placement_unit_selected = "watchtower";
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_2)
	{
		placement_unit_selected = "greenhouse";
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_3)
	{
		placement_unit_selected = "hunter";
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		if (game_state == in_game) {
			int w, h;
			glfwGetWindowSize(window, &w, &h);

			restart();
		}
	}

	// Debugging
	if (action == GLFW_PRESS && key == GLFW_KEY_D)
		DebugSystem::in_debug_mode = !DebugSystem::in_debug_mode;
		//DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

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
	if (game_state != in_game) {
		return;
	}
	auto view = registry.view<Motion, MouseMovement>();
	auto &camera_motion = view.get<Motion>(camera);
	auto &camera_scale = camera_motion.scale;
	auto &camera_position = camera_motion.position;

	auto &mouse_movement = view.get<MouseMovement>(camera);

	double temp_scale = 20.0f;

	// zoom out limit
	if (camera_scale.y + (yoffset / temp_scale) < 1)
	{
		camera_scale = {1.f, 1.f};
		camera_motion.position = {0.f, 0.f};
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
			if (ui_element.tag == "in_game_ui_background")
			{
				ui_element_background_height = ui_element.scale.y;
			}
		}
		int unsigned map_height = WINDOW_SIZE_IN_PX.y - ui_element_background_height;
		if ((map_height * camera_motion.scale.y) - new_cam_pos_y < map_height)
		{
			new_cam_pos_y = (map_height * camera_motion.scale.y) - map_height;
		}

		camera_position = vec2(new_cam_pos_x, new_cam_pos_y);
	}
}

//will move this eventually
//atm this is repeated code because ui uses a different position/scale than gridnode
void grid_highlight_system(vec2 mouse_pos, std::string unit_selected, GridMap current_map)
{
	auto view_ui = registry.view<Motion, HighlightBool>();

	auto &node = current_map.getNodeAtCoord(pixel_to_coord(mouse_pos));
	for (auto [entity, grid_motion, highlight] : view_ui.each())
	{
		if (sdBox(mouse_pos, grid_motion.position, grid_motion.scale / 2.0f) < 0.0f && node.occupancy == OCCUPANCY_VACANT && node.terrain >= TERRAIN_DEFAULT)
		{
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
	if (in_game_area && placement_unit_selected != "" && player_state == set_up_stage)
		grid_highlight_system(mouse_pos, placement_unit_selected, current_map);

	// if village is alive
	if (health > 0)
	{
	}

	// camera control
	auto view = registry.view<Motion, MouseMovement>();
	auto &cam_motion = view.get<Motion>(camera);
	auto &mouse_move = view.get<MouseMovement>(camera);
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
			if (ui_element.tag == "in_game_ui_background")
			{
				ui_element_background_height = ui_element.scale.y;
			}
		}
		int unsigned map_height = WINDOW_SIZE_IN_PX.y - ui_element_background_height;
		if ((map_height * cam_motion.scale.y) - new_pos_y < map_height)
		{
			new_pos_y = (map_height * cam_motion.scale.y) - map_height;
		}
		cam_motion.position = vec2(new_pos_x, new_pos_y);
	}
}

void WorldSystem::on_mouse_click(int button, int action, int mod)
{
	//getting cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
  
	switch (game_state)
	{
	case start_menu:
		start_menu_click_handle(xpos, ypos, button, action, mod);
		break;
	case settings_menu:
		settings_menu_click_handle(xpos, ypos, button, action, mod);
		break;
	case in_game:
		unit_select_click_handle(xpos, ypos, button, action, mod);
		in_game_click_handle(xpos, ypos, button, action, mod);
		break;
	case help_menu:
		help_menu_click_handle(xpos, ypos, button, action, mod);
		break;
	case story_card:
		story_card_click_handle(xpos, ypos, button, action, mod);
		break;
	}
}

void WorldSystem::help_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		auto view = registry.view<Menu, ShadedMeshRef>();
		for (auto entity : view)
		{
			auto& shaded_mesh_ref = view.get<ShadedMeshRef>(entity);
			shaded_mesh_ref.show = false;
		}
		if (registry.empty<StoryCard>()) {
			game_state = in_game;
		}
		else {
			game_state = story_card;
		}
	}
}

void WorldSystem::story_card_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		auto story_card_view = registry.view<StoryCard>();
		for (auto entity : story_card_view)
		{
			registry.destroy(entity);
		}
		auto story_card_text_view = registry.view<StoryCardText>();
		for (auto entity : story_card_text_view)
		{
			registry.destroy(entity);
		}
		game_state = in_game;
	}
}

void WorldSystem::sell_unit_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
}

void WorldSystem::unit_select_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		bool unit_selected = false;
		bool sell_clicked = false;
		auto view_unit = registry.view<Unit>();
		auto view_highlight = registry.view<HighlightBool>();
		auto view_selectable = registry.view<Selectable, Motion>();
		auto view_ui_mesh = registry.view<UI_element, ShadedMeshRef>();
		entt::entity entity_selected;
		vec2 mouse_pos = mouse_in_world_coord({mouse_pos_x, mouse_pos_y});

		// get the unit modification buttons
		auto view_ui = registry.view<UI_element>();
		UI_element unit_mod_buttons;
		for (auto entity : view_ui)
		{
			unit_mod_buttons = view_ui.get<UI_element>(entity);
			if (unit_mod_buttons.tag == UPGRADE_BUTTON_TITLE || unit_mod_buttons.tag == SELL_BUTTON_TITLE)
			{
				if (sdBox(mouse_pos, unit_mod_buttons.position, unit_mod_buttons.scale / 2.0f) < 0.0f)
				{
					// check if something is already selected
					for (auto [entity, selectable, motion] : view_selectable.each())
					{
						if (selectable.selected)
						{
							entity_selected = entity;
							unit_selected = true;
							break;
						}
					}
					if (unit_mod_buttons.tag == SELL_BUTTON_TITLE)
					{
						sell_clicked = true;
					}
				}
			}
			if (unit_selected)
			{
				break;
			}
		}

		if (!unit_selected)
		{
			for (auto [entity, selectable, motion] : view_selectable.each())
			{
				// check click on units
				if (sdBox(mouse_pos, motion.position, motion.scale / 2.0f) < 0.0f)
				{
					selectable.selected = true;
					view_highlight.get<HighlightBool>(entity).highlight = true;

					if (registry.has<Unit>(entity))
					{
						auto unit_stats = view_unit.get<Unit>(entity);
						std::cout << "=== Unit stats ===\n";
						std::cout << "attack damage: " << unit_stats.damage << "\n";
						std::cout << "attack rate: " << unit_stats.attack_interval_ms << "\n";
						std::cout << "attack range: " << unit_stats.attack_range << "\n";
						entity_selected = entity;
						unit_selected = true;
					}
				}
				else
				{
					view_highlight.get<HighlightBool>(entity).highlight = false;
					selectable.selected = false;
				}
			}
		}
		// if a unit is selected and the sell button is not clicked
		if (unit_selected && !sell_clicked)
		{
			for (auto entity : view_ui_mesh)
			{
				if (view_ui_mesh.get<UI_element>(entity).tag == UPGRADE_BUTTON_TITLE)
				{
					Unit& unit = view_unit.get<Unit>(entity_selected);
					std::string button_text = "-" + std::to_string(unit.upgrade_cost);
					change_button_text(entity, button_text);
					auto& shaded_mesh_ref = view_ui_mesh.get<ShadedMeshRef>(entity);
					shaded_mesh_ref.show = true;
				}
				else if (view_ui_mesh.get<UI_element>(entity).tag == SELL_BUTTON_TITLE)
				{
					
					Unit& unit = view_unit.get<Unit>(entity_selected);
					std::string button_text = "+" + std::to_string(unit.sell_price);
					change_button_text(entity, button_text);
					auto& shaded_mesh_ref = view_ui_mesh.get<ShadedMeshRef>(entity);
					shaded_mesh_ref.show = true;
				}
			}
		}
		else
		{
			for (auto entity : view_ui_mesh)
			{
				if (view_ui_mesh.get<UI_element>(entity).tag == UPGRADE_BUTTON_TITLE || view_ui_mesh.get<UI_element>(entity).tag == SELL_BUTTON_TITLE)
				{
					//std::cout << "not upgrading\n";
					auto& shaded_mesh_ref = view_ui_mesh.get<ShadedMeshRef>(entity);
					shaded_mesh_ref.show = false;
				}
			}
		}
	}
}

void WorldSystem::start_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	std::string button_tag = "";
	if (action == GLFW_PRESS)
	{
		button_tag = on_click_button({mouse_pos_x, mouse_pos_y});
	}

	if (button_tag == EXIT)
	{
		// close window
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (button_tag == NEW_GAME)
	{
		remove_menu_buttons();
		restart();
        // show controls overlay
        auto help_menu_entity = create_help_menu();
        ShadedMeshRef &shaded_mesh_ref = registry.view<ShadedMeshRef>().get<ShadedMeshRef>(help_menu_entity);
        game_state = help_menu;
        button_tag = HELP_MENU;
	}
	else if (button_tag == SETTINGS_MENU)
	{
		remove_menu_buttons();
		game_state = settings_menu;
		create_controls_menu();
	}
	else if (button_tag == LOAD_GAME)
	{
		remove_menu_buttons();
		restart();
		load_game();
		game_state = in_game;
	}
}

void WorldSystem::settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	std::string button_tag = "";
	if (action == GLFW_PRESS)
	{
		button_tag = on_click_button({mouse_pos_x, mouse_pos_y});
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
		auto menu_text_view = registry.view<MenuText>();
		for (auto entity : menu_text_view)
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

void WorldSystem::game_setup()
{
	registry.clear();
	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);
	camera = Camera::createCamera();
}

void WorldSystem::create_start_menu()
{
	std::cout << "In Start Menu\n";
	Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "start_menu", Menu_texture::title_screen, 89, {1.0, 0.9});
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 3 / 7, "new_game", empty_button, "New game");
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 4 / 7, "load_game", empty_button, "Load game");
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 5 / 7, "settings_menu", empty_button, "Controls");
	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 6 / 7, "exit", empty_button, "Exit");
}

void WorldSystem::create_controls_menu()
{
	std::cout << "In Controls Menu\n";
	int menu_layer = 90;
	std::string menu_name = "controls";
	auto menu = Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, menu_name, Menu_texture::settings, menu_layer, { WINDOW_SIZE_IN_PX.x / 10, WINDOW_SIZE_IN_PX.x / 10});
	// title text
	std::string title_text = "Controls";
	auto title_text_scale = 1.2f;
	auto title_x_offset = (title_text.length() * title_text_scale * 27) / 2;
	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	auto& t = registry.emplace<Text>(menu, Text(title_text, notoRegular, vec2(WINDOW_SIZE_IN_PX.x / 2 - title_x_offset, WINDOW_SIZE_IN_PX.y - 170)));
	t.scale = title_text_scale;
	t.colour = { 1.0f, 0.8f, 0.0f };
	// hotkey text
	std::vector<std::string> hotkey_list = {"1", "2", "3", "4", "Space + mouse", "Scroll", "H", "Esc"};
	int para_y_offset = 230;
	int hotkey_para_x_offset = 410;
	auto hotkey_text_scale = 0.8f;
	for (int i = 0; i <= 7; i++) {
		auto entity = registry.create();
		auto& menu_text = registry.emplace<MenuText>(entity);
		menu_text.menu_name = menu_name;
		int y_offset = para_y_offset + (i * hotkey_text_scale * 60);
		std::string hotkey_text = hotkey_list[i];
		auto x_offset = (hotkey_text.length() * hotkey_text_scale * 27);
		auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
		auto& t = registry.emplace<Text>(entity, Text(hotkey_text, notoRegular, vec2(hotkey_para_x_offset - x_offset, WINDOW_SIZE_IN_PX.y - y_offset)));
		t.scale = hotkey_text_scale;
		t.colour = { 1.0f, 0.8f, 0.0f };
	}
	// hotkey description text
	std::vector<std::string> hotkey_des_list = { "Select watchtower", "Select greenhouse", "Select hunter", "Select wall",
											 "Move camera", "Zoom in / out", "Show  controls menu", "Goto title screen" };
	int des_para_x_offset = 570;
	for (int i = 0; i <= 7; i++) {
		auto entity = registry.create();
		auto& menu_text = registry.emplace<MenuText>(entity);
		menu_text.menu_name = menu_name;
		int y_offset = para_y_offset + (i * hotkey_text_scale * 60);
		std::string hotkey_text = hotkey_des_list[i];
		auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
		auto& t = registry.emplace<Text>(entity, Text(hotkey_text, notoRegular, vec2(des_para_x_offset, WINDOW_SIZE_IN_PX.y - y_offset)));
		t.scale = hotkey_text_scale;
		t.colour = { 1.0f, 0.8f, 0.0f };
	}
	// arrows
	for (int i = 0; i <= 7; i++) {
		auto arrow = registry.create();
		ShadedMesh& resource = cache_resource("control_arrow");
		if (resource.effect.program.resource == 0) {
			resource = ShadedMesh();
		}
		RenderSystem::createSprite(resource, menu_texture_path("control_arrow.png"), "textured");
		auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(arrow, resource);
		shaded_mesh_ref.layer = menu_layer + 1;
		UI_element& ui_element = registry.emplace<UI_element>(arrow);
		ui_element.tag = "control_arrow";
		ui_element.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
		int y_offset = para_y_offset + (i * hotkey_text_scale * 60) - 10;
		ui_element.position = vec2(hotkey_para_x_offset + (des_para_x_offset - hotkey_para_x_offset) / 2, y_offset);
		auto& menu_text = registry.emplace<MenuText>(arrow);
		menu_text.menu_name = menu_name;
	}

	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 4 / 5, "back", empty_button, "back");
}

entt::entity WorldSystem::create_help_menu()
{
	std::cout << "In Help Menu\n";
	return Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "help_menu", Menu_texture::help_menu, 98, {0.5, 0.5});
}

void WorldSystem::in_game_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	Motion camera_motion = registry.get<Motion>(camera);

	// cursor position in world pos
	vec2 mouse_world_pos = mouse_in_world_coord(vec2({xpos, ypos}));

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
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && placement_unit_selected != "" && in_game_area)
		{
			auto &node = current_map.getNodeAtCoord(pixel_to_coord(vec2(x, y)));

			if (node.occupancy == OCCUPANCY_VACANT && node.terrain >= TERRAIN_DEFAULT)
			{
				if (placement_unit_selected == HUNTER_NAME && health >= HUNTER_COST)
				{
					entt::entity entity = Hunter::createHunter({x, y});
					health -= HUNTER_COST;
					current_map.setGridOccupancy(pixel_to_coord(vec2(x,y)), OCCUPANCY_HUNTER);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == GREENHOUSE_NAME && health >= GREENHOUSE_COST)
				{
					entt::entity entity = GreenHouse::createGreenHouse({x, y});
					health -= GREENHOUSE_COST;
                    current_map.setGridOccupancy(pixel_to_coord(vec2(x,y)), OCCUPANCY_GREENHOUSE);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == WATCHTOWER_NAME && health >= WATCHTOWER_COST)
				{
					entt::entity entity = WatchTower::createWatchTower({x, y});
					health -= WATCHTOWER_COST;
                    current_map.setGridOccupancy(pixel_to_coord(vec2(x,y)), OCCUPANCY_TOWER);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == WALL_NAME && health >= WALL_COST)
				{
					entt::entity entity = Wall::createWall({x, y}, false);
					health -= WALL_COST;
                    current_map.setGridOccupancy(pixel_to_coord(vec2(x,y)), OCCUPANCY_WALL);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else
				{
					//insufficent funds -- should feedback be given here, or when the button is pressed?
					Mix_PlayChannel(-1, ui_sound_negative_tick, 0);
				}
				placement_unit_selected = "";
				un_highlight();
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !in_game_area)
		{

			if (ui_button == Button::tower_button)
			{

				placement_unit_selected = WATCHTOWER_NAME;
			}
			else if (ui_button == Button::green_house_button)
			{

				placement_unit_selected = GREENHOUSE_NAME;
			}
			else if (ui_button == Button::stick_figure_button)
			{

				placement_unit_selected = HUNTER_NAME;
			}
			else if (ui_button == Button::wall_button)
			{
				placement_unit_selected = WALL_NAME;
			}
			else if (ui_button == Button::sell_button)
			{
				auto view_selectable = registry.view<Selectable>();
				auto view_unit = registry.view<Unit>();
				for (auto entity : view_selectable)
				{
					if (view_selectable.get<Selectable>(entity).selected)
					{
						auto& unit = view_unit.get<Unit>(entity);
						health += unit.sell_price;
						sell_unit(entity);
					}
				}
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
						auto &unit = view_unit.get<Unit>(entity);
						health -= unit.upgrade_cost;
						upgrade_unit(unit);
						std::cout << "Damage increased!\n";
					}
				}
			}
			else if (ui_button == Button::save_button)
			{
				save_game();
			}
			else
			{
				placement_unit_selected = "";
			}
		}

		//std::cout << "selected: " << unit_selected << std::endl;

		// handle clicks in the start menu
	}
}

void WorldSystem::sell_unit(entt::entity &entity)
{
    auto& motion = registry.get<Motion>(entity);
    current_map.setGridOccupancy(pixel_to_coord(motion.position), OCCUPANCY_VACANT, motion.scale);
	registry.destroy(entity);
}

void WorldSystem::upgrade_unit(Unit &unit)
{
	unit.damage += 5;
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
	for (auto &entity : view_selectable)
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

	std::vector<std::vector<nlohmann::json>> map;
	for (int x = 0; x < MAP_SIZE_IN_COORD.x; x++) {
		std::vector<nlohmann::json> map_row;
		for (int y = 0; y < MAP_SIZE_IN_COORD.y; y++) {
			auto& node = current_map.getNodeAtCoord(ivec2(x, y));
			nlohmann::json node_json; 
			node_json["terrain"] = node.terrain;
			node_json["occupancy"] = node.occupancy;
			map_row.push_back(node_json);
		}
		map.push_back(map_row);
	}

	save_json["map_data"] = map;

	std::ofstream file(SAVE_PATH);
	file << save_json.dump(4);
	file.close();

	// references file from build folder but requires the folder /data/save_files to be present
	// TODO: not sure how it works
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
	if (save_json == NULL) {
		return;
	}

	health = save_json["health"];
	round_number = save_json["round_number"];

	setup_round_from_round_number(round_number);

	for (nlohmann::json unit : save_json["units"])
	{
		int x = unit["x_coord"];
		int y = unit["y_coord"];
		auto &node = current_map.getNodeAtCoord(pixel_to_coord(vec2(x, y)));
		std::string type = unit["type"];
		entt::entity entity;
		if (type == WATCHTOWER_NAME)
		{
			entity = WatchTower::createWatchTower({x, y});
			node.occupancy = OCCUPANCY_TOWER;
		}
		else if (type == GREENHOUSE_NAME)
		{
			entity = GreenHouse::createGreenHouse({x, y});
			node.occupancy = OCCUPANCY_TOWER;
		}
		else if (type == WALL_NAME)
		{
			entity = Wall::createWall({x, y}, unit["rotate"]);
			node.occupancy = OCCUPANCY_TOWER;
		}
		else if (type == HUNTER_NAME)
		{
			entity = Hunter::createHunter({x, y});
			node.occupancy = OCCUPANCY_TOWER;
		}

		auto view_unit = registry.view<Unit>();
		auto &curr_unit = view_unit.get<Unit>(entity);
		for (int i = 0; i < unit["upgrades"]; i++)
		{
			upgrade_unit(curr_unit);
		}
	}

	std::vector<std::vector<nlohmann::json>> map = save_json["map_data"];
	for (int x = 0; x < MAP_SIZE_IN_COORD.x; x++) {
		for (int y = 0; y < MAP_SIZE_IN_COORD.y; y++) {
			int terrain = map[x][y]["terrain"];
			int occupancy = map[x][y]["occupancy"];
			current_map.setGridTerrain(ivec2(x, y), terrain);
			current_map.setGridOccupancy(ivec2(x, y), occupancy);
		}
	}
}
