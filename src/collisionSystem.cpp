#include "collisionSystem.hpp"

#include <vector>
#include <cstdint>
#include <cstddef>
#include <variant>
#include <algorithm>

#include "raylib.h"

#include "commands.hpp"
#include "signals.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

#include "enemyData.hpp"
#include "projectileData.hpp"
#include "constants.hpp"

CollisionSystem::CollisionSystem(const float map_width, const float map_height)
{
	const size_t largest_dimension = std::max(
			static_cast<size_t>(map_width / TEXTURE_TILE_SIZE),
			static_cast<size_t>(map_height / TEXTURE_TILE_SIZE)
			);

	const size_t grid_size = largest_dimension * largest_dimension;

	this->EnemyGrid.resize(grid_size, -1);
}

void CollisionSystem::PollSignals(
		MessageSystem& message_system, const std::vector<Rectangle>& enemy_rect, const Vector2 player_centre,
		const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(CollisionSystemSignal::COUNT); i++)
	{
		const uint16_t times = message_system.CollisionSystemSignals[i];
		
		auto signal_handler = this->SignalHandlers[i];

		if (times > 0)
			(this->*signal_handler)(enemy_rect, player_centre, modifier_system, message_system, ticks);

		message_system.CollisionSystemSignals[i] = 0;
	}
}

void CollisionSystem::UpdateEnemyGrid(const std::vector<Rectangle>& enemy_rect) noexcept
{
	const size_t grid_size = this->EnemyGrid.size();

	for (size_t i = 0, n = this->EnemyGrid.size(); i < n; i++)
		this->EnemyGrid[i] = -1;

	for (size_t i = 0, n = enemy_rect.size(); i < n; i++)
	{
		const float x = enemy_rect[i].x + enemy_rect[i].width / 2.0f;
		const float y = enemy_rect[i].y + enemy_rect[i].height / 2.0f;

		const size_t index = this->GetMortonCode(x, y);
		
		if (index < grid_size)
			this->EnemyGrid[index] = i;
	}
}

void CollisionSystem::ProjectileCollision(
		const std::vector<Rectangle>& projectile_rect, const std::vector<ProjectileType>& projectile_types,
		const std::vector<Vector2>& projectile_direction, const std::vector<Rectangle>& enemy_rect,
		MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	// TODO: unfuck this mess, then unfuck the asset manager map call in main
	const size_t grid_size = this->EnemyGrid.size();

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
		
		const float x = projectile_rect[i].x + projectile_rect[i].width / 2.0f;
		const float y = projectile_rect[i].y + projectile_rect[i].height / 2.0f;

		const size_t index = this->GetMortonCode(x, y);

		if (index < grid_size && this->EnemyGrid[index] >= 0)
		{
			const size_t enemy_index = this->EnemyGrid[index];

			message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct DamageEnemy>, enemy_index, damage);

			message_system.ProjectileSystemCommands.emplace_back(std::in_place_type<struct ProjectileHit>, i);

			message_system.ParticleSystemCommands.emplace_back(
					ticks, damage, projectile_direction[i], x, y,
					10, 30, 48, TICK_RATE / 2, 256, RED, RED
					);

			total_damage_done += static_cast<unsigned int>(damage);
		}

