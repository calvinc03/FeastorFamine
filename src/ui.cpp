#include "ui.hpp"

#include "world.hpp"
void UI_highlight_system(vec2 mouse_pos) {
	auto view_ui = registry.view<UI_element, HighlightBool>(); //may make separate registry for UI elements. Could have position+scale instead of motion component
	for (auto [entity, ui_element, highlight] : view_ui.each()) {
		if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f  ) < 0.0f) {
			highlight.highlight = true;
		}
		else {
			highlight.highlight = false;
		}
	}
}

Button UI_click_system() {
	auto view_buttons = registry.view<Button, HighlightBool, ShadedMeshRef>();
	for (auto [entity, button, highlight, shadedmeshref] : view_buttons.each()) {
		if (highlight.highlight && shadedmeshref.show) { // if a button is highlighted and we click -> button was pressed.
			return button;
		}
	}
	return no_button_pressed;
}

std::string button_to_string(int button) {
	switch (button) {
	case watchtower_button:
		return "tower_button";
	case green_house_button:
		return "green_house_button";
	case hunter_button:
		return "stick_figure_button";
	case wall_button:
		return "wall_button";
	case save_button:
		return "save_button";
	case tips_button:
		return "tips_button";
	}
		
	return "no button / invalid button / or this method is broken!";
}

//creates text that is independent of aspect ratio and resolution
entt::entity create_ui_text(vec2 position, std::string content) {
	auto entity = registry.create();
	auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
	auto& ui_text = registry.emplace<Text>(entity, Text(content, notoRegular, position));
	ui_text.scale = 0.3f;
	ui_text.colour = { 1.0f,1.0f,1.0f };
	return entity;
}

entt::entity UI_background::createUI_background()
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "UI";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("UI-texture-15.png"), "textured");
	}
	
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 90;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "in_game_ui_background";
	ui_element.scale = vec2(WINDOW_SIZE_IN_PX.x, UI_TAB_HEIGHT);
	ui_element.position = vec2(WINDOW_SIZE_IN_PX.x/2,WINDOW_SIZE_IN_PX.y - ui_element.scale.y/2.0f);

	registry.emplace<UI_background>(entity);

	return entity;
}

entt::entity UI_button::createUI_button(int pos, Button button, std::string tag, bool show) //later: reference vars for cost in world.
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "UI_button " + pos + tag;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		if (button == watchtower_button) {
			RenderSystem::createSprite(resource, ui_texture_path("tower_icon.png"), "ui");
		}
		else if (button == green_house_button) {
			RenderSystem::createSprite(resource, ui_texture_path("green_house_icon.png"), "ui");
		}
		else if (button == hunter_button) {
			RenderSystem::createSprite(resource, ui_texture_path("stickfigure.png"), "ui");
		}
		else if (button == wall_button) {
			RenderSystem::createSprite(resource, ui_texture_path("wall_icon.png"), "ui");
		}
		else if (button == upgrade_button) {
			RenderSystem::createSprite(resource, ui_texture_path("upgrade_icon.png"), "ui");
		}
		else if (button == sell_button) {
			RenderSystem::createSprite(resource, ui_texture_path("sell_button.png"), "ui");
		}
		else if (button == save_button) {
			RenderSystem::createSprite(resource, ui_texture_path("save_button.png"), "ui");
		}
		else if (button == start_button) {
			RenderSystem::createSprite(resource, ui_texture_path("start_button.png"), "ui");
		}
		else if (button == tips_button) {
			RenderSystem::createSprite(resource, ui_texture_path("tips_button.png"), "ui");
		}
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 91;
	shaded_mesh.show = show;
	
	RenderProperty& render_property = registry.emplace<RenderProperty>(entity);
	render_property.show = show;
	
	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = tag;
	ui_element.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = vec2(175 + pos * ui_element.scale.x, WINDOW_SIZE_IN_PX.y - ui_element.scale.y / 2.0f);


	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, button);
	registry.emplace<UI_button>(entity);


	return entity;
}



