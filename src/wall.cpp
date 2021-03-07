// Created by Ray 

#include "wall.hpp"
#include "render.hpp"

entt::entity Wall::createWall(vec2 position, bool rotate) // rotation should be based on direction of path
{
	auto entity = registry.create();
	// Create rendering primitives
	std::string key = "wall" + rotate;
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		resource = ShadedMesh();
		
		if(rotate) 
			RenderSystem::createSprite(resource, textures_path("wall90.png"), "textured");
		else
			RenderSystem::createSprite(resource, textures_path("wall.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	auto& shaded_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
	shaded_mesh_ref.layer = 40;

	// Setting initial motion values
	Motion& motion = registry.emplace<Motion>(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({0.5f,0.5f }) * static_cast<vec2>(resource.texture.size);

	auto& unit = registry.emplace<Unit>(entity);
	unit.damage = 0;
	unit.attack_rate = 0;
	unit.attack_range = 0;
	unit.workers = 0;
	unit.upgrades = 0;
	unit.type = WALL_NAME;
	unit.rotate = rotate;

	registry.emplace<Wall>(entity);
	registry.emplace<Selectable>(entity);
	registry.emplace<HighlightBool>(entity);

	return entity;
}