#include <map>
#include <glm/vec3.hpp>

// food label
static const float FOOD_LABEL_X_OFFSET = 30.f;
static const float FOOD_LABEL_Y_OFFSET = 60.f;
static const float FOOD_LABEL_SCALE = 0.6f;
// food num
static const float FOOD_NUM_X_OFFSET = FOOD_LABEL_X_OFFSET + 80.f;
static const float FOOD_NUM_Y_OFFSET = FOOD_LABEL_Y_OFFSET;
static const float FOOD_NUM_SCALE = 1.0f;
// round label
static const float ROUND_LABEL_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x / 2.f - 100;
static const float ROUND_LABEL_Y_OFFSET = FOOD_LABEL_Y_OFFSET;
static const float ROUND_LABEL_SCALE = 0.6f;
// round number
static const float ROUND_NUM_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x / 2 + 20;
static const float ROUND_NUM_Y_OFFSET = FOOD_LABEL_Y_OFFSET;
static const float ROUND_NUM_SCALE = 1.2f;


// season text colour
const std::map<int, vec3> season_str_colour = {
        {SPRING,    {0.f, 196.f / 255.f, 26.f / 255.f}},
        {SUMMER,    {1.f, 0.f, 0.f}},
        {FALL,      {1.f, 123.f / 255.f, 0.f}},
        {WINTER,    {0.f, 221.f / 255.f, 1.f}},
};
// weather text colour
const std::map<int, vec3> weather_str_colour = {
        {CLEAR,     {0.f, 221.f / 255.f, 1.f}},
        {RAIN,      {0.f, 110.f / 255.f, 1.f}},
        {DROUGHT,   {1.f, 190.f / 255.f, 92.f / 255.f}},
        {FOG,       {0.7f, 0.7f, 0.7f}},
        {SNOW,      {1.f, 1.f, 1.f}}
};
// season wheel
static const float SEASON_WHEEL_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x - 300;
static const float SEASON_WHEEL_Y_OFFSET = FOOD_LABEL_Y_OFFSET - 23.f;
// weather icon
static const float WEATHER_ICON_X_OFFSET = (float)WINDOW_SIZE_IN_PX.x - 130;
static const float WEATHER_ICON_Y_OFFSET = FOOD_LABEL_Y_OFFSET - 23.f;

// season text
static const float SEASON_X_OFFSET = SEASON_WHEEL_X_OFFSET + 50.f;
static const float SEASON_Y_OFFSET = SEASON_WHEEL_Y_OFFSET + 10.f;
static const float SEASON_SCALE = 0.45f;
// weather text
static const float WEATHER_TEXT_X_OFFSET = WEATHER_ICON_X_OFFSET + 37.f;
static const float WEATHER_TEXT_Y_OFFSET = WEATHER_ICON_Y_OFFSET + 10.f;
static const float WEATHER_TEXT_SCALE = 0.45f;

// season wheel
//static const float SEASON_WHEEL_X_OFFSET = SEASON_X_OFFSET + 105.f;
//static const float SEASON_WHEEL_Y_OFFSET = SEASON_Y_OFFSET - 13.f;
//// weather icon
//static const float WEATHER_ICON_X_OFFSET = WEATHER_TEXT_X_OFFSET + 105.f;
//static const float WEATHER_ICON_Y_OFFSET = WEATHER_TEXT_Y_OFFSET - 13.f;