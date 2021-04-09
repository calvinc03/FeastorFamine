#include "rope.hpp"
const std::string link= "monsters/dragon_rig/chain_link.png";

entt::entity RopeRig::createRope(vec2 start, int length) {

    for (int i = 0; i < length; i++) {
        createRopePart();
    }
}

entt::entity RopeRig::createRopePart(vec2 pos) {
    
    auto entity = registry.create();

    std::string key = link;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(link), "monster");
    }


    ShadedMeshRef& mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
    mesh_ref.layer = 24;

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.0f;
    motion.velocity = { 0, 0 };
    motion.scale = vec2(10,10); 
    motion.position = pos;
    //motion.scale.y *= -1;
    motion.boundingbox = motion.scale;
    
    return entity;
    

}