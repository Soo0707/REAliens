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

ProjectileSystem::ProjectileSystem()
{
	this->ProjectileIsVisible.reserve(1024);
	this->ProjectileSpeed.reserve(1024);
	this->ProjectileRotation.reserve(1024);
	this->ProjectileScale.reserve(1024);
	this->ProjectileRect.reserve(1024);
	this->ProjectileDirection.reserve(1024);
	this->ProjectileColour.reserve(1024);
	this->ProjectileTexture.reserve(1024);
	this->ProjectileTypes.reserve(1024);
}

void ProjectileSystem::Reset() noexcept
{
	this->ProjectileIsVisible.clear();
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
	for (auto const& command : message_system.ProjectileSystemCommands)
	{
		const size_t index = static_cast<size_t>(command.Type);
		const TextureKey texture_key = this->ProjectileAttributes[index].Texture;

		const float texture_width = assets.Textures.at(texture_key).width;
		const float texture_height = assets.Textures.at(texture_key).height;

		this->CreateProjectile(command.X, command.Y, command.Speed, command.Scale, command.Direction, command.Type, texture_width, texture_height);
	}

	message_system.ProjectileSystemCommands.clear();
}

void ProjectileSystem::UpdateProjectiles(const size_t ticks, const Rectangle update_area, MessageSystem& message_system) noexcept
{
/*
	unsigned int total_damage_done = 0;

	for (auto &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
		{
			projectile.Update();
			const unsigned int damage = Collisions::ProjectileCollision(projectile, this->Enemies, ticks, this->Attributes); 

			if (damage > 0)
			{
				total_damage_done += damage;

				message_system.GameTextSystemCommands.emplace_back(ticks, ticks + TICK_RATE / 4, projectile.Rect.x, projectile.Rect.y, 48.0f, damage, 48, YELLOW);
				message_system.ParticleSystemCommands.emplace_back(ticks, 5, projectile.Direction, projectile.Rect.x, projectile.Rect.y, 5, 20, 120, TICK_RATE, 256, RED, RED);
			}

			if (spawn_particle)
			{
				message_system.ParticleSystemCommands.emplace_back(
						ticks, 1, projectile.Direction, projectile.Rect.x, projectile.Rect.y, 
						5, 20, 120, TICK_RATE, 128, projectile.Colour, projectile.Colour
						);
			}
		}
		else
			projectile.Kill = true;
	}

	if (this->Effects.count(Effect::LifeSteal))
		this->Player->IncreaseHealth( total_damage_done * this->Attributes.at(Attribute::LifeStealMultiplier) );

	this->Stats[static_cast<size_t>(Stat::TotalDamage)] += total_damage_done;
*/
	this->VisibilityCheck(update_area);
	this->MoveProjectiles();
	this->SpawnParticles(message_system, ticks);
	this->RemoveProjectiles();

}

void ProjectileSystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->ProjectileRect.size(); i < n; i++)
	{
		if (this->ProjectileIsVisible[i])
		{
			const Vector2 position = { this->ProjectileRect[i].x, this->ProjectileRect[i].y };

			DrawTextureEx(
					assets.Textures.at(this->ProjectileTexture[i]),
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
	//std::max(this->ProjectileRect[i].width, this->ProjectileRect[i].height) * 2.0f, 	}
}

void ProjectileSystem::CreateProjectile(
		const float x, const float y, const float speed, const float scale, const Vector2 direction,
		const ProjectileType type, const float texture_width, const float texture_height
		) noexcept
{
	const size_t index = static_cast<size_t>(type);

	this->ProjectileIsVisible.emplace_back(static_cast<uint8_t>(false));
	this->ProjectileSpeed.emplace_back(speed);

	this->ProjectileRotation.emplace_back(atan2(direction.y, direction.x) * TO_DEG);
	this->ProjectileScale.emplace_back(scale);

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

void ProjectileSystem::RemoveProjectiles() noexcept
{
	for (size_t i = 0; i < this->ProjectileIsVisible.size(); )
	{
		if (!this->ProjectileIsVisible[i])
		{
			this->ProjectileIsVisible[i] = this->ProjectileIsVisible.back();
			this->ProjectileSpeed[i] = this->ProjectileSpeed.back();
			this->ProjectileRotation[i] = this->ProjectileRotation.back();
			this->ProjectileScale[i] = this->ProjectileScale.back();
			this->ProjectileRect[i] = this->ProjectileRect.back();
			this->ProjectileDirection[i] = this->ProjectileDirection.back();
			this->ProjectileColour[i] = this->ProjectileColour.back();
			this->ProjectileTexture[i] = this->ProjectileTexture.back();
			this->ProjectileTypes[i] = this->ProjectileTypes.back();


			this->ProjectileIsVisible.pop_back();
			this->ProjectileSpeed.pop_back();
			this->ProjectileRotation.pop_back();
			this->ProjectileScale.pop_back();
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
			message_system.ParticleSystemCommands.emplace_back(
					ticks, 1, this->ProjectileDirection[i], this->ProjectileRect[i].x, this->ProjectileRect[i].y, 
					5, 20, 120, TICK_RATE, 128, this->ProjectileColour[i], this->ProjectileColour[i]
					);
		}
	}
}
