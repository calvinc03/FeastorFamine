// Header
#include "render.hpp"
#include "spider.hpp"
#include "mob.hpp"


//TODO
//void animate(time_elapsed) -- interpolate keyframes.
//generalize rig. 


// this function creates a hierarchy through accumulating transforms.
void Spider::update_rigs() {
    auto view_parts = registry.view<Spider, Motion>();
    for (auto [entity,spider, root_motion] : view_parts.each()) { // motion == root

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

        //test code
        L_lower_leg_motion.angle = 3.14 / 2.0f; // keyframes are an array of angles. 
        L_upper_leg_motion.angle += 0.2f;
        R_lower_leg_motion.angle = 3.14 / 2.0f; 
        R_upper_leg_motion.angle -= 0.2f;

        //body becomes the root.
        body_transform.mat = glm::mat3(1.0);
        body_transform.translate(root_motion.position);
        body_transform.rotate(root_motion.angle);

        //create parent constraints
        L_upper_leg_transform = parent(body_transform, L_upper_leg_motion, vec2(0, 70));
        L_lower_leg_transform = parent(L_upper_leg_transform, L_lower_leg_motion, vec2(0, -70));

        R_upper_leg_transform = parent(body_transform, R_upper_leg_motion, vec2(0, 70));
        R_lower_leg_transform = parent(R_upper_leg_transform, R_lower_leg_motion, vec2(0, -70));

        
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

    auto body = createSpiderPart("body");
    auto L_upper_leg = createSpiderPart("rect7", { 50,0 });
    auto L_lower_leg = createSpiderPart("rect7");
    auto R_upper_leg = createSpiderPart("rect7", { -50,0 });
    auto R_lower_leg = createSpiderPart("rect7");

    auto& spider = registry.emplace<Spider>(entity);
    spider.body = body;
    spider.L_lower_leg = L_lower_leg;
    spider.L_upper_leg = L_upper_leg;
    spider.R_lower_leg = R_lower_leg;
    spider.R_upper_leg = R_upper_leg;

    // acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.scale = { 50, 50};
    motion.position = { 400,100 };
    motion.boundingbox = motion.scale;

    return entity;
}
entt::entity Spider::createSpiderPart(std::string name, vec2 offset)
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
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = offset;
    motion.scale = resource.mesh.original_size;
    motion.scale.y *= -1;
    motion.boundingbox = motion.scale;

    registry.emplace<Transform>(entity);

    return entity;
}

Transform parent(Transform parent, Motion child_motion, vec2 origin_offset) {
    Transform child;
    child.mat = glm::mat3(1.0);
    child.translate(child_motion.position);
    child.translate(origin_offset); //TODO: these magic numbers are the pivot of rotation. should scale with motion.scale.
    child.rotate(child_motion.angle);
    child.translate(-origin_offset);
    child.mat = parent.mat * child.mat; //this applies parent's transforms to child
    return child;
}
