#include "rig.hpp"
#include <iostream>

//TODO: find_keyframe function + make it based on elapsed_ms
//TODO: complex prescribed motion for keyframes interpolation

//create a simple entity that takes part in kinematic chain
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
    registry.emplace<RigPart>(entity, root_entity);
    registry.emplace<KeyFrames>(entity);
    return entity;
}


Transform parent(Transform parent, Motion child_motion, Motion root_motion) {
    Transform child;
    child.mat = glm::mat3(1.0);
    child.translate(child_motion.position * root_motion.scale);
    child.translate(child_motion.origin * root_motion.scale); //translate, rotate, -translate == change rotation's pivot
    child.rotate(child_motion.angle);
    child.translate(-child_motion.origin * root_motion.scale);
    child.mat = parent.mat * child.mat; //this applies parent's transforms to child
    return child;
}


//updates transforms after updating angles
void RigSystem::update_rig(entt::entity character) {
    auto& rig = registry.get<Rig>(character);
    auto& root_motion = registry.get<Motion>(character);
    //create parent constraints
    for (auto& chain : rig.chains) {
        Transform previous_transform;
        previous_transform.mat = glm::mat3(1.0);
        for (auto& part : chain) {
            auto& transform = registry.get<Transform>(part);
            auto& motion = registry.get<Motion>(part);
            transform = parent(previous_transform, motion, root_motion);
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


void RigSystem::animate_rig_fk(entt::entity character, float elapsed_ms) {
    auto& timeline = registry.get<Timeline>(character);
    timeline.current_time += elapsed_ms/1000.0f;
    float t_current = timeline.current_time;


    auto& rig = registry.get<Rig>(character);
    for (auto chain : rig.chains) {
        for (auto part : chain) {
            std::map<float, float>::iterator lo, hi;
            auto& keyframes = registry.get<KeyFrames>(part);
            lo = keyframes.data.lower_bound(t_current);
            hi = keyframes.data.upper_bound(t_current);

            if (lo != keyframes.data.end() && hi != keyframes.data.end()) {
                lo--;
                float t0 = lo->first;
                float t1 = hi->first;
                float a0 = lo->second;
                float a1 = hi->second;
                float ratio = (t_current - t0) / (t1 - t0);
                auto& motion = registry.get<Motion>(part);
                float new_angle = mix(a0, a1, ratio);
                motion.angle = new_angle;

                //std::cout <<"time: "<< t_current << std::endl;
                //std::cout << "t0: "<<t0 <<" a0: " << a0 << std::endl;
                //std::cout << "t1: " << t1 << " a1: " << a1 << std::endl;
                //std::cout <<"ratio: " << (t_current - t0) / (t1 - t0) << std::endl << std::endl;
            }
        }
    }
}


//make a procedurally animated character. animation speed based on velocity.
//able to take recoil from hits
void RigSystem::animate_rig_ik(entt::entity character, float elapsed_ms) {

}


/*
    IK solver
*/

//find distant of joint-end and goal
float score_dist(entt::entity part, vec2 mouse, Transform root_transform) {
    auto& motion = registry.get<Motion>(part);
    const auto& transform = registry.get<Transform>(part);
    vec3 end_effector = root_transform.mat * transform.mat * vec3(motion.origin.x, motion.origin.y, 1); // point in world space
    return length(mouse - vec2(end_effector.x, end_effector.y));
}

//TODO: on creating rig, find segment lengths
void RigSystem::ik_solve(entt::entity character, vec2 goal, int chain_idx) {
    auto& rig = registry.get<Rig>(character);
    auto& root_motion = registry.get<Motion>(character);

    vec2 mouse = goal;
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
            RigSystem::update_rig(character);// 2) update rigs with angle changes
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