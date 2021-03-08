// Created by Ray 

#include "village.hpp"
#include "render.hpp"

entt::entity Village::createVillage(GridMap& current_map)
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "village";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, textures_path("units/village.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh.layer = 11;

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
	motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 3);
    motion.position = coord_to_pixel(VILLAGE_COORD);
    current_map.setGridOccupancy(VILLAGE_COORD, OCCUPANCY_VILLAGE);

	Food& food = registry.emplace<Food>(entity);
	food.food = 100;
	food.food_production_speed = 10;

	Animate& animate = registry.emplace<Animate>(entity);
	animate.frame = 0.f;
	animate.state = 0.f;
	animate.frame_num = 1.f;
	animate.state_num = 1.f;

	registry.emplace<Village>(entity);

	return entity;
}