/*
		for (size_t j = 0, m = enemy_rect.size(); j < m; j++)
		{
			if (CheckCollisionRecs(projectile_rect[i], enemy_rect[j]))
			{
				message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct DamageEnemy>, j, damage);

				message_system.ProjectileSystemCommands.emplace_back(std::in_place_type<struct ProjectileHit>, i);

				message_system.ParticleSystemCommands.emplace_back(
						ticks, damage, projectile_direction[i], projectile_rect[i].x, projectile_rect[i].y,
						10, 30, 48, TICK_RATE / 2, 256, RED, RED
						);

				total_damage_done += static_cast<unsigned int>(damage);
				break;
			}
		}
		*/
	}

	if (modifier_system.EffectStatus(Effect::LifeSteal))
	{
		message_system.PlayerCommands.emplace_back(
				std::in_place_type<struct IncreasePlayerHealth>,
				total_damage_done * modifier_system.GetAttribute(Attribute::LifeStealMultiplier)
				);
	}

	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_damage_done);
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

			message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct EnemyLeAttacked>, i, ticks);
		}
	}

	message_system.PlayerCommands.emplace_back(std::in_place_type<struct DamagePlayer>, total_damage);
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

			message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct DamageEnemy>, i, damage_done);

			message_system.ParticleSystemCommands.emplace_back(
					ticks, damage_done, player_direction, enemy_rect[i].x, enemy_rect[i].y,
					15, 25, 60, TICK_RATE / 2, 512, ORANGE, RED
					);

			total_damage_done += static_cast<unsigned int>(damage_done);
		}
	}
	
	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_damage_done);
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
			message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct DamageEnemy>, i, aura_damage);

			const Vector2 velocity = { static_cast<float>(GetRandomValue(-192, 192)), static_cast<float>(GetRandomValue(-192, 192)) };

			message_system.ParticleSystemCommands.emplace_back(
					ticks, aura_damage, velocity, enemy_rect[i].x, enemy_rect[i].y,
					15, 30, 60, TICK_RATE / 2, 256, PURPLE, RED
					);

			total_hit++;
		}
	}

	if (modifier_system.EffectStatus(Effect::LifeSteal))
	{
		message_system.PlayerCommands.emplace_back(
				std::in_place_type<struct IncreasePlayerHealth>,
				total_hit * aura_damage * modifier_system.GetAttribute(Attribute::LifeStealMultiplier)
				);
	}

	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_hit * static_cast<unsigned int>(aura_damage));
}

void CollisionSystem::XpCollision(
		const Rectangle player_rect, const std::vector<Rectangle>& xp_rect,
		size_t* collected_xp, const ModifierSystem& modifier_system,
		MessageSystem& message_system
		) const noexcept
{
	const bool has_magnetism = modifier_system.EffectStatus(Effect::Magnetism);

	for (size_t i = 0, n = xp_rect.size(); i < n; i++)
	{
		if (CheckCollisionRecs(player_rect, xp_rect[i]) || has_magnetism)
		{
			*collected_xp += 1;

			message_system.XpSystemCommands.emplace_back(std::in_place_type<struct KillXp>, i);
		}
	}
}

void CollisionSystem::ApplyAussie(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyAussie)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), false, Timer::AussieExpire);
}

void CollisionSystem::ApplyPoison(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyPoison)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(1), true, Timer::PoisonTick);
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::PoisonExpire);
}

void CollisionSystem::ApplyTrapped(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyTrapped)]++;
}

void CollisionSystem::ApplyDrunk(MessageSystem& message_system, const size_t ticks) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyDrunk)]++;
	
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(5), false, Timer::DrunkExpire);
}

void CollisionSystem::ApplyNone(MessageSystem& message_system, const size_t ticks) const noexcept
{}

inline uint16_t CollisionSystem::SeparateBits(uint16_t bits) const noexcept
{
	/*
	note: the process below can be done with a single pdep intrinsic
	for haswell and newer but the target is sandybridge and newer
	0000 0000 ABCD EFGH << 4
	0000 ABCD EFGH 0000 |
	0000 ABCD IJKL EFGH & (0000 1111 0000 1111)
	0000 ABCD 0000 EFGH

	0000 ABCD 0000 EFGH << 2
	00AB CD00 00EF GH00 |
	00AB IJCD 00EF KLGH & (0011 0011 0011 0011)
	00AB 00CD 00EF 00GH

	00AB 00CD 00EF 00GH << 1
	0AB0 0CD0 0EF0 0GH0 |
	0AIB 0CJD 0EKF 0GLH & (0101 0101 0101 0101)
	0A0B 0C0D 0E0F 0G0H
	*/

	bits = (bits | (bits << 4)) & 0x0f0f;
	bits = (bits | (bits << 2)) & 0x3333;
	bits = (bits | (bits << 1)) & 0x5555;

	return bits;
}

size_t CollisionSystem::GetMortonCode(const float x, const float y) const noexcept
{

	uint16_t x_tile = static_cast<uint16_t>(x / TEXTURE_TILE_SIZE);
	uint16_t y_tile = static_cast<uint16_t>(y / TEXTURE_TILE_SIZE);

	x_tile = this->SeparateBits(x_tile);
	y_tile = this->SeparateBits(y_tile);

	return static_cast<size_t>(x_tile | (y_tile << 1));
}
