#include "collisions.hpp"
#include "gameObjects.hpp"
#include "projectiles.hpp"
#include "raylib.h"
#include "game.hpp"

void Collisions::PlayerCollisionResolverX(Player& player, const Rectangle& rect)
{
	if (player.Direction.x > 0)
		player.Rect.x = rect.x - player.Rect.width - buffer; 
	else if (player.Direction.x < 0)
		player.Rect.x = rect.x + rect.width + buffer;

	player.Direction.x = 0;
}

void Collisions::PlayerCollisionResolverY(Player& player, const Rectangle& rect)
{
	if (player.Direction.y > 0)
		player.Rect.y = rect.y - player.Rect.height - buffer;
	else if (player.Direction.y < 0)
		player.Rect.y = rect.y + rect.height + buffer;

	player.Direction.y = 0;
}

void Collisions::PlayerCollisionX(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props)
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
}


void Collisions::PlayerCollisionY(Player& player, std::vector<Wall>& walls, std::vector<Prop>& props)
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
}

void Collisions::EnemyCollisionResolverX(Enemy& enemy, const Rectangle& rect)
{
	if (enemy.Direction.x > 0)
		enemy.Rect.x = rect.x - enemy.Rect.width - buffer; 
	else if (enemy.Direction.x < 0)
		enemy.Rect.x = rect.x + rect.width + buffer;
	
	enemy.Direction.x = 0;
}

void Collisions::EnemyCollisionResolverY(Enemy& enemy, const Rectangle& rect)
{
	if (enemy.Direction.y > 0)
		enemy.Rect.y = rect.y - enemy.Rect.height - buffer;
	else if (enemy.Direction.y < 0)
		enemy.Rect.y = rect.y + rect.height + buffer;

	enemy.Direction.y = 0;
}

void Collisions::EnemyCollisionX(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props)
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

}

void Collisions::EnemyCollisionY(Enemy& enemy, std::vector<Wall>& walls, std::vector<Prop>& props)
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
}

void Collisions::ProjectileCollision(Projectile& proj, std::vector<Wall>& walls, std::vector<Enemy>& enemies, size_t& ticks)
{
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

	for (auto &enemy : enemies)
	{
		if (CheckCollisionRecs(proj.Rect, enemy.Rect))
		{
			enemy.Health -= proj.Damage;
			enemy.Flash = true;
			enemy.FlashTriggered = ticks;
			
			if (proj.Type == ProjectileType::Bullet)
				proj.Kill = true;
		}
	}
}

void Collisions::LeAttack(Player& player, Enemy& enemy, std::unordered_map<EffectKey, float>& effects, std::unordered_map<EffectKey, size_t>& effect_timeouts)
{
	if (CheckCollisionRecs(player.Rect, enemy.Rect))
	{
		player.Health -= enemy.Damage;
/*
		switch(enemy.Type)
		{
			case EnemyType::Australian:
				effects[EffectKey::Aussie] = 0.0f;
				break;
			case EnemyType::Bomber:
				enemy.UniqueState = UniqueStates::BomberExplode;
				break;
			case EnemyType::Poison:
				effects[EffectKey::Poisoned] = (float) 0.5f * enemy.Damage;
				break;
			case EnemyType::Trapper:
				effects[EffectKey::Trapped] = (float) 5;
				break;
		}
		*/
	}
}
