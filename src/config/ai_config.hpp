static const std::vector<ivec2>& all_neighbors = {ivec2(1,0), ivec2(1,-1),ivec2(1,1),
                                       ivec2(0,-1),ivec2(0,1),
                                       ivec2(-1,0),ivec2(-1,1),ivec2(-1,-1)};

// maps <weather, terrain> to probability
const std::map<std::pair<int, int>, float> weather_terrain_prob_multiplier = {
        {std::pair(CLEAR, TERRAIN_MUD), 1.0},
        {std::pair(CLEAR, TERRAIN_PUDDLE), 1.0},
        {std::pair(CLEAR, TERRAIN_DRY), 1.0},
        {std::pair(CLEAR, TERRAIN_FIRE), 0.8},
        {std::pair(CLEAR, TERRAIN_ICE), 0.8},

        {std::pair(RAIN, TERRAIN_MUD), 1.0},
        {std::pair(RAIN, TERRAIN_PUDDLE), 1.2},
        {std::pair(RAIN, TERRAIN_DRY), 0.6},
        {std::pair(RAIN, TERRAIN_FIRE), 0.6},
        {std::pair(RAIN, TERRAIN_ICE), 0.6},

        {std::pair(DROUGHT, TERRAIN_MUD), 0.8},
        {std::pair(DROUGHT, TERRAIN_PUDDLE), 0.6},
        {std::pair(DROUGHT, TERRAIN_DRY), 1.2},
        {std::pair(DROUGHT, TERRAIN_FIRE), 1.0},
        {std::pair(DROUGHT, TERRAIN_ICE), 0.6},

        {std::pair(FOG, TERRAIN_MUD), 1.0},
        {std::pair(FOG, TERRAIN_PUDDLE), 1.0},
        {std::pair(FOG, TERRAIN_DRY), 0.8},
        {std::pair(FOG, TERRAIN_FIRE), 0.6},
        {std::pair(FOG, TERRAIN_ICE), 0.6},

        {std::pair(SNOW, TERRAIN_MUD), 0.8},
        {std::pair(SNOW, TERRAIN_PUDDLE), 0.6},
        {std::pair(SNOW, TERRAIN_DRY), 0.6},
        {std::pair(SNOW, TERRAIN_FIRE), 0.6},
        {std::pair(SNOW, TERRAIN_ICE), 1.2},
};