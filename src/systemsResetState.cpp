/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "systemsResetState.hpp"

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

SystemsResetState::SystemsResetState(
		std::shared_ptr<struct MessageSystem> message_system, std::shared_ptr<class TimerSystem> timer_system,
		std::shared_ptr<class ModifierSystem> modifier_system, std::shared_ptr<class ParticleSystem> particle_system,
		std::shared_ptr<class ProjectileSystem> projectile_system, std::shared_ptr<class EnemySystem> enemy_system,
		std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<class XpSystem> xp_system,
		std::shared_ptr<class CollisionSystem> collision_system
		) :
	MessageSystem(message_system),
	TimerSystem(timer_system),
	ModifierSystem(modifier_system),
	ParticleSystem(particle_system),
	ProjectileSystem(projectile_system),
	EnemySystem(enemy_system),
	StatSystem(stat_system),
	XpSystem(xp_system),
	CollisionSystem(collision_system)
{}

void SystemsResetState::Reset()
{
	this->MessageSystem->Reset();
	this->TimerSystem->Reset();
	this->ModifierSystem->Reset();
	this->ParticleSystem->Reset();
	this->ProjectileSystem->Reset();
	this->EnemySystem->Reset();
	this->StatSystem->Reset();
	this->XpSystem->Reset();
	this->CollisionSystem->Reset();

	//game.Reset();
}
