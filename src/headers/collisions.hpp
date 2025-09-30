#pragma once

#include <vector>
#include <memory>
#include "gameObjects.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "raylib.h"
#include "game.hpp"

static constexpr float buffer = 32;

namespace Collisions
{
	void PlayerCollisionX(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props);
	void PlayerCollisionY(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props);

	void ProjectileCollision(Projectile& proj, std::vector<Wall>& walls, std::vector<Enemy>& enemies, size_t& ticks);

	void EnemyCollisionX(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props);
	void EnemyCollisionY(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props);

	void PlayerCollisionResolverX(Player& player, const Rectangle& rect);
	void PlayerCollisionResolverY(Player& player, const Rectangle& rect);

	void EnemyCollisionResolverX(Enemy& enemy, const Rectangle& rect);
	void EnemyCollisionResolverY(Enemy& enemy, const Rectangle& rect);

	void LeAttack(Player& player, Enemy& enemy, std::unordered_map<EffectKey, float>& effects);
}
