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
        {TOWER,      "tower"},
        {GREENHOUSE, "greenhouse"},
        {HUNTER,     "hunter"},
        {WALL,       "wall"},
};

const std::map<int, std::string> season_str = {
        {SPRING,    "spring"},
        {SUMMER,    "summer"},
        {FALL,      "fall"},
        {WINTER,    "winter"},
};


