#pragma once

#include "common.hpp"
#include "entt.hpp"

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

// Projectile from
struct TalkyBoi
{
    // Creates all the associated render resources and default transform
    static entt::entity createTalkyBoiEntt(int round_number);
   
};

