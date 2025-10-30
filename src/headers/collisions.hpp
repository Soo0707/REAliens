#pragma once

#include <vector>
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

	void Aura(Game& game) noexcept;
}
