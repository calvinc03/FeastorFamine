// internal
#include "ai.hpp"
#include "entt.hpp"
#include "common.hpp"
#include <projectile.hpp>
#include <render_components.hpp>

AISystem::AISystem(PhysicsSystem* physics) 
{
	this->physics = physics;
	this->physics->attach(this);
}

AISystem::~AISystem() {}

void AISystem::step(float elapsed_ms)
{
	//(void)elapsed_ms; // placeholder to silence unused warning until implemented
	
	// Attack mobs if in range of hunter
	for (auto monster : registry.view<Monster>()) {
		auto animal = entt::to_entity(registry, monster);
		auto& motion_m = registry.get<Motion>(animal);
		for (auto unit : registry.view<Unit>()) {
			auto hunter = entt::to_entity(registry, unit);
			auto& motion_h = registry.get<Motion>(hunter);
			auto& placeable_unit = registry.get<Unit>(hunter);

			float opposite = motion_m.position.y - motion_h.position.y;
			float adjacent = motion_m.position.x - motion_h.position.x;
			float distance = sqrt(pow(adjacent, 2) + pow(opposite, 2));

			if (distance <= placeable_unit.attack_range) {
				// TODO: scale projectile spawn with the current speed of the game 
				placeable_unit.next_projectile_spawn -= elapsed_ms;
				if (placeable_unit.next_projectile_spawn < 0.f) {
					placeable_unit.next_projectile_spawn = FIRING_RATE;
					Projectile::createProjectile(motion_h.position, vec2(adjacent, opposite) / distance, placeable_unit.damage);
					motion_h.angle = atan2(opposite, adjacent);
				}

			}
		}
	}
}

void AISystem::updateCollisions(entt::entity entity_i, entt::entity entity_j)
{
	if (registry.has<Projectile>(entity_i)) {
		if (registry.has<Boss>(entity_j))
		{
			auto& boss = registry.get<Boss>(entity_j);
			if (!boss.hit)
			{
				boss.hit = true;

				if (boss.speed_multiplier > 1)
				{
					boss.sprite = boss.run_sprite;
					boss.frames = boss.run_frames;
				}

				auto& motion = registry.get<Motion>(entity_j);
				motion.velocity *= boss.speed_multiplier;

			}
		}
		if (registry.has<Monster>(entity_j)) {
			auto& hit_reaction = registry.get<HitReaction>(entity_j);
			hit_reaction.hit_bool = true;

			// increase velocity of monsters that are hit - removed
			auto& motion = registry.get<Motion>(entity_j);
			//motion.velocity.x += motion.velocity.x > 0 ? 100.f : 0;
			//motion.velocity.y += motion.velocity.y > 0 ? 100.f : 0;
		}
	}
}
