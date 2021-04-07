
// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "monsters/spring_boss.hpp"
#include "monsters/fall_boss.hpp"
#include "monsters/summer_boss.hpp"
#include "monsters/winter_boss.hpp"
#include "monsters/burrow_boss.hpp"
#include "monsters/final_boss.hpp"
#include "monsters/fireball_boss.hpp"
#include "monsters/mob.hpp"
#include <projectile.hpp>

#include "grid_map.hpp"
#include "units/hunter.hpp"
#include "units/greenhouse.hpp"
#include "units/watchtower.hpp"
#include "units/village.hpp"
#include "units/wall.hpp"
#include "units/unit.hpp"
#include "camera.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "story_card.hpp"
#include "ui.hpp"
#include "ai.hpp"
#include "particle.hpp"

#include "rig.hpp"
#include "monsters/spider.hpp"
#include <BehaviorTree.hpp>

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <units/forest.hpp>

#include "json.hpp"
#include <units/exterminator.hpp>
#include <units/robot.hpp>
#include <units/priestess.hpp>
#include <units/snowmachine.hpp>
#include <units/rangecircle.hpp>

const size_t ANIMATION_FPS = 20;
const int STARTING_HEALTH = 1000;

int WorldSystem::health = 1000;
float WorldSystem::speed_up_factor = 1.f;
float WorldSystem::reward_multiplier = 1.f;
GridMap WorldSystem::current_map;
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.

std::map<int, std::vector<ivec2>> default_monster_paths;
std::vector<int> current_round_monster_types;
const std::string NEW_GAME = "new_game";
const std::string SAVE_GAME = "save_game";
const std::string LOAD_GAME = "load_game";
const std::string HELP_MENU = "help_menu";
const std::string SETTINGS_MENU = "settings_menu";
const std::string EXIT = "exit";
const std::string PATH_1_UPGRADE_BUTTON_TITLE = "path_1_upgrade_button";
const std::string PATH_2_UPGRADE_BUTTON_TITLE = "path_2_upgrade_button";
const std::string SELL_BUTTON_TITLE = "sell_button";
const std::string START_BUTTON_TITLE = "start_button";

const std::string TIPS_BUTTON_TITLE = "tips_button";
const std::string SPRING_TITLE = "spring";
const std::string SUMMER_TITLE = "summer";
const std::string FALL_TITLE = "fall";
const std::string WINTER_TITLE = "winter";
const std::string FINAL_TITLE = "final";
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
	selected_view_change(true),
	game_tips(true)
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

    for (int monster_type = 0; monster_type < monster_type_count; monster_type++) {
        default_monster_paths.insert(std::pair<int, std::vector<ivec2>>(monster_type, {}));
    }
	speed_up_factor = 1.f;
	tip_manager = TipManager::TipManager();
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
void WorldSystem::step(float elapsed_ms)
{
	if (game_state == in_game) {

		//rig animation
		auto view_rigs = registry.view<Timeline>();
		for (auto entity : view_rigs) {
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
			
			registry.emplace<DamageProperties>(boss);
			auto& monster = registry.get<Monster>(boss);
			monster.path_coords = default_monster_paths.at(monster.type);

			num_bosses_spawned += 1;
			BTCollision->init(boss);
		}

		// Spawning new mobs
		next_mob_spawn -= (int)(elapsed_ms * current_speed);
		if (num_mobs_spawned < max_mobs && next_mob_spawn < 0.f)
		{
			next_mob_spawn = (mob_delay_ms / 2) + uniform_dist(rng) * (mob_delay_ms / 2);
			entt::entity mob = Mob::createMobEntt();

			registry.emplace<DamageProperties>(mob);
			auto& monster = registry.get<Monster>(mob);
            monster.path_coords = default_monster_paths.at(monster.type);

			num_mobs_spawned += 1;
			BTCollision->init(mob);
		}

		// spawn new fireballs for the final boss
		next_fireball_spawn -= (int)(elapsed_ms * current_speed);
		if (!registry.empty<FinalBoss>() && next_fireball_spawn < 0.f)
		{
			next_fireball_spawn = FIREBALL_DELAY_MS;
			entt::entity fireball = FireballBoss::createFireballBossEntt();

			registry.emplace<DamageProperties>(fireball);
			auto& monster = registry.get<Monster>(fireball);
            monster.path_coords = default_monster_paths.at(monster.type);

			BTCollision->init(fireball);
		}

		// update velocity for every monster
		for (auto entity : registry.view<Monster>())
		{
			auto& damage = registry.get<DamageProperties>(entity);
			for (auto& [key, value] : damage.dot_map) {
				value -= ELAPSED_MS * WorldSystem::speed_up_factor;
			}

			auto& monster = registry.get<Monster>(entity);
			float num = -1.f;
			float max_slow = 0;
			while (!damage.slow_queue.empty()) {
				std::pair<float, float> pair = damage.slow_queue.top();
				if (num == -1.f) {
					pair.second -= elapsed_ms;
					num = SLOW_DELAY - pair.second;
				}
				else {
					pair.second -= num;
				}

				if (pair.second < 0) {
					damage.slow_queue.pop();
				}
				else {
					damage.slow_queue.pop();
					damage.slow_queue.push(pair);
					max_slow = pair.first;
					break;
				}
			}
			monster.speed_multiplier *= (100 / (100 - damage.current_slow));
			monster.speed_multiplier *= ((100 - max_slow) / 100);
			damage.current_slow = max_slow;

			auto state = BTCollision->process(entity);
			if (health < 0) {
				//restart_with_save();
				start_lost_game_screen();
				return;
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

		// remove disapperaing text when time's up 
		auto view_disappearing_text = registry.view<DisappearingText>();
		for (auto entity : view_disappearing_text)
		{
			auto& disap_time = view_disappearing_text.get<DisappearingText>(entity);
			disap_time.on_screen_time_ms -= elapsed_ms * current_speed;
			if (disap_time.on_screen_time_ms < 0)
			{
				registry.destroy(entity);
			}
		}

		// change hit point text scale
		auto view_hitpoints_text = registry.view<HitPointsText>();
		for (auto entity : view_hitpoints_text)
		{
			auto& hitpoints_text = view_hitpoints_text.get<HitPointsText>(entity);
			auto& text = registry.get<Text>(entity);
			float scale_change = 0.005 * elapsed_ms * current_speed;
			if (text.scale - scale_change > hitpoints_text.min_scale)
			{
				text.scale -= scale_change;
			}
		}

		auto& food_num_text = registry.get<Text>(food_text_entity);
		food_num_text.content = std::to_string(health);

		// Increment round number if all enemies are not on the map and projectiles are removed
		if (num_bosses_spawned == max_boss && num_mobs_spawned == max_mobs)
		{
			if (registry.view<Monster>().empty() && registry.view<Projectile>().empty())
			{
				// round cleared text
				auto closeness_outline = TextFont::load("data/fonts/Closeness/closeness.outline-regular.ttf");
				auto closeness_regular = TextFont::load("data/fonts/Closeness/closeness.regular.ttf");
				vec2 text_position = get_center_text_position(WINDOW_SIZE_IN_PX, {WINDOW_SIZE_IN_PX.x/2, WINDOW_SIZE_IN_PX.y/2}, 2.f, "ROUND CLEARED!");
				DisappearingText::createDisappearingText(closeness_regular, "ROUND CLEARED!", text_position, 500, 2.f, vec3({ 245.f / 255.f, 216.f / 255.f, 51.f / 255.f}));
				DisappearingText::createDisappearingText(closeness_outline, "ROUND CLEARED!", text_position, 500, 2.f, vec3({ 0.f, 0.f, 0.f }));
				// if no greenhouse, shorten the end phase delay
				if (!greenhouse_food_increased)
				{
					if (registry.view<GreenHouse>().size() == 0)
					{
						end_of_battle_stage_dealy_ms -= 500;
						greenhouse_food_increased = true;
					}
					else if (end_of_battle_stage_dealy_ms < END_OF_BATTLE_STAGE_DELAY_MS - 500)
					{
						// greenhouse triggers at the end of battle phase once with a short delay
						int total_greenhouse_food = 0;
						for (auto entity : registry.view<GreenHouse>())
						{
							auto greenhouse = registry.get<Unit>(entity);
							total_greenhouse_food += (int)((float)greenhouse.damage * reward_multiplier);
						}
						if (total_greenhouse_food != 0)
						{
							add_health(total_greenhouse_food);
						}
						greenhouse_food_increased = true;
					}

				}
								// count down timer
				end_of_battle_stage_dealy_ms -= elapsed_ms * current_speed;
				// end battle phase and set up next round 
				if (end_of_battle_stage_dealy_ms <= 0.f)
				{
					end_battle_phase();
					greenhouse_food_increased = false;
					end_of_battle_stage_dealy_ms = END_OF_BATTLE_STAGE_DELAY_MS;
				}
				
			}
		}
	}
	
}

void WorldSystem::title_screen_step(float elapsed_ms)
{
	// eyes blinking
	for (auto entity : registry.view<TitleEyes, ShadedMeshRef>())
	{
		auto& eyes = registry.get<TitleEyes>(entity);
		auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);

		eyes.blink_delay_ms -= elapsed_ms;
		// time to blink
		if (eyes.blink_delay_ms < 0.f)
		{
			shaded_mesh_ref.show = false;
			eyes.blink_time_ms -= elapsed_ms;
		}
		// time to open eyes
		if (eyes.blink_time_ms < 0.f)
		{
			if (eyes.show) {
				shaded_mesh_ref.show = true;
			}
			eyes.blink_delay_ms = rand() % 4000 + 1000; // 1 ~ 5 sec
			eyes.blink_time_ms = 200;
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

		if (next_particle_spawn < 0.f)
		{
			next_particle_spawn = 200;
			vec2 velocity = { rand() % 100 - 50, rand() % 100 - 50 };
			vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , rand() % 250 + 1};
			float life = 20000.0f;
			std::string texture = "snow.png";
			std::string shader = "snow";
			ParticleSystem::createParticle(velocity, position, life, texture, shader);
		}
	}

	for (auto entity : particle_view) {
		auto& motion = registry.get<Motion>(entity);
		if (motion.position.x < 0 || motion.position.x > WINDOW_SIZE_IN_PX.x)
			registry.destroy(entity);
		if (motion.position.y < 0 || motion.position.y > 250)
			motion.velocity.y *= -1;
	}
}

void WorldSystem::lost_game_screen_step(float elapsed_ms)
{
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
}

void WorldSystem::darken_screen_step(float elapsed_ms)
{
	auto& screen_state = registry.get<ScreenState>(screen_state_entity);
	if (screen_state.darken_screen_factor < 2.f)
	{
		screen_state.darken_screen_factor += elapsed_ms / 1000;
	}
	else
	{
		game_state = GameState::lost_game_screen;
		screen_state.darken_screen_factor = 0.f;
		registry.clear();
		screen_state_entity = registry.create();
		registry.emplace<ScreenState>(screen_state_entity);
		camera = Camera::createCamera();
		Menu::createLostMenu();
		/*auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
		registry.emplace<Text>(registry.create(), Text::Text("Famine", notoRegular, { WINDOW_SIZE_IN_PX.x / 2, 200 }, 2.0f, { 1.f, 0.f, 0.f }));*/
		MenuButton::create_button(RESTART_ROUND_BUTTON_X, RESTART_ROUND_BUTTON_Y, MenuButtonType::restart_round_button, "Restart round");
		MenuButton::create_button(EXIT_BUTTON_X, EXIT_BUTTON_Y, MenuButtonType::exit_button, "Exit");
	}
	
}

// lost game
void WorldSystem::start_lost_game_screen()
{
	auto& screen_state = registry.get<ScreenState>(screen_state_entity);
	screen_state.darken_screen_factor = 0.3;
	screen_state.all_dark = false;
	game_state = GameState::darken_screen;
	
}

// called at the end of battle pahse to set up next round
void WorldSystem::end_battle_phase()
{
	world_round_number++;

	if (world_round_number == MAX_ROUND_NUMBER)
	{
		restart_with_save();
	}

	setup_round_from_round_number(world_round_number);
	// re-roll some fraction of map for weather terrains
	int max_rerolls = (int)ceil(0.3 * MAP_SIZE_IN_COORD.x * MAP_SIZE_IN_COORD.y);
	AISystem::MapAI::setRandomWeatherTerrain(current_map, max_rerolls, weather);
	player_state = set_up_stage;
	num_bosses_spawned = 0;
	num_mobs_spawned = 0;
	prepare_setup_stage();

	
	save_game();
}

void WorldSystem::handle_game_tips()
{
	switch (tip_manager.tip_index)
	{
	case 0:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_0);
		game_state = paused;
		tip_manager.tip_index++;
		game_tips = false;
		break;
	case 1:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_1);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 2:
		TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_BOTTOM_Y, start_tips_2);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 3:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_3);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 4:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_4);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 5:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_CENTRE_Y, start_tips_5);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 6:
		TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_BOTTOM_Y, start_tips_6);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 7:
		TipCard::createTipCard(TIP_CARD_CENTRE_X, TIP_CARD_TOP_Y, start_tips_7);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 8:
		TipCard::createTipCard(TIP_CARD_RIGHT_X, TIP_CARD_TOP_Y, start_tips_8);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 9:
		TipCard::createTipCard(TIP_CARD_LEFT_X - 50, TIP_CARD_TOP_Y, start_tips_9);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	case 10:
		TipCard::createTipCard(TIP_CARD_RIGHT_X, TIP_CARD_BOTTOM_Y, start_tips_10);
		game_state = paused;
		tip_manager.tip_index++;
		break;
	}
}

