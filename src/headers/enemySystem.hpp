#pragma once

#include <vector>
#include <array>
#include <cstddef>
#include <cstdint>

#include "raylib.h"
#include "enemyData.hpp"
#include "constants.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"
#include "assetManager.hpp"

class EnemySystem
{
	public:
		EnemySystem();
		~EnemySystem() = default;

		void Reset() noexcept;

		void PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level, const size_t ticks) noexcept;
		void ExecuteCommands(MessageSystem& message_system) noexcept;

		void UpdateEnemies(
				const size_t ticks, const Rectangle update_area, const Vector2 player_centre, const float map_width, const float map_height,
				const bool is_stinky, const size_t level, MessageSystem& message_system, const TimerSystem& timer_system
				) noexcept;

		void Draw(const AssetManager& assets) const noexcept;

		const std::vector<Rectangle>& GetEnemyRect() const noexcept;
		const std::vector<EnemyAttackComponent>& GetEnemyAttackComponents() const noexcept;

		const std::vector<EnemyType>& GetEnemyType() const noexcept;
		const std::vector<float>& GetEnemyHealth() const noexcept;

	private:
		static constexpr std::array<EnemyData, static_cast<size_t>(EnemyType::COUNT)> EnemyAttributes = 
		{
			(EnemyData) { TextureKey::Australian, 5, 87.5, 75, 30, 2 },
			(EnemyData) { TextureKey::Drunkard, 5, 125, 50, 15,	2 },
			(EnemyData) { TextureKey::Pleb, 5, 75,	75,	30,	2 },
			(EnemyData) { TextureKey::Poison, 5, 62.5, 100, 30, 2 },
		};

		void GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept;
		void GenerateTypes(const size_t spawn_count) noexcept;

		void PrepareSpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept;

		void EmitParticlesFromLocations(const size_t ticks, const size_t level, MessageSystem& message_system) noexcept;
		void SpawnEnemies(const size_t ticks, const size_t level, MessageSystem& message_system) noexcept;

		static constexpr std::array<void(EnemySystem::*)(const size_t, const size_t, MessageSystem&) noexcept, static_cast<size_t>(EnemySystemSignal::COUNT)> SignalHandlers =
		{
			&EmitParticlesFromLocations,
			&SpawnEnemies
		};

		void DamageEnemyHandler(const EnemySystemCommand& command) noexcept;
		void EnemyLeAttackedHandler(const EnemySystemCommand& command) noexcept;

		static constexpr std::array<void(EnemySystem::*)(const EnemySystemCommand&) noexcept, 2> CommandHandlers = 
		{
			&DamageEnemyHandler,
			&EnemyLeAttackedHandler
		};

		static constexpr std::array<uint8_t, static_cast<size_t>(EnemyType::COUNT)> EnemyXpValues = 
		{
			1, 2, 1, 2
		};

		void CreateEnemy(const float x, const float y, const float level_scale, const EnemyType type) noexcept;
		void KillEnemies(MessageSystem& message_system) noexcept;

		void VisibilityCheck(const Rectangle update_area) noexcept;
		void MoveEnemies(const float map_width, const float map_height) noexcept;
		void EnemiesSetDirection(const Vector2 player_centre, const bool is_stinky) noexcept;
		void AnimateEnemies(const size_t ticks) noexcept;
		void EnemiesUpdateTimers(const size_t ticks) noexcept;

		bool CheckIndex(const size_t index) const noexcept;

		std::vector<float> EnemyHealth;
		std::vector<float> EnemySpeed;

		std::vector<uint8_t> EnemyIsVisible;
		std::vector<Vector2> EnemyDirection;
		std::vector<Rectangle> EnemyRect;
		std::vector<EnemyType> EnemyTypes;
		std::vector<EnemyAttackComponent> EnemyAttackComponents;
		std::vector<TextureKey> EnemyTextureKey;
		std::vector<EnemyAnimationComponent> EnemyAnimationComponents;


		std::vector<EnemyType> FutureEnemyTypes;
		std::vector<Vector2> FutureSpawnLocations;
};
