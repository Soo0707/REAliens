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
	this->ProjectileKill.reserve(128);
	this->ProjectileSpeed.reserve(128);
	this->ProjectileRotation.reserve(128);
	this->ProjectileHitCount.reserve(128);
	this->ProjectileRect.reserve(128);
	this->ProjectileCentre.reserve(128);
	this->ProjectileDirection.reserve(128);
	this->ProjectileColour.reserve(128);
	this->ProjectileTexture.reserve(128);
	this->ProjectileTypes.reserve(128);
}

void ProjectileSystem::Reset() noexcept
{
	this->ProjectileIsVisible.clear();
	this->ProjectileKill.clear();
	this->ProjectileSpeed.clear();
	this->ProjectileRotation.clear();
	this->ProjectileHitCount.clear();
	this->ProjectileRect.clear();
	this->ProjectileCentre.clear();
	this->ProjectileDirection.clear();
	this->ProjectileColour.clear();
	this->ProjectileTexture.clear();
	this->ProjectileTypes.clear();
}

void ProjectileSystem::Update(
		MessageSystem& message_system, const AssetManager& assets, const ModifierSystem& modifier_system,
		const Rectangle& update_area, const size_t ticks
		) noexcept
{
	this->ExecuteCommands(message_system, assets);

	this->VisibilityCheck(update_area);
	this->MoveProjectiles();
	this->SpawnParticles(message_system, ticks);
	this->EvaluateHitCount(modifier_system);
	this->RemoveProjectiles();
}

void ProjectileSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.ProjectileSystemCommands)
	{
		const size_t handler_index = command.index();

		auto handler_function = this->CommandHandlers[handler_index];
		(this->*handler_function)(command, assets);
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

void ProjectileSystem::Draw(const AssetManager& assets) const noexcept
{
	// TODO: get texture from type
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		if (this->ProjectileIsVisible[i])
		{
			const Vector2 position = { this->ProjectileRect[i].x, this->ProjectileRect[i].y };

			DrawTextureEx(
					assets.GetTexture(this->ProjectileTexture[i]),
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
			DrawCircleGradient(
					this->ProjectileRect[i].x, this->ProjectileRect[i].y,
					this->ProjectileRect[i].width * 2.0f,
					this->ProjectileColour[i], LIGHTGRAY);
		}
	}
}

void ProjectileSystem::CreateProjectile(
		const float x, const float y, const float speed, const Vector2 direction,
		const ProjectileType type, const float texture_width, const float texture_height
		) noexcept
{
	const size_t index = static_cast<size_t>(type);

	this->ProjectileIsVisible.emplace_back(static_cast<uint8_t>(false));
	this->ProjectileKill.emplace_back(static_cast<uint8_t>(false));
	this->ProjectileSpeed.emplace_back(speed);

	this->ProjectileRotation.emplace_back(atan2(direction.y, direction.x) * TO_DEG);
	this->ProjectileHitCount.emplace_back(0);

	this->ProjectileRect.emplace_back(x, y, texture_width, texture_height);
	this->ProjectileCentre.emplace_back(x + texture_width / 2.0f, y + texture_height / 2.0f);
	this->ProjectileDirection.emplace_back(Vector2Normalize(direction));

	this->ProjectileColour.emplace_back(this->ProjectileAttributes[index].Colour);
	this->ProjectileTexture.emplace_back(this->ProjectileAttributes[index].Texture);

	this->ProjectileTypes.emplace_back(type);
}

void ProjectileSystem::VisibilityCheck(const Rectangle& update_area) noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
		this->ProjectileIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->ProjectileRect[i]));
}

void ProjectileSystem::MoveProjectiles() noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		this->ProjectileRect[i].x += this->ProjectileDirection[i].x * this->ProjectileSpeed[i] * TICK_TIME;
		this->ProjectileRect[i].y += this->ProjectileDirection[i].y * this->ProjectileSpeed[i] * TICK_TIME;

		this->ProjectileCentre[i].x = this->ProjectileRect[i].x + this->ProjectileRect[i].width / 2.0f;
		this->ProjectileCentre[i].y = this->ProjectileRect[i].y + this->ProjectileRect[i].height / 2.0f;
	}
}

