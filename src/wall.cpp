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


	//animate breakdown of wall
	//Animate& animate = registry.emplace<Animate>(entity);
	//animate.frame = 0.f;
	//animate.state = 0.f;
	//animate.frame_num = 1.f;
	//animate.state_num = 1.f;

	registry.emplace<Wall>(entity);

	return entity;
}