#include "particleSystem.hpp"

#include <algorithm>
#include <cstddef>

#include "raylib.h"
#include "particle.hpp"
#include "assetManager.hpp"
#include "messageSystem.hpp"

ParticleSystem::ParticleSystem()
{
	this->Particles.reserve(1024);
}

void ParticleSystem::Reset() noexcept
{
	this->Particles.clear();
}

void ParticleSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.ParticleSystemCommands)
	{
		for (size_t i = 0; i < command.Number; i++)
		{
			const float size = static_cast<float>(GetRandomValue(command.MinSize, command.MaxSize));
			const float rotation = static_cast<float>(GetRandomValue(0, 90));
			const size_t expiry = command.Ticks + static_cast<size_t>(GetRandomValue(command.MinLifetime, command.MaxLifetime));
			Vector2 velocity = command.Velocity;

			velocity.x += static_cast<float>(GetRandomValue(-command.MaxSpeed, command.MaxSpeed));
			velocity.y += static_cast<float>(GetRandomValue(-command.MaxSpeed, command.MaxSpeed));

			this->Particles.emplace_back(
					command.X, command.Y, size, rotation, command.Ticks,
					expiry, velocity, command.StartColour, command.EndColour, assets
					);
		}
	}

	message_system.ParticleSystemCommands.clear();
}

void ParticleSystem::UpdateParticles(const Rectangle update_area, const size_t ticks) noexcept
{
	for (auto &particle : this->Particles)
	{
		if (CheckCollisionRecs(update_area, particle.Rect))
			particle.Update(ticks);
	}

	std::erase_if(this->Particles, [ticks](const Particle& particle) { return (ticks >= particle.Expiry); });
}

void ParticleSystem::Draw(const Rectangle update_area, const size_t ticks) const noexcept
{
	for (auto const &particle : this->Particles)
	{
		if (CheckCollisionRecs(update_area, particle.Rect))
			particle.Draw(ticks);
	}
}
