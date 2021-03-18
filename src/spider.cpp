#include "spider.hpp"

//does not have a mesh, but a set of entities
entt::entity  Spider::createSpider() {
    auto entity = registry.create();

    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.scale = { 40, 40 };
    motion.position = { 400,100 };
    motion.boundingbox = motion.scale;

    //create entities/parts to be part of the kinematic chains -- requires setting position offset, pivot/origin of rotation, and intial angle
    auto body = Rig::createPart(entity, "face");

    auto L_upper_leg = Rig::createPart(entity,"arm_simple", vec2(0.5f, 0), vec2(0, 0.5f), 3.14f); // position, origin, angle
    auto L_lower_leg = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f), 3.14f);

    auto R_upper_leg = Rig::createPart(entity, "arm_simple", vec2(-0.5f, 0), vec2(0, 0.5f), 3.14f);
    auto R_lower_leg = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f), 3.14f);

    //create a component <Rig> to then point to these entities for later
    auto& rig = registry.emplace<Rig>(entity);
    rig.chains.push_back({ body });
    rig.chains.push_back({ L_upper_leg, L_lower_leg }); // added by the chain, leaf node last
    rig.chains.push_back({ R_upper_leg, R_lower_leg });

 
    // timeline holds a 'pointer' to the current frame and all the frame data.
    auto& timeline = registry.emplace<Timeline>(entity);
    timeline.frame.push_back(Frame({ 0.0f, 3.0f, 3.0f, 3.0f, 3.0f }));
    timeline.frame.push_back(Frame({ 0.0f, 3.14f, 3.14f, 3.14f, 3.26f }));
    timeline.frame.push_back(Frame({ 0.0f, 3.14f, 2.0f, 1.0f, 1.0f }));

    return entity;
}