#include "collisionSystem.hpp"

#include <vector>
#include <cstddef>

#include "raylib.h"

#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

#include "enemy.hpp"
#include "enemyData.hpp"
#include "constants.hpp"

void CollisionSystem::ProjectileCollision(std::vector<Projectile>& projectiles, std::vector<Enemy>& enemies,
		MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks) noexcept
{
	unsigned int total_damage_done = 0;

	for (auto& projectile : projectiles)
	{
		float damage;

		switch (projectile.Type)
		{
			case ProjectileType::Lazer:
				damage = modifier_system.GetAttribute(Attribute::LazerDamage);
				break;
			case ProjectileType::Bullet:
				damage = modifier_system.GetAttribute(Attribute::BulletDamage);
				break;
			default:
				damage = 0;
		}

		for (auto& enemy : enemies)
		{
			if (CheckCollisionRecs(projectile.Rect, enemy.Rect))
			{
				//TODO: message enemy system to damage enemy instead
				
				enemy.Health -= damage;
				enemy.FlashSprite(ticks);
				
				if (projectile.Type == ProjectileType::Bullet)
					projectile.Kill = true;

				total_damage_done += static_cast<unsigned int>(damage);

				message_system.GameTextSystemCommands.emplace_back(
						ticks, ticks + TICK_RATE / 4, projectile.Rect.x, projectile.Rect.y,
						48.0f, damage, 48, YELLOW
						);

				message_system.ParticleSystemCommands.emplace_back(
						ticks, 5, projectile.Direction, projectile.Rect.x, projectile.Rect.y,
						5, 20, 120, TICK_RATE, 256, RED, RED
						);
			}
		}
	}

	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_damage_done);
}

void CollisionSystem::LeAttack(std::vector<Enemy>& enemies, Player& player, MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks) noexcept
{
	for (auto& enemy : enemies)
	{
		if (enemy.CanLeAttack && CheckCollisionCircleRec(player.Centre, player.Radius, enemy.Rect))
		{
			player.Health -= enemy.Damage;

			const unsigned int scale = static_cast<unsigned int>(enemy.Scale);

			if (!modifier_system.EffectStatus(Effect::Milk))
			{
				auto le_attack_hook = CollisionSystem::LeAttackHooks[static_cast<size_t>(enemy.Type)];
				(this->*le_attack_hook)(message_system, ticks, scale);
			};

			enemy.CanLeAttack = false;
			enemy.LastLeAttack = ticks;
		}
	}
}

void CollisionSystem::SlideAttack(Player& player, std::vector<Enemy>& enemies, MessageSystem& message_system, const size_t ticks) noexcept
{
	unsigned int total_damage_done = 0;

	for (auto& enemy : enemies)
	{
		if (CheckCollisionCircleRec(player.Centre, player.Radius, enemy.Rect))
		{
			float damage_done = enemy.Health;

			enemy.Health = 0.0f;

			message_system.GameTextSystemCommands.emplace_back(
					ticks, ticks + TICK_RATE / 4, enemy.Rect.x, enemy.Rect.y,
					64.0f, damage_done, 52, ORANGE
					);

			message_system.ParticleSystemCommands.emplace_back(
					ticks, 50, player.Direction, enemy.Rect.x, enemy.Rect.y,
					10, 25, 60, TICK_RATE, 512, ORANGE, RED
					);

			total_damage_done += static_cast<unsigned int>(damage_done);
		}
	}

	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_damage_done);
}

void CollisionSystem::Aura(std::vector<Enemy>& enemies, MessageSystem& message_system, const Rectangle aura, const float aura_damage, const size_t ticks) noexcept
{
	unsigned int total_hit = 0;

	for (auto& enemy : enemies)
	{
		if (CheckCollisionRecs(aura, enemy.Rect))
		{
			enemy.Health -= aura_damage;
			enemy.FlashSprite(ticks);

			message_system.GameTextSystemCommands.emplace_back(
					ticks, ticks + TICK_RATE / 4, enemy.Rect.x, enemy.Rect.y,
					64.0f, aura_damage, 42, MAGENTA
					);

			const Vector2 velocity = { static_cast<float>(GetRandomValue(-192, 192)), static_cast<float>(GetRandomValue(-192, 192)) };

			message_system.ParticleSystemCommands.emplace_back(
					ticks, 15, velocity, enemy.Rect.x, enemy.Rect.y,
					10, 20, 60, TICK_RATE, 0, PURPLE, RED
					);

			total_hit++;
		}
	}
/*
	if (game_state.Effects.count(Effect::LifeSteal))
		game_state.Player->IncreaseHealth(total_hit * aura_damage * game_state.Attributes.at(Attribute::LifeStealMultiplier));
		*/
	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_hit * aura_damage);
}


void CollisionSystem::ApplyAussie(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::ApplyAussie);

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1) * scale, false, Timer::AussieExpire);
}

void CollisionSystem::ApplyPoison(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::ApplyPoison);

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), true, Timer::PoisonTick);
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::PoisonExpire);
}

void CollisionSystem::ApplyTrapped(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::ApplyTrapped);
}

void CollisionSystem::ApplyDrunk(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::ApplyDrunk);
	
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5) * scale, false, Timer::DrunkExpire);
}

void CollisionSystem::ApplyNone(MessageSystem& message_system, const size_t ticks, const unsigned int scale) noexcept
{}
