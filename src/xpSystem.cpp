#include "xpSystem.hpp"

#include <cstddef>
#include <cstdint>
#include <variant>

#include "raylib.h"
#include "constants.hpp"
#include "assetManager.hpp"

#include "commands.hpp"
#include "messageSystem.hpp"

XpSystem::XpSystem()
{
	this->XpKill.reserve(1024);
	this->XpIsVisible.reserve(1024);
	this->XpValue.reserve(1024);
	this->XpCentre.reserve(1024);
	this->XpRect.reserve(1024);
}

void XpSystem::Reset() noexcept
{
	this->XpKill.clear();
	this->XpIsVisible.clear();
	this->XpValue.clear();
	this->XpCentre.clear();
	this->XpRect.clear();
}

void XpSystem::ExecuteCommands(MessageSystem& message_system, const AssetManager& assets) noexcept
{
	for (auto const& command : message_system.XpSystemCommands)
	{
		const size_t handler_index = command.index();
		auto handler = this->CommandHandlers[handler_index];
		(this->*handler)(command, assets);
	}

	message_system.XpSystemCommands.clear();
}

void XpSystem::UpdateXps(MessageSystem& message_system, const Rectangle update_area, const size_t ticks) noexcept
{
	this->VisibilityCheck(update_area);
	this->EmitParticles(message_system, ticks);
	this->RemoveXp();
}

void XpSystem::Draw(const AssetManager& assets) const noexcept
{
	for (size_t i = 0, n = this->XpIsVisible.size(); i < n; i++)
	{
		if (this->XpIsVisible[i])
		{
			DrawTexture(
					assets.GetTexture(TextureKey::Xp),
					static_cast<int>(this->XpRect[i].x),
					static_cast<int>(this->XpRect[i].y),
					WHITE
					);
		}
	}
}

void XpSystem::DrawLightmap() const noexcept
{
	for (size_t i = 0, n = this->XpIsVisible.size(); i < n; i++)
	{
		if (this->XpIsVisible[i])
			DrawCircleGradient(this->XpCentre[i].x, this->XpCentre[i].y, 24, GREEN, LIGHTGRAY);
	}
}

void XpSystem::VisibilityCheck(const Rectangle update_area) noexcept
{
	for (size_t i = 0, n = this->XpIsVisible.size(); i < n; i++)
		this->XpIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->XpRect[i]));
}

void XpSystem::CreateXp(const float x, const float y, const float texture_width, const float texture_height, const uint8_t value) noexcept
{
	this->XpKill.emplace_back(static_cast<bool>(false));
	this->XpIsVisible.emplace_back(static_cast<bool>(false));
	this->XpValue.emplace_back(value);
	this->XpCentre.emplace_back(x + texture_width / 2.0f, y + texture_height / 2.0f);
	this->XpRect.emplace_back(x, y, texture_width, texture_height);
}

void XpSystem::RemoveXp() noexcept
{
	for (size_t i = 0; i < this->XpKill.size(); )
	{
		if (this->XpKill[i])
		{
			this->XpKill[i] = this->XpKill.back();
			this->XpIsVisible[i] = this->XpIsVisible.back();
			this->XpValue[i] = this->XpValue.back();
			this->XpCentre[i] = this->XpCentre.back();
			this->XpRect[i] = this->XpRect.back();


			this->XpKill.pop_back();
			this->XpIsVisible.pop_back();
			this->XpValue.pop_back();
			this->XpCentre.pop_back();
			this->XpRect.pop_back();
		}
		else
			i++;
	}
}

void XpSystem::EmitParticles(MessageSystem& message_system, const size_t ticks) noexcept
{
	for (size_t i = 0, n = this->XpIsVisible.size(); i < n; i++)
	{
		const bool spawn_particles = GetRandomValue(1, 100) < 3;

		if (spawn_particles && this->XpIsVisible[i])
		{
			const Vector2 velocity = { static_cast<float>(GetRandomValue(-64, 64)), static_cast<float>(GetRandomValue(-96, -32)) };

			message_system.ParticleSystemCommands.emplace_back(
					ticks, 1, velocity, this->XpCentre[i].x, this->XpCentre[i].y, 
					10, 25, 120, TICK_RATE, 0, GREEN, DARKGREEN
					);
		}
	}
}

void XpSystem::CreateXpHandler(const XpSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct CreateXp& data = std::get<struct CreateXp>(command);
	const float texture_width = assets.GetTexture(TextureKey::Xp).width;
	const float texture_height = assets.GetTexture(TextureKey::Xp).height;

	this->CreateXp(data.X, data.Y, texture_width, texture_height, data.Value);
}

void XpSystem::KillXpHandler(const XpSystemCommand& command, const AssetManager& assets) noexcept
{
	const struct KillXp& data = std::get<struct KillXp>(command);
	this->XpKill[data.XpIndex] = static_cast<uint8_t>(true);
}


const std::vector<Rectangle>& XpSystem::GetXpRect() const noexcept
{
	return this->XpRect;
}

const std::vector<uint8_t>& XpSystem::GetXpValue() const noexcept
{
	return this->XpValue;
}