void WorldSystem::deduct_health(int num) {
	WorldSystem::health -= num;
	HealthChangeText::create_haelth_deduct_text(num, health);
}


void WorldSystem::add_health(int num) {
	WorldSystem::health += num;
	HealthChangeText::create_haelth_gain_text(num, health);
}

void un_highlight()
{
	auto view_ui = registry.view<HighlightBool>();
	for (auto [entity, highlight] : view_ui.each())
	{
		highlight.highlight = false;
	}
}

// helper for mouse_hover_ui_button
// show unit description when hover on unit button
void remove_descriptions()
{
	//std::cout << "Hover: " << build_ui.unit_name << "\n";
	for (auto entity : registry.view<UI_unit_description_card>())
		registry.destroy(entity);

	for (auto entity : registry.view<UI_selected_description_card>())
		registry.destroy(entity);
}

void WorldSystem::prepare_setup_stage()
{
	player_state = set_up_stage;
	remove_descriptions();
	auto view_ui_button = registry.view<UI_element, ShadedMeshRef>();
	for (auto button_entt : view_ui_button)
	{
		auto ui_button = view_ui_button.get<UI_element>(button_entt);

		if (ui_button.tag == START_BUTTON_TITLE)
		{
			RenderSystem::show_entity(button_entt);
		}
	}
	// remove hit point text that are still on the screen
	auto view_hit_point_text = registry.view<DisappearingText>();
	for (auto entity : view_hit_point_text)
	{
		registry.destroy(entity);
	}
}

void WorldSystem::set_up_step(float elapsed_ms)
{
	// Restart/End game after max rounds
	if (world_round_number == 0 && game_tips) {
		handle_game_tips();
	}

	if (world_round_number >= 9) {
		reward_multiplier = .5f;
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
	}
	ParticleSystem::updateParticle();

	next_particle_spawn -= elapsed_ms;

	if (weather == RAIN && next_particle_spawn < 0.f)
	{
		next_particle_spawn = 60;
		vec2 velocity = { 0.f, 450.0f };
		vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , UI_TOP_BAR_HEIGHT };
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
		vec2 position = { WINDOW_SIZE_IN_PX.x + 500, rand() % (WINDOW_SIZE_IN_PX.y - 230) };
		float life = 22000.f;
		std::string texture = "cloud.png";
		std::string shader = "fog";
		ParticleSystem::createParticle(velocity, position, life, texture, shader);
	}
	else if (weather == SNOW && next_particle_spawn < 0.f)
	{
		next_particle_spawn = 40;
		vec2 velocity = { rand() % 400 + (-200), 300.0f };
		vec2 position = { rand() % WINDOW_SIZE_IN_PX.x + 1 , UI_TOP_BAR_HEIGHT };
		float life = 1800.0f;
		std::string texture = "snow.png";
		std::string shader = "snow";
		ParticleSystem::createParticle(velocity, position, life, texture, shader);
	}

	// remove disapperaing text when time's up 
	auto view_disappearing_text = registry.view<DisappearingText>();
	for (auto entity : view_disappearing_text)
	{
		auto& disap_time = view_disappearing_text.get<DisappearingText>(entity);
		disap_time.on_screen_time_ms -= elapsed_ms * current_speed;
		if (disap_time.on_screen_time_ms < 0)
		{
			registry.destroy(entity);
		}
	}

	//registry.get<Text>(round_text_entity).content = std::to_string(round_number + 1);
	// only supports up to 2 digit rounds (99 max round)
	if (registry.get<Text>(round_text_entity).content.length() == 2)
		registry.get<Text>(round_text_entity).position.x = ROUND_NUM_X_OFFSET - 20;
	registry.get<Text>(food_text_entity).content = std::to_string(health);
}

