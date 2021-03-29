#pragma once

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

const int TIP_CARD_X = 400;
const int TIP_CARD_Y = 200;

class TipManager
{
public:
	TipManager();
	
	int tip_index;
	bool hunter_tip;
	bool tower_tip;
	bool wall_tip;
	bool greenhouse_tip;
};

struct TipCard
{
	static entt::entity createTipCard(int x, int y, std::vector<std::string> text_lines);
};

const std::vector<std::string> start_tips_0 = {
	"Welcome to Feast or Famine!",
	"",
	"To start the game by playing the tutorial,",
	"click the \'Game Tips\' button in the ",
	"bottom right hand corner of the screen."
};

const std::vector<std::string> start_tips_1 = {
	"Welcome to the tutorial!",
	"We will start by going over the basics.",
	"Feast or Famine is a tower defence game where",
	"the objective is to create units on the map",
	"to defend against waves of enemies."
};

const std::vector<std::string> start_tips_2 = {
	"Units are selected and placed on the map by",
	"clicking on the unit icons in the bottom left.",
	"After clicking on the icon, click on the map",
	"where you want to place the unit."
};

const std::vector<std::string> start_tips_3 = {
	"Enemies spawn in the forest in the top left",
	"and will attack your village in the bottom",
	"right. They will try to take the shortest path,",
	"so place your units wisely. Note that they",
	"will not always take the grey stone road!"
};

const std::vector<std::string> start_tips_4 = {
	"Different enemies prefer different map tiles.",
	"Enemies from the arctic might prefer ice, and",
	"they will travel faster on those tiles."
};

const std::vector<std::string> start_tips_5 = {
	"Different monsters can have different",
	"abilities. Some may be fast, some have high",
	"health, and some react differently when you",
	"attack them. There are even boss levels that",
	"will attack you in unexpected ways."
};

const std::vector<std::string> start_tips_6 = {
	"You can only place, upgrade, and sell units",
	"during the setup phase, which we are",
	"currently on. Once the battle phase starts,",
	"you will not be able to control units until",
	"they are defeated."
};

const std::vector<std::string> start_tips_7 = {
	"In the top bar, you'll see a few game",
	"statistics. You can see which level you are",
	"on, the amount of food you have, the current",
	"season, and the current weather. weather has",
	"no effect on the game but seasons do."
};

const std::vector<std::string> start_tips_8 = {
	"The current season dictates which enemies",
	"spawn, the weather pattern, and which map",
	"tiles will appear. The enemies that spawn in",
	"that season will prefer the map tiles that are",
	"unique to that season."
};

const std::vector<std::string> start_tips_9 = {
	"Your food is your health and resources.",
	"Get more food by killing enemies, and spend",
	"it on more units for the next round. Be",
	"careful, though, if an enemy attacks and",
	"takes all your food, the game is over!"
};

const std::vector<std::string> start_tips_10 = {
	"When you want to start the game, press the",
	"play button in the bottom right. For more",
	"information on each unit, click the unit",
	"icons in the bottom left."
};

const std::vector<std::string> hunter_tips = {
	"These units are slow and deal ranged damage.",
	"Place them at the front of your forces",
	"to make up for their limited range.",
	"Upgrading them will increase their damage,",
	"accuracy, and projectile speed."
};

const std::vector<std::string> tower_tips = {
	"Towers have large range but deal less damage.",
	"Upgrading towers increases their damage, and ",
	"if upgraded enough, they can deal area of effect",
	"damage with a short range. Upgrade them and place",
	"them in front of your other units for best effect."
};

const std::vector<std::string> greenhouse_tips = {
	"Greenhouses generate food at the end of every",
	"round. Buying them early may be risky, but",
	"they can provide a huge bonus after they",
	"pay for themselves within a few rounds.",
};

const std::vector<std::string> wall_tips = {
	"Walls do not attack, but they block enemies",
	"from traveling through that square. Place",
	"them strategically to funnel the enemies",
	"towards your defenses!"
};