#include "grid_node.hpp"

const std::map<int, std::string> terran_texture_path = {
        {TERRAIN_DEFAULT,  "grid/grass.png"},
        {TERRAIN_PAVEMENT, "grid/pavement.png"},
        {TERRAIN_MUD,      "grid/mud.png"},
        {TERRAIN_PUDDLE,   "grid/puddle.png"}
};

const std::map<int, std::string> terran_str = {
        {TERRAIN_DEFAULT,  "grass"},
        {TERRAIN_PAVEMENT, "pavement"},
        {TERRAIN_MUD,      "mud"},
        {TERRAIN_PUDDLE,   "puddle"}
};

entt::entity GridNode::createGridNode(int terran, vec2 coord)
{
    // get up node components
    auto entity = registry.create();
    auto& node = registry.emplace<GridNode>(entity);
    node.terran = terran;
    node.coord = coord;

    // set up mesh components
    const std::string& key = terran_str.at(terran);
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terran_texture_path.at(terran)), key);
    }
    registry.emplace<ShadedMeshRef>(entity, resource);

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = coordToPixel(coord);
    // Setting initial values, scale is 1
    motion.scale = vec2({ 1, 1 }) * static_cast<vec2>(resource.texture.size);

    registry.emplace<HighlightBool>(entity); //component that stores whether this gridnode should be highlighted

    return entity;
}

void GridNode::setTerran(entt::entity entity, int new_terran) {
    this->terran = new_terran;
    const std::string& key = terran_str.at(new_terran);

    auto& shaded_mesh_ref = registry.get<ShadedMeshRef>(entity);
    ShadedMesh& resource = cache_resource(key);

    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(terran_texture_path.at(new_terran)), key);
    }
    else
    {
        resource.texture.load_from_file(textures_path(terran_texture_path.at(new_terran)).c_str());
    }
    shaded_mesh_ref.reference_to_cache = &resource;
}

void GridNode::setOccupancy(int new_occupancy) {
    this->occupancy = new_occupancy;
}
