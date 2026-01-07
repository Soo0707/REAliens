#include "gameTextSystem.hpp"

#include <algorithm>
#include <string>
#include <cstddef>

#include "raylib.h"
#include "commands.hpp"
#include "messageSystem.hpp"

GameTextSystem::GameTextSystem()
{
	this->GameTexts.reserve(1024);
}

void GameTextSystem::Reset() noexcept
{
	this->GameTexts.clear();
}

void GameTextSystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.GameTextSystemCommands)
	{
		this->GameTexts.emplace_back(
				command.X, command.Y, command.Speed, std::to_string(command.Text),
				command.Size, command.Colour, command.Creation, command.Expiry
				);
	}

	message_system.GameTextSystemCommands.clear();
}

void GameTextSystem::UpdateGameText(const size_t ticks, const Rectangle update_area) noexcept
{
	for (auto &text : this->GameTexts)
	{
		if (CheckCollisionRecs(update_area, text.Rect))
			text.Update();
	}

	std::erase_if(this->GameTexts, [ticks](const GameText& text) { return (ticks >= text.Expiry); });
}

void GameTextSystem::Draw(const size_t ticks, const Rectangle update_area) const noexcept
{
	for (auto const &text : this->GameTexts)
	{
		if (CheckCollisionRecs(update_area, text.Rect))
			text.Draw(ticks);
	}
}
