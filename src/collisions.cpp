#include "collisions.hpp"

#include <vector>

#include "globalDataWrapper.hpp"

#include "enemy.hpp"
#include "enemyData.hpp"
#include "game.hpp"
#include "constants.hpp"

unsigned int Collisions::ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, GlobalDataWrapper& global_data) noexcept
{
	float damage;
	unsigned int damage_done = 0;

	switch (proj.Type)
	{
		case ProjectileType::Lazer:
			damage = global_data.Attributes.at(Attribute::LazerDamage);
			break;
		case ProjectileType::Bullet:
			damage = global_data.Attributes.at(Attribute::BulletDamage);
			break;
		case ProjectileType::Ball:
			damage = global_data.Attributes.at(Attribute::BallDamage);
			break;
		default:
			damage = 0;
	}

	for (auto &enemy : enemies)
	{
		if (enemy.Layer == global_data.CurrentLayer && CheckCollisionRecs(proj.Rect, enemy.Rect))
		{
			enemy.Health -= damage;
			enemy.FlashSprite(global_data.Ticks);
			
			if (proj.Type == ProjectileType::Bullet)
				proj.Kill = true;

			damage_done += damage;
		}
	}

	return damage_done;
}

bool Collisions::LeAttack(Player& player, Enemy& enemy, GlobalDataWrapper& global_data) noexcept
{
	if (CheckCollisionRecs(player.Rect, enemy.Rect) && enemy.CanLeAttack)
	{
		const unsigned int scale = static_cast<unsigned int>(enemy.Scale);

		player.Health -= EnemyAttributes.at(enemy.Type).damage * scale;

		if (!global_data.Effects.count(Effect::Milk))
		{
			switch(enemy.Type)
			{
				case EnemyType::Australian:
					global_data.Effects.insert(Effect::Aussie);
					global_data.Events[Event::AussieExpire] = global_data.Ticks + SECONDS_TO_TICKS(1) * scale;

					break;
				case EnemyType::Poison:
					global_data.Effects.insert(Effect::Poison);

					global_data.Attributes[Attribute::PoisonDamage] = 2.0f;

					global_data.Events[Event::PoisonTick] = global_data.Ticks + SECONDS_TO_TICKS(1);
					global_data.Events[Event::PoisonExpire] = global_data.Ticks + SECONDS_TO_TICKS(5) * scale;
					break;
				case EnemyType::Trapper:
					global_data.Effects.insert(Effect::Trapped);
					break;
				case EnemyType::Drunkard:
					global_data.Effects.insert(Effect::Drunk);
					global_data.Events[Event::DrunkExpire] = global_data.Ticks + SECONDS_TO_TICKS(5) * scale;
					break;
			}
		}

		enemy.CanLeAttack = false;
		enemy.LastLeAttack = global_data.Ticks;

		return true;
	}
	return false;
}

void Collisions::Aura(Game& game) noexcept
{
	float aura_damage = game.GlobalData->Attributes.at(Attribute::AuraDamage);
	unsigned int total_hit = 0;

	for (auto &enemy : game.Enemies)
	{
		if (enemy.Layer == game.GlobalData->CurrentLayer && CheckCollisionRecs(game.PlayerInstance->Aura, enemy.Rect))
		{
			enemy.Health -= aura_damage;
			enemy.FlashSprite(game.GlobalData->Ticks);

			game.GameTexts.emplace_back(enemy.Rect.x, enemy.Rect.y, std::to_string(static_cast<int>(aura_damage)), game.GlobalData->Ticks);
			total_hit++;
		}
	}

	if (game.GlobalData->Effects.count(Effect::LifeSteal))
		game.PlayerInstance->IncreaseHealth(total_hit * aura_damage * game.GlobalData->Attributes.at(Attribute::LifeStealMultiplier));
}
