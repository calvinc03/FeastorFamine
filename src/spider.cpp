// Header
#include "render.hpp"
#include "spider.hpp"
#include "mob.hpp"

// this function creates a hierarchy through accumulating transforms.
void Spider::update_rigs() {
    auto view_parts = registry.view<Spider, Motion>();
    for (auto [entity,spider, motion] : view_parts.each()) { // motion == root

        auto& body_motion = registry.get<Motion>(spider.body);
        auto& L_upper_leg_motion = registry.get<Motion>(spider.L_upper_leg);
        auto& L_lower_leg_motion = registry.get<Motion>(spider.L_lower_leg);
        

        //test code
        L_upper_leg_motion.position = vec2(50, 0); //offset should be an argument for part.
        L_lower_leg_motion.angle += 0.1f; // keyframes are an array of angles. 
        L_upper_leg_motion.angle += 0.2f;

        // create kinematic chain via transforms
        auto& body_transform = registry.get<Transform>(spider.body);
        auto& L_upper_leg_transform = registry.get<Transform>(spider.L_upper_leg);
        auto& L_lower_leg_transform = registry.get<Transform>(spider.L_lower_leg);

        body_transform.mat = glm::mat3(1.0);
        body_transform.translate(motion.position); //Note: can't have a body offset doing this.
        body_transform.rotate(motion.angle);
        
        L_upper_leg_transform.mat = glm::mat3(1.0);
        L_upper_leg_transform.translate(L_upper_leg_motion.position);
        L_upper_leg_transform.translate(vec2(0, 70)); //TODO: these magic numbers are the pivot of rotation. should scale with motion.scale.
        L_upper_leg_transform.rotate(L_upper_leg_motion.angle);
        L_upper_leg_transform.translate(vec2(0, -70));
        L_upper_leg_transform.mat = body_transform.mat * L_upper_leg_transform.mat; //this applies parent's transforms to child

        L_lower_leg_transform.mat = glm::mat3(1.0);
        L_lower_leg_transform.translate(L_lower_leg_motion.position);
        L_lower_leg_transform.translate(vec2(0, -70));
        L_lower_leg_transform.rotate(L_lower_leg_motion.angle);
        L_lower_leg_transform.translate(vec2(0, 70));
        L_lower_leg_transform.mat =  L_upper_leg_transform.mat * L_lower_leg_transform.mat;

        //must adjust scale after!!
        body_transform.scale(motion.scale*body_motion.scale); //TODO, make sure this makes sense
        L_upper_leg_transform.scale(motion.scale * L_upper_leg_motion.scale);
        L_lower_leg_transform.scale(motion.scale * L_lower_leg_motion.scale);

    }
}

//does not have a mesh, but a set of entities
entt::entity  Spider::createSpider() {
    auto entity = registry.create();

    auto body = createSpiderPart("body");
    auto L_upper_leg = createSpiderPart("rect7", { 50,0 });
    auto L_lower_leg = createSpiderPart("rect7");

    auto& spider = registry.emplace<Spider>(entity);
    spider.body = body;
    spider.L_lower_leg = L_lower_leg;
    spider.L_upper_leg = L_upper_leg;

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
