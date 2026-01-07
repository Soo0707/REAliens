#include "projectileSystem.hpp"

#include <algorithm>
#include <cstddef>

#include "raylib.h"
#include "assetManager.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"

ProjectileSystem::ProjectileSystem()
{
	this->Projectiles.reserve(1024);
}

void ProjectileSystem::Reset() noexcept
{
	this->Projectiles.clear();
}

void ProjectileSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.ProjectileSystemCommands)
		this->Projectiles.emplace_back(command.X, command.Y, command.Speed, command.Scale, command.Direction, command.Type, assets);

	message_system.ProjectileSystemCommands.clear();
}

void ProjectileSystem::UpdateProjectiles(const size_t ticks, const Rectangle update_area, MessageSystem& message_system) noexcept
{
	unsigned int total_damage_done = 0;

	const bool spawn_particle = GetRandomValue(1, 100) < 5;

	for (auto &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
		{
			projectile.Update();
/*
			const unsigned int damage = Collisions::ProjectileCollision(projectile, this->Enemies, ticks, this->Attributes); 

			if (damage > 0)
			{
				total_damage_done += damage;

				message_system.GameTextSystemCommands.emplace_back(ticks, ticks + TICK_RATE / 4, projectile.Rect.x, projectile.Rect.y, 48.0f, damage, 48, YELLOW);
				message_system.ParticleSystemCommands.emplace_back(ticks, 5, projectile.Direction, projectile.Rect.x, projectile.Rect.y, 5, 20, 120, TICK_RATE, 256, RED, RED);
			}
			*/

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

	std::erase_if(this->Projectiles, [](const Projectile& proj){ return proj.Kill; });
/*
	if (this->Effects.count(Effect::LifeSteal))
		this->Player->IncreaseHealth( total_damage_done * this->Attributes.at(Attribute::LifeStealMultiplier) );

	this->Stats[static_cast<size_t>(Stat::TotalDamage)] += total_damage_done;
*/
}

void ProjectileSystem::Draw(const Rectangle update_area) const noexcept
{
	for (auto const &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
			projectile.Draw();
	}
}

void ProjectileSystem::DrawLightmap(const Rectangle update_area) const noexcept
{
	for (auto const &projectile : this->Projectiles)
	{
		if (CheckCollisionRecs(update_area, projectile.Rect))
			projectile.DrawLightmap();
	}
}
