#pragma once

#include <vector>
#include "player.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "raylib.h"
#include "game.hpp"

namespace Collisions
{
	void ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, size_t& ticks);
	void LeAttack(Player& player, Enemy& enemy, std::unordered_map<EffectKey, float>& effects, std::unordered_map<EventKey, size_t>& events);
}
