// Header
#include "render.hpp"
#include "spider.hpp"
#include "mob.hpp"

// TODO: find_keyframe function

void Anim::animate() {
    auto view_rigs = registry.view<Timeline>();
    for (auto [entity,timeline] : view_rigs.each()) {

        auto& rig = registry.get<Rig>(entity);
        auto& root_motion = registry.get<Motion>(entity);

        timeline.current_frame = (++timeline.current_frame)%(24*(timeline.frame.size()-1)); // increment frame..

        //test code
        root_motion.position += vec2(0, 1);

        
        float t = float(timeline.current_frame) / 24.0f; // a number [0,1] we use as parameter 't' in the linear interpolation

       //updates all angles given a certain frame
        for (int i = 1; i < rig.parts.size(); i++){
            auto& motion = registry.get<Motion>(rig.parts[i]); // could be better to have a motion array in top node.
            motion.angle = mix(timeline.frame[0].angle[i], timeline.frame[1].angle[i], t);
        }

    }
}

// this function creates a hierarchy through accumulating transforms.
void Rig::update_rigs() {
    auto view_rigs = registry.view<Rig, Motion>();
    for (auto [entity,rig, root_motion] : view_rigs.each()) { // motion == root

        //indexing spider.parts is not so great. needs a refactor
        auto& body_motion = registry.get<Motion>(rig.parts[0]);
        auto& L_lower_leg_motion = registry.get<Motion>(rig.parts[1]);
        auto& L_upper_leg_motion = registry.get<Motion>(rig.parts[2]);
        auto& R_lower_leg_motion = registry.get<Motion>(rig.parts[3]);
        auto& R_upper_leg_motion = registry.get<Motion>(rig.parts[4]);

        // create kinematic chain via transforms
        auto& body_transform = registry.get<Transform>(rig.parts[0]);
        auto& L_lower_leg_transform = registry.get<Transform>(rig.parts[1]);
        auto& L_upper_leg_transform = registry.get<Transform>(rig.parts[2]);
        auto& R_lower_leg_transform = registry.get<Transform>(rig.parts[3]);
        auto& R_upper_leg_transform = registry.get<Transform>(rig.parts[4]);


        //body becomes the root. kind of weird.
        body_transform.mat = glm::mat3(1.0);
        body_transform.translate(root_motion.position);
        body_transform.rotate(root_motion.angle);

        //create parent constraints
        //left leg chain
        L_upper_leg_transform = parent(body_transform, L_upper_leg_motion, root_motion);
        L_lower_leg_transform = parent(L_upper_leg_transform, L_lower_leg_motion, root_motion);

        //right leg chain
        R_upper_leg_transform = parent(body_transform, R_upper_leg_motion, root_motion);
        R_lower_leg_transform = parent(R_upper_leg_transform, R_lower_leg_motion, root_motion);

        
        //must adjust scale after!!
        body_transform.scale(root_motion.scale*body_motion.scale);
        L_upper_leg_transform.scale(root_motion.scale * L_upper_leg_motion.scale);
        L_lower_leg_transform.scale(root_motion.scale * L_lower_leg_motion.scale);
        R_upper_leg_transform.scale(root_motion.scale * R_upper_leg_motion.scale);
        R_lower_leg_transform.scale(root_motion.scale * R_lower_leg_motion.scale);
    }
}

//does not have a mesh, but a set of entities
entt::entity  Spider::createSpider() {
    auto entity = registry.create();

    //create entities/parts to be part of the kinematic chains -- requires setting position offset, pivot/origin of rotation, and intial angle
    auto body = Rig::createPart("body");
    auto L_upper_leg = Rig::createPart("rect7", vec2(2.0f,0), vec2(0,1.4f), 3.14f); // position, origin, angle
    auto L_lower_leg = Rig::createPart("rect7", vec2(), vec2(0, -1.4f), 3.14 / 1.0f);

    auto R_upper_leg = Rig::createPart("rect7", vec2(-2.0f,0), vec2(0, 1.4f), 3.14f);
    auto R_lower_leg = Rig::createPart("rect7", vec2(), vec2(0, -1.4f), 3.14 / 1.5f);

    //create a component <Rig> to then point to these entities for later
    auto& rig = registry.emplace<Rig>(entity);
    rig.parts.push_back( body);
    rig.parts.push_back( L_lower_leg);
    rig.parts.push_back( L_upper_leg);
    rig.parts.push_back( R_lower_leg);
    rig.parts.push_back( R_upper_leg);

    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.scale = { 40, 40};
    motion.position = { 400,100 };
    motion.boundingbox = motion.scale;

    
    // timeline holds a 'pointer' to the current frame and all the frame data.
    auto& timeline = registry.emplace<Timeline>(entity);
    timeline.frame.push_back(Frame({0.0f, 0.0f, 0.0f, 0.0f,0.0f }));
    timeline.frame.push_back(Frame({ 1.0f, 2.0f, 1.0f, -1.0f ,0.0f }));
    timeline.frame.push_back(Frame({ 3.14f, 2.0f, 1.0f, 1.0f,0.0f }));

    return entity;
}
entt::entity Rig::createPart(std::string name, vec2 offset, vec2 origin, float angle)
{
    auto entity = registry.create();

    std::string key = name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.size() == 0)
    {
        resource.mesh.loadFromOBJFile(mesh_path(name+".obj"));
        RenderSystem::createColoredMesh(resource, "spider");
    }
    ShadedMeshRef& body_mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);
    body_mesh_ref.layer = 60;

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

Transform parent(Transform parent, Motion child_motion, Motion root_motion) {
    Transform child;
    child.mat = glm::mat3(1.0);
    child.translate(child_motion.position * root_motion.scale);
    child.translate(child_motion.origin * root_motion.scale);
    child.rotate(child_motion.angle);
    child.translate(-child_motion.origin * root_motion.scale);
    child.mat = parent.mat * child.mat; //this applies parent's transforms to child
    return child;
}
