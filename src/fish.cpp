// Header
#include "fish.hpp"
#include "render.hpp"

entt::entity Fish::createFish(vec2 position)
{
		// Reserve en entity
	//auto entity = entt::Entity();

	//entt::registry registry;
	auto entity = registry.create();


	// Create the rendering components
	std::string key = "fish";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0)
	{
		resource = ShadedMesh();
		RenderSystem::createSprite(resource, textures_path("fish.png"), "textured");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	//ECS::registry<ShadedMeshRef>.emplace(entity, resource);
	registry.emplace<ShadedMeshRef>(entity,resource);

	// Initialize the position, scale, and physics components
	//auto& motion = ECS::registry<Motion>.emplace(entity);
	auto& motion = registry.emplace<Motion>(entity);

	motion.angle = 0.f;
	motion.velocity = { 380.f, 0 };
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);

	// Create and (empty) Fish component to be able to refer to all fish
	//ECS::registry<Fish>.emplace(entity);
	registry.emplace<Fish>(entity);

	return entity;
}
