#pragma once

#include <vector>
#include <array>
#include <cstddef>

#include "raylib.h"
#include "enemyData.hpp"
#include "projectileData.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

class CollisionSystem
{
	public:
		CollisionSystem() = default;
		~CollisionSystem() = default;

		void PollSignals(
				MessageSystem& message_system, const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre,
				const ModifierSystem& modifier_system, const size_t ticks
				) const noexcept;

		void ProjectileCollision(
				const std::vector<Rectangle>& projectile_rect, const std::vector<ProjectileType>& projectile_type,
				const std::vector<Vector2>& projectile_direction, const std::vector<Rectangle>& enemy_rect,
				MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
				) const noexcept;

		void LeAttack(
				const std::vector<Rectangle>& enemy_rect, const std::vector<EnemyAttackComponent>& enemy_attack_components,
				const std::vector<EnemyType>& enemy_type, const Vector2 player_centre, const float player_radius,
				MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
				) const noexcept;

		void SlideAttack(
				const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre, const float player_radius,
				const Vector2 player_direction,	const std::vector<float>& enemy_health, MessageSystem& message_system,
				const size_t ticks
				) const noexcept;

		void XpCollision(
				const Rectangle player_rect, const std::vector<Rectangle>& xp_rect,
				const std::vector<uint8_t>& xp_value, size_t* collected_xp,
				const ModifierSystem& modifier_system, MessageSystem& message_system
				) const noexcept;

	private:
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
				const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre, const ModifierSystem& modifier_system,
				MessageSystem& message_system, const size_t ticks
				) const noexcept;

		using SignalHandler = void(CollisionSystem::*)(const std::vector<Rectangle>&, const Vector2, const ModifierSystem&, MessageSystem&, const size_t) const noexcept;

		static constexpr std::array<SignalHandler, static_cast<size_t>(CollisionSystemSignal::COUNT)> SignalHandlers =
		{
			&Aura
		};
};

