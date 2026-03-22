/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <cstdint>
#include <vector>

#include "raylib.h"

#include "assetManager.hpp"
#include "settingsManager.hpp"
#include "stringCache.hpp"

#include "player.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "projectileSystem.hpp"
#include "enemySystem.hpp"
#include "statSystem.hpp"
#include "xpSystem.hpp"
#include "collisionSystem.hpp"

enum class Action : uint8_t
{
	LMB,
	RMB,
	Slide,

	COUNT
};

class Game
{
	public:
		Game(
			std::shared_ptr<struct StringCache> string_cache, std::shared_ptr<class AssetManager> assets,
			std::shared_ptr<class SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
			std::shared_ptr<class TimerSystem> timer_system, std::shared_ptr<class ModifierSystem> modifier_system,
			std::shared_ptr<class ParticleSystem> particle_system, std::shared_ptr<class ProjectileSystem> projectile_system,
			std::shared_ptr<class EnemySystem> enemy_system, std::shared_ptr<class StatSystem> stat_system,
			std::shared_ptr<class XpSystem> xp_system, std::shared_ptr<class CollisionSystem> collision_system,
			std::shared_ptr<class Player> player
			);

		~Game();

		void Draw(const size_t ticks, const RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;
		void Update(const size_t ticks) noexcept;

		void Reset() noexcept;

		RenderTexture2D LightingLayer;
		RenderTexture2D GameLayer;

		const std::shared_ptr<struct StringCache> StringCache;
		const std::shared_ptr<class AssetManager> Assets;
		const std::shared_ptr<class SettingsManager> Settings;

		const std::shared_ptr<struct MessageSystem> MessageSystem;
		const std::shared_ptr<class TimerSystem> TimerSystem;
		const std::shared_ptr<class ModifierSystem> ModifierSystem;
		const std::shared_ptr<class ParticleSystem> ParticleSystem;
		const std::shared_ptr<class ProjectileSystem> ProjectileSystem;
		const std::shared_ptr<class EnemySystem> EnemySystem;
		const std::shared_ptr<class StatSystem> StatSystem;
		const std::shared_ptr<class XpSystem> XpSystem;
		const std::shared_ptr<class CollisionSystem> CollisionSystem;
		const std::shared_ptr<class Player> Player;

		Camera2D Camera;
		Rectangle UpdateArea;

		std::array<bool, static_cast<size_t>(Action::COUNT)> CanPerform;

	private:
		void PollSignals() noexcept;

		void UpdateCamera() noexcept;

		float Accumulator = 0.0f;
};
