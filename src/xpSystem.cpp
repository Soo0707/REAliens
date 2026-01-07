#include "xpSystem.hpp"

#include <algorithm>
#include <cstddef>

#include "raylib.h"
#include "assetManager.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"

XpSystem::XpSystem()
{
	this->Xps.reserve(1024);
}

void XpSystem::Reset() noexcept
{
	this->Xps.clear();
}

void XpSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.XpSystemCommands)
		this->Xps.emplace_back(command.X, command.Y, command.Value, assets);

	message_system.XpSystemCommands.clear();
}

void XpSystem::UpdateXps(const size_t ticks, const Rectangle update_area, MessageSystem& message_system) noexcept
{
	/*
	const bool has_magnetism = this->Effects.count(Effect::Magnetism);
*/
	const bool spawn_particles = GetRandomValue(1, 100) < 3;

	for (auto &xp : this->Xps)
	{
		/*
		if (CheckCollisionRecs(this->Player->Rect, xp.Rect) || has_magnetism)
		{
			this->CollectedXp += xp.Value;
			xp.Kill = true;
		}
		*/

		if (CheckCollisionRecs(update_area, xp.Rect) && spawn_particles) 
		{
			const Vector2 velocity = (Vector2) { static_cast<float>(GetRandomValue(-64, 64)), static_cast<float>(GetRandomValue(-96, -32)) };
			message_system.ParticleSystemCommands.emplace_back(
					ticks, 1, velocity, xp.Rect.x, xp.Rect.y, 
					10, 25, 120, TICK_RATE, 0, GREEN, DARKGREEN
					);
		}
	}

	std::erase_if(this->Xps, [](const Xp& xp) { return xp.Kill; });
}

void XpSystem::Draw(const Rectangle update_area) const noexcept
{
	for (auto const &xp : this->Xps)
	{
		if (CheckCollisionRecs(update_area, xp.Rect))
			xp.Draw();
	}
}

void XpSystem::DrawLightmap(const Rectangle update_area) const noexcept
{
	for (auto const &xp : this->Xps)
	{
		if (CheckCollisionRecs(update_area, xp.Rect))
			xp.DrawLightmap();
	}
}

