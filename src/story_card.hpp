#pragma once

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

//enum Story { title_screen, settings, default_menu, help_menu };

static const std::string STORY_TEXT_PER_LEVEL[] = {
	"Adventurer, please help us! There are some horrible monsters approaching! Lay your defenses so that they don't reach our village!",
	"Thank you, adventurer! I fear that this may not be the last of them. At this time in the year, some of the villagers tell stories about giant spiders lurking in the forest...",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"words",
	"Oh no... there's a huge monster approaching! Get ready, this is going to be a tough fight! Give it all you've got!"
};

struct StoryCard
{
	static entt::entity createStoryCard(std::string story_card_text, std::string level_number);
};

//inline std::string menu_texture_path(const std::string& name) { return data_path() + "/textures/menu/" + name; };

struct StoryCardText
{
	std::string story_card_text;
};