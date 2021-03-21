#pragma once

#include "entt.hpp"
#include "common.hpp"
#include "ui.hpp"

//enum Story { title_screen, settings, default_menu, help_menu };

static const std::string STORY_TEXT_PER_LEVEL[] = {
	"Mayor, please help us! There are some@horrible monsters approaching! Lay your@defenses so that they don't reach our village!",
	"Whew, thank you! That was too close.@I feat that's not the last of them, though.@Legend tells of an evil lurking in the forest.@...@What's that in the sky?",
	"Eagles?!? What will they think of next?@That's all of them for now, but there are@more smaller monsters approaching.@Spring is over, get ready for the heat!",
	"Thank you, mayor! I fear that this may not@be the last of them. At this time in@the summer, some of the villagers tell stories@about giant spiders lurking in the forest...",
	"You've taken care of all the spiders!@The weather is turning rainy as we go into fall,@and a lot of monsters will be searching for@places to lay low for the winter...",
	"Bad news, mayor. We have word from our@scouts that there are several bears headed@towards the village. They must be looking for@some winter lodging.@Don't let them steal our food!",
	"Brrrrr...Food is getting scarce.@The monsters are getting desperate.@Be careful, I sense a large attack force...",
	"We don't know how, but the murderous@emperor penguin kill squads have found us.@God help us all.",
	"Mayor, there is a MASSIVE amount of monsters@at the outskirts of the village!@I don't know if we'll be able to stop@them all.@Kill as many as you can!",
	"They are getting desperate. All the monsters@that are attacking are thin and scrawny, but@they still pack a punch. We'll only get half@the food value from them from this point@forward.",
	"Mayor, the eagles! The eagles are coming!@They don't look friendly!@We really must be making the ancient gods angry.@I'll talk to the villagers and make sure that@everyone is involved in its defense.",
	"There are murmers in the village of unrest.@Not too worry, I have rounded up the dissidents@and constructed gallows in the town square.@@Wait, that's illegal? Uhhh, I'll be right back.",
	"Hey, so about that thing we talked about?@That was a joke, right? No? Uh, well,@good thing we definitely didn't put any@villagers to death. No sir. This requisition@for gravestones? Must be unrelated",
	"Guys, can't we put this behind us? Are the@handcuffs really necessary? You can't do this@to me, I'm the town cryer! Who else are you@going to find to warn you about incoming@attacks?? What do you mean the scouts can do it?",
	"Greetings my Lord, as your new cryer I must@inform you that there is a large attack wave@incoming! It seems the bears have journeyed@home from fishing in the rivers of Thrace.@Be wary, we must construct extra guard towers!",
	"Children of the village! I see now in your eyes@the same fear that would take the heart of me.@A day may come when the courage of the village@fails, when we forsake our friends and break@all bonds of fellowship. BUT IT IS NOT THIS DAY!",
	"The penguin kill squads are back!@Arise, arise, riders of the village! Spears@shall be shaken, shield shall be splintered!@A sword-day! A red day! Ere the sun rises!@RIDE NOW, RIDE! RIDE FOR RUIN!",
	"This is it...our successful defense of the@village has brought about the wrath of the@ancient evil eldritch god of the forest.@To arms! Man the battlestations! No surrender!@Don't let it through the gates!"
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