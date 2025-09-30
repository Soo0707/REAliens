#include "collisions.hpp"
#include "game.hpp"


void Collisions::ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, size_t& ticks)
{
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
