#pragma once

#include <vector>
#include <array>
#include <cstddef>

#include "player.hpp"
#include "projectileData.hpp"

#include "messageSystem.hpp"
#include "modifierSystem.hpp"
/*
class CollisionSystem
{
	public:
		CollisionSystem() = default;
		~CollisionSystem() = default;

		void ProjectileCollision(std::vector<Projectile>& projectiles, std::vector<Enemy>& enemies,
				MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks) noexcept;

		void LeAttack(std::vector<Enemy>& enemies, Player& player, MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks) noexcept;

		void SlideAttack(Player& player, std::vector<Enemy>& enemies, MessageSystem& message_system, const size_t ticks) noexcept;

		void Aura(std::vector<Enemy>& enemies, MessageSystem& message_system, const Rectangle aura, const float aura_damage, const size_t ticks) noexcept;

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
*/
