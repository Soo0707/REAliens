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

void Collisions::LeAttack(Player& player, Enemy& enemy, GlobalDataWrapper& global_data) noexcept
{
	if (enemy.CanLeAttack && CheckCollisionCircleRec(player.Centre, player.Radius, enemy.Rect))
	{
		unsigned int scale = static_cast<unsigned int>(enemy.Scale);

		player.Health -= EnemyAttributes[static_cast<size_t>(enemy.Type)].damage * scale;

		if (!global_data.Effects.count(Effect::Milk))
			Collisions::LeAttackHooks[static_cast<size_t>(enemy.Type)](global_data, scale);

		enemy.CanLeAttack = false;
		enemy.LastLeAttack = global_data.Ticks;
	}
}

unsigned int Collisions::SlideAttack(Player& player, Enemy& enemy) noexcept
{
	float damage_done = 0;

	if (CheckCollisionCircleRec(player.Centre, player.Radius, enemy.Rect))
	{
		damage_done = enemy.Health;
		enemy.Health = 0;
	}

	return static_cast<unsigned int>(damage_done);
}

unsigned int Collisions::Aura(Game& game) noexcept
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

	return static_cast<unsigned int>(total_hit * aura_damage);
}


void Collisions::ApplyAustralian(GlobalDataWrapper& global_data, unsigned int scale) noexcept
{
	global_data.Effects.insert(Effect::Aussie);
	global_data.Events[Event::AussieExpire] = global_data.Ticks + SECONDS_TO_TICKS(1) * scale;
}

void Collisions::ApplyPoison(GlobalDataWrapper& global_data, unsigned int scale) noexcept
{
	global_data.Effects.insert(Effect::Poison);

	global_data.Events[Event::PoisonTick] = global_data.Ticks + SECONDS_TO_TICKS(1);
	global_data.Events[Event::PoisonExpire] = global_data.Ticks + SECONDS_TO_TICKS(5) * scale;
}

void Collisions::ApplyTrapped(GlobalDataWrapper& global_data, unsigned int scale) noexcept
{
	global_data.Effects.insert(Effect::Trapped);
}

void Collisions::ApplyDrunk(GlobalDataWrapper& global_data, unsigned int scale) noexcept
{
	global_data.Effects.insert(Effect::Drunk);
	global_data.Events[Event::DrunkExpire] = global_data.Ticks + SECONDS_TO_TICKS(5) * scale;
}

void Collisions::ApplyNone(GlobalDataWrapper& global_data, unsigned int scale) noexcept
{}
