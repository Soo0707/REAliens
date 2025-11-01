#pragma once

#include <vector>
#include <array>
#include <cstddef>

#include "player.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "globalDataWrapper.hpp"
#include "game.hpp"
#include "raylib.h"

namespace Collisions
{
	unsigned int ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, GlobalDataWrapper& global_data) noexcept;
	bool LeAttack(Player& player, Enemy& enemy, GlobalDataWrapper& global_data) noexcept;
	unsigned int SlideAttack(Player& player, Enemy& enemy) noexcept;

	void Aura(Game& game) noexcept;

	void ApplyAustralian(GlobalDataWrapper& global_data, unsigned int scale) noexcept;
	void ApplyPoison(GlobalDataWrapper& global_data, unsigned int scale) noexcept;
	void ApplyTrapped(GlobalDataWrapper& global_data, unsigned int scale) noexcept;

	void ApplyDrunk(GlobalDataWrapper& global_data, unsigned int scale) noexcept;
	void ApplyNone(GlobalDataWrapper& global_data, unsigned int scale) noexcept;

	static inline constexpr std::array<void(*)(GlobalDataWrapper&, unsigned int) noexcept, static_cast<size_t>(EnemyType::COUNT)> LeAttackHooks =
	{
		&ApplyAustralian,
		&ApplyDrunk,
		&ApplyNone,
		&ApplyPoison,
		&ApplyTrapped
	};
}

