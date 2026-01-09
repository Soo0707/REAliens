#include "particleSystem.hpp"

#include <cstddef>

#include "raylib.h"
#include "assetManager.hpp"
#include "messageSystem.hpp"

ParticleSystem::ParticleSystem()
{
	this->ParticleIsVisible.reserve(1024);
	this->ParticleCreation.reserve(1024);
	this->ParticleExpiry.reserve(1024);
	this->ParticleRotation.reserve(1024);
	this->ParticleScale.reserve(1024);
	this->ParticleVelocity.reserve(1024);
	this->ParticleStartColour.reserve(1024);
	this->ParticleEndColour.reserve(1024);
	this->ParticleRect.reserve(1024);
}

void ParticleSystem::Reset() noexcept
{
	this->ParticleIsVisible.clear();
	this->ParticleCreation.clear();
	this->ParticleExpiry.clear();
	this->ParticleRotation.clear();
	this->ParticleScale.clear();
	this->ParticleVelocity.clear();
	this->ParticleStartColour.clear();
	this->ParticleEndColour.clear();
	this->ParticleRect.clear();
}

void ParticleSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.ParticleSystemCommands)
	{
		for (size_t i = 0; i < command.Number; i++)
		{
			const float size = static_cast<float>(GetRandomValue(command.MinSize, command.MaxSize));
			const float rotation = static_cast<float>(GetRandomValue(0, 90));
			const size_t expiry = command.Creation + static_cast<size_t>(GetRandomValue(command.MinLifetime, command.MaxLifetime));
			Vector2 velocity = command.Velocity;

			velocity.x += static_cast<float>(GetRandomValue(-command.MaxSpeed, command.MaxSpeed));
			velocity.y += static_cast<float>(GetRandomValue(-command.MaxSpeed, command.MaxSpeed));

			this->CreateParticle(command.X, command.Y, size, rotation, command.Creation, expiry, velocity, command.StartColour, command.EndColour);
		}
	}

	message_system.ParticleSystemCommands.clear();
}

void ParticleSystem::UpdateParticles(const Rectangle update_area, const size_t ticks) noexcept
{
	this->VisibilityCheck(update_area);
	this->MoveAndScaleParticles(ticks);
	this->RemoveParticles(ticks);
}

void ParticleSystem::Draw(const AssetManager& assets, const size_t ticks) const noexcept
{
	for (size_t i = 0, n = this->ParticleIsVisible.size(); i < n; i++)
	{
		if (this->ParticleIsVisible[i])
		{
			const float lerp_factor = static_cast<float>(this->ParticleExpiry[i] - ticks) / static_cast<float>(this->ParticleExpiry[i] - this->ParticleCreation[i]);
			const Color colour = ColorLerp(this->ParticleEndColour[i], this->ParticleStartColour[i], lerp_factor);

			DrawTexturePro(
					assets.Textures.at(TextureKey::WhitePixel),
					{ 0.0f, 0.0f, 1.0f, 1.0f },
					this->ParticleRect[i],
					{ 0.0f, 0.0f },
					this->ParticleRotation[i],
					colour
					);
		}
	}
}

void ParticleSystem::VisibilityCheck(const Rectangle update_area) noexcept
{
	for (size_t i = 0, n = this->ParticleRect.size(); i < n; i++)
		this->ParticleIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->ParticleRect[i]));
}

void ParticleSystem::CreateParticle(
		const float x, const float y, const float scale, const float rotation, const size_t creation,
		const size_t expiry, const Vector2 velocity, const Color start_colour, const Color end_colour
		) noexcept
{
	this->ParticleIsVisible.emplace_back(static_cast<uint8_t>(false));

	this->ParticleCreation.emplace_back(creation);
	this->ParticleExpiry.emplace_back(expiry);

	this->ParticleRotation.emplace_back(rotation);
	this->ParticleScale.emplace_back(scale);

	this->ParticleVelocity.emplace_back(velocity);

	this->ParticleStartColour.emplace_back(start_colour);
	this->ParticleEndColour.emplace_back(end_colour);

	this->ParticleRect.emplace_back(x, y, scale, scale);
}

void ParticleSystem::RemoveParticles(const size_t ticks) noexcept
{
	for (size_t i = 0; i < this->ParticleExpiry.size(); )
	{
		if (ticks >= this->ParticleExpiry[i])
		{
			this->ParticleIsVisible[i] = this->ParticleIsVisible.back();
			this->ParticleCreation[i] = this->ParticleCreation.back();
			this->ParticleExpiry[i] = this->ParticleExpiry.back();
			this->ParticleRotation[i] = this->ParticleRotation.back();
			this->ParticleScale[i] = this->ParticleScale.back();
			this->ParticleVelocity[i] = this->ParticleVelocity.back();
			this->ParticleStartColour[i] = this->ParticleStartColour.back();
			this->ParticleEndColour[i] = this->ParticleEndColour.back();
			this->ParticleRect[i] = this->ParticleRect.back();


			this->ParticleIsVisible.pop_back();
			this->ParticleCreation.pop_back();
			this->ParticleExpiry.pop_back();
			this->ParticleRotation.pop_back();
			this->ParticleScale.pop_back();
			this->ParticleVelocity.pop_back();
			this->ParticleStartColour.pop_back();
			this->ParticleEndColour.pop_back();
			this->ParticleRect.pop_back();
		}
		else
			i++;
	}
}

void ParticleSystem::MoveAndScaleParticles(const size_t ticks) noexcept
{
	for (size_t i = 0, n = this->ParticleRect.size(); i < n; i++)
	{
		this->ParticleRect[i].x += this->ParticleVelocity[i].x * TICK_TIME;
		this->ParticleRect[i].y += this->ParticleVelocity[i].y * TICK_TIME;

		const float lerp_factor = static_cast<float>(this->ParticleExpiry[i] - ticks) / static_cast<float>(this->ParticleExpiry[i] - this->ParticleCreation[i]);

		this->ParticleRect[i].width = this->ParticleScale[i] * lerp_factor;
		this->ParticleRect[i].height = this->ParticleScale[i] * lerp_factor;
	}
}
