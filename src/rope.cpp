#include "rope.hpp"
#include <iostream>


//const std::string link= "monsters/dragon_rig/chain_link.png";
const std::string link_top = "monsters/dragon_rig/chain_top.png";
const std::string link_side = "monsters/dragon_rig/chain_side.png";

entt::entity RopeRig::createRope(vec2 start, int length, vec2 offset) {
    auto entity = registry.create();
    auto& rope = registry.emplace<RopeRig>(entity);

    for (int i = 0; i < length; i++) {
        if (i % 2 == 0) {
            rope.chain.push_back(createRopePart(start + vec2(45 * i, 40 * i), link_top)); // add links to a vector
        }
        else {
            rope.chain.push_back(createRopePart(start + vec2(45 * i, 40 * i), link_side)); // add links to a vector
        }
            
    }
    rope.anchor = rope.chain[0];
    rope.offset = offset;

    update_rig(entity); //initialize rig
    return entity;
}

entt::entity RopeRig::createRopePart(vec2 pos, std::string name) {
    
    auto entity = registry.create();

    std::string key = name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(name), "monster");
    }


    ShadedMeshRef& mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);

    if(name == link_side)
        mesh_ref.layer = 24;
    else 
        mesh_ref.layer = 23;
 
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.0f;
    motion.velocity = { 0, 500 };
    motion.scale = vec2(50,50); 
    motion.position = pos;
    //motion.scale.y *= -1;
    motion.boundingbox = motion.scale;
    
    return entity;
    

}

void update_helper(RopeRig ropeRig) {

    for (int i = 0; i < ropeRig.chain.size() - 1; i++) {
        auto link0 = ropeRig.chain[i];
        auto link1 = ropeRig.chain[(int)i + 1];

        auto& link0_motion = registry.get<Motion>(link0);
        auto& link1_motion = registry.get<Motion>(link1);

        vec2 p1p0 = link1_motion.position - link0_motion.position;
        vec2 dir = normalize(p1p0);
        float dist = length(p1p0);

        //update dist
        float dif = abs(dist - 20.0f);
        link0_motion.position = link0_motion.position + dir * dif / 2.0f;
        link1_motion.position = link1_motion.position - dir * dif / 2.0f;
        
        //update rotation
        float angle = atan(dir.y/ dir.x);
        link1_motion.angle = angle;
   
    }
}
// can do physics here, or have a separate physics function for physics.cpp
void RopeRig::update_rig(entt::entity rope_rig) {
    auto& ropeRig = registry.get<RopeRig>(rope_rig);

    //fix initial chain link to a particular position
    auto link = ropeRig.chain[0];
    auto& link_motion = registry.get<Motion>(link);
    link_motion.position = vec2(200, 200);

    // iterative solver.
    for (int i = 0; i < 10; i++) { 
        update_helper(ropeRig);
    }
}

void RopeRig::update_physics(entt::entity rope_rig) {

}