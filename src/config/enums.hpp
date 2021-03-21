// Terrains with neg value are ones that cannot be placed on
// May be refactored later if too hard to keep track
enum grid_terrain
{
    TERRAIN_PAVEMENT = -1,
    TERRAIN_DEFAULT = 0,
    TERRAIN_MUD = 1,
    TERRAIN_PUDDLE = 2,
    TERRAIN_DRY = 3,
    TERRAIN_FIRE = 4,
    TERRAIN_ICE = 5,
};

enum grid_occupancy
{
    OCCUPANCY_BLOCKED = -1,
    OCCUPANCY_VACANT = 0,
    OCCUPANCY_FOREST = 1,
    OCCUPANCY_VILLAGE = 2,
    OCCUPANCY_GREENHOUSE = 3,
    OCCUPANCY_TOWER = 4,
    OCCUPANCY_WALL = 5,
    OCCUPANCY_HUNTER = 6,
};

enum season
{
    SPRING = 0,
    SUMMER = 1,
    FALL = 2,
    WINTER = 3
};

// Weather
enum weather
{
    CLEAR = 0,
    RAIN = 1,
    DROUGHT = 2,
    FOG = 3,
    SNOW = 4,
};

//enum directions
//{
//    UP = 0,
//    DOWN = 1,
//    LEFT = 2,
//    RIGHT = 3,
//    UL = 4,
//    UR = 5,
//    LL = 6,
//    LR = 7,
//};