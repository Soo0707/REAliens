#include "collisions.hpp"

#include "globalDataWrapper.hpp"
#include "game.hpp"
#include "raymath.h"

void Collisions::ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, GlobalDataWrapper& global_data, size_t& ticks)
{
	float damage;
	switch (proj.Type)
	{
		case ProjectileType::Lazer:
			damage = global_data.Attributes.at(Attribute::LazerDamage);
			break;
		case ProjectileType::Bullet:
			damage = global_data.Attributes.at(Attribute::BulletDamage);
			break;
		case ProjectileType::Circle:
			damage = global_data.Attributes.at(Attribute::CircleDamage);
			break;
		default:
			damage = 0;
	}

	for (auto &enemy : enemies)
	{
		if (CheckCollisionRecs(proj.Rect, enemy.Rect))
		{
			enemy.Health -= damage;
			enemy.Flash = true;
			enemy.FlashTriggered = ticks;
			
			if (proj.Type == ProjectileType::Bullet)
				proj.Kill = true;
		}
	}
}

void Collisions::LeAttack(Player& player, Enemy& enemy, GlobalDataWrapper& global_data)
{
	if (CheckCollisionRecs(player.Rect, enemy.Rect))
	{
		switch(enemy.Type)
		{
			case EnemyType::Australian:
				global_data.Attributes[Attribute::Aussie] = 0.0f;
				global_data.Events[Event::AussieExpire] = global_data.Ticks + 480;
				break;
			case EnemyType::Bomber:
				//enemy.UniqueState = UniqueStates::BomberExplode;
				break;
			case EnemyType::Poison:
				global_data.Attributes[Attribute::PoisonDamage] = 2.0f;
				global_data.Events[Event::PoisonTick] = global_data.Ticks + 240;
				global_data.Events[Event::PoisonExpire] = global_data.Ticks + 1200;
				break;
			case EnemyType::Trapper:
				//effects[EffectKey::Trapped] = (float) 5;
				break;
		}
	}
}
