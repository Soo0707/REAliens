#include "collisions.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "raylib.h"

void Collisions::ResolveCollisionPlayerX(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	bool collided = false;
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(player.NextRect, wall.Rect))
		{
			collided = true;
			break;
		}
	}
	
	if (!collided)
	{
		for (auto const& prop : props)
		{
			if (CheckCollisionRecs(player.NextRect, prop.Rect))
				collided = true;
		}
	}

	if (!collided)
	{
		for (auto const& spawner : spawners)
		{
			if (CheckCollisionRecs(player.NextRect, spawner.Rect))
				collided = true;
		}
	}
	
	(collided) ? (player.Direction.x = 0) : (player.Rect.x = player.NextRect.x);
}


void Collisions::ResolveCollisionPlayerY(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	bool collided = false;
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(player.NextRect, wall.Rect))
		{
			collided = true;
			break;
		}
	}
	
	if (!collided)
	{
		for (auto const& prop : props)
		{
			if (CheckCollisionRecs(player.NextRect, prop.Rect))
				collided = true;
		}
	}

	if (!collided)
	{
		for (auto const& spawner : spawners)
		{
			if (CheckCollisionRecs(player.NextRect, spawner.Rect))
				collided = true;
		}
	}
	
	(collided) ? (player.Direction.y = 0) : (player.Rect.y = player.NextRect.y);
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

void Collisions::ResolveCollisionEnemyX(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	bool collided = false;
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(enemy.NextRect, wall.Rect))
		{
			collided = true;
			break;
		}
	}
	
	if (!collided)
	{
		for (auto const& prop : props)
		{
			if (CheckCollisionRecs(enemy.NextRect, prop.Rect))
				collided = true;
		}
	}

	if (!collided)
	{
		for (auto const& spawner : spawners)
		{
			if (CheckCollisionRecs(enemy.NextRect, spawner.Rect))
				collided = true;
		}
	}
	
	(collided) ? (enemy.Direction.x = 0) : (enemy.Rect.x = enemy.NextRect.x);
}


void Collisions::ResolveCollisionEnemyY(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props, std::vector<Spawner>& spawners)
{
	bool collided = false;
	for (auto const& wall : walls)
	{
		if (CheckCollisionRecs(enemy.NextRect, wall.Rect))
		{
			collided = true;
			break;
		}
	}
	
	if (!collided)
	{
		for (auto const& prop : props)
		{
			if (CheckCollisionRecs(enemy.NextRect, prop.Rect))
				collided = true;
		}
	}

	if (!collided)
	{
		for (auto const& spawner : spawners)
		{
			if (CheckCollisionRecs(enemy.NextRect, spawner.Rect))
				collided = true;
		}
	}
	
	(collided) ? (enemy.Direction.y = 0) : (enemy.Rect.y = enemy.NextRect.y);
}