void WorldSystem::start_round()
{

	game_tips = false;
	// hide start_button
	auto view_ui_button = registry.view<UI_element, ShadedMeshRef>();
	for (auto button_entt : view_ui_button)
	{
		auto ui_button = view_ui_button.get<UI_element>(button_entt);

		if (ui_button.tag == START_BUTTON_TITLE || ui_button.tag == TIPS_BUTTON_TITLE)
		{
			RenderSystem::hide_entity(button_entt);
		}
	}
	player_state = battle_stage;
	next_mob_spawn = 0;
	next_boss_spawn = 0;
	un_highlight();

	auto& stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "BATTLE";
	stage_text.colour = { 1.0f, 0.1f, 0.1f };

	// set default paths for monster AI for this round
	for (int monster_type : current_round_monster_types) {
		default_monster_paths.at(monster_type) = AISystem::MapAI::findPathAStar(current_map, monster_type);
	}
	std::cout << world_season_str << " season! \n";
	std::cout << "weather " << weather << " \n";
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

void destroy_entity(const entt::entity entity)
{
	registry.destroy(entity);
}

void WorldSystem::restart()
{
		
	std::cout << "Restarting\n";

	// Reset the game state
	current_speed = 1.f;
	health = STARTING_HEALTH;				  //reset health
	placement_unit_selected = NONE; // no initial selection
	world_round_number = 0;
	reward_multiplier = 1;
	num_bosses_spawned = 0;
	num_mobs_spawned = 0;
	player_state = set_up_stage;

	registry.each(destroy_entity);
	registry.clear(); // Remove all entities that we created

	screen_state_entity = registry.create();
	registry.emplace<ScreenState>(screen_state_entity);

	//create UI	
	//UI_button::createUI_build_unit_button(0, watchtower_button, watchtower_unit.cost);
	UI_button::createUI_build_unit_button(0, hunter_button, unit_cost.at(HUNTER));
	UI_button::createUI_build_unit_button(1, exterminator_button, unit_cost.at(EXTERMINATOR));
	UI_button::createUI_build_unit_button(2, robot_button, unit_cost.at(ROBOT));
	UI_button::createUI_build_unit_button(3, priestess_button, unit_cost.at(PRIESTESS));
	UI_button::createUI_build_unit_button(4, snowmachine_button, unit_cost.at(SNOWMACHINE));
	UI_button::createUI_build_unit_button(5, green_house_button, unit_cost.at(GREENHOUSE));
	UI_button::createUI_build_unit_button(6, wall_button, unit_cost.at(WALL));
	// general buttons
	//UI_button::createUI_button(7, tips_button, TIPS_BUTTON_TITLE);
	//UI_button::createUI_button(8, start_button, START_BUTTON_TITLE);
	//UI_button::createUI_button(9, save_button, SAVE_BUTTON_TITLE);
	// ui background
	UI_background::createUI_background();
	UI_background::createUI_top_bar();
	// season wheel
	UI_season_wheel::createUI_season_wheel();
	season_wheel_arrow_entity = UI_season_wheel::createUI_season_wheel_arrow();
	// weather icon
	weather_icon_entity = UI_weather_icon::createUI_weather_icon();
	// ui text
	season_text_entity = create_ui_text(vec2(SEASON_X_OFFSET, WINDOW_SIZE_IN_PX.y - SEASON_Y_OFFSET), "Spring", SEASON_SCALE);
	weather_text_entity = create_ui_text(vec2(WEATHER_TEXT_X_OFFSET, WINDOW_SIZE_IN_PX.y - WEATHER_TEXT_Y_OFFSET), "Clear", WEATHER_TEXT_SCALE);
	stage_text_entity = create_ui_text(vec2(5, 65), "PREPARE");
	// round label
	create_ui_text(vec2(ROUND_LABEL_X_OFFSET, WINDOW_SIZE_IN_PX.y - ROUND_LABEL_Y_OFFSET), "Round:          / " + std::to_string(MAX_ROUND_NUMBER), ROUND_LABEL_SCALE);
	// round number text
	round_text_entity = create_ui_text(vec2(ROUND_NUM_X_OFFSET, WINDOW_SIZE_IN_PX.y - ROUND_NUM_Y_OFFSET), "1", ROUND_NUM_SCALE, { 1.f, 0.f, 0.f });
	// food label
	create_ui_text(vec2(FOOD_LABEL_X_OFFSET, WINDOW_SIZE_IN_PX.y - FOOD_LABEL_Y_OFFSET), "Food:", FOOD_LABEL_SCALE);
	// food number text
	food_text_entity = create_ui_text(vec2(FOOD_NUM_X_OFFSET, WINDOW_SIZE_IN_PX.y - FOOD_NUM_Y_OFFSET), "", FOOD_NUM_SCALE, { 0.f, 1.f, 0.f });
	// pause menu
	pause_menu_entity = Menu::createMenu((float)WINDOW_SIZE_IN_PX.x / 2, (float)WINDOW_SIZE_IN_PX.y / 2, "pause_menu", Menu_texture::pause_menu, 90, vec2({ 22.f, 26.f }));
	registry.get<ShadedMeshRef>(pause_menu_entity).show = false;
	// help menu
	help_menu_entity = Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "help_menu", Menu_texture::help_menu, 98, { 0.5, 0.5 });
	RenderSystem::hide_entity(help_menu_entity);
	// create grid map
	current_map = registry.get<GridMap>(GridMap::createGridMap());
	village = Village::createVillage(current_map);
	Forest::createForest(current_map);

	// set up tip manager
	tip_manager = TipManager();

    BTCollision = AISystem::MonstersAI::createBehaviorTree();

	camera = Camera::createCamera();

	// set up variables for first round
	setup_round_from_round_number(0);
}

void WorldSystem::setup_round_from_round_number(int round_number)
{
	auto& stage_text = registry.get<Text>(stage_text_entity);
	stage_text.content = "PREPARE";
	stage_text.colour = { 1.0f, 1.0f, 1.0f };

	nlohmann::json round_json = get_json(INPUT_PATH + std::to_string(round_number) + JSON_EXTENSION);
	max_mobs = round_json["max_mobs"];
	mob_delay_ms = round_json["mob_delay_ms"];
	max_boss = round_json["max_bosses"];
	boss_delay_ms = round_json["boss_delay_ms"];
	world_season_str = round_json["season"];
	int prev_weather = weather;

	for (auto& story_card : registry.view<StoryCard>())
	{
		registry.destroy(story_card);
	}

	for (auto entity : registry.view<StoryCardText>())
	{
		registry.destroy(entity);
	}

	if (game_state != help_menu)
	{
		game_state = story_card;
		StoryCard::createStoryCard(STORY_TEXT_PER_LEVEL[round_number], std::to_string(round_number + 1));
	}

    current_round_monster_types.clear();
    current_round_monster_types.emplace_back(MOB);
    if (world_season_str == SPRING_TITLE)
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
        current_round_monster_types.emplace_back(SPRING_BOSS);
    }
    else if (world_season_str == SUMMER_TITLE)
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
        current_round_monster_types.emplace_back(SPIDER);
    }
    else if (world_season_str == FALL_TITLE)
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
        current_round_monster_types.emplace_back(FALL_BOSS);
    }
    else if (world_season_str == WINTER_TITLE)
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
        current_round_monster_types.emplace_back(WINTER_BOSS);
    }
	else if (world_season_str == FINAL_TITLE)
	{
		season = SUMMER;

		fireball_delay_ms = 5100;
		next_fireball_spawn = fireball_delay_ms;

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
        // current_round_monster_types.emplace_back(FINAL_BOSS);
	}
	if (prev_weather != weather || round_number == 0) {
	    AISystem::MapAI::setRandomMapWeatherTerrain(current_map, weather);
	}

	// update text
	auto& round_text = registry.get<Text>(round_text_entity);
	round_text.content = std::to_string(round_number + 1);
	// only supports up to 2 digit rounds (99 max round)
	if (round_text.content.length() == 2)
		round_text.position.x = ROUND_NUM_X_OFFSET - 20;

	auto& food_num_text = registry.get<Text>(food_text_entity);
	food_num_text.content = std::to_string(health);

	auto& season_text = registry.get<Text>(season_text_entity);
	season_text.content = season_str.at(season);
	season_text.colour = season_str_colour.at(season);
	//aligne_text_right(season_text_entity, SEASON_WHEEL_X_OFFSET - 5.f);

	auto& weather_text = registry.get<Text>(weather_text_entity);
	weather_text.content = weather_str.at(weather);
	weather_text.colour = weather_str_colour.at(weather);
	// update season wheel angle
	auto& season_wheel_arrow = registry.get<UI_element>(season_wheel_arrow_entity);
	season_wheel_arrow.angle += PI / (2 * ROUND_PER_SEASON);

	UI_weather_icon::change_weather_icon(weather_icon_entity, weather);
}

void WorldSystem::damage_monster_helper(entt::entity e_monster, int damage, bool slow) {
	
	auto& monster = registry.get<Monster>(e_monster);
	Mix_PlayChannel(-1, impact_sound, 0);

	if (slow) {
		auto& damage_properties = registry.get<DamageProperties>(e_monster);
		damage_properties.slow_queue.push({ damage, SLOW_DELAY });
		float max_slow = damage_properties.slow_queue.top().first;
		monster.speed_multiplier *= (100 / (100 - damage_properties.current_slow));
		monster.speed_multiplier *= ((100 - max_slow) / 100);
		damage_properties.current_slow = max_slow;
		// add hit point text
		HitPointsText::create_hit_points_text(damage, e_monster, {0.f, 0.f, 1.f});
	}
	else {
		monster.health -= damage;
		// add hit point text
		HitPointsText::create_hit_points_text(damage, e_monster, { 1.f, 0.f, 0.f });
	}
	
	monster.collided = true;

	auto& hit_reaction = registry.get<HitReaction>(e_monster);
	hit_reaction.counter_ms = 750; //ms duration used by health bar

	if (monster.health <= 0)
	{
		//health += (int)((float)monster.reward * reward_multiplier);
		add_health((int)((float)monster.reward * reward_multiplier));
		if (registry.has<Rig>(e_monster)) {
			Rig::delete_rig(e_monster); //rigs have multiple pieces to be deleted
		}
		else {
			registry.destroy(e_monster);
		}

	}
}

