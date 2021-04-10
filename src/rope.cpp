#include "rope.hpp"
#include <iostream>

/* Physics Based Animation
Implement time stepping based physical simulation which can either serve as a background effects (e.g. water, 
smoke implemented using particles) or as active game elements (throwing a ball, swinging a rope, etc.). 
A subset of the game entities (main or background) should possess non-trivial physics properties 
such as momentum (linear or angular) and acceleration, and act based on those.
*/


const std::string link_top = "monsters/dragon_rig/chain_top.png";
const std::string link_side = "monsters/dragon_rig/chain_side.png";

entt::entity RopeRig::createRope(entt::entity anchor, int length, vec2 offset) {
    auto entity = registry.create();
    auto& rope = registry.emplace<RopeRig>(entity);

    rope.anchor = anchor;

    for (int i = 0; i < length; i++) {
        if (i % 2 == 0) {
            rope.chain.push_back(createRopePart( vec2(45 * i, 40 * i), link_top)); // add links to a vector
        }
        else {
            rope.chain.push_back(createRopePart( vec2(45 * i, 40 * i), link_side)); // add links to a vector
        }
            
    }
    rope.anchor = rope.chain[0];
    rope.offset = offset;

    RopeSystem::update_rig(entity); //initialize rig
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
        mesh_ref.layer = LAYER_MONSTERS + SPIDER + 1;
    else 
        mesh_ref.layer = LAYER_MONSTERS + SPIDER;
 
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.0f;
    motion.velocity = { 0, 0 };
    motion.scale = vec2(25,25); 
    motion.position = pos;
    //motion.scale.y *= -1;
    motion.boundingbox = motion.scale;
    motion.acceleration = { 0,50 }; //TODO: tweak this
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
        float dif = abs(dist - 10.0f);
        //link0_motion.position = link0_motion.position + dir * dif / 2.0f;
        //link1_motion.position = link1_motion.position - dir * dif / 2.0f;
         link1_motion.position = link1_motion.position - dir * dif ;

        //update rotation
        float angle = atan(dir.y/ dir.x);
       
        link1_motion.angle = angle;
   
    }
}
#include "camera.hpp"
//normalizes link distances and fixes position of 'anchor'
void RopeSystem::update_rig(entt::entity rope_rig) {
    auto& ropeRig = registry.get<RopeRig>(rope_rig);

    //fix initial chain link to a particular position
    auto& mouse = registry.get<MouseMovement>(camera).mouse_pos;
    auto link = ropeRig.chain[0];
    auto& link_motion = registry.get<Motion>(link);
    link_motion.position = mouse;

    //auto& anchor_motion = registry.get<Motion>(ropeRig.anchor); 
    //auto link = ropeRig.chain[0];
    //auto& link_motion = registry.get<Motion>(link);
    //link_motion.position = anchor_motion.position + ropeRig.offset;

    // iterative solver.
    for (int i = 0; i < 10; i++) { 
        update_helper(ropeRig);
    }
}

//should apply drag/disspative forces, gravity, and angular and linear momentum D:
void RopeSystem::update_physics(entt::entity rope_rig,float elapsed_ms) {
    float step_seconds = elapsed_ms / 1000.0f;

    auto& ropeRig = registry.get<RopeRig>(rope_rig);

    for (int i = 0; i < ropeRig.chain.size(); i++) {
        auto link = ropeRig.chain[i];
        auto& motion = registry.get<Motion>(link);
        motion.velocity += motion.acceleration * step_seconds;
        motion.velocity = min(motion.velocity, vec2(500,500)); //terminal velocity
        
    }
}