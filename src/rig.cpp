#include "rig.hpp"
#include "camera.hpp"
#include <iostream>

//TODO: find_keyframe function + make it based on elapsed_ms
//TODO: complex prescribed motion for keyframes interpolation

void Rig::animate_rig(entt::entity character) {
    auto& timeline = registry.get<Timeline>(character);
    //updates all angles given a certain frame
    timeline.current_frame = (++timeline.current_frame) % (24 * (timeline.frame.size() - 1)); // increment frame..
    float t = float(timeline.current_frame) / 24.0f; // a number [0,1] we use as parameter 't' in the linear interpolation
    auto& rig = registry.get<Rig>(character);

    int angle_idx = 0; // this is needed since i we have a 2d vector and 1d vector of angle_data
    for (int k = 0; k < rig.chains.size(); k++) {
        for (int i = 0; i < rig.chains[k].size(); i++) {
            auto& motion = registry.get<Motion>(rig.chains[k][i]); // could be better to have a motion array in top node.
            motion.angle = mix(timeline.frame[0].angle[angle_idx], timeline.frame[1].angle[angle_idx], t);
            angle_idx++;
        }
    }
    
}

Transform Rig::parent(Transform parent, Motion child_motion, Motion root_motion) {
    Transform child;
    child.mat = glm::mat3(1.0);
    child.translate(child_motion.position * root_motion.scale);
    child.translate(child_motion.origin * root_motion.scale); //translate, rotate, -translate == change rotation's pivot
    child.rotate(child_motion.angle);
    child.translate(-child_motion.origin * root_motion.scale);
    child.mat = parent.mat * child.mat; //this applies parent's transforms to child
    return child;
}

//create kinematic chain via transforms
//must be called every step... only way to change this is too have root accessible in render.cpp and then render by top node
void Rig::update_rig(entt::entity character) {

        auto& rig = registry.get<Rig>(character);
        auto& root_motion = registry.get<Motion>(character);
        //create parent constraints
        for (auto& chain : rig.chains) {
            Transform previous_transform;
            previous_transform.mat = glm::mat3(1.0);
            for (auto& part : chain) {
                auto& transform = registry.get<Transform>(part);
                auto& motion = registry.get<Motion>(part);
                transform = Rig::parent(previous_transform, motion, root_motion);
                previous_transform = transform;
            }
        }

        //must adjust scale after parent constraints!!
        for (auto& chain : rig.chains) {
            for (auto& part : chain) {
                auto& transform = registry.get<Transform>(part);
                auto& motion = registry.get<Motion>(part);
                transform.scale(root_motion.scale * motion.scale);
            }
        } 
}


entt::entity Rig::createPart(entt::entity root_entity, std::string name, vec2 offset, vec2 origin, float angle)
{

    auto entity = registry.create();

    std::string key = name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.size() == 0)
    {
        resource.mesh.loadFromOBJFile(mesh_path(name + ".obj"));
        RenderSystem::createColoredMesh(resource, "spider"); // TODO: need texturedMesh function
    }
    ShadedMeshRef& mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
    mesh_ref.layer = 60;

    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = angle;
    motion.velocity = { 0, 0 };
    motion.position = offset;
    motion.scale = resource.mesh.original_size;
    motion.scale.y *= -1;
    motion.boundingbox = motion.scale;
    motion.origin = origin;

    registry.emplace<Transform>(entity);
    registry.emplace<Root>(entity, root_entity);
    return entity;
}


/*
    IK solve stuff
*/


float score_dist(entt::entity part, vec2 mouse, Transform root_transform) {
    auto& motion = registry.get<Motion>(part);
    const auto& transform = registry.get<Transform>(part);
    vec3 end_effector = root_transform.mat * transform.mat * vec3(motion.origin.x, motion.origin.y, 1); // point in world space
    return length(mouse - vec2(end_effector.x, end_effector.y));
}




void Rig::ik_solve(entt::entity character, entt::entity camera, int chain_idx) {
    auto& rig = registry.get<Rig>(character);
    auto& root_motion = registry.get<Motion>(character);

    auto& camera_motion = registry.get<Motion>(camera);
    auto& camera_scale = registry.get<Motion>(camera).scale;

    vec2 mouse = registry.get<MouseMovement>(camera).mouse_pos;
    mouse = mouse_in_world_coord(mouse);

    Transform root_transform;
    root_transform.translate(root_motion.position);
    root_transform.rotate(root_motion.angle);

    float segments[] = { 50,0 }; // magic numbers. 
      
    for (int k = 0; k < rig.chains[chain_idx].size(); k++) {
        float alpha = 0.1f;
        auto& part = rig.chains[chain_idx][k];
        auto& motion = registry.get<Motion>(part);

        for (int i = 0; i < 10; i++) {
            float score_old = score_dist(part, mouse, root_transform);
            motion.angle += alpha;// 1) change angle
            Rig::update_rig(character);// 2) update rigs with angle changes
            float score_new = score_dist(part, mouse, root_transform);// 3) score
            if (abs(score_old -segments[k]) < abs(score_new - segments[k])) {
                motion.angle -= alpha; //reverse changes 
                alpha *= -1; //change direction
            }
            else {
                alpha *= 0.66f; //lower step size
            }
        }
    }
    
}