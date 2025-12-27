#include "collisions.hpp"

#include <unordered_map>
#include <unordered_set>

#include "gameState.hpp"
#include "enemy.hpp"
#include "enemyData.hpp"
#include "constants.hpp"

unsigned int Collisions::ProjectileCollision(Projectile& proj, std::vector<Enemy>& enemies, size_t ticks,
		const std::unordered_map<Attribute, float>& attributes) noexcept
{
	float damage;
	unsigned int damage_done = 0;

	switch (proj.Type)
	{
		case ProjectileType::Lazer:
			damage = attributes.at(Attribute::LazerDamage);
			break;
		case ProjectileType::Bullet:
			damage = attributes.at(Attribute::BulletDamage);
			break;
		default:
			damage = 0;
	}

	for (auto &enemy : enemies)
	{
		if (CheckCollisionRecs(proj.Rect, enemy.Rect))
		{
			enemy.Health -= damage;
			enemy.FlashSprite(ticks);
			
			if (proj.Type == ProjectileType::Bullet)
				proj.Kill = true;

			damage_done += damage;
		}
	}

	return damage_done;
}

void Collisions::LeAttack(Player& player, Enemy& enemy, size_t ticks,
		std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events) noexcept
{
	if (enemy.CanLeAttack && CheckCollisionCircleRec(player.Centre, player.Radius, enemy.Rect))
	{
		player.Health -= enemy.Damage;

		const unsigned int scale = static_cast<unsigned int>(enemy.Scale);

		if (!effects.count(Effect::Milk))
			Collisions::LeAttackHooks[static_cast<size_t>(enemy.Type)](effects, events, ticks, scale);

		enemy.CanLeAttack = false;
		enemy.LastLeAttack = ticks;
	}
}

long long Collisions::SlideAttack(Player& player, Enemy& enemy) noexcept
{
	float damage_done = 0;

	if (CheckCollisionCircleRec(player.Centre, player.Radius, enemy.Rect))
	{
		damage_done = enemy.Health;
		enemy.Health = 0;
	}

	return static_cast<long long>(damage_done);
}

bool Collisions::Aura(Enemy& enemy, Rectangle aura, float aura_damage, size_t ticks) noexcept
{
	if (CheckCollisionRecs(aura, enemy.Rect))
	{
		enemy.Health -= aura_damage;
		enemy.FlashSprite(ticks);
		return true;
	}
	return false;
}


void Collisions::ApplyAustralian(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events,
		size_t ticks, unsigned int scale) noexcept
{
	effects.insert(Effect::Aussie);
	events[Event::AussieExpire] = ticks + SECONDS_TO_TICKS(1) * scale;
}

void Collisions::ApplyPoison(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events,
		size_t ticks, unsigned int scale) noexcept
{
	effects.insert(Effect::Poison);

	events[Event::PoisonTick] = ticks + SECONDS_TO_TICKS(1);
	events[Event::PoisonExpire] = ticks + SECONDS_TO_TICKS(5) * scale;
}

void Collisions::ApplyTrapped(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events,
		size_t ticks, unsigned int scale) noexcept
{
	effects.insert(Effect::Trapped);
}

void Collisions::ApplyDrunk(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events,
		size_t ticks, unsigned int scale) noexcept
{
	effects.insert(Effect::Drunk);
	events[Event::DrunkExpire] = ticks + SECONDS_TO_TICKS(5) * scale;
}

void Collisions::ApplyNone(std::unordered_set<Effect>& effects, std::unordered_map<Event, size_t>& events,
		size_t ticks, unsigned int scale) noexcept
{}
