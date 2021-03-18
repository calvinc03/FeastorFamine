// Created by Ray 

#include "menu.hpp"
#include "render.hpp"

entt::entity Menu::createMenu(double x, double y, std::string menu_name, Menu_texture texture, int layer, vec2 scale)
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
			case settings:
				texture_file_name = "settings_background.png";
				break;
			case help_menu:
				texture_file_name = "help_menu.png";
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
	Motion& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
	motion.scale = scale * static_cast<vec2>(resource.texture.size);
	motion.position = { x, y };

	registry.emplace<Menu>(entity);

	return entity;
}