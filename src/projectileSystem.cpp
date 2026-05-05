/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "projectileSystem.hpp"

#include <cstdint>
#include <cstddef>
#include <limits>
#include <cmath>

#include "raylib.h"
#include "raymath.h"

#include "constants.hpp"
#include "assetManager.hpp"

#include "projectileData.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifiers.hpp"
#include "modifierSystem.hpp"

ProjectileSystem::ProjectileSystem()
{
	this->ProjectileIsVisible.reserve(128);
	this->ProjectileSpeed.reserve(128);
	this->ProjectileRotation.reserve(128);
	this->ProjectileHitsLeft.reserve(128);
	this->ProjectileRect.reserve(128);
	this->ProjectileCentre.reserve(128);
	this->ProjectileDirection.reserve(128);
	this->ProjectileTypes.reserve(128);
}

void ProjectileSystem::Reset() noexcept
{
	this->ProjectileIsVisible.clear();
	this->ProjectileSpeed.clear();
	this->ProjectileRotation.clear();
	this->ProjectileHitsLeft.clear();
	this->ProjectileRect.clear();
	this->ProjectileCentre.clear();
	this->ProjectileDirection.clear();
	this->ProjectileTypes.clear();
}

void ProjectileSystem::Update(
		MessageSystem& message_system, const AssetManager& assets, const ModifierSystem& modifier_system,
		const Rectangle& update_area, const float map_width, const float map_height, const size_t ticks
		) noexcept
{
	this->ExecuteCommands(message_system, modifier_system, assets);

	this->VisibilityCheck(update_area);
	this->MoveProjectiles(map_width, map_height);
	this->SpawnParticles(message_system, ticks);
	this->KillProjectiles();
}

void ProjectileSystem::ExecuteCommands(MessageSystem& message_system, const ModifierSystem& modifier_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.ProjectileSystemCommands)
	{
		const size_t handler_index = command.index();

		auto handler_function = this->CommandHandlers[handler_index];
		(this->*handler_function)(message_system, modifier_system, assets, command);
	}

	message_system.ProjectileSystemCommands.clear();
}

const std::vector<Vector2>& ProjectileSystem::GetProjectileCentres() const noexcept
{
	return this->ProjectileCentre;
}

const std::vector<ProjectileType>& ProjectileSystem::GetProjectileTypes() const noexcept
{
	return this->ProjectileTypes;
}

const std::vector<Vector2>& ProjectileSystem::GetProjectileDirection() const noexcept
{
	return this->ProjectileDirection;
}

const std::vector<float>& ProjectileSystem::GetProjectileRotation() const noexcept
{
	return this->ProjectileRotation;
}

size_t ProjectileSystem::GetEntityCount() const noexcept
{
	return this->ProjectileTypes.size();
}

void ProjectileSystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		if (this->ProjectileIsVisible[i])
		{
			const Vector2 position = { this->ProjectileRect[i].x, this->ProjectileRect[i].y };

			const size_t type_index = static_cast<size_t>(this->ProjectileTypes[i]);
			const TextureKey texture_key = this->ProjectileAttributes[type_index].Texture;

			DrawTextureEx(
					assets.GetTexture(texture_key),
					position,
					this->ProjectileRotation[i],
					1.0f,
					WHITE
					);
		}
	}
}

void ProjectileSystem::DrawLightmap() const noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		if (this->ProjectileIsVisible[i])
		{
			const size_t type_index = static_cast<size_t>(this->ProjectileTypes[i]);
			const Color projectile_colour = this->ProjectileAttributes[type_index].Colour;

			DrawCircleGradient(
					this->ProjectileRect[i].x, this->ProjectileRect[i].y,
					this->ProjectileRect[i].width * 2.0f,
					projectile_colour, LIGHTGRAY);
		}
	}
}

void ProjectileSystem::CreateProjectile(
		const float x, const float y, const Vector2 direction, const ProjectileType type,
		const ModifierSystem& modifier_system, const AssetManager& assets
		) noexcept
{
	if (this->GetEntityCount() + 1 >= static_cast<size_t>(std::numeric_limits<uint32_t>::max()))
		return;

	const size_t type_index = static_cast<size_t>(type);
	const TextureKey texture_key = this->ProjectileAttributes[type_index].Texture;

	const float texture_width = assets.GetTexture(texture_key).width;
	const float texture_height = assets.GetTexture(texture_key).height;

	const Attribute speed_key = this->ProjectileAttributes[type_index].SpeedKey;
	const Attribute penetration_power_key = this->ProjectileAttributes[type_index].PenetrationPowerKey;

	float speed = static_cast<float>(modifier_system.GetAttribute(speed_key));
	uint16_t max_hits = static_cast<uint16_t>(modifier_system.GetAttribute(penetration_power_key));

	this->ProjectileIsVisible.emplace_back(static_cast<uint8_t>(false));
	this->ProjectileSpeed.emplace_back(speed);
	this->ProjectileRotation.emplace_back(atan2(direction.y, direction.x) * TO_DEG);
	this->ProjectileHitsLeft.emplace_back(max_hits);
	this->ProjectileRect.emplace_back(x, y, texture_width, texture_height);
	this->ProjectileCentre.emplace_back(x + texture_width / 2.0f, y + texture_height / 2.0f);
	this->ProjectileDirection.emplace_back(Vector2Normalize(direction));
	this->ProjectileTypes.emplace_back(type);
}

void ProjectileSystem::VisibilityCheck(const Rectangle& update_area) noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
		this->ProjectileIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->ProjectileRect[i]));
}

