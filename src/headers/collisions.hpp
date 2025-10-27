#pragma once

#include <vector>
#include <cstddef>

#include "player.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "globalDataWrapper.hpp"
#include "raylib.h"

namespace Collisions
{
	unsigned int ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, GlobalDataWrapper& global_data) noexcept;
	void LeAttack(Player& player, Enemy& enemy, GlobalDataWrapper& global_data) noexcept;

	bool Aura(const float damage, const size_t ticks, Rectangle& aura, Enemy& enemy) noexcept;
}