void WorldSystem::updateProjectileMonsterCollision(entt::entity e_projectile, entt::entity e_monster)
{
	auto &prj = registry.get<Projectile>(e_projectile);

	if (registry.has<Snowball>(e_projectile)) {
		if (!registry.has<EntityDeath>(e_projectile)) {
			auto& death = registry.emplace<EntityDeath>(e_projectile);
			death.timer = 1000;

			auto& rock = registry.get<Snowball>(e_projectile);
			auto& prj_motion = registry.get<Motion>(e_projectile);
			auto& monster_motion = registry.get<Motion>(e_monster);

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
		damage_monster_helper(e_monster, prj.damage, true);
	}
	else if (registry.has<Flamethrower>(e_projectile) || registry.has<LaserBeam>(e_projectile) || registry.has<Explosion>(e_projectile) || registry.has<IceField>(e_projectile)) {
		auto& dot = registry.get<DamageProperties>(e_monster);
		if (dot.dot_map.find(e_projectile) == dot.dot_map.end()) {
			dot.dot_map.insert({ e_projectile, DOT_DELAY });
			damage_monster_helper(e_monster, prj.damage, registry.has<IceField>(e_projectile));
		}
		else {
			if (dot.dot_map[e_projectile] <= 0) {
				dot.dot_map[e_projectile] = DOT_DELAY;
				damage_monster_helper(e_monster, prj.damage, registry.has<IceField>(e_projectile));
			}
		}
	}

	else if (registry.has<Missile>(e_projectile)) {
		Explosion::createExplosion(e_projectile, prj.damage);
		registry.destroy(e_projectile);
	}

	else {
		damage_monster_helper(e_monster, prj.damage);
		registry.destroy(e_projectile);
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
		speed_up_factor = 1.f;
	}
	if (key == GLFW_KEY_Y) // for testing rigs
	{
		speed_up_factor = 2.f;
	}

	// keys used to skip rounds; used to debug and test rounds
	if (action == GLFW_RELEASE && key == GLFW_KEY_G)
	{
		if (player_state == set_up_stage)
		{
			start_round();
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

	if (action == GLFW_RELEASE && key == GLFW_KEY_P && game_state == in_game) {
		pause_game();
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_P && game_state == paused) {
		resume_game();
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
			RenderSystem::show_entity(help_menu_entity);
            game_state = help_menu;
	    }
	    else if (game_state == help_menu) {
			RenderSystem::hide_entity(help_menu_entity);
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
		auto &mouse_move = view.get<MouseMovement>(camera);
		mouse_move.is_pan_state = 0;
	}

	// Hot keys for selecting placeable units
	/*else if (action == GLFW_PRESS && key == GLFW_KEY_1)
	{
		placement_unit_selected = WATCHTOWER;
		create_unit_indicator = WatchTower::createWatchTower;
	}*/
	else if (action == GLFW_PRESS && key == GLFW_KEY_2)
	{
		placement_unit_selected = GREENHOUSE;
		create_unit_indicator = GreenHouse::createGreenHouse;
	}
    else if (action == GLFW_PRESS && key == GLFW_KEY_3)
	{
		placement_unit_selected = HUNTER;
		create_unit_indicator = Hunter::createHunter;
	}
    else if (action == GLFW_PRESS && key == GLFW_KEY_4)
    {
        placement_unit_selected = WALL;
		create_unit_indicator = Wall::createWall;
    }
	else if (action == GLFW_PRESS && key == GLFW_KEY_5)
	{
		placement_unit_selected = EXTERMINATOR;
		create_unit_indicator = Exterminator::createExterminator;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_6)
	{
		placement_unit_selected = ROBOT;
		create_unit_indicator = Robot::createRobot;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_7)
	{
		placement_unit_selected = PRIESTESS;
		create_unit_indicator = Priestess::createPriestess;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_8)
	{
		placement_unit_selected = SNOWMACHINE;
		create_unit_indicator = SnowMachine::createSnowMachine;
	}
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		if (game_state == in_game) {
			int w, h;
			glfwGetWindowSize(window, &w, &h);

			restart_with_save();
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

void WorldSystem::pause_game()
{
	std::cout << "Paused" << std::endl;
	game_state = paused;
	// pause menu
	registry.get<ShadedMeshRef>(pause_menu_entity).show = true;
	auto menu_ui = registry.get<UI_element>(pause_menu_entity);
	
	float top_button_y_offset = menu_ui.position.y - menu_ui.scale.y / 2.f - 10;
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 1.f / 3.5f, MenuButtonType::restart_round_button, "Restart round");
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 2.f / 3.5f, MenuButtonType::help_button, "Help");
	MenuButton::create_button(menu_ui.position.x, top_button_y_offset + menu_ui.scale.y * 3.f / 3.5f, MenuButtonType::exit_button, "Exit");
}

void WorldSystem::resume_game()
{
	std::cout << "Game Resumed" << std::endl;
	game_state = in_game;
	// hide pause menu and destroy all menu buttons
	registry.get<ShadedMeshRef>(pause_menu_entity).show = false;
	auto menu_button_view = registry.view<MenuButton>();
	for (auto entity : menu_button_view)
		registry.destroy(entity);
}

bool mouse_in_game_area(vec2 mouse_pos)
{
	return (mouse_pos.x > 0 && mouse_pos.y > UI_TOP_BAR_HEIGHT && mouse_pos.x < MAP_SIZE_IN_PX.x && mouse_pos.y < MAP_SIZE_IN_PX.y + UI_TOP_BAR_HEIGHT);
}

void WorldSystem::scroll_callback(double xoffset, double yoffset)
{
	(void)xoffset; // avoid 'unreferenced formal parameter' warning message
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
	if (camera_scale.y + (float)(yoffset / temp_scale) < 1)
	{
		camera_scale = {1.f, 1.f};
		camera_motion.position = {0.f, 0.f};
		return;
	}

	camera_scale.y += (float)yoffset / (float)temp_scale;
	camera_scale.x = camera_scale.y;

	float mouse_in_world_x = (float)abs(yoffset) * ((mouse_movement.mouse_pos.x + camera_position.x) / camera_scale.x) / (float)temp_scale;
	float mouse_in_world_y = (float)abs(yoffset) * ((mouse_movement.mouse_pos.y + camera_position.y) / camera_scale.y) / (float)temp_scale;

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
		int ui_element_background_height = 0;
		for (auto entity : view_ui)
		{
			UI_element ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "in_game_ui_background")
			{
				ui_element_background_height = (int)ui_element.scale.y;
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
void grid_highlight_system(vec2 mouse_pos, unit_type unit_selected, GridMap current_map)
{
	auto view_ui = registry.view<Motion, HighlightBool>();

	ivec2 mouse_coord = pixel_to_coord(mouse_pos);
	auto &node = current_map.getNodeAtCoord(pixel_to_coord(mouse_pos));
	for (auto [entity, grid_motion, highlight] : view_ui.each())
	{
		if (sdBox(mouse_pos, grid_motion.position, grid_motion.scale / 2.0f) < 0.0f && node.occupancy == NONE && node.terrain != TERRAIN_PAVEMENT)
		{
			highlight.highlight = true;
		}
		else
		{
			highlight.highlight = false;
		}
	}
}

// helper for on_mouse_move
// manage camera translation control
void camera_control(vec2 mouse_pos)
{
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
		int ui_element_background_height = 0;
		for (auto entity : view_ui)
		{
			UI_element ui_element = view_ui.get<UI_element>(entity);
			if (ui_element.tag == "in_game_ui_background")
			{
				ui_element_background_height = (int)ui_element.scale.y;
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

// helper for mouse_hover_ui_button
// show unit description when hover on unit button
void update_unit_description(entt::entity entity)
{
	//std::cout << "Hover: " << build_ui.unit_name << "\n";
	for (auto card_entity : registry.view<UI_unit_description_card>())
		registry.destroy(card_entity);
	UI_unit_description_card::createUI_unit_description_card(entity);
}

// helper for mouse_hover_ui_button
// show upgrade description when hover on upgrade button
void update_upgrade_description(entt::entity entity)
{
	for (auto entity : registry.view<UI_selected_description_card>())
		registry.destroy(entity);
	UI_selected_description_card::createUI_selected_description_card(entity);
}

// helper for on_mouse_move
// check if mouse is on top of unit buttons, and dispaly unit description if it is
void mouse_hover_ui_button()
{
	remove_descriptions();
	auto view_buttons = registry.view<Button, UI_element, HighlightBool, ShadedMeshRef>();
	for (auto [entity, button, ui_element, highlight, shadedmeshref] : view_buttons.each()) {
		if (highlight.highlight && shadedmeshref.show) { // if a button is highlighted and we click -> button was pressed.
			if (registry.has<UI_build_unit>(entity))
			{
				update_unit_description(entity);
			}
			else if (registry.has<UI_selected_unit>(entity))
			{
				auto& selected_unit = registry.get<UI_selected_unit>(entity);
				if (selected_unit.path_num < 3)
					update_upgrade_description(entity);
			}
			break;
		}
		else
		{
			remove_descriptions();
		}
	}
}

//helper function for on_mouse_move 
void WorldSystem::createEntityRangeIndicator(vec2 mouse_pos)
{
	if (registry.valid(entity_selected)) {
		auto& unit = registry.get<Unit>(entity_selected);
		
		if (placement_unit_selected == unit.type)
		{
			auto& motion = registry.get<Motion>(entity_selected);
			motion.position = mouse_pos;
			auto& circle_motion = registry.get<Motion>(entity_range_circle);
			circle_motion.position = mouse_pos;
		}
		else 
		{
			registry.destroy(entity_selected);
			entity_selected = create_unit_indicator(mouse_pos);
			unit = registry.get<Unit>(entity_selected);
			
			registry.destroy(entity_range_circle);
			entity_range_circle = RangeCircle::createRangeCircle(mouse_pos, unit.attack_range);
		}
	}
	else {
		entity_selected = create_unit_indicator(mouse_pos);

		auto& unit = registry.get<Unit>(entity_selected);
		entity_range_circle = RangeCircle::createRangeCircle(mouse_pos, unit.attack_range);
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_pos)
{
	if (game_state == in_game) {
		//if mouse is hovering over a button, then highlight
		UI_highlight_system(mouse_pos);
		camera_control(mouse_pos);
		mouse_hover_ui_button();
		bool in_game_area = mouse_in_game_area(mouse_pos);
		if (in_game_area && placement_unit_selected != NONE && player_state == set_up_stage)
		{
			grid_highlight_system(mouse_pos, placement_unit_selected, current_map);
			createEntityRangeIndicator(mouse_pos);
		}
		else {
			if (registry.valid(entity_selected))
				registry.destroy(entity_selected);
			if (registry.valid(entity_range_circle))
				registry.destroy(entity_range_circle);
		}
	}
	else if (game_state == start_menu)
	{
		// hover on title screen buttons, show eyes
		auto view_ui = registry.view<MenuButton, UI_element>();
		bool is_hovering = false;
		for (auto ui_entity : view_ui)
		{
			auto ui_element = registry.get<UI_element>(ui_entity);
			if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f) {
				is_hovering = true;
				registry.get<UI_element>(title_button_highlight_entity).position = vec2({ ui_element.position.x,  ui_element.position.y });
				registry.get<UI_element>(title_button_highlight_entity).angle = ui_element.angle;
				registry.get<ShadedMeshRef>(title_button_highlight_entity).show = true;
			}
		}
		if (is_hovering)
		{
			for (auto entity : registry.view<TitleEyes>())
				registry.get<TitleEyes>(entity).show = true;
		}
		else
		{
			registry.get<ShadedMeshRef>(title_button_highlight_entity).show = false;
			for (auto entity : registry.view<TitleEyes>())
				registry.get<TitleEyes>(entity).show = false;
		}
	}
}

// helper for update_look_for_selected_buttons
// update unit stats
void update_unit_stats(Unit unit)
{
	int x_position = 200;
	int y_position = 65;
	int y_line_offset = 15;
	entt::entity damage_stats;
	// create stats text
	if (unit.damage_buff > 0) 
		damage_stats = create_ui_text(vec2(x_position, y_position), "Attack Damage: " + std::to_string(unit.damage + unit.damage_buff), 0.3f, {1, 0, 0});
	else
		damage_stats = create_ui_text(vec2(x_position, y_position), "Attack Damage: " + std::to_string(unit.damage));
	registry.emplace<UI_unit_stats>(damage_stats);

	// attacks per seconds
	float aps = 0.f;
	if (unit.attack_interval_ms != 0)
	{
		aps = 1000 / (float)unit.attack_interval_ms;
	}
	
	entt::entity attack_speed_stats;
	if (unit.attack_speed_buff > 1) {
		// display aps to 2 decimals
		std::ostringstream aps_out;
		aps_out.precision(2);
		aps_out << std::fixed << (aps * unit.attack_speed_buff);

		attack_speed_stats = create_ui_text(vec2(x_position, y_position - y_line_offset), "Attack speed: " + aps_out.str() + " (aps)", 0.3f, {1,0,0});
	}
	else {
		// display aps to 2 decimals
		std::ostringstream aps_out;
		aps_out.precision(2);
		aps_out << std::fixed << aps;

		attack_speed_stats = create_ui_text(vec2(x_position, y_position - y_line_offset), "Attack speed: " + aps_out.str() + " (aps)");
	}
	
	registry.emplace<UI_unit_stats>(attack_speed_stats);

	// attack range
	auto attack_range_stats = create_ui_text(vec2(x_position, y_position - (2 * y_line_offset)), "Attack range: " + std::to_string(unit.attack_range));
	registry.emplace<UI_unit_stats>(attack_range_stats);
}

void update_unit_portrait(Unit unit)
{
	// clear unit portrait
	for (auto entity : registry.view<UI_selected_unit_portrait>())
	{
		registry.destroy(entity);
	}
	UI_selected_unit_portrait::createUI_selected_unit_portrait(unit.type);
}

//helper for the selected buttons
void update_selected_button(entt::entity e_button, Unit unit)
{
	auto& selected_components = registry.get<UI_selected_unit>(e_button);
	for (auto component : selected_components.button_components) {
		registry.destroy(component);
	}
	selected_components.button_components = std::vector<entt::entity>();

	auto ui = registry.get<UI_element>(e_button);

	int path_num;
	if (ui.tag == PATH_1_UPGRADE_BUTTON_TITLE) {
		path_num = unit.path_1_upgrade;
	}
	else {
		path_num = unit.path_2_upgrade;
	}

	auto image = UI_selected_unit::create_selected_button_image(vec2(ui.position.x - ui.scale.x / 4, ui.position.y + 10), ui.tag, unit);
	auto progress = UI_selected_unit::create_selected_button_progress_bar(vec2(ui.position.x + ui.scale.x / 4, ui.position.y), path_num);

	// text
	float line_size = 35; // relative to the text size
	float left_margin = 3;
	// unit name text
	std::string key = ui.tag + "_" + unit_str.at(unit.type) + "_" + std::to_string(path_num);
	std::string short_description = upgrade_short_descriptions.at(key);
	auto title_text_scale = 0.4f;
	auto bubblegum = TextFont::load("data/fonts/MagicalMystery/MAGIMT__.ttf");
	// center text
	auto x_offset = (ui.scale.x - (short_description.length() * title_text_scale * 27)) / 2;
	// place title text at the top
	float top_margin = 10;
	auto y_title_offset = ui.scale.y / 2 - title_text_scale * line_size - top_margin;
	vec2 title_text_position = get_center_text_position(vec2(ui.scale.x / 2, ui.scale.y), vec2(ui.position.x - ui.scale.x / 4, ui.position.y), title_text_scale, short_description);
	auto& title = registry.emplace_or_replace<Text>(e_button, Text(short_description, bubblegum, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
	title.scale = title_text_scale;
	title.colour = { 0.f, 0.f, 0.f };

	selected_components.button_components.push_back(image);
	selected_components.button_components.push_back(progress);
}

//helper for the selected buttons
void update_sell_button_text(entt::entity e_button, int sell_price)
{
	auto& UI_sell = registry.get<UI_sell_button>(e_button);
	if (registry.valid(UI_sell.sell_text))
	{
		registry.destroy(UI_sell.sell_text);
	}

	auto text_ent = registry.create();
	auto& ui = registry.get<UI_element>(e_button);
	// text
	float line_size = 35; // relative to the text size
	// unit name text
	std::string short_description = "$" + std::to_string(sell_price);
	auto title_text_scale = 0.4f;
	auto bubblegum = TextFont::load("data/fonts/MagicalMystery/MAGIMT__.ttf");
	// place title text at the top
	float top_margin = 45;
	auto y_title_offset = ui.scale.y / 2 - title_text_scale * line_size - top_margin;
	vec2 title_text_position = get_center_text_position(vec2(2 * ui.scale.x / 3, ui.scale.y), vec2(ui.position.x + ui.scale.x / 8, ui.position.y), title_text_scale, short_description);
	auto& title = registry.emplace_or_replace<Text>(text_ent, Text(short_description, bubblegum, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
	title.scale = title_text_scale;
	title.colour = { 0.f, 0.f, 0.f };

	UI_sell.sell_text = text_ent;
}

// remove upgrade button and sell button
void remove_selected_unit_buttons()
{
	for (auto entity : registry.view<UI_selected_unit, UI_element, ShadedMeshRef>())
	{
		auto& selected_components = registry.get<UI_selected_unit>(entity);
		for (auto component : selected_components.button_components) {
			registry.destroy(component);
		}
		registry.destroy(entity);
	}

	for (auto entity : registry.view<UI_sell_button>())
	{
		auto& UI_sell = registry.get<UI_sell_button>(entity);
		if (registry.valid(UI_sell.sell_text))
		{
			registry.destroy(UI_sell.sell_text);
		}
		registry.destroy(entity);
	}
}

// update the appearance of ui depending on the given flags
void WorldSystem::update_look_for_selected_buttons(int action, bool unit_selected, bool sell_clicked)
{
	// prevent this function gets called twice with one mouse click (press & release)
	if (action != GLFW_PRESS)
	{
		return;
	}
	// clear unit stats text
	for (auto entity : registry.view<UI_unit_stats>())
	{
		registry.destroy(entity);
	}
	auto view_ui_selected_buttons = registry.view<UI_selected_unit, UI_element, ShadedMeshRef>();
	auto view_ui_build_buttons = registry.view<UI_build_unit, UI_element, ShadedMeshRef>();

	if (registry.valid(selected_range_circle))
		registry.destroy(selected_range_circle);
	
	// if a unit is selected and the sell button is not clicked
	// show upgrade buttons and sell button
	if (unit_selected && !sell_clicked)
	{
		auto view_unit = registry.view<Unit>();
		auto view_selectable = registry.view<Selectable>();
		// get the selected unit
		Unit selected_unit;
		for (auto entity : view_selectable)
		{
			if (view_selectable.get<Selectable>(entity).selected)
			{
				selected_unit = view_unit.get<Unit>(entity);
				selected_view_change = previous_selected != entity || selected_view_change;
				previous_selected = entity;
				auto& motion = registry.get<Motion>(entity);
				selected_range_circle = RangeCircle::createRangeCircle(motion.position, selected_unit.attack_range);
			}
		}

		if (selected_view_change) {
			remove_selected_unit_buttons();

			upgrade_button_1 = UI_selected_unit::createUI_selected_unit_upgrade_button(2, upgrade_path_1_button, PATH_1_UPGRADE_BUTTON_TITLE, selected_unit.type, selected_unit.path_1_upgrade);
			upgrade_button_2 = UI_selected_unit::createUI_selected_unit_upgrade_button(3, upgrade_path_2_button, PATH_2_UPGRADE_BUTTON_TITLE, selected_unit.type, selected_unit.path_2_upgrade);
			button_sell = UI_sell_button::createUI_sell_button(5, sell_button, SELL_BUTTON_TITLE);
			selected_view_change = false;
		}

		update_selected_button(upgrade_button_1, selected_unit);
		if (registry.has<HighlightBool>(upgrade_button_1) && selected_unit.path_1_upgrade >= 3) {
			registry.remove<HighlightBool>(upgrade_button_1);
		}
		
		update_selected_button(upgrade_button_2, selected_unit);
		if (registry.has<HighlightBool>(upgrade_button_2) && selected_unit.path_2_upgrade >= 3) {
			registry.remove<HighlightBool>(upgrade_button_2);
		}

		update_sell_button_text(button_sell, selected_unit.sell_price);

		//update unit portrait
		update_unit_portrait(selected_unit);

		// update unit stats
		update_unit_stats(selected_unit);

		// hide all build buttons
		for (auto entity : view_ui_build_buttons)
		{
			RenderSystem::hide_entity(entity);
		}
	}
	else
	{
		selected_view_change = true;
		remove_selected_unit_buttons();

		// show build unit buttons
		for (auto entity : view_ui_build_buttons)
		{
			RenderSystem::show_entity(entity);
		}
	}
}

// mouse click callback function
void WorldSystem::on_mouse_click(int button, int action, int mod)
{
	if (!(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS))
		return;

	//getting cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (game_tips) {
		remove_game_tip();
	}
  
	switch (game_state)
	{
        case start_menu:
        {
            start_menu_click_handle(xpos, ypos, button, action, mod);
            break;
        }
        case settings_menu:
        {
            settings_menu_click_handle(xpos, ypos, button, action, mod);
            break;
        }
        case in_game:
        {
			vec2 selected_flags = on_click_select_unit(xpos, ypos, button, action, mod);
			in_game_click_handle(xpos, ypos, button, action, mod);
			update_look_for_selected_buttons(action, selected_flags.x, selected_flags.y);
            break;
        }
		case paused:
		{
			paused_click_handle(xpos, ypos, button, action, mod);
			break;
		}
        case help_menu:
        {
            help_menu_click_handle(xpos, ypos, button, action, mod);
            break;
        }
        case story_card:
        {
            story_card_click_handle(xpos, ypos, button, action, mod);
            break;
        }
		case lost_game_screen:
		{
			lost_game_click_handle(xpos, ypos, button, action, mod);
			break;
		}
	}
}

void WorldSystem::help_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action == GLFW_PRESS)
	{
		auto view = registry.view<Menu, ShadedMeshRef>();
		for (auto entity : view)
		{
			RenderSystem::hide_entity(entity);
		}
		
		if (world_round_number == 0) {
			game_state = story_card;
			StoryCard::createStoryCard(STORY_TEXT_PER_LEVEL[world_round_number], std::to_string(1));
		}

		if (registry.empty<StoryCard>()) {
			game_state = in_game;
		}
		else {
			game_state = story_card;
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mouse_pos_x;
	(void)mouse_pos_y;
	(void)button;
	(void)mod;
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
	// avoid 'unreferenced formal parameter' warning message
	(void)mouse_pos_x;
	(void)mouse_pos_y;
	(void)button;
	(void)mod;
}

void WorldSystem::remove_game_tip()
{
	auto tip_card_view = registry.view<TipCard>();
	for (auto entity : tip_card_view)
	{
		registry.destroy(entity);
	}
}

// helper for unit_select_click_handle
bool check_unit_already_selected()
{
	auto view_selectable = registry.view<Selectable>();
	for (auto [entity, selectable] : view_selectable.each())
	{
		if (selectable.selected)
		{
			return true;
		}
	}
	return false;
}

// helper for unit_select_click_handle
bool check_click_on_sell_button(double mouse_pos_x, double mouse_pos_y)
{
	auto view_selected_buttons = registry.view<UI_sell_button, UI_element>();
	for (auto [entity, ui_selected_unit, ui_element] : view_selected_buttons.each())
	{
		if (ui_element.tag == SELL_BUTTON_TITLE)
		{
			if (sdBox({ mouse_pos_x, mouse_pos_y }, ui_element.position, ui_element.scale / 2.0f) < 0.0f)
			{
				return true;
			}
		}
	}
	return false;
}

// helper for unit_select_click_handle
// return true if click on unit modification buttons, except the sell button
bool check_click_on_unit_selected_buttons(double mouse_pos_x, double mouse_pos_y)
{
	auto view_selected_buttons = registry.view<UI_selected_unit, UI_element>();
	for (auto [entity, ui_selected_unit, ui_element] : view_selected_buttons.each())
	{
		if (sdBox({ mouse_pos_x, mouse_pos_y}, ui_element.position, ui_element.scale / 2.0f) < 0.0f)
		{
			if (ui_element.tag != SELL_BUTTON_TITLE)
				return true;
		}
	}
	return false;
}

// helper for unit_select_click_handle
// set the unit to selected
// return true if a unit is selected; otherwise, false
bool WorldSystem::click_on_unit(double mouse_pos_x, double mouse_pos_y)
{
	bool clicked_on_unit = false;
	auto view_highlight = registry.view<HighlightBool>();
	auto view_unit = registry.view<Unit>();
	vec2 mouse_pos = mouse_in_world_coord({ mouse_pos_x, mouse_pos_y });
	auto view_selectable = registry.view<Selectable, Motion>();
	for (auto [entity, selectable, motion] : view_selectable.each())
	{
		// check click on units
		if (sdBox(mouse_pos, motion.position, motion.scale / 2.0f) < 0.0f && entity != entity_selected)
		{
			// add selected status
			selectable.selected = true;
			view_highlight.get<HighlightBool>(entity).highlight = true;

			clicked_on_unit = true;
		}
		else
		{
			// remove selected status on all other units
			selectable.selected = false;
			view_highlight.get<HighlightBool>(entity).highlight = false;
		}
	}
	return clicked_on_unit;
}

vec2 WorldSystem::on_click_select_unit(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	if (action != GLFW_PRESS)
	{
		return { false, false };
	}

	bool unit_selected = false;
	bool sell_clicked = false;

	if (check_unit_already_selected())
	{
		bool on_selected_buttons = check_click_on_unit_selected_buttons(mouse_pos_x, mouse_pos_y);
		bool on_sell_button = check_click_on_sell_button(mouse_pos_x, mouse_pos_y);
		unit_selected = (on_selected_buttons || on_sell_button);
		sell_clicked = on_sell_button;
	}
	
	if (!unit_selected)
	{
		unit_selected = click_on_unit(mouse_pos_x, mouse_pos_y);
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)button;
	(void)mod;
	return { unit_selected, sell_clicked };
}

void WorldSystem::start_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	MenuButtonType button_tag;
	
	if (action == GLFW_PRESS)
	{
		/*for (auto entity : registry.view<TitleEyes>())
		{
			registry.destroy(entity);
		}
		std::cout << "mouse pos :" << mouse_pos_x << ", " << mouse_pos_y << "\n";
		TitleEyes::createTitleEyes(vec2({ mouse_pos_x, mouse_pos_y }));*/
		//MenuButton::create_button(mouse_pos_x, mouse_pos_y, MenuButtonType::exit_button, "X");
		button_tag = on_click_button({mouse_pos_x, mouse_pos_y});
		switch (button_tag)
		{
		case (MenuButtonType::new_game_button):
			remove_menu_buttons();
			game_state = help_menu;
			restart_with_save();
			// show controls overlay
			RenderSystem::show_entity(help_menu_entity);
			break;
		case (MenuButtonType::load_game_button):
			remove_menu_buttons();
			restart();
			load_game();
			game_state = story_card;
			break;
		case (MenuButtonType::title_help_button):
			remove_menu_buttons();
			registry.destroy(title_button_highlight_entity);
			game_state = settings_menu;
			create_controls_menu();
			break;
		case (MenuButtonType::title_exit_button):
			// close window
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}
	}	
	// avoid 'unreferenced formal parameter' warning message
	(void)button;
	(void)mod;
}

void WorldSystem::settings_menu_click_handle(double mouse_pos_x, double mouse_pos_y, int button, int action, int mod)
{
	MenuButtonType menu_button;
	if (action == GLFW_PRESS)
	{
		menu_button = on_click_button({mouse_pos_x, mouse_pos_y});
		switch (menu_button)
		{
		case MenuButtonType::back_button:
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
			break;
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)button;
	(void)mod;
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
	Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, "start_menu", Menu_texture::title_screen, 85, {1.0, 0.9});
	//Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, 100, "title_screen_title", Menu_texture::title_screen_title, 86, { 0.9, 0.9 });
	// title: Feast or Famine
	Menu::createMenu(300, 150, "title_screen_title2", Menu_texture::title_screen_title2, 86, { 1.1, 1.1 });
	Menu::createMenu(470, 120, "title_screen_title_or", Menu_texture::title_screen_title2_or, 86, { 0.7, 0.7 });
	//buttons
	MenuButton::create_button(NEW_GAME_BUTTON_X, NEW_GAME_BUTTON_Y, MenuButtonType::new_game_button, "", NEW_GAME_BUTTON_ANGLE);
	MenuButton::create_button(LOAD_GAME_BUTTON_X, LOAD_GAME_BUTTON_Y, MenuButtonType::load_game_button);
	MenuButton::create_button(TITLE_HELP_BUTTON_X, TITLE_HELP_BUTTON_Y, MenuButtonType::title_help_button, "", TITLE_HELP_BUTTON_ANGLE);
	MenuButton::create_button(TITLE_EXIT_BUTTON_X, TITLE_EXIT_BUTTON_Y, MenuButtonType::title_exit_button);
	title_button_highlight_entity = MenuButton::create_button_arrow();
	// blinking eyes
	std::vector<vec2> locations = { vec2({984, 442}), vec2({891, 429}), vec2({851, 427}), vec2({764, 434}), vec2({719, 435}),
								   vec2({576, 410}), vec2({501, 417}), vec2({397, 421}), vec2({355, 422}), vec2({40, 420}) };
	for (vec2 position : locations)
	{
		TitleEyes::createTitleEyes(position);
	}
	
}

void WorldSystem::create_controls_menu()
{
	std::cout << "In Controls Menu\n";
	int menu_layer = 90;
	std::string menu_name = "controls";
	auto menu = Menu::createMenu(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2, menu_name, Menu_texture::controls, menu_layer, { WINDOW_SIZE_IN_PX.x / 10, WINDOW_SIZE_IN_PX.x / 10});
	// title text
	std::string title_text = "Controls";
	auto title_text_scale = 1.2f;
	auto title_x_offset = (title_text.length() * title_text_scale * 27) / 2;
	auto notoRegular = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	auto& title = registry.emplace<Text>(menu, Text(title_text, notoRegular, vec2(WINDOW_SIZE_IN_PX.x / 2 - title_x_offset, WINDOW_SIZE_IN_PX.y - 170)));
	title.scale = title_text_scale;
	title.colour = { 1.0f, 0.8f, 0.0f };
	// hotkey text
	std::vector<std::string> hotkey_list = {"1", "2", "3", "4", "Space + mouse", "Scroll", "H", "Esc"};
	int para_y_offset = 230;
	int hotkey_para_x_offset = 410;
	auto hotkey_text_scale = 0.8f;
	for (int i = 0; i <= 7; i++) {
		auto entity = registry.create();
		auto& menu_text = registry.emplace<MenuText>(entity);
		menu_text.menu_name = menu_name;
		int y_offset = para_y_offset + (int)(i * hotkey_text_scale * 60);
		std::string hotkey_text = hotkey_list[i];
		auto x_offset = (hotkey_text.length() * hotkey_text_scale * 27);
		auto& hotkey = registry.emplace<Text>(entity, Text(hotkey_text, notoRegular, vec2(hotkey_para_x_offset - x_offset, WINDOW_SIZE_IN_PX.y - y_offset)));
		hotkey.scale = hotkey_text_scale;
		hotkey.colour = { 1.0f, 0.8f, 0.0f };
	}
	// hotkey description text
	std::vector<std::string> hotkey_des_list = { "Select watchtower", "Select greenhouse", "Select hunter", "Select wall",
											 "Move camera", "Zoom in / out", "Show  controls menu", "Goto title screen" };
	int des_para_x_offset = 570;
	for (int i = 0; i <= 7; i++) {
		auto entity = registry.create();
		auto& menu_text = registry.emplace<MenuText>(entity);
		menu_text.menu_name = menu_name;
		int y_offset = para_y_offset + (int)(i * hotkey_text_scale * 60);
		std::string hotkey_text = hotkey_des_list[i];
		auto& description = registry.emplace<Text>(entity, Text(hotkey_text, notoRegular, vec2(des_para_x_offset, WINDOW_SIZE_IN_PX.y - y_offset)));
		description.scale = hotkey_text_scale;
		description.colour = { 1.0f, 0.8f, 0.0f };
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
		int y_offset = para_y_offset + (int)(i * hotkey_text_scale * 60) - 10;
		ui_element.position = vec2(hotkey_para_x_offset + (des_para_x_offset - hotkey_para_x_offset) / 2, y_offset);
		auto& menu_text = registry.emplace<MenuText>(arrow);
		menu_text.menu_name = menu_name;
	}

	MenuButton::create_button(WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y * 6 / 7, MenuButtonType::back_button, "back");
}

void WorldSystem::in_game_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	Motion camera_motion = registry.get<Motion>(camera);

	// cursor position in world pos
	vec2 mouse_world_pos = mouse_in_world_coord(vec2({xpos, ypos}));

	Button ui_button = UI_click_system(); // returns enum of button pressed or no_button_pressed enum

	bool in_game_area = mouse_in_game_area(vec2(xpos, ypos));

	if (player_state == set_up_stage)
	{
		// Mouse click for placing units
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && placement_unit_selected != NONE && in_game_area)
		{
			auto &node = current_map.getNodeAtCoord(pixel_to_coord(mouse_world_pos));
			vec2 unit_position = coord_to_pixel(node.coord);
            bool can_place_unit = true;
            entt::entity entity;
			if (node.occupancy == NONE && node.terrain != TERRAIN_PAVEMENT && node.terrain != TERRAIN_FIRE)
			{
				if (placement_unit_selected == HUNTER && health >= hunter_unit.cost)
				{
                    entity = Hunter::createHunter(unit_position);
					deduct_health(hunter_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == GREENHOUSE && health >= greenhouse_unit.cost)
				{
					entity = GreenHouse::createGreenHouse(unit_position);
					deduct_health(greenhouse_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				/*else if (placement_unit_selected == WATCHTOWER && health >= watchtower_unit.cost)
				{
					entity = WatchTower::createWatchTower(unit_position);
					deduct_health(watchtower_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}*/
				else if (placement_unit_selected == EXTERMINATOR && health >= exterminator_unit.cost)
				{
					entity = Exterminator::createExterminator(unit_position);
					deduct_health(exterminator_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == ROBOT && health >= robot_unit.cost)
				{
					entity = Robot::createRobot(unit_position);
					deduct_health(robot_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == PRIESTESS && health >= priestess_unit.cost)
				{
					entity = Priestess::createPriestess(unit_position);
					deduct_health(priestess_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == SNOWMACHINE && health >= snowmachine_unit.cost)
				{
					entity = SnowMachine::createSnowMachine(unit_position);
					deduct_health(snowmachine_unit.cost);
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else if (placement_unit_selected == WALL && health >= wall_unit.cost)
				{
					entity = Wall::createWall(unit_position/*, false*/);
					health -= wall_unit.cost;
					Mix_PlayChannel(-1, ui_sound_bottle_pop, 0);
				}
				else
				{
					//insufficent funds -- should feedback be given here, or when the button is pressed?
					Mix_PlayChannel(-1, ui_sound_negative_tick, 0);
                    can_place_unit = false;
				}
				if (can_place_unit) {
				    auto& motion = registry.get<Motion>(entity);
                    current_map.setGridOccupancy(node.coord, placement_unit_selected, entity, motion.scale);
				}
				placement_unit_selected = NONE;

				if (registry.valid(entity_selected))
				{
					registry.destroy(entity_selected);
					registry.destroy(entity_range_circle);
				}

				un_highlight();
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !in_game_area)
		{
			/*if (ui_button == Button::watchtower_button)
			{
				if (game_tips && tip_manager.tower_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.tower_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, tower_tips);
				}
				create_unit_indicator = WatchTower::createWatchTower;
				placement_unit_selected = WATCHTOWER;
			}
			else*/ if (ui_button == Button::green_house_button)
			{
				if (game_tips && tip_manager.greenhouse_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.greenhouse_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, greenhouse_tips);
				}

				placement_unit_selected = GREENHOUSE;
				create_unit_indicator = GreenHouse::createGreenHouse;
			}
			else if (ui_button == Button::hunter_button)
			{
				if (game_tips && tip_manager.hunter_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.hunter_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, hunter_tips);
				}

				placement_unit_selected = HUNTER;
				create_unit_indicator = Hunter::createHunter;
			}
			else if (ui_button == Button::exterminator_button)
			{
				if (game_tips && tip_manager.exterminator_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.exterminator_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, exterminator_tips);
				}

				placement_unit_selected = EXTERMINATOR;
				create_unit_indicator = Exterminator::createExterminator;
			}
			else if (ui_button == Button::robot_button)
			{
				if (game_tips && tip_manager.robot_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.robot_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, robot_tips);
				}

				placement_unit_selected = ROBOT;
				create_unit_indicator = Robot::createRobot;
			}
			else if (ui_button == Button::priestess_button)
			{
				if (game_tips && tip_manager.priestess_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.priestess_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, priestess_tips);
				}

				placement_unit_selected = PRIESTESS;
				create_unit_indicator = Priestess::createPriestess;
			}
			else if (ui_button == Button::snowmachine_button)
			{
				if (game_tips && tip_manager.snowmachine_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.snowmachine_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, snowmachine_tips);
				}

				placement_unit_selected = SNOWMACHINE;
				create_unit_indicator = SnowMachine::createSnowMachine;
			}
			else if (ui_button == Button::wall_button)
			{
				if (game_tips && tip_manager.wall_tip)
				{
					game_state = paused;
					WorldSystem::tip_manager.wall_tip = false;
					TipCard::createTipCard(TIP_CARD_LEFT_X, TIP_CARD_CENBOT_Y, wall_tips);
				}
				create_unit_indicator = Wall::createWall;
				placement_unit_selected = WALL;
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
			else if (ui_button == Button::upgrade_path_1_button && health >= hunter_unit.upgrade_path_1_cost)
			{
				// upgrade button is hit
				auto view_selectable = registry.view<Selectable>();
				auto view_unit = registry.view<Unit>();
				for (auto entity : view_selectable)
				{
					if (view_selectable.get<Selectable>(entity).selected)
					{
						upgrade_unit_path_1(entity);
						auto& UIselection = registry.get<UI_selected_unit>(upgrade_button_1);
						UIselection.path_num += 1;
						mouse_hover_ui_button();
					}
				}
			}
			else if (ui_button == Button::upgrade_path_2_button && health >= hunter_unit.upgrade_path_2_cost)
			{
				// upgrade button is hit
				auto view_selectable = registry.view<Selectable>();
				auto view_unit = registry.view<Unit>();
				for (auto entity : view_selectable)
				{
					if (view_selectable.get<Selectable>(entity).selected)
					{
						upgrade_unit_path_2(entity);
						auto& UIselection = registry.get<UI_selected_unit>(upgrade_button_2);
						UIselection.path_num += 1;
						mouse_hover_ui_button();
					}
				}
			}
			else if (ui_button == Button::start_button)
			{
				if (player_state == set_up_stage)
				{
					start_round();
				}
			}
			else if (ui_button == Button::tips_button)
			{
				game_tips = !game_tips;
				std::cout << std::boolalpha;
				std::cout << "Game tips: " << game_tips << std::endl;
			}
			else
			{
				placement_unit_selected = NONE;
			}
		}
		Priestess::updateBuffs();
	}

	// avoid 'unreferenced formal parameter' warning message
	(void)mod;
}

void WorldSystem::lost_game_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		vec2 mouse_pos = { (float)xpos, (float)ypos };
		MenuButtonType button_clicked = on_click_button(mouse_pos);
		// world_round_number get reset to 0 in restart();
		int temp_world_round_number = world_round_number;
		switch (button_clicked)
		{
		case MenuButtonType::restart_round_button:
			remove_menu_buttons();
			restart();
			load_game();
			game_state = story_card;
			break;
		case MenuButtonType::exit_button:
			game_setup();
			create_start_menu();
			player_state = set_up_stage;
			game_state = start_menu;
			break;
		}
	}
	
}

// unpause if paused
void WorldSystem::paused_click_handle(double xpos, double ypos, int button, int action, int mod)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		vec2 mouse_pos = { (float)xpos, (float)ypos };
		auto menu_ui = registry.get<UI_element>(pause_menu_entity);
		// check menu is visible and
		//click within menu
		if (registry.get<ShadedMeshRef>(pause_menu_entity).show && sdBox(mouse_pos, menu_ui.position, menu_ui.scale / 2.0f) < 0.0f)
		{
			MenuButtonType button_clicked = on_click_button(mouse_pos);
			// world_round_number get reset to 0 in restart();
			int temp_world_round_number = world_round_number;
			switch (button_clicked)
			{
			case MenuButtonType::menu_save_button:
				save_game();
				break;
			case MenuButtonType::load_game_button:
				remove_menu_buttons();
				restart();
				load_game();
				game_state = story_card;
				break;
			case MenuButtonType::restart_round_button:
				remove_menu_buttons();
				RenderSystem::hide_entity(pause_menu_entity);
				restart();
				load_game();
				game_state = story_card;
				break;
			case MenuButtonType::help_button:
				resume_game();
				RenderSystem::show_entity(help_menu_entity);
				game_state = GameState::help_menu;
				break;
			case MenuButtonType::exit_button:
				game_setup();
				create_start_menu();
				player_state = set_up_stage;
				game_state = start_menu;
				break;
			}
		}
		else
		{
			// remove game tips if exist
			remove_game_tip();
			resume_game();
		}
	}
	// avoid 'unreferenced formal parameter' warning message
	(void)mod;
}

void WorldSystem::sell_unit(entt::entity &entity)
{
    auto& motion = registry.get<Motion>(entity);
    current_map.setGridOccupancy(pixel_to_coord(motion.position), NONE, entity, motion.scale);
	registry.destroy(entity);
}

void WorldSystem::save_game()
{
	nlohmann::json save_json;
	save_json["round_number"] = world_round_number;
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

        curr_unit["type_str"] = unit_str.at(unit.type);
		curr_unit["type"] = unit.type;
		curr_unit["x_coord"] = motion.position.x;
		curr_unit["y_coord"] = motion.position.y;
		curr_unit["path_1_upgrades"] = unit.path_1_upgrade;
		curr_unit["path_2_upgrades"] = unit.path_2_upgrade;
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
	world_round_number = save_json["round_number"];

	setup_round_from_round_number(world_round_number);

	for (nlohmann::json unit : save_json["units"])
	{
		int x = unit["x_coord"];
		int y = unit["y_coord"];
		int type = unit["type"];
		entt::entity entity;
		/*if (type == WATCHTOWER)
		{
			entity = WatchTower::createWatchTower({x, y});
		}
		else */if (type == GREENHOUSE)
		{
			entity = GreenHouse::createGreenHouse({x, y});
		}
		else if (type == WALL)
		{
			entity = Wall::createWall({x, y}/*, unit["rotate"]*/);
		}
		else if (type == HUNTER)
		{
			entity = Hunter::createHunter({x, y});
		}
		auto& motion = registry.get<Motion>(entity);
        current_map.setGridOccupancy(pixel_to_coord(vec2(x,y)), type, entity, motion.scale);
		auto view_unit = registry.view<Unit>();
		auto &curr_unit = view_unit.get<Unit>(entity);
		
		for (int i = 0; i < unit["path_1_upgrades"]; i++) {
			auto& unit = registry.get<Unit>(entity);
			health += unit.upgrade_path_1_cost;
			upgrade_unit_path_1(entity);
		}

		for (int j = 0; j < unit["path_2_upgrades"]; j++) {
			auto& unit = registry.get<Unit>(entity);
			health += unit.upgrade_path_2_cost;
			upgrade_unit_path_2(entity);
		}
	}

	std::vector<std::vector<nlohmann::json>> map = save_json["map_data"];
	for (int x = 0; x < MAP_SIZE_IN_COORD.x; x++) {
		for (int y = 0; y < MAP_SIZE_IN_COORD.y; y++) {
			terrain_type terrain = map[x][y]["terrain"];
			current_map.setGridTerrain(ivec2(x, y), terrain);
		}
	}
}

void WorldSystem::restart_with_save() {
	restart();
	save_game();
}