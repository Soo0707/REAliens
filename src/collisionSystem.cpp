#include "collisionSystem.hpp"

#include <vector>
#include <cstdint>
#include <cstddef>
#include <variant>
#include <mutex>
#include <atomic>

#include "raylib.h"

#include "commands.hpp"
#include "signals.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

#include "enemyData.hpp"
#include "projectileData.hpp"
#include "constants.hpp"

void CollisionSystem::PollSignals(
		MessageSystem& message_system, const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre,
		const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(CollisionSystemSignal::COUNT); i++)
	{
		const uint16_t times = message_system.CollisionSystemSignals[i].exchange(0);
		
		auto signal_handler = this->SignalHandlers[i];

		if (times > 0)
			(this->*signal_handler)(enemy_rect, player_centre, modifier_system, message_system, ticks);
	}
}


void CollisionSystem::ProjectileCollision(
		const std::vector<Rectangle>& projectile_rect, const std::vector<ProjectileType>& projectile_types,
		const std::vector<Vector2>& projectile_direction, const std::vector<Rectangle>& enemy_rect,
		MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	unsigned int total_damage_done = 0;

	for (size_t i = 0, n = projectile_rect.size(); i < n; i++)
	{
		float damage = 0.0f;

		switch (projectile_types[i])
		{
			case ProjectileType::Lazer:
				damage = modifier_system.GetAttribute(Attribute::LazerDamage);
				break;
			case ProjectileType::Bullet:
				damage = modifier_system.GetAttribute(Attribute::BulletDamage);
				break;
			case ProjectileType::Ball:
				damage = modifier_system.GetAttribute(Attribute::BallDamage);
				break;
		}

		for (size_t j = 0, m = enemy_rect.size(); j < m; j++)
		{
			if (CheckCollisionRecs(projectile_rect[i], enemy_rect[j]))
			{
				{
					std::lock_guard<std::mutex> lock(message_system.EnemySystemMutex);
					message_system.EnemySystemCommandsWrite.emplace_back(std::in_place_type<struct DamageEnemy>, j, damage);
				}

				{
					std::lock_guard<std::mutex> lock(message_system.ProjectileSystemMutex);
					message_system.ProjectileSystemCommandsWrite.emplace_back(std::in_place_type<struct ProjectileHit>, i);
				}

				{
					std::lock_guard<std::mutex> lock(message_system.ParticleSystemMutex);
					message_system.ParticleSystemCommandsWrite.emplace_back(
							ticks, damage, projectile_direction[i], projectile_rect[i].x, projectile_rect[i].y,
							10, 30, 48, TICK_RATE / 2, 256, RED, RED
							);
				}

				total_damage_done += static_cast<unsigned int>(damage);
				break;
			}
		}
	}

	if (modifier_system.EffectStatus(Effect::LifeSteal))
	{
		std::lock_guard<std::mutex> lock(message_system.PlayerMutex);
		message_system.PlayerCommandsWrite.emplace_back(
				std::in_place_type<struct IncreasePlayerHealth>,
				total_damage_done * modifier_system.GetAttribute(Attribute::LifeStealMultiplier)
				);
	}

	std::lock_guard<std::mutex> lock(message_system.StatSystemMutex);
	message_system.StatSystemCommandsWrite.emplace_back(Stat::TotalDamage, total_damage_done);
}

void CollisionSystem::LeAttack(
		const std::vector<Rectangle>& enemy_rect, const std::vector<EnemyAttackComponent>& enemy_attack_components,
		const std::vector<EnemyType>& enemy_type, const Vector2 player_centre, const float player_radius,
		MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	const bool has_milk = modifier_system.EffectStatus(Effect::Milk);
	float total_damage = 0.0f;

	for (size_t i = 0, n = enemy_attack_components.size(); i < n; i++)
	{
		if (enemy_attack_components[i].CanLeAttack && CheckCollisionCircleRec(player_centre, player_radius, enemy_rect[i]))
		{
			total_damage += enemy_attack_components[i].Damage;
			
			if (!has_milk)
			{
				auto le_attack_hook = CollisionSystem::LeAttackHooks[static_cast<size_t>(enemy_type[i])];
				(this->*le_attack_hook)(message_system, ticks);
			}

			{
				std::lock_guard<std::mutex> lock(message_system.EnemySystemMutex);
				message_system.EnemySystemCommandsWrite.emplace_back(std::in_place_type<struct EnemyLeAttacked>, i, ticks);
			}
		}
	}

	std::lock_guard<std::mutex> lock(message_system.PlayerMutex);
	message_system.PlayerCommandsWrite.emplace_back(std::in_place_type<struct DamagePlayer>, total_damage);
}

