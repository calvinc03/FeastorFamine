// Created by Ray 

#include "village.hpp"
#include "render.hpp"
#include "grid_map.hpp"

entt::entity Village::createVillage()
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "village";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, textures_path("village.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.emplace<ShadedMeshRef>(entity, resource);

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size);
    motion.position = coordToPixel(VILLAGE_COORD);


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