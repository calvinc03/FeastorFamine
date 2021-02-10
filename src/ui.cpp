#include "ui.hpp"
#include "render.hpp"
#include <string>

entt::entity UI::createUI()
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "UI";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, textures_path("UI-texture-15.png"), "textured");
	}
	
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.emplace<ShadedMeshRef>(entity, resource);

	Motion& motion = registry.emplace<Motion>(entity);
	motion.scale = vec2(WINDOW_SIZE_IN_PX.x, WINDOW_SIZE_IN_PX.y / 10.0f);
	motion.position = vec2(WINDOW_SIZE_IN_PX.x/2, WINDOW_SIZE_IN_PX.y - motion.scale.y/2.0f) / (float)GRID_CELL_SIZE;

	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	

	registry.emplace<UI>(entity);

	return entity;
}

entt::entity UI_element::createUI_button(int pos, Button button) {
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "UI_button " + pos;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		if (button == tower_button) {
			registry.emplace<Button>(entity, tower_button);
			RenderSystem::createSprite(resource, textures_path("tower_icon.png"), "ui");
		}
		else if (button  == green_house_button) {
			registry.emplace<Button>(entity, green_house_button);
			RenderSystem::createSprite(resource, textures_path("green_house_icon.png"), "ui");
		}
		else if (button == stick_figure_button) {
			registry.emplace<Button>(entity, stick_figure_button);
			RenderSystem::createSprite(resource, textures_path("stickfigure.png"), "ui");
		}
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.emplace<ShadedMeshRef>(entity, resource);

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	motion.position = vec2( 200+ pos * motion.scale.x, WINDOW_SIZE_IN_PX.y  - motion.scale.y/2.0f) / (float)GRID_CELL_SIZE; //WINDOW_SIZE_IN_PX.x / 2 +

	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	
	registry.emplace<HighlightBool>(entity);
	
	registry.emplace<UI_element>(entity);

	return entity;
}