void UI_build_unit::fill_UI_build_unit_component(UI_build_unit& ui_build_unit, Button button)
{
	try
	{
		ui_build_unit.unit_name = unit_name_str.at(button);
		ui_build_unit.descriptions = unit_description_str.at(button);
		
	} 
	catch (const std::out_of_range& oor)
	{
		std::cout << "[Warning]: UI button (enum: " << button <<") has no name or description specified in the config.\n";
	}
}

entt::entity UI_button::createUI_build_unit_button(int pos, Button button, size_t cost, std::string tag, bool show) //later: reference vars for cost in world.
{
	auto entity = UI_button::createUI_button(pos, button, tag, show);
	auto ui_element = registry.view<UI_element>().get<UI_element>(entity);
	auto& ui_build_unit = registry.emplace<UI_build_unit>(entity);
	UI_build_unit::fill_UI_build_unit_component(ui_build_unit, button);

	if (cost != 0) {
		auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
		auto& t = registry.emplace<Text>(entity, Text(std::to_string(cost), notoRegular, vec2(ui_element.position.x, WINDOW_SIZE_IN_PX.y - ui_element.position.y - 40)));
		t.scale = 0.3f;
		t.colour = { 1.0f,1.0f,1.0f };
	}

	return entity;
}

entt::entity UI_selected_unit_portrait::createUI_selected_unit_portrait(unit_type type)
{
	auto entity = registry.create();
	std::string key = "UI_protrait" + type;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		std::string file_name = "hunter_portrait.png";
		switch (type)
		{
		case HUNTER:
			file_name = "hunter_portrait.png";
			break;
		case WATCHTOWER:
			file_name = "watchtower_portrait.png";
			break;
		case GREENHOUSE:
			file_name = "greehouse_portrait.png";
			break;
		case WALL:
			file_name = "wall_portrait.png";
			break;
		}
		RenderSystem::createSprite(resource, ui_texture_path(file_name), "textured");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.show = true;
	shaded_mesh.layer = 99;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "portraits";
	ui_element.scale = vec2({ 1.2f, 1.2f }) * static_cast<vec2>(resource.texture.size);
	int x_offset = 100;
	ui_element.position = vec2(x_offset + ui_element.scale.x / 2, WINDOW_SIZE_IN_PX.y - ui_element.scale.y / 2.0f);

	registry.emplace<UI_selected_unit>(entity);
	registry.emplace<UI_selected_unit_portrait>(entity);

	return entity;
}

entt::entity UI_button::createUI_selected_unit_button(int pos, Button button, std::string tag, bool show)
{
	entt::entity entity = UI_button::createUI_button(pos, button, tag, show);
	registry.emplace<UI_selected_unit>(entity);
	return entity;
}

static entt::entity createUI_Banner(std::string content, vec2 position, vec3 colour, float duration) {
	auto entity = registry.create();

	std::string key = "UI_banner";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("UI-texture-15.png"), "ui");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);


	auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");

	auto& ui_text = registry.emplace<Text>(entity, Text(content, notoRegular, position));
	ui_text.scale = 1.0f;
	ui_text.colour = { 1.0f,1.0f,1.0f };

	//auto& timer = registry.emplace<Timer>(entity);
	//timer.counter_ms = duration;
	return entity;
}

void change_button_text(entt::entity button_entity, std::string button_text)
{
	auto view_ui = registry.view<UI_element>();
	auto view_unit = registry.view<Unit>();
	if (registry.has<Text>(button_entity))
	{
		// remove the original text
		registry.remove<Text>(button_entity);
	}
	// set the new text
	UI_element& ui_element = view_ui.get<UI_element>(button_entity);
	auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
	auto& t = registry.emplace<Text>(button_entity, Text(button_text, notoRegular, vec2(ui_element.position.x, WINDOW_SIZE_IN_PX.y - ui_element.position.y - 40)));
	t.scale = 0.3f;
	t.colour = { 1.0f,1.0f,1.0f };
}
