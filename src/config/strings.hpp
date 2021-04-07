#include <string>
#include <map>

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

const std::map<int, std::string> terrain_str = {
        {TERRAIN_DEFAULT,  "grass"},
        {TERRAIN_PAVEMENT, "pavement"},
        {TERRAIN_MUD,      "mud"},
        {TERRAIN_PUDDLE,   "puddle"},
        {TERRAIN_DRY,   "dry"},
        {TERRAIN_FIRE,   "fire"},
        {TERRAIN_ICE,   "ice"},
};

const std::map<int, std::string> unit_str = {
        //{WATCHTOWER, "watchtower"},
        {ROBOT,      "robot"},
        {GREENHOUSE, "greenhouse"},
        {EXTERMINATOR, "exterminator"},
        {PRIESTESS, "priestess"},
        {SNOWMACHINE, "snowmachine"},
        {HUNTER,     "hunter"},
        {WALL,       "wall"},
};

const std::map<int, std::string> season_str = {
        {SPRING,    "spring"},
        {SUMMER,    "summer"},
        {FALL,      "fall"},
        {WINTER,    "winter"},
};

const std::map<int, std::string> weather_str = {
        {CLEAR, "clear"},
        {RAIN, "rain"},
        {DROUGHT, "drought"},
        {FOG, "fog"},
        {SNOW, "snow"}
};

const std::map<MenuButtonType, std::string> menu_button_ui_tag = {
    {new_game_button,   "new_game_button"},
    {restart_round_button, "restart_round_button"},
    {load_game_button,  "load_game_button"},
    {title_help_button,   "title_help_button"},
    {title_exit_button,       "title_exit_button"},
    {exit_button,       "exit_button"},
    {help_button,       "help_button"},
    {back_button,       "back_button"},
    {menu_save_button,  "save_game_button"},
    {empty_button,      "empty_button"}
};

const std::string INPUT_PATH = "data/monster_rounds/";
const std::string JSON_EXTENSION = ".json";