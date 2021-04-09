#include "dragon_rig.hpp"


const std::string DRAGON_HEAD = "monsters/dragon_rig/head.png";
const std::string DRAGON_MOUTH = "monsters/dragon_rig/jaw.png";
const std::string DRAGON_BODY = "monsters/dragon_rig/body.png";
const std::string DRAGON_NECK = "monsters/dragon_rig/neck.png";
const std::string DRAGON_OUTERPAW= "monsters/dragon_rig/outerpaw.png";
const std::string DRAGON_OUTERWING = "monsters/dragon_rig/outerwing.png";
const std::string DRAGON_INNERWING = "monsters/dragon_rig/innerwing.png";
static void add_frames_FK(Rig rig, FK_Animations& fk_animations);
static void add_attack(Rig rig, FK_Animations& fk_animations);

//does not have a mesh, but a set of entities
entt::entity  DragonRig::createDragon() {
    auto entity = registry.create();

    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(vec2(0, 0));
    motion.scale = vec2(150, 150);
    motion.position = coord_to_pixel(DRAGON_COORD) + vec2(200, 0); // vec2(100, 500);// coord_to_pixel(FOREST_COORD);
    motion.boundingbox = motion.scale * 2.0f;

    //create entities/parts to be part of the kinematic chains -- requires setting position offset, pivot/origin of rotation, and intial angle
    auto body = Rig::createPart(entity, "face_box", vec2(), vec2(),0);

    auto neck = Rig::createPart(entity, "arm_simple", vec2(0.5f,0.2f), vec2(0, -0.5), 3.14/2.0f);
    auto head = Rig::createPart(entity, "arm_simple", vec2(-0.1f, 0), vec2(0, 0.5), 3.14f);

    auto wing = Rig::createPart(entity, "arm_simple", vec2(0,0), vec2(0, -0.5), 3.14f); // position offset, origin, angle

    auto outer_arm = Rig::createPart(entity, "arm_simple", vec2(0,0.8), vec2(0, -0.5f), 0);

    auto mouth = Rig::createPart(entity, "arm_simple", vec2(0,-0.1), vec2(0, 0.5f), 3.14 / 2.0f);

    //create a component <Rig> to then point to these entities for later
    auto& rig = registry.emplace<Rig>(entity);
    rig.chains.push_back(Chain(entity, { body }));
    rig.chains.push_back(Chain(body, { neck, head }));
    rig.chains.push_back(Chain(body, { wing })); 
    rig.chains.push_back(Chain(body, { outer_arm }));

    rig.chains.push_back(Chain(head, { mouth }));


    //has a current_time var used to animate fk/ik systems
    auto& timeline = registry.emplace<Timeline>(entity);

    auto& fk_animations = registry.emplace<FK_Animations>(entity);

    add_attack(rig, fk_animations);
    add_frames_FK(rig, fk_animations);

    //temporary soln
    auto head_texture= Rig::createPartTextured(head, DRAGON_HEAD, vec2(-0.2f,0.1f), -3.14/2.0f, 2.0f*vec2(1,1), 24);
    auto mouth_texture = Rig::createPartTextured(mouth, DRAGON_MOUTH, vec2(0, 0.2f), -3.14 / 2.0f, 1.5f*vec2(1, 1), 23);
    auto neck_texture = Rig::createPartTextured(neck, DRAGON_NECK, vec2(0, 0), 2.0f, 1.5f * vec2(1, 1), 21);
    auto body_texture = Rig::createPartTextured(body, DRAGON_BODY, vec2(0, 0), 0.0f, 2.0f * vec2(1, 1), 20);
    auto wing_texture = Rig::createPartTextured(wing, DRAGON_OUTERWING, vec2(0, 0.5f), 3.10f,2.0f* vec2(1, 1), 22);
    auto arm_texture = Rig::createPartTextured(outer_arm, DRAGON_OUTERPAW, vec2(0, 0), 1.0f, vec2(1, 1), 24);


    //auto& monster = registry.emplace<Monster>(entity);
    //monster.max_health = 5000;
    //monster.health = monster.max_health;
    //monster.damage = 0;
    //monster.reward = 10000;

    registry.emplace<DragonRig>(entity);
    registry.emplace<HitReaction>(entity);

    auto& transform = registry.emplace<Transform>(entity);
    transform.mat = glm::mat3(1.0);

    RigSystem::update_rig(entity);

    RopeRig::createRope(neck, 10, vec2(100, 0));
    
    return entity;
}

