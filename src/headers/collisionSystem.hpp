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
#include <limits>

#include "raylib.h"
#include "player.hpp"
#include "enemyData.hpp"
#include "projectileData.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

class CollisionSystem
{
	public:
		CollisionSystem(const float map_width, const float map_height);
		~CollisionSystem() = default;

		void Reset();

		void Update(
				MessageSystem& message_system, const ModifierSystem& modifier_system, const std::vector<Vector2>& enemy_centre,
				const std::vector<float>& enemy_health, const std::vector<EnemyAttackComponent>& enemy_attack_components,
				const std::vector<EnemyType>& enemy_type, const std::vector<Rectangle>& projectile_rect,
				const std::vector<ProjectileType>& projectile_type, const std::vector<Vector2>& projectile_direction,
				const std::vector<Vector2>& item_centre, const Player& player, const size_t ticks
				) noexcept;

	private:
		void PollSignals(
				MessageSystem& message_system, const Vector2 player_centre,
				const ModifierSystem& modifier_system, const size_t ticks
				) const noexcept;

		void ProjectileCollision(
				const std::vector<Rectangle>& projectile_rect, const std::vector<ProjectileType>& projectile_type,
				const std::vector<Vector2>& projectile_direction, MessageSystem& message_system,
				const ModifierSystem& modifier_system, const size_t ticks
				) const noexcept;

		void LeAttack(
				const std::vector<EnemyAttackComponent>& enemy_attack_components,
				const std::vector<EnemyType>& enemy_type, const Vector2 player_centre,
				MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
				) const noexcept;

		void SlideAttack(
				const Vector2 player_centre, const Vector2 player_direction,
				const std::vector<float>& enemy_health, MessageSystem& message_system,
				const size_t ticks
				) const noexcept;

		void ItemCollision(const Vector2 player_centre, MessageSystem& message_system) const noexcept;
		void EnemyItemCollision(MessageSystem& message_system, const std::vector<Vector2>& enemy_centre) const noexcept;

		void UpdateEnemyGrid(const std::vector<Vector2>& enemy_centre) noexcept;
		void UpdateItemGrid(const std::vector<Vector2>& item_centre) noexcept;

		size_t GetMortonCode(const float x, const float y) const noexcept;
		inline uint16_t SeparateBits(uint16_t bits) const noexcept;

		void ApplyAussie(MessageSystem& message_system,	const size_t ticks) const noexcept;
		void ApplyDrunk(MessageSystem& message_system, const size_t ticks) const noexcept;
		void ApplyPoison(MessageSystem& message_system, const size_t ticks) const noexcept;

		using LeAttackHook = void(CollisionSystem::*)(MessageSystem&, const size_t) const noexcept;
		static constexpr std::array<LeAttackHook, static_cast<size_t>(EnemyType::COUNT)> LeAttackHooks =
		{
			&CollisionSystem::ApplyAussie,
			&CollisionSystem::ApplyDrunk,
			nullptr,
			&CollisionSystem::ApplyPoison
		};
		
		void Aura(
				const Vector2 player_centre, const ModifierSystem& modifier_system,
				MessageSystem& message_system, const size_t ticks
				) const noexcept;

		using SignalHandler = void(CollisionSystem::*)(const Vector2, const ModifierSystem&, MessageSystem&, const size_t) const noexcept;

		static constexpr std::array<SignalHandler, static_cast<size_t>(CollisionSystemSignal::COUNT)> SignalHandlers =
		{
			&CollisionSystem::Aura
		};

		std::vector<size_t> EnemyGrid;
		std::vector<size_t> ItemGrid;

		const size_t GridSize;
		static constexpr size_t EmptyCell = std::numeric_limits<size_t>::max();
};

