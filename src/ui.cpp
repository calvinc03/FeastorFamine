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
entt::entity create_ui_text(vec2 position, std::string content, float scale, vec3 colour) {
	auto entity = registry.create();
	auto notoRegular = TextFont::load("data/fonts/Noto/NotoSans-Regular.ttf");
	auto& ui_text = registry.emplace<Text>(entity, Text(content, notoRegular, position));
	ui_text.scale = scale;
	ui_text.colour = colour;
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

entt::entity UI_background::createUI_top_bar()
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
	ui_element.scale = vec2(WINDOW_SIZE_IN_PX.x, UI_TOP_BAR_HEIGHT);
	ui_element.position = vec2(WINDOW_SIZE_IN_PX.x / 2, 0 + ui_element.scale.y / 2.0f);

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
		else if (button == exterminator_button) {
			RenderSystem::createSprite(resource, ui_texture_path("stickfigure.png"), "ui");
		}
		else if (button == robot_button) {
			RenderSystem::createSprite(resource, ui_texture_path("stickfigure.png"), "ui");
		}
		else if (button == priestess_button) {
			RenderSystem::createSprite(resource, ui_texture_path("stickfigure.png"), "ui");
		}
		else if (button == snowmachine_button) {
			RenderSystem::createSprite(resource, ui_texture_path("stickfigure.png"), "ui");
		}
		else if (button == wall_button) {
			RenderSystem::createSprite(resource, ui_texture_path("wall_icon.png"), "ui");
		}
		else if (button == upgrade_path_1_button || button == upgrade_path_2_button) {
			RenderSystem::createSprite(resource, ui_texture_path("upgrade_icon.png"), "ui");
		}
		else if (button == sell_button) {
			RenderSystem::createSprite(resource, ui_texture_path("unit_selected_button_background.png"), "ui");
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

entt::entity UI_button::createTips_button(vec2 position)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "Tips_button";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("tips_button.png"), "ui");
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 91;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.5f, 1.5f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = position;

	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, Button::tips_button);
	registry.emplace<UI_button>(entity);


	return entity;
}

entt::entity UI_button::createStart_button(vec2 position)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "Start_button";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("start_button.png"), "ui");
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 91;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.7f, 1.7f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = position;

	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, Button::start_button);
	registry.emplace<UI_button>(entity);


	return entity;
}

entt::entity UI_button::createPause_button(vec2 position)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "Pause_button";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("pause_button.png"), "ui");
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 91;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.7f, 1.7f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = position;

	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, Button::pause_button);
	registry.emplace<UI_button>(entity);


	return entity;
}

entt::entity UI_button::createMore_button(vec2 position)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "More_button";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("more_options.png"), "ui");
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 91;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.7f, 1.7f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = position;

	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, Button::more_options_button);
	registry.emplace<UI_button>(entity);


	return entity;
}

void UI_button::fastforward_light_up()
{
	auto view = registry.view<Button>();
	for (auto entity: view)
	{
		if (registry.get<Button>(entity) == Button::fastforward_button)
		{
			registry.remove<ShadedMeshRef>(entity);
			std::string key = "Fastforward_button_light";
			ShadedMesh& resource = cache_resource(key);
			if (resource.effect.program.resource == 0) {
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, ui_texture_path("fastforward_button_light.png"), "textured");
			}
			ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
			shaded_mesh.layer = 91;
		}
	}	
}

void UI_button::fastforward_light_off()
{
	auto view = registry.view<Button>();
	for (auto entity : view)
	{
		if (registry.get<Button>(entity) == Button::fastforward_button)
		{
			registry.remove<ShadedMeshRef>(entity);
			std::string key = "Fastforward_button";
			ShadedMesh& resource = cache_resource(key);
			if (resource.effect.program.resource == 0) {
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, ui_texture_path("fastforward_button.png"), "ui");
			}
			ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
			shaded_mesh.layer = 91;
		}
	}
	
}

