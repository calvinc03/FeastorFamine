enum grid_occupancy
{
    OCCUPANCY_VACANT ,
    OCCUPANCY_FOREST ,
    OCCUPANCY_VILLAGE,
    OCCUPANCY_GREENHOUSE,
    OCCUPANCY_TOWER,
    OCCUPANCY_WALL,
    OCCUPANCY_HUNTER,
};

// Terrains with neg value are ones that cannot be placed on
// May be refactored later if too hard to keep track
enum terrain_type
{
    TERRAIN_PAVEMENT,
    TERRAIN_DEFAULT,
    TERRAIN_MUD,
    TERRAIN_PUDDLE,
    TERRAIN_DRY,
    TERRAIN_FIRE,
    TERRAIN_ICE,
};

const int monster_type_count = 9;

enum monster_type
{
    MOB,
    SPRING_BOSS,
    SUMMER_BOSS,
    FALL_BOSS,
    WINTER_BOSS,
    DRAGON_BOSS,
    FIREBALL_BOSS,
    BURROW_BOSS,
    SPIDER,
};

enum unit_type
{
    NONE,
    WATCHTOWER,
    TOWER,
    GREENHOUSE,
    HUNTER,
    WALL,
};

enum neighbor_type
{
    DIRECT_NBRS,
    DIAGONAL_NBRS,
    ALL_NBRS,
};

enum season
{
    SPRING,
    SUMMER,
    FALL,
    WINTER,
};

// Weather
enum weather
{
    CLEAR,
    RAIN ,
    DROUGHT,
    FOG ,
    SNOW,
};

//enum directions
//{
//    UP,
//    DOWN,
//    LEFT,
//    RIGHT,
//    UL,
//    UR,
//    LL,
//    LR,
//};