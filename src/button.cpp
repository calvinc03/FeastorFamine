#include "button.hpp"
#include "render.hpp"

entt::entity MenuButton::create_button(double x, double y, std::string button_name, Button_texture button_texture)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "menu_button" + button_texture;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		std::string texture_file_name = "empty_button.png";
		switch (button_texture)
		{
			case new_game_button : texture_file_name = "new_game_button.png";  break;
			case exit_button	 : texture_file_name = "exit_button.png";	   break;
			case load_game_button: texture_file_name = "load_game_button.png"; break;
			case settings_button : texture_file_name = "settings_button.png";  break;
			case back_button     : texture_file_name = "back_button.png";	   break;
			case empty_button    : texture_file_name = "empty_button.png";     break;
		}
		RenderSystem::createSprite(resource, menu_button_texture_path(texture_file_name), "textured");
	}
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = 98;

	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.tag = button_name;
	ui_element.scale = vec2({ 1.0f, 1.0f }) * static_cast<vec2>(resource.texture.size) / 2.0f;
	ui_element.position = vec2(x, y);

	registry.emplace<MenuButton>(entity);

	return entity;
}

std::string on_click_button(vec2 mouse_pos)
{
	auto view_menu_button = registry.view<UI_element, MenuButton>();
	for (auto [entity, ui_element] : view_menu_button.each()) {
		if (sdBox(mouse_pos, ui_element.position, ui_element.scale / 2.0f) < 0.0f) {
			return ui_element.tag;
		}
	}
	return "";
}