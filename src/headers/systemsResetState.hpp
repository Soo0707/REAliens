/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>

#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "projectileSystem.hpp"
#include "enemySystem.hpp"
#include "statSystem.hpp"
#include "itemSystem.hpp"
#include "collisionSystem.hpp"
#include "player.hpp"
#include "cameraSystem.hpp"
#include "stringCache.hpp"

class SystemsResetState
{
	public:
		SystemsResetState(
			std::shared_ptr<struct MessageSystem> message_system, std::shared_ptr<class TimerSystem> timer_system,
			std::shared_ptr<class ModifierSystem> modifier_system, std::shared_ptr<class ParticleSystem> particle_system,
			std::shared_ptr<class ProjectileSystem> projectile_system, std::shared_ptr<class EnemySystem> enemy_system,
			std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<class ItemSystem> item_system,
			std::shared_ptr<class CollisionSystem> collision_system, std::shared_ptr<class Player> player,
			std::shared_ptr<class CameraSystem> camera_system, std::shared_ptr<struct StringCache> string_cache
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
		const std::shared_ptr<class ItemSystem> ItemSystem;
		const std::shared_ptr<class CollisionSystem> CollisionSystem;
		const std::shared_ptr<class Player> Player;
		const std::shared_ptr<class CameraSystem> CameraSystem;
		const std::shared_ptr<struct StringCache> StringCache;
};
