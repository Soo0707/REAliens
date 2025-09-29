#pragma once

#include <vector>
#include <memory>
#include "gameObjects.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "projectiles.hpp"
#include "raylib.h"

namespace Collisions
{
	void ResolveCollisionPlayerX(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners);
	void ResolveCollisionPlayerY(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners);

	void ProjectileCollisions(std::vector<Projectile>& projectile, std::vector<Wall>& walls, const Rectangle& update_area);

	void ResolveCollisionEnemyX(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners);
	void ResolveCollisionEnemyY(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners);

}