void ProjectileSystem::EvaluateHitCount(const ModifierSystem& modifier_system) noexcept
{
	for (size_t i = 0, n = this->ProjectileHitCount.size(); i < n; i++)
	{
		const uint16_t hit_count = this->ProjectileHitCount[i];

		if (hit_count == 0)
			continue;
		// TODO : projectile mark hit effect 
		switch (this->ProjectileTypes[i])
		{
			case ProjectileType::Lazer:
			{
				const uint16_t max_hit_count = modifier_system.GetAttribute(Attribute::LazerMaxHit);

				if (hit_count >= max_hit_count)
					this->ProjectileKill[i] = static_cast<uint8_t>(true);
				break;
			}
			default:
				this->ProjectileKill[i] = static_cast<uint8_t>(true);
		}
	}
}

void ProjectileSystem::RemoveProjectiles() noexcept
{
	for (size_t i = 0; i < this->ProjectileIsVisible.size(); )
	{
		if (!this->ProjectileIsVisible[i] || this->ProjectileKill[i])
		{
			this->ProjectileIsVisible[i] = this->ProjectileIsVisible.back();
			this->ProjectileKill[i] = this->ProjectileKill.back();
			this->ProjectileSpeed[i] = this->ProjectileSpeed.back();
			this->ProjectileRotation[i] = this->ProjectileRotation.back();
			this->ProjectileHitCount[i] = this->ProjectileHitCount.back();
			this->ProjectileRect[i] = this->ProjectileRect.back();
			this->ProjectileCentre[i] = this->ProjectileCentre.back();
			this->ProjectileDirection[i] = this->ProjectileDirection.back();
			this->ProjectileColour[i] = this->ProjectileColour.back();
			this->ProjectileTexture[i] = this->ProjectileTexture.back();
			this->ProjectileTypes[i] = this->ProjectileTypes.back();


			this->ProjectileIsVisible.pop_back();
			this->ProjectileKill.pop_back();
			this->ProjectileSpeed.pop_back();
			this->ProjectileRotation.pop_back();
			this->ProjectileHitCount.pop_back();
			this->ProjectileRect.pop_back();
			this->ProjectileCentre.pop_back();
			this->ProjectileDirection.pop_back();
			this->ProjectileColour.pop_back();
			this->ProjectileTexture.pop_back();
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
			message_system.ParticleSystemCommands.emplace_back(
					ticks, 1, this->ProjectileDirection[i], this->ProjectileRect[i].x, this->ProjectileRect[i].y, 
					5, 20, 120, TICK_RATE, 128, this->ProjectileColour[i], this->ProjectileColour[i]
					);
		}
	}
}

void ProjectileSystem::CreateProjectileHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct CreateProjectile& data = std::get<struct CreateProjectile>(command);

	const size_t index = static_cast<size_t>(data.Type);
	const TextureKey texture_key = this->ProjectileAttributes[index].Texture;

	const float texture_width = assets.GetTexture(texture_key).width;
	const float texture_height = assets.GetTexture(texture_key).height;

	this->CreateProjectile(data.X, data.Y, data.Speed, data.Direction, data.Type, texture_width, texture_height);
}


void ProjectileSystem::ProjectileHitHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct ProjectileHit& data = std::get<struct ProjectileHit>(command);
	const size_t index = data.ProjectileIndex;

	if (!this->CheckIndex(index))
		return;

	if (this->ProjectileTypes[index] == ProjectileType::Ball)
	{
		const TextureKey ball_texture_key = this->ProjectileAttributes[static_cast<size_t>(ProjectileType::Ball)].Texture;
		const float ball_size = assets.GetTexture(ball_texture_key).width;
		const Rectangle ball_rect = this->ProjectileRect[index];
		const float ball_speed = this->ProjectileSpeed[index];

		for (uint8_t i = 0; i < 2; i++)
		{
			const float up_down = (i % 2) ? 1.0f : -1.0f;
			const Vector2 ball_direction = Vector2Rotate(this->ProjectileDirection[index], up_down * 10.0f * TO_RAD);

			this->CreateProjectile(
					ball_rect.x, ball_rect.y, ball_speed, ball_direction,
					ProjectileType::Ball, ball_size, ball_size
					);
		}
	}

	this->ProjectileHitCount[index]++;
}

bool ProjectileSystem::CheckIndex(const size_t index) const noexcept
{
	return (index < this->ProjectileKill.size());
}
