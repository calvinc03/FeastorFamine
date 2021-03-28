// Terrains with neg value are ones that cannot be placed on
// May be refactored later if too hard to keep track
enum terrain_type
{
    TERRAIN_DEFAULT,
    TERRAIN_PAVEMENT,
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
    FINAL_BOSS,
    FIREBALL_BOSS,
    BURROW_BOSS,
    SPIDER,
};

enum unit_type
{
    NONE,
    FOREST,
    VILLAGE,
    WATCHTOWER,
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

// UI button
enum Button
{
    no_button_pressed,
    watchtower_button,
    green_house_button,
    hunter_button,
    wall_button,
    upgrade_button,
    save_button,
    sell_button,
    start_button
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