#include "spider.hpp"

void add_frames_FK(Rig rig);
void add_frames_IK(KeyFrames_IK& kf_ik);

//TODO: refactor to store multiple animations. vector?

//does not have a mesh, but a set of entities
entt::entity  Spider::createSpider() {
    auto entity = registry.create();

    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(1.5f, 0)); //{ 0, 0 };
    motion.scale = vec2(20,20);
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.boundingbox = motion.scale*2.0f;

    //create entities/parts to be part of the kinematic chains -- requires setting position offset, pivot/origin of rotation, and intial angle
    auto body = Rig::createPart(entity, "face2");

    auto L_upper_leg = Rig::createPart(entity,"arm_simple", vec2(1.0f, -0.5f), vec2(0, 0.5f), 0); // position, origin, angle
    auto L_lower_leg = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f), 3.14f);

    auto R_upper_leg = Rig::createPart(entity, "arm_simple", vec2(-1.0f, -0.5f), vec2(0, 0.5f), 0);
    auto R_lower_leg = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f), 3.14f);

    //create a component <Rig> to then point to these entities for later
    auto& rig = registry.emplace<Rig>(entity);
    rig.chains.push_back({ body });
    rig.chains.push_back({ L_upper_leg, L_lower_leg }); // added by the chain, leaf node last
    rig.chains.push_back({ R_upper_leg, R_lower_leg });

    //has a current_time var used to animate fk/ik systems
    auto& timeline = registry.emplace<Timeline>(entity);

    add_frames_FK(rig); //add hardcoded angles to joints

    auto& keyframes_ik = registry.emplace<KeyFrames_IK>(entity); //FK lives in each rig part... IK lives here!
    add_frames_IK(keyframes_ik);


    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 10;
    monster.health = monster.max_health;
    monster.damage = 50;
    monster.reward = 100;
    registry.emplace<HitReaction>(entity);

    return entity;
}


//hardcoded test frames
void add_frames_FK(Rig rig) {
    auto& kfs0 = registry.get<KeyFrames_FK>(rig.chains[1][0]);
    kfs0.data.emplace(0.0f, 3.14f);
    kfs0.data.emplace(1.0f, 0.5f);
    kfs0.data.emplace(2.0f, 1.0f);
    kfs0.data.emplace(5.0f, 1.3f);

    auto& kfs1 = registry.get<KeyFrames_FK>(rig.chains[1][1]);
    kfs1.data.emplace(0.0f, 3.14f);
    kfs1.data.emplace(1.0f, 0.5f);
    kfs1.data.emplace(2.0f, 2.0f);
    kfs1.data.emplace(4.0f, 0.5f);
}

// walk cycle
void add_frames_IK(KeyFrames_IK& kf_ik) { //pos defined relative to root_motion

    float x = 1.4f;
    float low = 1.2f;
    float hi = 0.5f;

    std::map<float, vec2> body_data;
    kf_ik.data.push_back(body_data);

    std::map<float, vec2> L_data;
    L_data.emplace(0.0f, vec2(x, hi));
    L_data.emplace(0.01f, vec2(x, hi));
    L_data.emplace(0.1f, vec2(x, low));
    L_data.emplace(0.2f, vec2(x, hi));
    kf_ik.data.push_back(L_data);

    std::map<float, vec2> R_data;
    R_data.emplace(0.0f, vec2(-x, low));
    R_data.emplace(0.1f, vec2(-x, hi));
    R_data.emplace(0.11f, vec2(-x, hi));
    R_data.emplace(0.2f,  vec2(-x, low));
    kf_ik.data.push_back(R_data);
}


