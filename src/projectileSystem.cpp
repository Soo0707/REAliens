#include "projectileSystem.hpp"

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <mutex>

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
	this->ProjectileIsVisible.reserve(256);
	this->ProjectileKill.reserve(256);
	this->ProjectileSpeed.reserve(256);
	this->ProjectileRotation.reserve(256);
	this->ProjectileScale.reserve(256);
	this->ProjectileRect.reserve(256);
	this->ProjectileDirection.reserve(256);
	this->ProjectileColour.reserve(256);
	this->ProjectileTexture.reserve(256);
	this->ProjectileTypes.reserve(256);
}

void ProjectileSystem::Reset() noexcept
{
	this->ProjectileIsVisible.clear();
	this->ProjectileKill.clear();
	this->ProjectileSpeed.clear();
	this->ProjectileRotation.clear();
	this->ProjectileScale.clear();
	this->ProjectileRect.clear();
	this->ProjectileDirection.clear();
	this->ProjectileColour.clear();
	this->ProjectileTexture.clear();
	this->ProjectileTypes.clear();
}


void ProjectileSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	{
		std::lock_guard<std::mutex> lock(message_system.ProjectileSystemMutex);
		message_system.ProjectileSystemCommandsRead.swap(message_system.ProjectileSystemCommandsWrite);
	}

	for (auto const& command : message_system.ProjectileSystemCommandsRead)
	{
		const size_t handler_index = command.index();

		auto handler_function = this->CommandHandlers[handler_index];
		(this->*handler_function)(command, assets);
	}

	message_system.ProjectileSystemCommandsRead.clear();
}

void ProjectileSystem::UpdateProjectiles(
		const size_t ticks, const Rectangle update_area,
		MessageSystem& message_system, const ModifierSystem& modifier_system
		) noexcept
{
	this->VisibilityCheck(update_area);
	this->MoveProjectiles();
	this->SpawnParticles(message_system, ticks);
	this->EvaluateHitCount(modifier_system);
	this->RemoveProjectiles();
}

const std::vector<Rectangle>& ProjectileSystem::GetProjectileRect() const noexcept
{
	return this->ProjectileRect;
}

const std::vector<ProjectileType>& ProjectileSystem::GetProjectileType() const noexcept
{
	return this->ProjectileTypes;
}

const std::vector<Vector2>& ProjectileSystem::GetProjectileDirection() const noexcept
{
	return this->ProjectileDirection;
}

void ProjectileSystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		if (this->ProjectileIsVisible[i])
		{
			const Vector2 position = { this->ProjectileRect[i].x, this->ProjectileRect[i].y };

			DrawTextureEx(
					assets.GetTexture(this->ProjectileTexture[i]),
					position,
					this->ProjectileRotation[i],
					this->ProjectileScale[i],
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
		const float x, const float y, const float speed, const float scale, const Vector2 direction,
		const ProjectileType type, const float texture_width, const float texture_height
		) noexcept
{
	const size_t index = static_cast<size_t>(type);

	this->ProjectileIsVisible.emplace_back(static_cast<uint8_t>(false));
	this->ProjectileKill.emplace_back(static_cast<uint8_t>(false));
	this->ProjectileSpeed.emplace_back(speed);

	this->ProjectileRotation.emplace_back(atan2(direction.y, direction.x) * TO_DEG);
	this->ProjectileScale.emplace_back(scale);
	this->ProjectileHitCount.emplace_back(0);

	this->ProjectileRect.emplace_back(x, y, texture_width * scale, texture_height * scale);
	this->ProjectileDirection.emplace_back(Vector2Normalize(direction));

	this->ProjectileColour.emplace_back(this->ProjectileAttributes[index].Colour);
	this->ProjectileTexture.emplace_back(this->ProjectileAttributes[index].Texture);

	this->ProjectileTypes.emplace_back(type);
}

void ProjectileSystem::VisibilityCheck(const Rectangle update_area) noexcept
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
	}
}

void ProjectileSystem::EvaluateHitCount(const ModifierSystem& modifier_system) noexcept
{
	for (size_t i = 0, n = this->ProjectileHitCount.size(); i < n; i++)
	{
		const uint16_t hit_count = this->ProjectileHitCount[i];
		if (hit_count == 0)
			continue;

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
			this->ProjectileScale[i] = this->ProjectileScale.back();
			this->ProjectileHitCount[i] = this->ProjectileHitCount.back();
			this->ProjectileRect[i] = this->ProjectileRect.back();
			this->ProjectileDirection[i] = this->ProjectileDirection.back();
			this->ProjectileColour[i] = this->ProjectileColour.back();
			this->ProjectileTexture[i] = this->ProjectileTexture.back();
			this->ProjectileTypes[i] = this->ProjectileTypes.back();


			this->ProjectileIsVisible.pop_back();
			this->ProjectileKill.pop_back();
			this->ProjectileSpeed.pop_back();
			this->ProjectileRotation.pop_back();
			this->ProjectileScale.pop_back();
			this->ProjectileHitCount.pop_back();
			this->ProjectileRect.pop_back();
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
			std::lock_guard<std::mutex> lock(message_system.ParticleSystemMutex);
			message_system.ParticleSystemCommandsWrite.emplace_back(
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

	this->CreateProjectile(data.X, data.Y, data.Speed, data.Scale, data.Direction, data.Type, texture_width, texture_height);
}

void ProjectileSystem::ProjectileHitHandler(const ProjectileSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct ProjectileHit& data = std::get<struct ProjectileHit>(command);
	const size_t index = data.ProjectileIndex;

	if (!this->CheckIndex(index))
		return;

	switch (this->ProjectileTypes[index])
	{
		case ProjectileType::Ball:
		{
			const TextureKey ball_texture_key = this->ProjectileAttributes[static_cast<size_t>(ProjectileType::Ball)].Texture;
			const float ball_size = assets.GetTexture(ball_texture_key).width;
			const Rectangle ball_rect = this->ProjectileRect[index];
			const float ball_speed = 1.25f * this->ProjectileSpeed[index];
			const float ball_scale = 0.75f * this->ProjectileScale[index];

			for (uint8_t i = 0; i < 2; i++)
			{
				const float up_down = (i % 2) ? 1.0f : -1.0f;
				const Vector2 ball_direction = Vector2Rotate(this->ProjectileDirection[index], up_down * 10.0f * TO_RAD);

				this->CreateProjectile(
						ball_rect.x, ball_rect.y, ball_speed, ball_scale,
						ball_direction, ProjectileType::Ball, ball_size, ball_size
						);
			}
		}
		case ProjectileType::Bullet:
			this->ProjectileKill[index] = static_cast<uint8_t>(true);
			break;
	}

	this->ProjectileHitCount[index]++;
}

bool ProjectileSystem::CheckIndex(const size_t index) const noexcept
{
	return (index <= this->ProjectileKill.size() - 1);
}
