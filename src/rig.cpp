#include "rig.hpp"
#include <iostream>

vec2 point_in_world_space(vec2 pos, Transform transform_part, Transform root_transform);
Transform parent(Transform parent, Motion child_motion, Motion root_motion);
void animate_rig_fk_helper(entt::entity character, float elapsed_ms);
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
    motion.scale = resource.mesh.original_size;
    motion.position =  offset;
    motion.scale.y *= -1;
    motion.boundingbox = motion.scale;
    motion.origin = origin;

    registry.emplace<Transform>(entity);
    auto& rigPart = registry.emplace<RigPart>(entity, root_entity);

    registry.emplace<KeyFrames_FK>(entity);

    return entity;
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


/*
    FK & IK animate functions
*/

void RigSystem::animate_rig_fk(entt::entity character, float elapsed_ms) {
    animate_rig_fk_helper(character, elapsed_ms);
    update_rig(character);
}

//TODO: check corner cases of lower/upper bound
void animate_rig_fk_helper(entt::entity character, float elapsed_ms) {
    auto& timeline = registry.get<Timeline>(character);
    timeline.current_time += elapsed_ms/1000.0f;
    float t_current = timeline.current_time;
    
    bool finished_loop = true;
    auto& rig = registry.get<Rig>(character);
    for (auto chain : rig.chains) {
        for (auto part : chain) {
            std::map<float, float>::iterator lo, hi;
            auto& keyframes = registry.get<KeyFrames_FK>(part);
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

                finished_loop = false; //when all chains are at the end, this doesn't get set
            }
        }
    }
    if (finished_loop && timeline.loop) {
        timeline.current_time = 0;
    }
}


//make a procedurally animated character. animation speed based on velocity.
//able to take recoil from hits
void RigSystem::animate_rig_ik(entt::entity character, float elapsed_ms) {
    auto& rig = registry.get<Rig>(character);
    auto& root_motion = registry.get<Motion>(character);
    auto& timeline = registry.get<Timeline>(character);
    timeline.current_time += elapsed_ms / 1000.0f;
    float t_current = timeline.current_time;

    auto& keyframes = registry.get<KeyFrames_IK>(character);
    bool finished_loop = true;

    for(int i = 0; i < keyframes.data.size(); i++) {
        std::map<float, vec2>::iterator lo, hi;

        lo = keyframes.data[i].lower_bound(t_current);
        hi = keyframes.data[i].upper_bound(t_current);

        if (lo != keyframes.data[i].end() && hi != keyframes.data[i].end()) {
            lo--;
            float t0 = lo->first;
            float t1 = hi->first;
            vec2 a0 = lo->second;
            vec2 a1 = hi->second;
            float ratio = (t_current - t0) / (t1 - t0);

            vec2 new_pos = mix(a0, a1, ratio); // linear interpolation

            ik_solve(character, new_pos * root_motion.scale, i);
           // ik_solve(character, (new_pos * root_motion.scale + root_motion.position), i);

            finished_loop = false;
        }
    }

    if (finished_loop && timeline.loop) {
        timeline.current_time = 0;
    }
}


/*
    IK solver
*/

//TODO: optimize to converge faster but also have smooth behavior when moving between frames??
//TODO: break down into two cases: out of reach and within reach
void RigSystem::ik_solve(entt::entity character, vec2 goal, int chain_idx) {
    auto& rig = registry.get<Rig>(character);
    auto& root_motion = registry.get<Motion>(character);
    Transform root_transform;
    root_transform.translate(root_motion.position);
    root_transform.rotate(root_motion.angle);
    
    //vec2 goal_world_space = goal;
    vec2 goal_world_space = root_transform.mat*vec3(goal.x, goal.y,1); 
    
    std::vector<float> segment;
    float total_length = 0;
    
    //get total length of arm and the length of each segment
    for (int k = 0; k < rig.chains[chain_idx].size(); k++) {
        auto& part_motion = registry.get<Motion>(rig.chains[chain_idx][k]);
        float len = 2 * length(part_motion.origin * root_motion.scale);
        segment.push_back(len);
        total_length += len;
    }
    float offset_goal = total_length;

    for (int k = 0; k < rig.chains[chain_idx].size(); k++) {
        float alpha = 0.1f;
        auto& part = rig.chains[chain_idx][k];
        auto& part_motion = registry.get<Motion>(part);
        auto& part_transform = registry.get<Transform>(part);

        offset_goal -= segment[k];
        //vec2 pt = point_in_world_space(part_motion.origin, part_transform, root_transform) - root_motion.position;
        //std::cout << pt.x << " " << pt.y << std::endl;

        float score_old = length(goal_world_space - point_in_world_space(part_motion.origin, part_transform, root_transform));
        for (int i = 0; i < 10; i++) {        

            part_motion.angle += alpha;// 1) change angle
            RigSystem::update_rig(character);// 2) update rigs with angle changes
            float score_new = length(goal_world_space - point_in_world_space(part_motion.origin, part_transform, root_transform));// 3) score
            
            if (abs(score_old - offset_goal) < abs(score_new - offset_goal)) {
                part_motion.angle -= alpha; //reverse changes 
                alpha *= -1; //change direction
            }
            else {
                alpha *= 0.66f; //lower step size
            }
            score_old = score_new;
        }
    }
}


/*
    helpers
*/
    

vec2 point_in_world_space(vec2 pos, Transform transform_part, Transform root_transform) { //end of part point
    return root_transform.mat * transform_part.mat * vec3(pos.x, pos.y, 1);
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

void Rig::delete_rig(entt::entity character) {
    auto& rig = registry.get<Rig>(character);
    for (auto chains : rig.chains) {
        for (auto part :chains) {
            registry.destroy(part);
        }
    }
    registry.destroy(character);
}