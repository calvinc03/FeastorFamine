#include "rig.hpp"

//TODO: find_keyframe function + make it based on elapsed_ms
//TODO: split into rig, anim, spider files
//TODO: make rig properly scale/transform wrt zoom/camera
//TODO: figure out if 1d array of <timestamp, angle> is better

void Rig::animate_rigs() {
    auto view_rigs = registry.view<Timeline>();
    for (auto [entity, timeline] : view_rigs.each()) {
        //auto& root_motion = registry.get<Motion>(entity); //test code
        //root_motion.position += vec2(0, 1);

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

// this function creates a hierarchy through accumulating transforms.
void Rig::update_rigs() {
    auto view_rigs = registry.view<Rig, Motion>();
    for (auto [entity, rig, root_motion] : view_rigs.each()) { // motion == root

        //TODO: make this more elegant... root is treated differently and restricts rigs to one root for chains
        // create kinematic chain via transforms
        auto& body_motion = registry.get<Motion>(rig.root);
        auto& body_transform = registry.get<Transform>(rig.root);
        body_transform.mat = glm::mat3(1.0);
        body_transform.translate(root_motion.position);
        body_transform.rotate(root_motion.angle);

        //create parent constraints
        for (auto& chain : rig.chains) {
            Transform previous_transform = body_transform;
            for (auto& part : chain) {
                auto& transform = registry.get<Transform>(part);
                auto& motion = registry.get<Motion>(part);
                transform = Rig::parent(previous_transform, motion, root_motion);
                previous_transform = transform;
            }
        }

        //must adjust scale after parent constraints!!
        body_transform.scale(root_motion.scale * body_motion.scale);
        for (auto& chain : rig.chains) {
            for (auto& part : chain) {
                auto& transform = registry.get<Transform>(part);
                auto& motion = registry.get<Motion>(part);
                transform.scale(root_motion.scale * motion.scale);
            }
        }
    }
}

entt::entity Rig::createPart(std::string name, vec2 offset, vec2 origin, float angle)
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

    return entity;
}

Transform Rig::parent(Transform parent, Motion child_motion, Motion root_motion) {
    Transform child;
    child.mat = glm::mat3(1.0);
    child.translate(child_motion.position * root_motion.scale);
    child.translate(child_motion.origin * root_motion.scale);
    child.rotate(child_motion.angle);
    child.translate(-child_motion.origin * root_motion.scale);
    child.mat = parent.mat * child.mat; //this applies parent's transforms to child
    return child;
}