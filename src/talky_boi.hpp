#pragma once

#include "common.hpp"
#include "entt.hpp"
#include <map>

static const std::string round_voice_file_name[] = {
    "please_help_us",
    "whew_thanks(extended)",                            // boss
    "get_ready_for_summer",
    "wait_is_that_a_moose",                             // boss
    "tim/were_heading_into_fall", // 5
    "tim/prepare_for_lockdown_the_bears_are_coming",    // boss
    "tim/winter_is_coming",
    "tim/beware_the_killer_penguins",                   // boss
    "tim/hunters_rifles_out_its_hunting_time",
    "wait_whats_that_in_the_sky",                       // 10 // boss
    "tim/shhh_its_wabbit_season",
    "oh(surprised)",                                    // boss
    "tim/hunters_rifles_out_its_hunting_time",
    "oh(surprised)",                                    // boss
    "tim/shhh_its_wabbit_season",           // 15
    "oh(surprised)",                                    // boss
    "tim/prepare_for_lockdown",           // 17
};

const std::map<int, std::string> round_talky_boi_sprite = {
    {0, "dave_animate.png"},
    {1, "dave_animate.png"},
    {2, "dave_animate.png"},
    {3, "dave_animate.png"},
    {4, "tim_animate.png"},
    {5, "tim_animate.png"},
    {6, "tim_animate.png"},
    {7, "tim_animate.png"},
    {8, "tim_animate.png"},
    {9, "dave_animate.png"},
    {10, "tim_animate.png"},
    {11, "dave_animate.png"},
    {12, "tim_animate.png"},
    {13, "dave_animate.png"},
    {14, "tim_animate.png"},
    {15, "dave_animate.png"},
    {16, "tim_animate.png"}
};

inline std::string speakers_texture_path(const std::string& name) { return data_path() + "/textures/speakers/" + name; };

// Projectile from
struct TalkyBoi
{
    // Creates all the associated render resources and default transform
    static entt::entity createTalkyBoiEntt(int round_number);
   
};

