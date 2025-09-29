#include "collisions.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "raylib.h"

void Collisions::PlayerCollisionResolverX(Player& player, const Rectangle& rect)
{
	if (player.Direction.x > 0)
		player.Rect.x = rect.x - player.Rect.width - buffer; 
	else if (player.Direction.x < 0)
		player.Rect.x = rect.x + rect.width + buffer;
}

void Collisions::PlayerCollisionResolverY(Player& player, const Rectangle& rect)
{
	if (player.Direction.y > 0)
		player.Rect.y = rect.y - player.Rect.height - buffer;
	else if (player.Direction.y < 0)
		player.Rect.y = rect.y + rect.height + buffer;
}

void Collisions::ResolveCollisionPlayerX(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(player.Rect, wall.Rect))
		{
			Collisions::PlayerCollisionResolverX(player, wall.Rect);
			return;
		}
	}
	
	for (auto const& prop : props)
	{
		if (CheckCollisionRecs(player.Rect, prop.Rect))
		{
			Collisions::PlayerCollisionResolverX(player, prop.Rect);
			return;
		}
	}

	for (auto const& spawner : spawners)
	{
		if (CheckCollisionRecs(player.Rect, spawner.Rect))
		{
			Collisions::PlayerCollisionResolverX(player, spawner.Rect);
			return;
		}
	}
}


void Collisions::ResolveCollisionPlayerY(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(player.Rect, wall.Rect))
		{
			Collisions::PlayerCollisionResolverY(player, wall.Rect);
			return;
		}
	}
	
	for (auto const& prop : props)
	{
		if (CheckCollisionRecs(player.Rect, prop.Rect))
		{
			Collisions::PlayerCollisionResolverY(player, prop.Rect);
			return;
		}
	}

	for (auto const& spawner : spawners)
	{
		if (CheckCollisionRecs(player.Rect, spawner.Rect))
		{
			Collisions::PlayerCollisionResolverY(player, spawner.Rect);
			return;
		}
	}
}

void Collisions::ProjectileCollisions(std::vector<Projectile>& projectiles, std::vector<Wall>& walls, const Rectangle& update_area)
{
	for (auto &proj : projectiles)
	{
		if (!CheckCollisionRecs(proj.Rect, update_area))
		{
			proj.Kill = true;
			continue;
		}

		if (proj.Type != ProjectileType::Lazer)
		{
			for (auto const &wall : walls)
			{
				if (CheckCollisionRecs(proj.Rect, wall.Rect))
				{
					proj.Kill = true;
					break;
				}
			}
		}
	}
}

void Collisions::EnemyCollisionResolverX(Enemy& enemy, const Rectangle& rect)
{
	if (enemy.Direction.x > 0)
		enemy.Rect.x = rect.x - enemy.Rect.width - buffer; 
	else if (enemy.Direction.x < 0)
		enemy.Rect.x = rect.x + rect.width + buffer;
}

void Collisions::EnemyCollisionResolverY(Enemy& enemy, const Rectangle& rect)
{
	if (enemy.Direction.y > 0)
		enemy.Rect.y = rect.y - enemy.Rect.height - buffer;
	else if (enemy.Direction.y < 0)
		enemy.Rect.y = rect.y + rect.height + buffer;
}

void Collisions::ResolveCollisionEnemyX(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(enemy.Rect, wall.Rect))
		{
			EnemyCollisionResolverX(enemy, wall.Rect);
			return;
		}
	}
	
	for (auto const& prop : props)
	{
		if (CheckCollisionRecs(enemy.Rect, prop.Rect))
		{
			EnemyCollisionResolverX(enemy, prop.Rect);
			return;
		}
	}

	for (auto const& spawner : spawners)
	{
		if (CheckCollisionRecs(enemy.Rect, spawner.Rect))
		{
			EnemyCollisionResolverX(enemy, spawner.Rect);
			return;
		}
	}
	
}


void Collisions::ResolveCollisionEnemyY(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(enemy.Rect, wall.Rect))
		{
			EnemyCollisionResolverY(enemy, wall.Rect);
			return;
		}
	}
	
	for (auto const& prop : props)
	{
		if (CheckCollisionRecs(enemy.Rect, prop.Rect))
		{
			EnemyCollisionResolverY(enemy, prop.Rect);
			return;
		}
	}

	for (auto const& spawner : spawners)
	{
		if (CheckCollisionRecs(enemy.Rect, spawner.Rect))
		{
			EnemyCollisionResolverY(enemy, spawner.Rect);
			return;
		}
	}
}
