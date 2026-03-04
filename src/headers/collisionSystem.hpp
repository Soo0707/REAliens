#pragma once

#include <vector>
#include <array>
#include <cstddef>

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

		void Update(
				MessageSystem& message_system, const ModifierSystem& modifier_system, const std::vector<Rectangle>& enemy_rect,
				const std::vector<float>& enemy_health, const std::vector<EnemyAttackComponent>& enemy_attack_components,
				const std::vector<EnemyType>& enemy_type, const std::vector<Rectangle>& projectile_rect,
				const std::vector<ProjectileType>& projectile_type, const std::vector<Vector2>& projectile_direction,
				const std::vector<Rectangle>& xp_rect, const Player& player, const size_t ticks
				) noexcept;

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

		void XpCollision(
				const Rectangle& player_rect, const std::vector<Rectangle>& xp_rect,
				size_t* collected_xp, const ModifierSystem& modifier_system,
				MessageSystem& message_system
				) const noexcept;

		void UpdateEnemyGrid(const std::vector<Rectangle>& enemy_rect) noexcept;

	private:
		size_t GetMortonCode(const float x, const float y) const noexcept;
		inline uint16_t SeparateBits(uint16_t bits) const noexcept;

		void ApplyAussie(MessageSystem& message_system,	const size_t ticks) const noexcept;
		void ApplyDrunk(MessageSystem& message_system, const size_t ticks) const noexcept;
		void ApplyNone(MessageSystem& message_system, const size_t ticks) const noexcept;
		void ApplyPoison(MessageSystem& message_system, const size_t ticks) const noexcept;
		void ApplyTrapped(MessageSystem& message_system, const size_t ticks) const noexcept;

		using LeAttackHook = void(CollisionSystem::*)(MessageSystem&, const size_t) const noexcept;
		static constexpr std::array<LeAttackHook, static_cast<size_t>(EnemyType::COUNT)> LeAttackHooks =
		{
			&ApplyAussie,
			&ApplyDrunk,
			&ApplyNone,
			&ApplyPoison
		};
		
		void Aura(
				const Vector2 player_centre, const ModifierSystem& modifier_system,
				MessageSystem& message_system, const size_t ticks
				) const noexcept;

		using SignalHandler = void(CollisionSystem::*)(const Vector2, const ModifierSystem&, MessageSystem&, const size_t) const noexcept;

		static constexpr std::array<SignalHandler, static_cast<size_t>(CollisionSystemSignal::COUNT)> SignalHandlers =
		{
			&Aura
		};

		std::vector<int16_t> EnemyGrid;
		const size_t GridSize;
};

