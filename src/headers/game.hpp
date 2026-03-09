#pragma once

#include <memory>
#include <cstdint>
#include <vector>

#include "raylib.h"

#include "assetManager.hpp"
#include "settingsManager.hpp"
#include "globalDataWrapper.hpp"

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
			std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
			std::shared_ptr<SettingsManager> settings, std::shared_ptr<struct MessageSystem> message_system,
			std::shared_ptr<class TimerSystem> timer_system, std::shared_ptr<class ModifierSystem> modifier_system,
			std::shared_ptr<class ParticleSystem> particle_system, std::shared_ptr<class ProjectileSystem> projectile_system,
			std::shared_ptr<class EnemySystem> enemy_system, std::shared_ptr<class StatSystem> stat_system,
			std::shared_ptr<class XpSystem> xp_system, std::shared_ptr<class CollisionSystem> collision_system
			);

		~Game();

		void Draw(const size_t ticks, const RenderTexture2D& canvas) const noexcept;
		void HandleInput() noexcept;
		void Update(const size_t ticks) noexcept;

		void Reset() noexcept;

		RenderTexture2D LightingLayer;
		RenderTexture2D GameLayer;

		const std::shared_ptr<GlobalDataWrapper> GlobalData;
		const std::shared_ptr<AssetManager> Assets;
		const std::shared_ptr<SettingsManager> Settings;

		const std::shared_ptr<struct MessageSystem> MessageSystem;
		const std::shared_ptr<class TimerSystem> TimerSystem;
		const std::shared_ptr<class ModifierSystem> ModifierSystem;
		const std::shared_ptr<class ParticleSystem> ParticleSystem;
		const std::shared_ptr<class ProjectileSystem> ProjectileSystem;
		const std::shared_ptr<class EnemySystem> EnemySystem;
		const std::shared_ptr<class StatSystem> StatSystem;
		const std::shared_ptr<class XpSystem> XpSystem;
		const std::shared_ptr<class CollisionSystem> CollisionSystem;
		const std::unique_ptr<class Player> Player;

		Camera2D Camera;
		Rectangle UpdateArea;

		std::array<bool, static_cast<size_t>(Action::COUNT)> CanPerform;

	private:
		void PollSignals() noexcept;

		void UpdateCamera() noexcept;

		float Accumulator = 0.0f;
};