void add_frames_FK(Rig rig, FK_Animations& fk_animations) {

    //neck
    Joint_angles j0;
    j0.data.emplace(0.0f, -2.3);
    j0.data.emplace(1.0f, -2.3f);
    j0.data.emplace(2.0f, -2.3f);
    j0.data.emplace(5.0f, -2.3f);

    //head
    Joint_angles j1;
    j1.data.emplace(0.0f, 3.9f);
    j1.data.emplace(1.0f, 3.8f);
    j1.data.emplace(2.0f, 3.5f);
    j1.data.emplace(4.0f, 3.8f);

    //wing
    Joint_angles j2;
    j2.data.emplace(0.0f, -3.6f);
    j2.data.emplace(1.0f, -3.1f);
    j2.data.emplace(2.0f, -3.3f);
    j2.data.emplace(4.0f, -3.5f);

    //arm
    Joint_angles j3;
    j3.data.emplace(0.0f, -0.3f);
    j3.data.emplace(1.0f, -0.3f);
    j3.data.emplace(2.0f, -0.3f);
    j3.data.emplace(4.0f, -0.5f);

    //mouth -- 0 = close, 1 == open
    Joint_angles j4;
    j4.data.emplace(0.0f, 0.3f);
    j4.data.emplace(1.0f, 0.3f);
    j4.data.emplace(2.0f, 0.3f);
    j4.data.emplace(4.0f, 0.5f);

    Keyframes_FK kfs;
    kfs.anim.emplace(rig.chains[1].chain_vector[0], j0);
    kfs.anim.emplace(rig.chains[1].chain_vector[1], j1);
    kfs.anim.emplace(rig.chains[2].chain_vector[0], j2);
    kfs.anim.emplace(rig.chains[3].chain_vector[0], j3);
    kfs.anim.emplace(rig.chains[4].chain_vector[0], j4);


    fk_animations.anims.push_back(kfs);
}


void add_attack(Rig rig, FK_Animations& fk_animations) {

    //neck
    Joint_angles j0;
    j0.data.emplace(0.0f, -2.3);
    j0.data.emplace(1.0f, -2.3f);
    j0.data.emplace(1.8f, -2.0f);
    j0.data.emplace(2.0f, -1.8f);
    j0.data.emplace(2.5f, -2.0f);
    j0.data.emplace(5.0f, -2.3f);

    //head
    Joint_angles j1;
    j1.data.emplace(0.0f, 3.9f);
    j1.data.emplace(1.0f, 3.8f);
    j1.data.emplace(1.8f, 3.5f);
    j1.data.emplace(2.0f, 3.0f);
    j1.data.emplace(2.5f, 3.5f);
    j1.data.emplace(4.0f, 3.8f);

    //wing
    Joint_angles j2;
    j2.data.emplace(0.0f, -3.6f);
    j2.data.emplace(1.0f, -3.1f);
    j2.data.emplace(2.0f, -3.3f);
    j2.data.emplace(4.0f, -3.5f);

    //arm
    Joint_angles j3;
    j3.data.emplace(0.0f, -0.3f);
    j3.data.emplace(1.0f, -0.3f);
    j3.data.emplace(2.0f, -0.3f);
    j3.data.emplace(4.0f, -0.5f);

    //mouth -- 0 = close, 1 == open
    Joint_angles j4;
    j4.data.emplace(0.0f, 0.2f);
    j4.data.emplace(1.0f, 0.0f);
    j4.data.emplace(1.8f, 0.1f);
    j4.data.emplace(2.0f, 0.5f);
    j4.data.emplace(2.6f, 0.4f);
    j4.data.emplace(3.0f, 0.0f);
    j4.data.emplace(4.0f, 0.0f);

    Keyframes_FK kfs;
    kfs.anim.emplace(rig.chains[1].chain_vector[0], j0);
    kfs.anim.emplace(rig.chains[1].chain_vector[1], j1);
    kfs.anim.emplace(rig.chains[2].chain_vector[0], j2);
    kfs.anim.emplace(rig.chains[3].chain_vector[0], j3);
    kfs.anim.emplace(rig.chains[4].chain_vector[0], j4);


    fk_animations.anims.push_back(kfs);
}