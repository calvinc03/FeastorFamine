static const std::vector<ivec2>& all_neighbors = {ivec2(1,0), ivec2(1,-1),ivec2(1,1),
                                       ivec2(0,-1),ivec2(0,1),
                                       ivec2(-1,0),ivec2(-1,1),ivec2(-1,-1)};

// no diagonals
static const std::vector<ivec2> direct_neighbors = {ivec2(0, -1), ivec2(-1, 0),
                                     ivec2(1,0), ivec2(0,1)};


//static const std::map<int, ivec2> neighbors = {
//        {UP, ivec2(0, -1)},
//        {DOWN, ivec2(0, 1)},
//        {LEFT, ivec2(-1, 0)},
//        {RIGHT, ivec2(1, 0)},
//        {UL, ivec2(-1, -1)},
//        {UR, ivec2(1, -1)},
//        {LL, ivec2(-1, 1)},
//        {LR, ivec2(1, 1)},
//};

const std::map<std::pair<int, int>, float> monster_move_cost = {
        // rabbit
        {std::pair(MOB, TERRAIN_PAVEMENT),         0.5},
        {std::pair(MOB, TERRAIN_DEFAULT),          1.0},
        {std::pair(MOB, TERRAIN_MUD),              1.5},
        {std::pair(MOB, TERRAIN_PUDDLE),           2.0},
        {std::pair(MOB, TERRAIN_DRY),              1.0},
        {std::pair(MOB, TERRAIN_FIRE),             10.0},
        {std::pair(MOB, TERRAIN_ICE),              1.0},
        // bird
        {std::pair(SPRING_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(SPRING_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(SPRING_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(SPRING_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(SPRING_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(SPRING_BOSS, TERRAIN_FIRE),             10.0},
        {std::pair(SPRING_BOSS, TERRAIN_ICE),              1.0},
        // moose
        {std::pair(SUMMER_BOSS, TERRAIN_PAVEMENT),         0.5},
        {std::pair(SUMMER_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(SUMMER_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(SUMMER_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(SUMMER_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(SUMMER_BOSS, TERRAIN_FIRE),             10.0},
        {std::pair(SUMMER_BOSS, TERRAIN_ICE),              1.0},
        // bear
        {std::pair(FALL_BOSS, TERRAIN_PAVEMENT),         0.5},
        {std::pair(FALL_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(FALL_BOSS, TERRAIN_MUD),              0.5},
        {std::pair(FALL_BOSS, TERRAIN_PUDDLE),           1.5},
        {std::pair(FALL_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(FALL_BOSS, TERRAIN_FIRE),             10.0},
        {std::pair(FALL_BOSS, TERRAIN_ICE),              1.0},
        // penguin
        {std::pair(WINTER_BOSS, TERRAIN_PAVEMENT),         0.5},
        {std::pair(WINTER_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(WINTER_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(WINTER_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(WINTER_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(WINTER_BOSS, TERRAIN_FIRE),             10.0},
        {std::pair(WINTER_BOSS, TERRAIN_ICE),              0.1},

        {std::pair(DRAGON_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_ICE),              1.0},

        {std::pair(FIREBALL_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_ICE),              1.0},

        {std::pair(BURROW_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(BURROW_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(BURROW_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(BURROW_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(BURROW_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(BURROW_BOSS, TERRAIN_FIRE),             10.0},
        {std::pair(BURROW_BOSS, TERRAIN_ICE),              1.0},
};

// maps <monster type, terrain> to unit move cost
const std::map<std::pair<int, int>, int> monster_move_speed_multiplier = {
        // rabbit
        {std::pair(MOB, TERRAIN_PAVEMENT),         1.2},
        {std::pair(MOB, TERRAIN_DEFAULT),          1.0},
        {std::pair(MOB, TERRAIN_MUD),              0.5},
        {std::pair(MOB, TERRAIN_PUDDLE),           0.3},
        {std::pair(MOB, TERRAIN_DRY),              1.0},
        {std::pair(MOB, TERRAIN_FIRE),             5.0},
        {std::pair(MOB, TERRAIN_ICE),              1.1},
        // eagle
        {std::pair(SPRING_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(SPRING_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(SPRING_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(SPRING_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(SPRING_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(SPRING_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(SPRING_BOSS, TERRAIN_ICE),              1.0},
        // moose
        {std::pair(SUMMER_BOSS, TERRAIN_PAVEMENT),         1.5},
        {std::pair(SUMMER_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(SUMMER_BOSS, TERRAIN_MUD),              0.8},
        {std::pair(SUMMER_BOSS, TERRAIN_PUDDLE),           0.5},
        {std::pair(SUMMER_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(SUMMER_BOSS, TERRAIN_FIRE),             5.0},
        {std::pair(SUMMER_BOSS, TERRAIN_ICE),              1.1},
        // bear
        {std::pair(FALL_BOSS, TERRAIN_PAVEMENT),         1.2},
        {std::pair(FALL_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(FALL_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(FALL_BOSS, TERRAIN_PUDDLE),           0.8},
        {std::pair(FALL_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(FALL_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(FALL_BOSS, TERRAIN_ICE),              1.0},
        // penguin
        {std::pair(WINTER_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(WINTER_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(WINTER_BOSS, TERRAIN_MUD),              0.5},
        {std::pair(WINTER_BOSS, TERRAIN_PUDDLE),           5.0},
        {std::pair(WINTER_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(WINTER_BOSS, TERRAIN_FIRE),             3.0},
        {std::pair(WINTER_BOSS, TERRAIN_ICE),              10.0},

        {std::pair(DRAGON_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(DRAGON_BOSS, TERRAIN_ICE),              1.0},

        {std::pair(FIREBALL_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(FIREBALL_BOSS, TERRAIN_ICE),              1.0},


        {std::pair(BURROW_BOSS, TERRAIN_PAVEMENT),         1.0},
        {std::pair(BURROW_BOSS, TERRAIN_DEFAULT),          1.0},
        {std::pair(BURROW_BOSS, TERRAIN_MUD),              1.0},
        {std::pair(BURROW_BOSS, TERRAIN_PUDDLE),           1.0},
        {std::pair(BURROW_BOSS, TERRAIN_DRY),              1.0},
        {std::pair(BURROW_BOSS, TERRAIN_FIRE),             1.0},
        {std::pair(BURROW_BOSS, TERRAIN_ICE),              1.0},
};

// maps <weather, terrain> to probability
const std::map<std::pair<int, int>, float> weather_terrain_prob_multiplier = {
        {std::pair(CLEAR, TERRAIN_MUD), 1.0},
        {std::pair(CLEAR, TERRAIN_PUDDLE), 1.0},
        {std::pair(CLEAR, TERRAIN_DRY), 1.0},
        {std::pair(CLEAR, TERRAIN_FIRE), 0.8},
        {std::pair(CLEAR, TERRAIN_ICE), 0.8},

        {std::pair(RAIN, TERRAIN_MUD), 1.0},
        {std::pair(RAIN, TERRAIN_PUDDLE), 1.2},
        {std::pair(RAIN, TERRAIN_DRY), 0.5},
        {std::pair(RAIN, TERRAIN_FIRE), 0.5},
        {std::pair(RAIN, TERRAIN_ICE), 0.5},

        {std::pair(DROUGHT, TERRAIN_MUD), 0.8},
        {std::pair(DROUGHT, TERRAIN_PUDDLE), 0.5},
        {std::pair(DROUGHT, TERRAIN_DRY), 1.2},
        {std::pair(DROUGHT, TERRAIN_FIRE), 1.0},
        {std::pair(DROUGHT, TERRAIN_ICE), 0.5},

        {std::pair(FOG, TERRAIN_MUD), 1.0},
        {std::pair(FOG, TERRAIN_PUDDLE), 1.0},
        {std::pair(FOG, TERRAIN_DRY), 0.8},
        {std::pair(FOG, TERRAIN_FIRE), 0.5},
        {std::pair(FOG, TERRAIN_ICE), 0.5},

        {std::pair(SNOW, TERRAIN_MUD), 0.8},
        {std::pair(SNOW, TERRAIN_PUDDLE), 0.5},
        {std::pair(SNOW, TERRAIN_DRY), 0.5},
        {std::pair(SNOW, TERRAIN_FIRE), 0.5},
        {std::pair(SNOW, TERRAIN_ICE), 1.2},
};