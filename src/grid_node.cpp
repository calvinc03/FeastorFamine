#include "grid_node.hpp"

const std::string node_shader = "node";

entt::entity GridNode::createGridNode(int terrain, vec2 coord)
{
    // get up node components
    auto entity = registry.create();
    auto& node = registry.emplace<GridNode>(entity);
    node.terrain = terrain;
    node.coord = coord;

    // set up mesh components
    if (terrain != TERRAIN_DEFAULT) {
        const std::string& key = terrain_str.at(terrain);
        ShadedMesh& resource = cache_resource(key);
        if (resource.effect.program.resource == 0)
        {
            resource = ShadedMesh();
            RenderSystem::createSprite(resource, textures_path(terrain_texture_path.at(terrain)), node_shader);
        }
        ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
        shaded_mesh.layer = 1;
    }

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
    int old_terrain = this->terrain;

    // no change in terrain
    if (old_terrain == new_terrain) {
        return;
    }

    this->terrain = new_terrain;

    // changing from special to default terrain
    if (new_terrain == TERRAIN_DEFAULT) {
        registry.remove<ShadedMesh>(entity);
        return;
    }

    // changing to special terrain
    const std::string &key = terrain_str.at(new_terrain);
    ShadedMesh &resource = cache_resource(key);
    if (resource.effect.program.resource == 0) {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path.at(terrain)), node_shader);
    }

    // changing from default to special terrain
    if (old_terrain == TERRAIN_DEFAULT) {
        ShadedMeshRef &shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
        shaded_mesh.layer = 1;
        return;
    }

    // changing from special to special
    auto &shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);
    shaded_mesh_ref.reference_to_cache = &resource;
}

void GridNode::setOccupancy(int new_occupancy) {
    this->occupancy = new_occupancy;
}
