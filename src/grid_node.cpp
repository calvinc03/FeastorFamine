#include "grid_node.hpp"

const std::string NODE_SHADER = "node";
std::string terrain_texture_path(int terrain) { return "map/"+terrain_str.at(terrain)+".png";};

entt::entity GridNode::createGridNode(int terrain, vec2 coord)
{
    // get up node components
    auto entity = registry.create();
    auto& node = registry.emplace<GridNode>(entity);
    node.terrain = terrain;
    node.coord = coord;

    // set up mesh components
    const std::string& key = terrain_str.at(terrain);
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path(terrain)), NODE_SHADER);
    }
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 1;

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
    motion.position = coord_to_pixel(coord);
    // Setting initial values, scale is 1
    motion.scale = (vec2)GRID_CELL_SIZE;

    registry.emplace<HighlightBool>(entity); //component that stores whether this gridnode should be highlighted

    return entity;
}

void GridNode::setTerrain(entt::entity entity, int new_terrain) {
    this->terrain = new_terrain;
    const std::string& key = terrain_str.at(new_terrain);

    auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);
    ShadedMesh& resource = cache_resource(key);

    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path(new_terrain)), NODE_SHADER);
    }
    else
    {
        resource.texture.load_from_file(textures_path(terrain_texture_path(new_terrain)));
    }
    shaded_mesh_ref.reference_to_cache = &resource;
}

void GridNode::setOccupancy(int new_occupancy) {
    this->occupancy = new_occupancy;
}
