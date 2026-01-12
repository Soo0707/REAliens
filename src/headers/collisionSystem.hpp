#pragma once

#include <vector>
#include <array>
#include <cstddef>

#include "enemyData.hpp"
#include "projectileData.hpp"

#include "messageSystem.hpp"
#include "modifierSystem.hpp"

class CollisionSystem
{
	public:
		CollisionSystem() = default;
		~CollisionSystem() = default;

		void ProjectileCollision(
				const std::vector<Rectangle>& projectile_rect, const std::vector<ProjectileType>& projectile_type,
				const std::vector<Vector2>& projectile_direction, const std::vector<Rectangle>& enemy_rect,
				MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
				) noexcept;

		void LeAttack(
				const std::vector<Rectangle>& enemy_rect, const std::vector<EnemyAttackComponent>& enemy_attack_components,
				const std::vector<EnemyType>& enemy_type, const Vector2 player_centre, const float player_radius,
				MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
				) noexcept;

		void SlideAttack(
				const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre, const float player_radius,
				const Vector2 player_direction,	const std::vector<float>& enemy_health, MessageSystem& message_system,
				const size_t ticks
				) noexcept;

		void Aura(
				const std::vector<Rectangle>& enemy_rect, const Rectangle aura, const ModifierSystem& modifier_system,
				MessageSystem& message_system, const float aura_damage, const size_t ticks
				) noexcept;

		void XpCollision(
				const Rectangle player_rect, const std::vector<Rectangle>& xp_rect,
				const std::vector<uint8_t>& xp_value, size_t* collected_xp,
				const ModifierSystem& modifier_system, MessageSystem& message_system
				) noexcept;

	private:
		void ApplyAussie(MessageSystem& message_system,	const size_t ticks, const unsigned int scale) noexcept;
		void ApplyDrunk(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept;
		void ApplyNone(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept;
		void ApplyPoison(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept;
		void ApplyTrapped(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept;

		using LeAttackHook = void(CollisionSystem::*)(MessageSystem&, const size_t, const unsigned int) noexcept;
		std::array<LeAttackHook, static_cast<size_t>(EnemyType::COUNT)> LeAttackHooks =
		{
			&ApplyAussie,
			&ApplyDrunk,
			&ApplyNone,
			&ApplyPoison,
			&ApplyTrapped
		};
};

