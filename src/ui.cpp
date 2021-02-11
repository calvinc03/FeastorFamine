#include "ui.hpp"
#include "render.hpp"
#include <string>

//will move this function outside of world eventually.
void UI_highlight_system(vec2 mouse_pos) {
	auto view_ui = registry.view<UI_element, HighlightBool>(); //may make separate registry for UI elements. Could have position+scale instead of motion component
	for (auto [entity, ui_element, highlight] : view_ui.each()) {
		if (sdBox(mouse_pos / (float)GRID_CELL_SIZE, ui_element.position, ui_element.scale / 2.0f / (float)GRID_CELL_SIZE) < 0.0f) {
			highlight.highlight = true;
		}
		else {
			highlight.highlight = false;
		}
	}
}

Button UI_click_system() {
	auto view_buttons = registry.view<Button, HighlightBool>();
	for (auto [entity, button, highlight] : view_buttons.each()) {
		if (highlight.highlight) { // if a button is highlighted and we click -> button was pressed.
			return button;
		}
	}
	return no_button_pressed;
}
std::string button_to_string(int button) {
	switch (button) {
	case tower_button:
		return "tower_button";
	case green_house_button:
		return "green_house_button";
	case stick_figure_button:
		return "stick_figure_button";
	}
	return "no button / invalid button / or this method is broken!";
}

entt::entity UI_background::createUI_background()
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

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.scale = vec2(WINDOW_SIZE_IN_PX.x, WINDOW_SIZE_IN_PX.y / 10.0f);
	ui_element.position = vec2(WINDOW_SIZE_IN_PX.x/2, WINDOW_SIZE_IN_PX.y - ui_element.scale.y/2.0f) / (float)GRID_CELL_SIZE;

	
	registry.emplace<UI_background>(entity);

	return entity;
}

entt::entity UI_button::createUI_button(int pos, Button button) {
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "UI_button " + pos;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		if (button == tower_button) {
			RenderSystem::createSprite(resource, textures_path("tower_icon.png"), "ui");
		}
		else if (button  == green_house_button) {
			RenderSystem::createSprite(resource, textures_path("green_house_icon.png"), "ui");
		}
		else if (button == stick_figure_button) {
			RenderSystem::createSprite(resource, textures_path("stickfigure.png"), "ui");
		}
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.emplace<ShadedMeshRef>(entity, resource);

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = vec2( 200+ pos * ui_element.scale.x, WINDOW_SIZE_IN_PX.y  - ui_element.scale.y/2.0f) / (float)GRID_CELL_SIZE;


	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, button);
	registry.emplace<UI_button>(entity);

	return entity;
}