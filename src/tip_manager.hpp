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
	
	bool hunter_tip;
	bool tower_tip;
	bool wall_tip;
	bool greenhouse_tip;
};

struct TipCard
{
	static entt::entity TipCard::createTipCard(int x, int y, std::vector<std::string> text_lines);
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