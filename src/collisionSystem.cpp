/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
#include "player.hpp"

CollisionSystem::CollisionSystem(const float map_width, const float map_height) :
	GridSize(
			std::max(
				static_cast<size_t>(map_width / TILE_SIZE),
				static_cast<size_t>(map_height / TILE_SIZE)
			)
			* 
			std::max(
				static_cast<size_t>(map_width / TILE_SIZE),
				static_cast<size_t>(map_height / TILE_SIZE)
			)
			)

{
	this->Reset();
}

void CollisionSystem::Reset()
{
	this->EnemyGrid.resize(this->GridSize, this->EmptyCell);
	this->ItemGrid.resize(this->GridSize, this->EmptyCell);
}

//TODO: clean up this monstrosity
void CollisionSystem::Update(
		MessageSystem& message_system, const ModifierSystem& modifier_system, const std::vector<Vector2>& enemy_centre,
		const std::vector<float>& enemy_health, const std::vector<EnemyAttackComponent>& enemy_attack_components,
		const std::vector<EnemyType>& enemy_types, const std::vector<Rectangle>& projectile_rect,
		const std::vector<ProjectileType>& projectile_type, const std::vector<Vector2>& projectile_direction,
		const std::vector<Vector2>& item_centre, const Player& player, const size_t ticks
		) noexcept
{
	const Vector2 player_centre = player.Centre;
	const Vector2 player_direction = player.Direction;

	this->UpdateEnemyGrid(enemy_centre);
	this->UpdateItemGrid(item_centre);

	this->PollSignals(message_system, player_centre, modifier_system, ticks);

	this->ProjectileCollision(
			projectile_rect, projectile_type, projectile_direction,
			message_system, modifier_system, ticks
			);

	const bool is_sliding = player.Sliding;

	if (is_sliding)
		this->SlideAttack(player_centre, player_direction, enemy_health, message_system, ticks);

	const bool has_greenbull = modifier_system.EffectStatus(Effect::Greenbull);

	this->EnemyItemCollision(message_system, enemy_centre, enemy_types);

	if (!has_greenbull && !is_sliding)
		this->LeAttack(enemy_attack_components, player_centre, message_system, modifier_system, ticks);
	
	this->ItemCollision(player_centre, message_system);
}

void CollisionSystem::PollSignals(
		MessageSystem& message_system, const Vector2 player_centre,
		const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(CollisionSystemSignal::COUNT); i++)
	{
		const uint16_t times = message_system.CollisionSystemSignals[i];
		
		auto signal_handler = this->SignalHandlers[i];

		if (times > 0)
			(this->*signal_handler)(player_centre, modifier_system, message_system, ticks);

		message_system.CollisionSystemSignals[i] = 0;
	}
}

void CollisionSystem::UpdateEnemyGrid(const std::vector<Vector2>& enemy_centre) noexcept
{
	for (size_t i = 0, n = this->GridSize; i < n; i++)
		this->EnemyGrid[i] = this->EmptyCell;

	for (size_t i = 0, n = enemy_centre.size(); i < n; i++)
	{
		const size_t index = this->GetMortonCode(enemy_centre[i].x, enemy_centre[i].y);
		
		if (index < this->GridSize)
			this->EnemyGrid[index] = i;
	}
}

void CollisionSystem::UpdateItemGrid(const std::vector<Vector2>& item_centre) noexcept
{
	for (size_t i = 0, n = this->GridSize; i < n; i++)
		this->ItemGrid[i] = this->EmptyCell;

	for (size_t i = 0, n = item_centre.size(); i < n; i++)
	{
		const size_t index = this->GetMortonCode(item_centre[i].x, item_centre[i].y);
		
		if (index < this->GridSize)
			this->ItemGrid[index] = i;
	}
}

void CollisionSystem::ProjectileCollision(
		const std::vector<Rectangle>& projectile_rect, const std::vector<ProjectileType>& projectile_types,
		const std::vector<Vector2>& projectile_direction, MessageSystem& message_system,
		const ModifierSystem& modifier_system, const size_t ticks
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
		
		float x = projectile_rect[i].x + projectile_rect[i].width / 2.0f;
		float y = projectile_rect[i].y + projectile_rect[i].height / 2.0f;

		const size_t index = this->GetMortonCode(x, y);

		if (index < this->GridSize && this->EnemyGrid[index] != this->EmptyCell)
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
		const std::vector<EnemyAttackComponent>& enemy_attack_components, const Vector2 player_centre,
		MessageSystem& message_system, const ModifierSystem& modifier_system, const size_t ticks
		) const noexcept
{
	const bool has_milk = modifier_system.EffectStatus(Effect::Milk);
	const size_t index = this->GetMortonCode(player_centre.x, player_centre.y);

	if (index < this->GridSize && this->EnemyGrid[index] != this->EmptyCell)
	{
		const size_t enemy_index = this->EnemyGrid[index];

		if (enemy_attack_components[enemy_index].CanLeAttack)
		{
			const float damage = enemy_attack_components[enemy_index].Damage;

			message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct EnemyLeAttacked>, enemy_index, ticks);
			message_system.PlayerCommands.emplace_back(std::in_place_type<struct DamagePlayer>, damage);
			message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, damage);
		}
	}
}

