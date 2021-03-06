#include "grid_node.hpp"

const std::map<int, std::string> terrain_texture_path = {
        {TERRAIN_DEFAULT,  "map/grass.png"},
        {TERRAIN_PAVEMENT, "map/pavement.png"},
        {TERRAIN_MUD,      "map/mud.png"},
        {TERRAIN_PUDDLE,   "map/puddle.png"}
};

const std::map<int, std::string> terrain_str = {
        {TERRAIN_DEFAULT,  "grass"},
        {TERRAIN_PAVEMENT, "pavement"},
        {TERRAIN_MUD,      "mud"},
        {TERRAIN_PUDDLE,   "puddle"}
};

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
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path.at(terrain)), key);
    }
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = 1;

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = coordToPixel(coord);
    // Setting initial values, scale is 1
    motion.scale = vec2({ 1, 1 }) * static_cast<vec2>(resource.texture.size);

    registry.emplace<HighlightBool>(entity); //component that stores whether this gridnode should be highlighted

    return entity;
}

void GridNode::set_terrain(entt::entity entity, int new_terrain) {
    this->terrain = new_terrain;
    const std::string& key = terrain_str.at(new_terrain);

    auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);
    ShadedMesh& resource = cache_resource(key);

    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terrain_texture_path.at(new_terrain)), key);
    }
    else
    {
        resource.texture.load_from_file(textures_path(terrain_texture_path.at(new_terrain)).c_str());
    }
    shaded_mesh_ref.reference_to_cache = &resource;
}

void GridNode::set_occupancy(int new_occupancy) {
    this->occupancy = new_occupancy;
}