entt::entity UI_button::createFastforward_button(vec2 position)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "Fastforward_button";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("fastforward_button.png"), "ui");
	}


	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 91;
	shaded_mesh.show = false;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = key;
	ui_element.scale = vec2({ 1.7f, 1.7f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = position;

	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, Button::fastforward_button);
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
		std::cout << "[Warning]: UI button (enum: " << button <<") has no name or description specified in the config.\n" << oor.what();
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
		/*case WATCHTOWER:
			file_name = "watchtower_portrait.png";
			break;*/
		case GREENHOUSE:
			file_name = "greehouse_portrait.png";
			break;
		case WALL:
			file_name = "wall_portrait.png";
			break;
		case EXTERMINATOR:
			file_name = "greehouse_portrait.png";
			break;
		case ROBOT:
			file_name = "greehouse_portrait.png";
			break;
		case PRIESTESS:
			file_name = "greehouse_portrait.png";
			break;
		case SNOWMACHINE:
			file_name = "greehouse_portrait.png";
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

entt::entity UI_banner::createUI_Banner(std::string content, vec2 position, vec3 colour, float duration) {
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

entt::entity UI_selected_unit::createUI_selected_unit_upgrade_button(int pos, Button button, std::string tag, int unit_type, int path_num, bool show)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "UI_button " + pos + tag;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		
		RenderSystem::createSprite(resource, ui_texture_path("unit_selected_button_background.png"), "ui");
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
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(175 + pos * (ui_element.scale.x + 10), WINDOW_SIZE_IN_PX.y - ui_element.scale.y / 2.0f);

	auto& UIselection = registry.emplace<UI_selected_unit>(entity);
	UIselection.path_num = path_num;
	UIselection.unit_type = unit_str.at(unit_type);
	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, button);
	registry.emplace<UI_button>(entity);
	
	return entity;
}

entt::entity UI_selected_unit::create_selected_button_image(vec2 pos, std::string tag, Unit unit)
{
	auto entity = registry.create();
	int path;
	if (tag == "path_1_upgrade_button") {
		path = unit.path_1_upgrade;
	}
	else if (tag == "path_2_upgrade_button") {
		path = unit.path_2_upgrade;
	}
	else {
		path = 5;
	}

	// Create rendering primitives
	std::string key = "UI_selected_button " + path + tag + unit_str.at(unit.type);
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		RenderSystem::createSprite(resource, ui_texture_path("damage_upgrade.png"), "ui");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 95;
	shaded_mesh.show = true;

	RenderProperty& render_property = registry.emplace<RenderProperty>(entity);
	render_property.show = true;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "UI_selected_button " + path + tag + unit_str.at(unit.type);
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = pos;

	return entity;
}

entt::entity UI_selected_unit::create_selected_button_progress_bar(vec2 pos, int path_num)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "UI_selected_button_progress_bar" + path_num;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		switch (path_num) {
		case 0:
			RenderSystem::createSprite(resource, ui_texture_path("progress_bar_0.png"), "ui");
			break;
		case 1:
			RenderSystem::createSprite(resource, ui_texture_path("progress_bar_1.png"), "ui");
			break;
		case 2:
			RenderSystem::createSprite(resource, ui_texture_path("progress_bar_2.png"), "ui");
			break;
		case 3:
			RenderSystem::createSprite(resource, ui_texture_path("progress_bar_3.png"), "ui");
			break;
		}
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 95;
	shaded_mesh.show = true;

	RenderProperty& render_property = registry.emplace<RenderProperty>(entity);
	render_property.show = true;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "UI_selected_buttonn_progress_bar" + path_num;
	ui_element.scale = static_cast<vec2>(resource.texture.size) * 0.9f;
	ui_element.position = pos;

	return entity;
}

entt::entity UI_sell_button::createUI_sell_button(int pos, Button button, std::string tag, bool show)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "UI_sell_button";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		RenderSystem::createSprite(resource, ui_texture_path("unit_selected_button_background.png"), "ui");
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
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(175 + pos * (ui_element.scale.x + 10), WINDOW_SIZE_IN_PX.y - ui_element.scale.y / 2.0f);

	registry.emplace<HighlightBool>(entity);
	registry.emplace<Button>(entity, button);
	registry.emplace<UI_button>(entity);
	registry.emplace<UI_sell_button>(entity);

	UI_sell_button::create_sell_button_icon(vec2(ui_element.position.x - ui_element.scale.x / 4, ui_element.position.y));

	// text
	float line_size = 35; // relative to the text size
	// unit name text
	std::string short_description = "Sell";
	auto title_text_scale = 0.4f;
	auto magicalMystery = TextFont::load("data/fonts/MagicalMystery/MAGIMT__.ttf");
	// place title text at the top
	float top_margin = 25;
	auto y_title_offset = ui_element.scale.y / 2 - title_text_scale * line_size - top_margin;
	vec2 title_text_position = get_center_text_position(vec2(2* ui_element.scale.x / 3, ui_element.scale.y), vec2(ui_element.position.x + ui_element.scale.x / 6, ui_element.position.y), title_text_scale, short_description);
	auto& title = registry.emplace<Text>(entity, Text(short_description, magicalMystery, vec2(title_text_position.x, title_text_position.y + y_title_offset)));
	title.scale = title_text_scale;
	title.colour = { 0.f, 0.f, 0.f };

	return entity;
}

