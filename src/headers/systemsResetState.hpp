#pragma once

#include <memory>

#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "projectileSystem.hpp"
#include "enemySystem.hpp"
#include "statSystem.hpp"
#include "xpSystem.hpp"
#include "collisionSystem.hpp"

class SystemsResetState
{
	public:
		SystemsResetState(
			std::shared_ptr<struct MessageSystem> message_system, std::shared_ptr<class TimerSystem> timer_system,
			std::shared_ptr<class ModifierSystem> modifier_system, std::shared_ptr<class ParticleSystem> particle_system,
			std::shared_ptr<class ProjectileSystem> projectile_system, std::shared_ptr<class EnemySystem> enemy_system,
			std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<class XpSystem> xp_system,
			std::shared_ptr<class CollisionSystem> collision_system
			);

		~SystemsResetState() = default;

		void Reset();

	private:
		const std::shared_ptr<struct MessageSystem> MessageSystem;
		const std::shared_ptr<class TimerSystem> TimerSystem;
		const std::shared_ptr<class ModifierSystem> ModifierSystem;
		const std::shared_ptr<class ParticleSystem> ParticleSystem;
		const std::shared_ptr<class ProjectileSystem> ProjectileSystem;
		const std::shared_ptr<class EnemySystem> EnemySystem;
		const std::shared_ptr<class StatSystem> StatSystem;
		const std::shared_ptr<class XpSystem> XpSystem;
		const std::shared_ptr<class CollisionSystem> CollisionSystem;
};