void ProjectileSystem::MoveProjectiles(const float map_width, const float map_height) noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		this->ProjectileRect[i].x += this->ProjectileDirection[i].x * this->ProjectileSpeed[i] * TICK_TIME;
		this->ProjectileRect[i].y += this->ProjectileDirection[i].y * this->ProjectileSpeed[i] * TICK_TIME;

		if (this->ProjectileRect[i].x < 0.0f)
			this->ProjectileRect[i].x = map_width - this->ProjectileRect[i].width;
		else if (this->ProjectileRect[i].x + this->ProjectileRect[i].width > map_width)
			this->ProjectileRect[i].x = 0.0f;

		if (this->ProjectileRect[i].y < 0.0f)
			this->ProjectileRect[i].y = map_height - this->ProjectileRect[i].height;
		else if (this->ProjectileRect[i].y + this->ProjectileRect[i].height > map_height)
			this->ProjectileRect[i].y = 0.0f;

		this->ProjectileCentre[i].x = this->ProjectileRect[i].x + this->ProjectileRect[i].width / 2.0f;
		this->ProjectileCentre[i].y = this->ProjectileRect[i].y + this->ProjectileRect[i].height / 2.0f;
	}
}

void ProjectileSystem::KillProjectiles() noexcept
{
	for (size_t i = 0; i < this->ProjectileHitsLeft.size(); )
	{
		if (this->ProjectileHitsLeft[i] == 0)
		{
			this->ProjectileIsVisible[i] = this->ProjectileIsVisible.back();
			this->ProjectileSpeed[i] = this->ProjectileSpeed.back();
			this->ProjectileRotation[i] = this->ProjectileRotation.back();
			this->ProjectileHitsLeft[i] = this->ProjectileHitsLeft.back();
			this->ProjectileRect[i] = this->ProjectileRect.back();
			this->ProjectileCentre[i] = this->ProjectileCentre.back();
			this->ProjectileDirection[i] = this->ProjectileDirection.back();
			this->ProjectileTypes[i] = this->ProjectileTypes.back();


			this->ProjectileIsVisible.pop_back();
			this->ProjectileSpeed.pop_back();
			this->ProjectileRotation.pop_back();
			this->ProjectileHitsLeft.pop_back();
			this->ProjectileRect.pop_back();
			this->ProjectileCentre.pop_back();
			this->ProjectileDirection.pop_back();
			this->ProjectileTypes.pop_back();
		}
		else
			i++;
	}
}

void ProjectileSystem::SpawnParticles(MessageSystem& message_system, const size_t ticks) const noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		const bool spawn_particle = GetRandomValue(1, 100) < 10;

		if (spawn_particle && this->ProjectileIsVisible[i])
		{
			const size_t type_index = static_cast<size_t>(this->ProjectileTypes[i]);
			const Color projectile_colour = this->ProjectileAttributes[type_index].Colour;

			message_system.ParticleSystemCommands.emplace_back(
					ticks, 1, this->ProjectileDirection[i], this->ProjectileRect[i].x, this->ProjectileRect[i].y, 
					5, 20, 120, TICK_RATE, 128, projectile_colour, projectile_colour
					);
		}
	}
}

void ProjectileSystem::CreateProjectileHandler(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const AssetManager& assets, const ProjectileSystemCommand& command
		) noexcept
{
	const struct CreateProjectile& data = std::get<struct CreateProjectile>(command);

	this->CreateProjectile(data.X, data.Y, data.Direction, data.Type, modifier_system, assets);
}


void ProjectileSystem::ProjectileHitHandler(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const AssetManager& assets, const ProjectileSystemCommand& command
		) noexcept
{
	const struct ProjectileHit& data = std::get<struct ProjectileHit>(command);
	const uint32_t index = data.ProjectileIndex;

	if (!this->CheckIndex(index))
		return;

	if (this->ProjectileHitsLeft[index] == 0)
		return;

	const size_t type_index = static_cast<size_t>(this->ProjectileTypes[index]);

	auto after_hit_handler = this->AfterProjectileHitHandlers[type_index];

	if (after_hit_handler)
		(this->*after_hit_handler)(message_system, modifier_system, assets, data);

	this->ProjectileHitsLeft[index]--;
}

void ProjectileSystem::BallHitHandler(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const AssetManager& assets, const ProjectileHit& data
		) noexcept
{
	if (modifier_system.IsLucky() || true)
	{
		const uint32_t index = data.ProjectileIndex;

		const Rectangle ball_rect = this->ProjectileRect[index];
		const ssize_t lazer_spawn_amount = static_cast<ssize_t>(modifier_system.GetAttribute(Attribute::BallSplitLazers) - 1) / 2;

		for (ssize_t i = -lazer_spawn_amount; i < lazer_spawn_amount; i++)
		{
			Vector2 ball_direction = this->ProjectileDirection[index];

			if (i != 0)
				ball_direction = Vector2Rotate(ball_direction, static_cast<float>(i) * 10.0f * TO_RAD);

			this->CreateProjectile(
					ball_rect.x, ball_rect.y, ball_direction, ProjectileType::Lazer,
					modifier_system, assets
					);
		}
	}
}

void ProjectileSystem::GlueHitHandler(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const AssetManager& assets, const struct ProjectileHit& data
		) noexcept
{
	message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct GlueEnemy>, data.EnemyIndex);
}

void ProjectileSystem::PlebifierHitHandler(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const AssetManager& assets, const struct ProjectileHit& data
		) noexcept
{
	message_system.EnemySystemCommands.emplace_back(std::in_place_type<struct PlebifyEnemy>, data.EnemyIndex);
}

bool ProjectileSystem::CheckIndex(const uint32_t index) const noexcept
{
	return (index < this->ProjectileTypes.size());
}
