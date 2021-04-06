// Created by Ray 

#include "menu.hpp"
#include "render.hpp"

entt::entity Menu::createMenu(float x, float y, std::string menu_name, Menu_texture texture, int layer, vec2 scale)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "menu" + menu_name;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		std::string texture_file_name = "title_screen.png";
		switch (texture)
		{
			case title_screen:
				texture_file_name = "title_screen.png";
				break;
			case controls:
				texture_file_name = "controls_background.png";
				break;
			case help_menu:
				texture_file_name = "help_menu.png";
				break;
			case pause_menu:
				texture_file_name = "pause_menu_background.png";
				break;
			case lost_game:
				texture_file_name = "lost_game.png";
				break;
			default:
				texture_file_name = "title_screen.png";
				break;
		}
		RenderSystem::createSprite(resource, menu_texture_path(texture_file_name), "textured");
	}
	
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = layer;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(entity);
	ui_element.angle = 0.f;
	ui_element.tag = menu_name;
	ui_element.scale = scale * static_cast<vec2>(resource.texture.size);
	ui_element.position = { x, y };

	registry.emplace<Menu>(entity);

	return entity;
}

entt::entity Menu::createLostMenu()
{
	auto background_entity = registry.create();
	// Create rendering primitives
	std::string key = "lost_game_screen";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, menu_texture_path("lost_screen.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(background_entity, resource);
	shaded_mesh_ref.layer = 90;

	// Setting initial motion values
	UI_element& ui_element = registry.emplace<UI_element>(background_entity);
	ui_element.tag = "lost_game_screen";
	//ui_element.scale = vec2({ WINDOW_SIZE_IN_PX.x / resource.texture.size.x, WINDOW_SIZE_IN_PX.y / resource.texture.size.y }) * static_cast<vec2>(resource.texture.size);
	ui_element.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
	ui_element.position = { WINDOW_SIZE_IN_PX.x / 2, WINDOW_SIZE_IN_PX.y / 2 };
	registry.emplace<Menu>(background_entity);
	
	// rabbit animation
	auto rabbits_entity = registry.create();
	std::string rabbit_key = "rabbits_eating_animation";
	ShadedMesh& rabbit_resource = cache_resource(rabbit_key);
	if (rabbit_resource.effect.program.resource == 0) {
		rabbit_resource = ShadedMesh();
		RenderSystem::createSprite(rabbit_resource, menu_texture_path("rabbit_eating_anmiation.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& rabbit_shaded_mesh_ref = registry.emplace<ShadedMeshRef>(rabbits_entity, rabbit_resource);
	rabbit_shaded_mesh_ref.layer = 91;
	auto& anime = registry.emplace<Animate>(rabbits_entity);
	anime.state_num = 1;
	anime.frame_num = 4;
	anime.frame = 0;
	anime.state = 0;

	UI_element& rabbit_ui_element = registry.emplace<UI_element>(rabbits_entity);
	rabbit_ui_element.tag = "rabbits_eating_animation";
	rabbit_ui_element.scale = vec2({ 0.8f, 0.8f }) * static_cast<vec2>(rabbit_resource.texture.size);
	// rabbit animation is a bit shifted to the left, offset to center
	rabbit_ui_element.position = { WINDOW_SIZE_IN_PX.x / 2 + 30, WINDOW_SIZE_IN_PX.y / 2 };

	return background_entity;
}