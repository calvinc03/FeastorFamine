#include "story_card.hpp"
#include "render.hpp"
#include <iostream>

entt::entity StoryCard::createStoryCard(std::string story_card_text, std::string level_number)
{
	ivec2 pos = ivec2(MAP_SIZE_IN_PX.x - 530, MAP_SIZE_IN_PX.y + UI_TOP_BAR_HEIGHT - 300);
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "story_card";
	ShadedMesh& resource = cache_resource(key);
	resource = ShadedMesh();
	RenderSystem::createSprite(resource, data_path() + "/textures/story/story_text_box.png", "textured");

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = 80;

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0, 0);
	motion.scale = vec2(1.f, 1.f) * static_cast<vec2>(resource.texture.size);
	motion.position = pos;

	registry.emplace<StoryCard>(entity);

	// text
	auto font = TextFont::load("data/fonts/cascadia-code/Cascadia.ttf");
	// Level number
	auto level_text_entity = registry.create();
	auto level_text_scale = 1.0f;
	auto level_x_offset = -370;
	auto level_y_offset = 165 - UI_TOP_BAR_HEIGHT;
	auto& t_level = registry.emplace<Text>(level_text_entity, Text("Level " + level_number, font, vec2(pos.x + level_x_offset, pos.y + level_y_offset)));
	t_level.scale = level_text_scale;
	t_level.colour = { 1.0f, 1.0f, 1.0f };
	registry.emplace<StoryCardText>(level_text_entity);

	// story text, break up lines based on delimiter '@'
	auto story_text_scale = .6f;
	auto story_text_x_offset = -400;
	auto story_text_y_offset = 100 - UI_TOP_BAR_HEIGHT;

	std::string delimiter = "@";
	while (story_card_text.length() > 0) {
		auto story_text_entity = registry.create();

		int delimiter_pos = (int)story_card_text.find(delimiter);
		if (delimiter_pos == std::string::npos) {
			delimiter_pos = (int)story_card_text.length();
		}

		std::string line_string = story_card_text.substr(0, delimiter_pos);
		auto& t_story = registry.emplace<Text>(story_text_entity, Text(line_string, font, vec2(pos.x + story_text_x_offset, pos.y + story_text_y_offset)));
		t_story.scale = story_text_scale;
		t_story.colour = { 1.0f, 1.0f, 1.0f };
		registry.emplace<StoryCardText>(story_text_entity);
		story_text_y_offset -= 40; // line spacing todo don't hardcode maybe

		story_card_text.erase(0, delimiter_pos + delimiter.length());
	}

	return entity;
}
