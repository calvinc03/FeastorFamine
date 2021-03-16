// Header
#include "render.hpp"
#include "spider.hpp"
#include "mob.hpp"

void Spider::animate() {
    auto view_rigs = registry.view<Spider, Motion>();
    for (auto [entity, spider, root_motion] : view_rigs.each()) {

        auto& body_motion = registry.get<Motion>(spider.body);
        auto& L_upper_leg_motion = registry.get<Motion>(spider.L_upper_leg);
        auto& L_lower_leg_motion = registry.get<Motion>(spider.L_lower_leg);
        auto& R_upper_leg_motion = registry.get<Motion>(spider.R_upper_leg);
        auto& R_lower_leg_motion = registry.get<Motion>(spider.R_lower_leg);

        auto& timeline = registry.get<Timeline>(entity);
        timeline.current_frame = (++timeline.current_frame)%(24*(timeline.frame.size()-1)); //later this should be based off elapsed_ms

        //test code
        root_motion.position += vec2(0, 1);
        auto& frame = timeline.frame;
        
        float a = float(timeline.current_frame) / 24.0f;
        int f = timeline.current_frame;

        // TODO: find_keyframe function
        L_upper_leg_motion.angle = mix(frame[0].angle[0], frame[1].angle[0], a); // mix == basic linear interp
        L_lower_leg_motion.angle = mix(frame[0].angle[1], frame[1].angle[1], a);

        R_upper_leg_motion.angle = mix(frame[0].angle[2], frame[1].angle[2], a); 
        R_lower_leg_motion.angle = mix(frame[0].angle[1], frame[1].angle[3], a);
    }
}

// this function creates a hierarchy through accumulating transforms.
void Spider::update_rigs() {
    auto view_rigs = registry.view<Spider, Motion>();
    for (auto [entity,spider, root_motion] : view_rigs.each()) { // motion == root

        auto& body_motion = registry.get<Motion>(spider.body);
        auto& L_upper_leg_motion = registry.get<Motion>(spider.L_upper_leg);
        auto& L_lower_leg_motion = registry.get<Motion>(spider.L_lower_leg);
        auto& R_upper_leg_motion = registry.get<Motion>(spider.R_upper_leg);
        auto& R_lower_leg_motion = registry.get<Motion>(spider.R_lower_leg);

        // create kinematic chain via transforms
        auto& body_transform = registry.get<Transform>(spider.body);
        auto& L_upper_leg_transform = registry.get<Transform>(spider.L_upper_leg);
        auto& L_lower_leg_transform = registry.get<Transform>(spider.L_lower_leg);
        auto& R_upper_leg_transform = registry.get<Transform>(spider.R_upper_leg);
        auto& R_lower_leg_transform = registry.get<Transform>(spider.R_lower_leg);

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
    auto body = createSpiderPart("body");
    auto L_upper_leg = createSpiderPart("rect7", vec2(2.0f,0), vec2(0,1.4f), 3.14f); // position, origin, angle
    auto L_lower_leg = createSpiderPart("rect7", vec2(), vec2(0, -1.4f), 3.14 / 1.0f);

    auto R_upper_leg = createSpiderPart("rect7", vec2(-2.0f,0), vec2(0, 1.4f), 3.14f);
    auto R_lower_leg = createSpiderPart("rect7", vec2(), vec2(0, -1.4f), 3.14 / 1.5f);

    //create a component <Spider> to then point to these entities for later
    auto& spider = registry.emplace<Spider>(entity);
    spider.body = body;
    spider.L_lower_leg = L_lower_leg;
    spider.L_upper_leg = L_upper_leg;
    spider.R_lower_leg = R_lower_leg;
    spider.R_upper_leg = R_upper_leg;

    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.scale = { 40, 40};
    motion.position = { 400,100 };
    motion.boundingbox = motion.scale;

    // timeline holds a 'pointer' to the current frame and all the frame data.
    auto& timeline = registry.emplace<Timeline>(entity);
    timeline.frame.push_back(Frame({0.0f, 0.0f, 0.0f, 0.0f }));
    timeline.frame.push_back(Frame({ 1.0f, 2.0f, 1.0f, -1.0f }));
    timeline.frame.push_back(Frame({ 3.14f, 2.0f, 1.0f, 1.0f }));

    return entity;
}
entt::entity Spider::createSpiderPart(std::string name, vec2 offset, vec2 origin, float angle)
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