void CollisionSystem::SlideAttack(
		const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre, const float player_radius,
		const Vector2 player_direction,	const std::vector<float>& enemy_health, MessageSystem& message_system,
		const size_t ticks
		) const noexcept
{
	unsigned int total_damage_done = 0.0f;

	for (size_t i = 0, n = enemy_rect.size(); i < n; i++)
	{
		if (CheckCollisionCircleRec(player_centre, player_radius, enemy_rect[i]))
		{
			const float damage_done = enemy_health[i];

			{
				std::lock_guard<std::mutex> lock(message_system.EnemySystemMutex);
				message_system.EnemySystemCommandsWrite.emplace_back(std::in_place_type<struct DamageEnemy>, i, damage_done);
			}

			{
				std::lock_guard<std::mutex> lock(message_system.ParticleSystemMutex);
				message_system.ParticleSystemCommandsWrite.emplace_back(
						ticks, damage_done, player_direction, enemy_rect[i].x, enemy_rect[i].y,
						15, 25, 60, TICK_RATE / 2, 256, ORANGE, RED
						);
			}

			total_damage_done += static_cast<unsigned int>(damage_done);
		}
	}
	
	std::lock_guard<std::mutex> lock(message_system.StatSystemMutex);
	message_system.StatSystemCommandsWrite.emplace_back(Stat::TotalDamage, total_damage_done);
}

void CollisionSystem::Aura(
		const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre, const ModifierSystem& modifier_system,
		MessageSystem& message_system, const size_t ticks
		) const noexcept
{
	unsigned int total_hit = 0;

	const float aura_size = modifier_system.GetAttribute(Attribute::AuraSize);
	const float aura_damage = modifier_system.GetAttribute(Attribute::AuraDamage);

	const Rectangle aura = { player_centre.x - aura_size / 2.0f, player_centre.y - aura_size / 2.0f, aura_size, aura_size };

	for (size_t i = 0, n = enemy_rect.size(); i < n; i++)
	{
		if (CheckCollisionRecs(aura, enemy_rect[i]))
		{
			{
				std::lock_guard<std::mutex> lock(message_system.EnemySystemMutex);
				message_system.EnemySystemCommandsWrite.emplace_back(std::in_place_type<struct DamageEnemy>, i, aura_damage);
			}

			const Vector2 velocity = { static_cast<float>(GetRandomValue(-192, 192)), static_cast<float>(GetRandomValue(-192, 192)) };

			{
				std::lock_guard<std::mutex> lock(message_system.ParticleSystemMutex);
				message_system.ParticleSystemCommandsWrite.emplace_back(
						ticks, aura_damage, velocity, enemy_rect[i].x, enemy_rect[i].y,
						10, 20, 60, TICK_RATE / 2, 0, PURPLE, RED
						);
			}

			total_hit++;
		}
	}

	if (modifier_system.EffectStatus(Effect::LifeSteal))
	{
		std::lock_guard<std::mutex> lock(message_system.PlayerMutex);
		message_system.PlayerCommandsWrite.emplace_back(
				std::in_place_type<struct IncreasePlayerHealth>,
				total_hit * aura_damage * modifier_system.GetAttribute(Attribute::LifeStealMultiplier)
				);
	}

	{
		std::lock_guard<std::mutex> lock(message_system.StatSystemMutex);
		message_system.StatSystemCommandsWrite.emplace_back(Stat::TotalDamage, total_hit * static_cast<unsigned int>(aura_damage));
	}
}

void CollisionSystem::XpCollision(
		const Rectangle player_rect, const std::vector<Rectangle>& xp_rect,
		const std::vector<uint8_t>& xp_value, size_t* collected_xp,
		const ModifierSystem& modifier_system, MessageSystem& message_system
		) const noexcept
{
	const bool has_magnetism = modifier_system.EffectStatus(Effect::Magnetism);

	for (size_t i = 0, n = xp_rect.size(); i < n; i++)
	{
		if (CheckCollisionRecs(player_rect, xp_rect[i]) || has_magnetism)
		{
			*collected_xp += xp_value[i];

			std::lock_guard<std::mutex> lock(message_system.XpSystemMutex);
			message_system.XpSystemCommandsWrite.emplace_back(std::in_place_type<struct KillXp>, i);
		}
	}
}

void CollisionSystem::ApplyAussie(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyAussie)]++;

	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), false, Timer::AussieExpire);
	}
}

void CollisionSystem::ApplyPoison(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyPoison)]++;

	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), true, Timer::PoisonTick);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::PoisonExpire);
	}
}

void CollisionSystem::ApplyTrapped(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyTrapped)]++;
}

void CollisionSystem::ApplyDrunk(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyDrunk)]++;
	
	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::DrunkExpire);
	}
}

void CollisionSystem::ApplyNone(MessageSystem& message_system, const size_t ticks) const noexcept
{}

