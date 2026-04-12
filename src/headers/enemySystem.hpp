/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
#include "modifierSystem.hpp"

class EnemySystem
{
	public:
		EnemySystem();
		~EnemySystem() = default;

		void Reset() noexcept;

		void Update(
				MessageSystem& message_system, const AssetManager& assets, const ModifierSystem& modifier_system,
				const TimerSystem& timer_system, const Rectangle& update_area,
				const Vector2 player_centre, const size_t ticks, const size_t level
				) noexcept;

		void Draw(const AssetManager& assets) const noexcept;

		const std::vector<Vector2>& GetEnemyCentre() const noexcept;
		const std::vector<EnemyAttackComponent>& GetEnemyAttackComponents() const noexcept;

		const std::vector<EnemyType>& GetEnemyType() const noexcept;
		const std::vector<float>& GetEnemyHealth() const noexcept;

	private:
		void PollSignals(MessageSystem& message_system, const AssetManager& assets, const size_t level, const size_t ticks) noexcept;
		void ExecuteCommands(MessageSystem& message_system, const ModifierSystem& modifier_system) noexcept;

		static constexpr std::array<EnemyData, static_cast<size_t>(EnemyType::COUNT)> EnemyAttributes = 
		{
			(EnemyData) { TextureKey::Australian, 5.0f, 87.5f, 75.0f, 30 },
			(EnemyData) { TextureKey::Drunkard, 5.0f, 125.0f, 50.0f, 15 },
			(EnemyData) { TextureKey::Pleb, 5.0f, 75.0f, 75.0f, 30 },
			(EnemyData) { TextureKey::Poison, 5.0f, 62.5f, 100.0f, 30 },
			(EnemyData) { TextureKey::Masochist, 5.0f, 90.0f, 90.0f, 20 },
			(EnemyData) { TextureKey::Orange, 10.0f, 62.5f, 150.0f, 30 },
			(EnemyData) { TextureKey::Tyrone, 15.0f, 50.0f, 250.0f, 50 }
		};

		void GenerateLocations(const size_t spawn_count, const float map_width, const float map_height) noexcept;
		void GenerateTypes(const size_t spawn_count) noexcept;

		void PrepareSpawnEnemies(const size_t level, const float map_width, const float map_height) noexcept;


		void ApplyAussie(MessageSystem& message_system) const noexcept;
		void ApplyDrunk(MessageSystem& message_system) const noexcept;
		void ApplyPoison(MessageSystem& message_system) const noexcept;
		void ApplyTariffs(MessageSystem& message_system) const noexcept;

		using LeAttackHook = void(EnemySystem::*)(MessageSystem&) const noexcept;
		static constexpr std::array<LeAttackHook, static_cast<size_t>(EnemyType::COUNT)> LeAttackHooks =
		{
			&EnemySystem::ApplyAussie,
			&EnemySystem::ApplyDrunk,
			nullptr,
			&EnemySystem::ApplyPoison,
			nullptr,
			&EnemySystem::ApplyTariffs,
			nullptr
		};


		void EmitParticlesFromLocations(const size_t ticks, const size_t level, MessageSystem& message_system) noexcept;
		void SpawnEnemies(const size_t ticks, const size_t level, MessageSystem& message_system) noexcept;

		using SignalHandler = void(EnemySystem::*)(const size_t, const size_t, MessageSystem&) noexcept;
		static constexpr std::array<SignalHandler, static_cast<size_t>(EnemySystemSignal::COUNT)> SignalHandlers =
		{
			&EnemySystem::EmitParticlesFromLocations,
			&EnemySystem::SpawnEnemies
		};


		void DamageEnemyHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept;
		void EnemyLeAttackedHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept;
		void EnemyGotGluedHandler(MessageSystem& message_system, const ModifierSystem& modifier_system, const EnemySystemCommand& command) noexcept;

		using CommandHandler = void(EnemySystem::*)(MessageSystem&, const ModifierSystem&, const EnemySystemCommand&) noexcept;
		static constexpr std::array<CommandHandler, 3> CommandHandlers = 
		{
			&EnemySystem::DamageEnemyHandler,
			&EnemySystem::EnemyLeAttackedHandler,
			&EnemySystem::EnemyGotGluedHandler
		};


		// animation_frames = number of actual frames -1 since it's 0 indexed
		static constexpr uint8_t AnimationFrames = 2;

		void CreateEnemy(const float x, const float y, const float level_scale, const EnemyType type) noexcept;
		void KillEnemies(MessageSystem& message_system, const bool has_magnetism) noexcept;

		void VisibilityCheck(const Rectangle& update_area) noexcept;
		void MoveEnemies(const float map_width, const float map_height) noexcept;
		void EnemiesSetDirection(const Vector2 player_centre, const bool is_stinky) noexcept;
		void AnimateEnemies(const size_t ticks) noexcept;
		void EnemiesUpdateTimers(const size_t ticks) noexcept;

		bool CheckIndex(const size_t index) const noexcept;

		std::vector<float> EnemyHealth;
		std::vector<float> EnemySpeed;
		std::vector<uint8_t> EnemyIsVisible;
		std::vector<EnemyEffect> EnemyEffects;
		std::vector<Vector2> EnemyDirection;
		std::vector<Rectangle> EnemyRect;
		std::vector<Vector2> EnemyCentre;
		std::vector<EnemyType> EnemyTypes;
		std::vector<EnemyAttackComponent> EnemyAttackComponents;
		std::vector<TextureKey> EnemyTextureKey;
		std::vector<EnemyAnimationComponent> EnemyAnimationComponents;

		std::vector<EnemyType> FutureEnemyTypes;
		std::vector<Vector2> FutureSpawnLocations;
};
