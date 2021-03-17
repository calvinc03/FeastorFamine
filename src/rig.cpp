#include "rig.hpp"
#include <iostream>
//TODO: find_keyframe function + make it based on elapsed_ms
//TODO: figure out if 1d array of <timestamp, angle> is better
//TODO: complex prescribed motion

void Rig::animate_rigs() {
    auto view_rigs = registry.view<Timeline>();
    for (auto [entity, timeline] : view_rigs.each()) {

        //auto& root_motion = registry.get<Motion>(entity); //test code
        //root_motion.position += vec2(0, 1);
        //root_motion.scale-= vec2(1, 1);

        //updates all angles given a certain frame
        timeline.current_frame = (++timeline.current_frame) % (24 * (timeline.frame.size() - 1)); // increment frame..
        float t = float(timeline.current_frame) / 24.0f; // a number [0,1] we use as parameter 't' in the linear interpolation
        auto& rig = registry.get<Rig>(entity);

        int angle_idx = 0; // this is needed since i we have a 2d vector and 1d vector of angle_data
        for (int k = 0; k < rig.chains.size(); k++) {
            for (int i = 0; i < rig.chains[k].size(); i++) {
                auto& motion = registry.get<Motion>(rig.chains[k][i]); // could be better to have a motion array in top node.
                motion.angle = mix(timeline.frame[0].angle[angle_idx], timeline.frame[1].angle[angle_idx], t);
                angle_idx++;
            }
        }
    }
}

//create kinematic chain via transforms
//must be called every step... only way to change this is too have root accessible in render.cpp and then render by top node
void Rig::update_rigs() {
    auto view_rigs = registry.view<Rig, Motion>();

    for (auto [entity, rig, root_motion] : view_rigs.each()) { // motion == root

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
}
#include "camera.hpp"
#include "debug.hpp"

// 1) make angle changes
// 2) update_rigs()
// 3) have a score function
// 4) repeat, learn and change step size/dir
void Rig::ik_solve(entt::entity camera) {
    auto& camera_motion= registry.get<Motion>(camera);
    auto& camera_scale = registry.get<Motion>(camera).scale;
    vec2 mouse = registry.get<MouseMovement>(camera).mouse_pos;
    mouse = mouse_in_world_coord(mouse);

    auto view_rigs = registry.view<Rig, Motion>();
    for (auto [entity, rig, root_motion] : view_rigs.each()) { // for every rig

        Transform root_transform;
        root_transform.translate(root_motion.position);
        root_transform.rotate(root_motion.angle);
            
        float alpha = 0.1f;
        auto& part = rig.chains[1][1]; //optimizing one part
        vec3 end_effector = vec3();
        for (int i = 0; i < 5; i++) { // iterate 5 times
            auto& motion_old = registry.get<Motion>(part);
            const auto& transform = registry.get<Transform>(part);
            end_effector = root_transform.mat * transform.mat * vec3(motion_old.origin.x, motion_old.origin.y, 1); // point in world space
            float dist_old = length(mouse - vec2(end_effector.x, end_effector.y));

            // 1) 
            motion_old.angle += alpha;

            // 2) update rigs with angle changes
            Rig::update_rigs();

            // 3) score
            auto& motion_new = registry.get<Motion>(part);
            end_effector = root_transform.mat * transform.mat * vec3(motion_new.origin.x, motion_new.origin.y, 1); // point in world space
            float dist_new = length(mouse - vec2(end_effector.x, end_effector.y));
            //std::cout << "score: " << dist << std::endl;
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