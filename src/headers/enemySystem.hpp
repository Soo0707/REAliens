#pragma once

#include <vector>
#include <array>
#include <cstddef>

#include "raylib.h"
#include "enemy.hpp"
#include "messageSystem.hpp"
#include "assetManager.hpp"

class EnemySystem
{
	public:
		EnemySystem();
		~EnemySystem() = default;

		void Reset() noexcept;

		void PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level) noexcept;

		void UpdateEnemies(
				const size_t ticks, const Rectangle update_area, const Vector2 player_centre,
				const float map_width, const float map_height, const bool is_stinky, MessageSystem& message_system) noexcept;

		void Draw(const Rectangle update_area) const noexcept;

	private:
		void GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept;
		void GenerateTypes(const size_t spawn_count) noexcept;
		void GenerateModifiers(const size_t spawn_count, const size_t level) noexcept;
		
		void NoModifiers(const size_t spawn_count, const size_t level) noexcept;
		void SameModifiers(const size_t spawn_count, const size_t level) noexcept;
		void RandomModifiers(const size_t spawn_count, const size_t level) noexcept;

		static constexpr std::array<void(EnemySystem::*)(const size_t, const size_t) noexcept, 3> GenerateModifierFunctions =
		{
			&NoModifiers,
			&SameModifiers,
			&RandomModifiers,
		};

		void EmitParticlesFromLocations(const size_t ticks, MessageSystem& message_system) noexcept;

		void PrepareSpawnEnemies(const size_t level, const AssetManager& assets) noexcept;
		void SpawnEnemies(const size_t level, const AssetManager& assets) noexcept;

		static constexpr std::array<void(EnemySystem::*)(const size_t, const AssetManager&), static_cast<size_t>(EnemySystemSignal::COUNT)> SignalHandlers =
		{
			&PrepareSpawnEnemies,
			&SpawnEnemies
		};
		
		std::vector<Enemy> Enemies; 
		std::vector<EnemyType> EnemyTypes;
		std::vector<BehaviourModifier> EnemyModifiers;
		std::vector<Vector2> SpawnLocations;
};