entt::entity UI_sell_button::create_sell_button_icon(vec2 pos)
{
	auto entity = registry.create();

	// Create rendering primitives
	std::string key = "UI_sell_icon";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();

		RenderSystem::createSprite(resource, ui_texture_path("sell_icon.png"), "ui");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 95;
	shaded_mesh.show = true;

	RenderProperty& render_property = registry.emplace<RenderProperty>(entity);
	render_property.show = true;

	// Setting initial ui_element values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "UI_sell_icon";
	ui_element.scale = static_cast<vec2>(resource.texture.size);
	ui_element.position = pos;

	registry.emplace<UI_sell_button>(entity);

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

void aligne_text_right(entt::entity entity, float right_alignment_position)
{
	auto& text_component = registry.get<Text>(entity);
	int str_length = (int)text_component.content.length();
	int x_offset = (int)pow(str_length * text_component.scale, 1.2) * 37;
	text_component.position = vec2(right_alignment_position - x_offset, text_component.position.y);
}

entt::entity UI_season_wheel::createUI_season_wheel() {
	auto entity = registry.create();

	std::string key = "UI_season_wheel";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("season_wheel.png"), "textured");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 98 ;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "UI_season_wheel";
	ui_element.scale = vec2({ 1.f, 1.f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(SEASON_WHEEL_X_OFFSET, SEASON_WHEEL_Y_OFFSET);

	return entity;
}


void UI_season_wheel::get_season_sequence() {
	// round_num, angle
	std::vector<int> season_sequence;
	for (int round_number = 0; round_number < MAX_ROUND_NUMBER; round_number++) {
		nlohmann::json round_json = get_json(INPUT_PATH + std::to_string(round_number) + JSON_EXTENSION);
		std::string loaded_season_str = round_json["season"];
		if (loaded_season_str == "spring")
		{
			season_sequence.push_back(season::SPRING);
		}
		// TODO
	}
	
}

entt::entity UI_season_wheel::createUI_season_wheel_arrow() {
	auto entity = registry.create();

	std::string key = "UI_season_wheel_arrow";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("season_wheel_arrow.png"), "textured");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 99;


	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "UI_season_wheel_arrow";
	ui_element.scale = vec2({ 1.f, 1.f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(SEASON_WHEEL_X_OFFSET, SEASON_WHEEL_Y_OFFSET);
	ui_element.angle = PI + PI / 12 - PI / (2 * ROUND_PER_SEASON);

	return entity;
}

entt::entity UI_weather_icon::createUI_weather_icon() {
	auto entity = registry.create();

	std::string key = "UI_weather_icon" + std::to_string(weather_type::CLEAR);
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, ui_texture_path("weather_clear.png"), "textured");
	}
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 98;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = "UI_weather_icon";
	ui_element.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = vec2(WEATHER_ICON_X_OFFSET, WEATHER_ICON_Y_OFFSET);

	return entity;
}

void UI_weather_icon::change_weather_icon(entt::entity entity, int weather) {

	std::string key = "UI_weather_icon" + std::to_string(weather);
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		// load weather textuer based on the weather
		switch (weather)
		{
		case weather_type::CLEAR:
			RenderSystem::createSprite(resource, ui_texture_path("weather_clear.png "), "textured");
			break;
		case weather_type::RAIN:
			RenderSystem::createSprite(resource, ui_texture_path("weather_rain.png "), "textured");
			break;
		case weather_type::DROUGHT:
			RenderSystem::createSprite(resource, ui_texture_path("weather_drought.png "), "textured");
			break;
		case weather_type::FOG:
			RenderSystem::createSprite(resource, ui_texture_path("weather_fog.png "), "textured");
			break;
		case weather_type::SNOW:
			RenderSystem::createSprite(resource, ui_texture_path("weather_snow.png "), "textured");
			break;
		}
		
	}
	ShadedMeshRef& shaded_mesh = registry.replace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 98;
}