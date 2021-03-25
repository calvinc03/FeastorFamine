#include <glm/vec2.hpp>				// vec2

using namespace glm;

static const int GRID_CELL_SIZE = 70;
static const ivec2 GRID_OFFSET =  ivec2(GRID_CELL_SIZE/2 , GRID_CELL_SIZE/2);
static const int UI_TAB_HEIGHT = 88;

static const ivec2 MAP_SIZE_IN_COORD = {15, 9};
static const ivec2 MAP_SIZE_IN_PX = GRID_CELL_SIZE * MAP_SIZE_IN_COORD;
static const ivec2 WINDOW_SIZE_IN_PX = MAP_SIZE_IN_PX + ivec2(0, UI_TAB_HEIGHT);

static const ivec2 FOREST_COORD = ivec2(0, 0);
// offset by 1 to account for base 0 counting
static const ivec2 VILLAGE_COORD = MAP_SIZE_IN_COORD - ivec2(1, 1);
static const ivec2 DRAGON_COORD = ivec2(-2, 5);

static const float ELAPSED_MS = 20;

static const int MAX_PARTICLES = 1000;
static const size_t FIRING_RATE = 3000;

const int FIREBALL_DELAY_MS = 2385;