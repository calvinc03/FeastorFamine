#include <string>

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

const std::string WATCHTOWER_NAME = "watchtower";
const std::string GREENHOUSE_NAME = "greenhouse";
const std::string HUNTER_NAME = "hunter";
const std::string WALL_NAME = "wall";

const std::string SPRING_BOSS_TYPE = "spring_boss_type";
const std::string SUMMER_BOSS_TYPE = "summer_boss_type";
const std::string FALL_BOSS_TYPE = "fall_boss_type";
const std::string WINTER_BOSS_TYPE = "winter_boss_type";
const std::string DRAGON_BOSS_TYPE = "dragon_boss_type";
const std::string FIREBALL_BOSS_TYPE = "fireball_boss_type";
const std::string BURROW_BOSS_TYPE = "burrow_boss_type";
