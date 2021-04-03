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
        {WATCHTOWER, "watchtower"},
        {ROBOT,      "robot"},
        {GREENHOUSE, "greenhouse"},
        {EXTERMINATOR, "exterminator"},
        {PRIESTESS, "priestess"},
        {SNOWMACHINE, "snowmachine"},
        {HUNTER,     "hunter"},
        {WALL,       "wall"},
};

const std::map<int, std::string> season_str = {
        {SPRING,    "Spring"},
        {SUMMER,    "Summer"},
        {FALL,      "Fall"},
        {WINTER,    "Winter"},
};

const std::map<int, std::string> weather_str = {
        {CLEAR, "Clear"},
        {RAIN, "Rain"},
        {DROUGHT, "Drought"},
        {FOG, "Fog"},
        {SNOW, "Snow"}
};

const std::string INPUT_PATH = "data/monster_rounds/";
const std::string JSON_EXTENSION = ".json";