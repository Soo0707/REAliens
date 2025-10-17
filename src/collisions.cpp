#include "collisions.hpp"

#include <mutex>

#include "globalDataWrapper.hpp"
#include "enemy.hpp"
#include "enemyData.hpp"

#include "constants.hpp"

void Collisions::ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, GlobalDataWrapper& global_data)
{
	float damage;

	{
		std::lock_guard attributes_lock(global_data.AttributesMutex);
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
	}

	for (auto &enemy : enemies)
	{
		if (CheckCollisionRecs(proj.Rect, enemy.Rect))
		{
			enemy.Health -= damage;
			enemy.FlashSprite(global_data.Ticks);
			
			if (proj.Type == ProjectileType::Bullet)
				proj.Kill = true;
		}
	}
}

void Collisions::LeAttack(Player& player, Enemy& enemy, GlobalDataWrapper& global_data)
{
	if (CheckCollisionRecs(player.Rect, enemy.Rect) && enemy.CanLeAttack)
	{
		player.Health -= EnemyAttributes.at(enemy.Type).damage;

		std::lock_guard<std::mutex> effects_lock(global_data.EffectsMutex);

		if (!global_data.Effects.count(Effect::Milk))
		{
			switch(enemy.Type)
			{
				case EnemyType::Australian:
					{
						global_data.Effects.insert(Effect::Aussie);

						std::lock_guard<std::mutex> events_lock(global_data.EventsMutex);
						global_data.Events[Event::AussieExpire] = global_data.Ticks + SECONDS_TO_TICKS(1);
					}
					break;
				case EnemyType::Poison:
					{
						global_data.Effects.insert(Effect::Poison);

						std::lock_guard<std::mutex> attributes_lock(global_data.AttributesMutex);
						global_data.Attributes[Attribute::PoisonDamage] = 2.0f;

						std::lock_guard<std::mutex> events_lock(global_data.EventsMutex);
						global_data.Events[Event::PoisonTick] = global_data.Ticks + SECONDS_TO_TICKS(1);
						global_data.Events[Event::PoisonExpire] = global_data.Ticks + SECONDS_TO_TICKS(5);
					}
					break;
				case EnemyType::Trapper:
					global_data.Effects.insert(Effect::Trapped);
					break;
				case EnemyType::Drunkard:
					{
						global_data.Effects.insert(Effect::Drunk);

						std::lock_guard<std::mutex> events_lock(global_data.EventsMutex);
						global_data.Events[Event::DrunkExpire] = global_data.Ticks + SECONDS_TO_TICKS(5);
					}
					break;
			}
		}

		enemy.CanLeAttack = false;
		enemy.LastLeAttack = global_data.Ticks;
	}
}


void Collisions::Aura(const float damage, const size_t ticks, Rectangle& aura, std::vector<Enemy>& enemies)
{
	for (auto& enemy : enemies)
	{
		if (CheckCollisionRecs(aura, enemy.Rect))
			enemy.Health -= damage;
			enemy.FlashSprite(ticks);
	}
}
