#pragma once

#include <memory>
#include <cstdint>

#include "raylib.h"

#include "assetManager.hpp"
#include "settingsManager.hpp"
#include "globalDataWrapper.hpp"

#include "player.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "modifierSystem.hpp"
#include "particleSystem.hpp"
#include "gameTextSystem.hpp"
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
			std::shared_ptr<class ParticleSystem> particle_system, std::shared_ptr<class GameTextSystem> game_text_system,
			std::shared_ptr<class ProjectileSystem> projectile_system, std::shared_ptr<class EnemySystem> enemy_system,
			std::shared_ptr<class StatSystem> stat_system, std::shared_ptr<class XpSystem> xp_system,
			std::shared_ptr<class CollisionSystem> collision_system
			);

		~Game();

		void Draw(RenderTexture2D& canvas) const noexcept;
		void TickedUpdate() noexcept;
		void Update() noexcept;

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
		const std::shared_ptr<class GameTextSystem> GameTextSystem;
		const std::shared_ptr<class ProjectileSystem> ProjectileSystem;
		const std::shared_ptr<class EnemySystem> EnemySystem;
		const std::shared_ptr<class StatSystem> StatSystem;
		const std::shared_ptr<class XpSystem> XpSystem;
		const std::shared_ptr<class CollisionSystem> CollisionSystem;

		Camera2D Camera;
		Rectangle UpdateArea;

		const std::unique_ptr<class Player> Player;
		
		std::array<bool, static_cast<size_t>(Action::COUNT)> CanPerform;
		std::array<size_t, static_cast<size_t>(Action::COUNT)> LastPerformed;

		size_t Ticks = 0;

		size_t Level = 1;
		size_t CollectedXp = 0;
		size_t LevelUpThreshold = 5;

	private:
		void UpdatePlayer(const size_t ticks) noexcept;
		void UpdateTimerSystem(const size_t ticks) noexcept;
		void UpdateModifierSystem() noexcept;
		void UpdateParticleSystem(const size_t ticks, const Rectangle update_area) noexcept;
		void UpdateGameTextSystem(const size_t ticks, const Rectangle update_area) noexcept;
		void UpdateProjectileSystem(const size_t ticks, const Rectangle update_area) noexcept;
		void UpdateEnemySystem(const size_t ticks, const size_t level, const Rectangle update_area) noexcept;
		void UpdateStatSystem() noexcept;
		void UpdateXpSystem(const size_t ticks, const Rectangle update_area) noexcept;
		void UpdateCollisionSystem(const size_t ticks) noexcept;

		void LevelUp() noexcept;
		void UpdateCamera() noexcept;
		void UpdateTimeouts() noexcept;

		float Accumulator = 0.0f;
};