void CollisionSystem::SlideAttack(
		const Vector2 player_centre, const Vector2 player_direction,
		const std::vector<float>& enemy_health, MessageSystem& message_system,
		const size_t ticks
		) const noexcept
{
	const size_t index = this->GetMortonCode(player_centre.x, player_centre.y);

	if (index < this->GridSize && this->EnemyGrid[index] != this->EmptyCell)
	{
		const size_t enemy_index = this->EnemyGrid[index];
		const float damage_done = enemy_health[enemy_index];

		message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct DamageEnemy>, enemy_index, damage_done);

		message_system.ParticleSystemCommands.emplace_back(
				ticks, damage_done, player_direction, player_centre.x, player_centre.y,
				15, 25, 60, TICK_RATE / 2, 512, ORANGE, RED
				);

		message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, static_cast<unsigned int>(damage_done));
	}
}

void CollisionSystem::Aura(
		const Vector2 player_centre, const ModifierSystem& modifier_system,
		MessageSystem& message_system, const size_t ticks
		) const noexcept
{
	unsigned int total_hit = 0;

	const float aura_size = modifier_system.GetAttribute(Attribute::AuraSize);
	const float aura_damage = modifier_system.GetAttribute(Attribute::AuraDamage);

	const Rectangle aura = { player_centre.x - aura_size / 2.0f, player_centre.y - aura_size / 2.0f, aura_size, aura_size };

	for (size_t i = 0, n = static_cast<size_t>(aura.width / TILE_SIZE); i < n; i++)
	{
		for (size_t j = 0, m = static_cast<size_t>(aura.height / TILE_SIZE); j < m; j++)
		{
			const float x = aura.x + static_cast<float>(i * TILE_SIZE);
			const float y = aura.y + static_cast<float>(j * TILE_SIZE);

			const size_t index = this->GetMortonCode(x, y);

			if (index < this->GridSize && this->EnemyGrid[index] != this->EmptyCell)
			{
				message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct DamageEnemy>, this->EnemyGrid[index], aura_damage);

				const Vector2 velocity = {
					static_cast<float>(GetRandomValue(-192, 192)),
					static_cast<float>(GetRandomValue(-192, 192))
				};

				message_system.ParticleSystemCommands.emplace_back(
						ticks, aura_damage, velocity, x, y,
						15, 30, 60, TICK_RATE / 2, 256, PURPLE, RED
						);

				total_hit++;
			}
		}
	}

	message_system.StatSystemCommands.emplace_back(Stat::TotalDamage, total_hit * static_cast<unsigned int>(aura_damage));
}

void CollisionSystem::ItemCollision(const Vector2 player_centre, MessageSystem& message_system) const noexcept
{
	const size_t index = this->GetMortonCode(player_centre.x, player_centre.y);

	if (index < this->GridSize && this->ItemGrid[index] != this->EmptyCell)
	{
		const size_t item_index = this->ItemGrid[index];

		message_system.ItemSystemCommands.emplace_back(std::in_place_type<struct CollidedWithItem>, item_index);
	}
}

void CollisionSystem::EnemyItemCollision(
		MessageSystem& message_system, const std::vector<Vector2>& enemy_centre,
		const std::vector<EnemyType>& enemy_types
		) const noexcept
{
	for (size_t i = 0, n = enemy_centre.size(); i < n; i++)
	{
		const size_t index = this->GetMortonCode(enemy_centre[i].x, enemy_centre[i].y);

		if (index < this->GridSize && this->ItemGrid[index] != this->EmptyCell)
		{
			const size_t item_index = this->ItemGrid[index];
			const EnemyType enemy_type = enemy_types[i];

			message_system.ItemSystemCommands.emplace_back(std::in_place_type<struct EnemyItemCollision>, item_index, i, enemy_type);
		}
	}
}

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

	bits = (bits | (bits << 4)) & 0x0F0F;
	bits = (bits | (bits << 2)) & 0x3333;
	bits = (bits | (bits << 1)) & 0x5555;

	return bits;
}

inline size_t CollisionSystem::GetMortonCode(const float x, const float y) const noexcept
{
	uint16_t x_tile = static_cast<uint16_t>(x / TILE_SIZE);
	uint16_t y_tile = static_cast<uint16_t>(y / TILE_SIZE);

	x_tile = this->SeparateBits(x_tile);
	y_tile = this->SeparateBits(y_tile);

	return static_cast<size_t>(x_tile | (y_tile << 1));
}
