#include "gameTextSystem.hpp"

#include <string>
#include <cstdint>
#include <cstddef>

#include "raylib.h"
#include "constants.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"

GameTextSystem::GameTextSystem()
{
	this->GameTextIsVisible.reserve(1024);
	this->GameTextSize.reserve(1024);
	this->GameTextSpeed.reserve(1024);
	this->GameTextCreation.reserve(1024);
	this->GameTextExpiry.reserve(1024);
	this->GameTextText.reserve(1024);
	this->GameTextColour.reserve(1024);
	this->GameTextDirection.reserve(1024);
	this->GameTextRect.reserve(1024);
}

void GameTextSystem::Reset() noexcept
{
	this->GameTextIsVisible.clear();
	this->GameTextSize.clear();
	this->GameTextSpeed.clear();
	this->GameTextCreation.clear();
	this->GameTextExpiry.clear();
	this->GameTextText.clear();
	this->GameTextColour.clear();
	this->GameTextDirection.clear();
	this->GameTextRect.clear();
}

void GameTextSystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.GameTextSystemCommands)
	{
		this->CreateGameText(
				command.X, command.Y, command.Speed, command.Text, 
				command.Size, command.Colour, command.Creation, command.Expiry
				);
	}

	message_system.GameTextSystemCommands.clear();
}

void GameTextSystem::UpdateGameText(const size_t ticks, const Rectangle update_area) noexcept
{
	this->CheckVisibility(update_area);
	this->MoveGameText();
	this->RemoveGameText(ticks);
}

void GameTextSystem::Draw(const size_t ticks) const noexcept
{
	for (size_t i = 0, n = this->GameTextIsVisible.size(); i < n; i++)
	{
		if (this->GameTextIsVisible[i])
		{
			const int size = this->GameTextSize[i] * (this->GameTextExpiry[i] - ticks) / (this->GameTextExpiry[i] - this->GameTextCreation[i]);
			DrawText(this->GameTextText[i].c_str(), this->GameTextRect[i].x, this->GameTextRect[i].y, size, this->GameTextColour[i]);
		}
	}
}

void GameTextSystem::CheckVisibility(const Rectangle update_area) noexcept
{
	for (size_t i = 0, n = this->GameTextRect.size(); i < n; i++)
		this->GameTextIsVisible[i] = static_cast<uint8_t>(CheckCollisionRecs(update_area, this->GameTextRect[i]));
}

void GameTextSystem::CreateGameText(
		const float x, const float y, const float speed, const unsigned int text, 
		const int size, const Color colour, const size_t creation, const size_t expiry
		) noexcept
{
	this->GameTextIsVisible.emplace_back(static_cast<uint8_t>(false));

	this->GameTextSize.emplace_back(size);
	this->GameTextSpeed.emplace_back(speed);

	this->GameTextCreation.emplace_back(creation);
	this->GameTextExpiry.emplace_back(expiry);

	this->GameTextText.emplace_back(std::to_string(text));

	this->GameTextColour.emplace_back(colour);
	this->GameTextDirection.emplace_back(0.0f, -1.0f);

	this->GameTextRect.emplace_back(x, y, static_cast<float>(size), static_cast<float>(size));
}

void GameTextSystem::RemoveGameText(const size_t ticks) noexcept
{
	for (size_t i = 0; i < this->GameTextExpiry.size(); )
	{
		if (ticks >= this->GameTextExpiry[i])
		{
			this->GameTextIsVisible[i] = this->GameTextIsVisible.back();
			this->GameTextSize[i] = this->GameTextSize.back();
			this->GameTextSpeed[i] = this->GameTextSpeed.back();
			this->GameTextCreation[i] = this->GameTextCreation.back();
			this->GameTextExpiry[i] = this->GameTextExpiry.back();
			this->GameTextText[i].swap(this->GameTextText.back());
			this->GameTextColour[i] = this->GameTextColour.back();
			this->GameTextDirection[i] = this->GameTextDirection.back();
			this->GameTextRect[i] = this->GameTextRect.back();


			this->GameTextIsVisible.pop_back();
			this->GameTextSize.pop_back();
			this->GameTextSpeed.pop_back();
			this->GameTextCreation.pop_back();
			this->GameTextExpiry.pop_back();
			this->GameTextText.pop_back();
			this->GameTextColour.pop_back();
			this->GameTextDirection.pop_back();
			this->GameTextRect.pop_back();
		}
		else
			i++;
	}
}

void GameTextSystem::MoveGameText() noexcept
{
	for (size_t i = 0, n = this->GameTextRect.size(); i < n; i++)
	{
		this->GameTextRect[i].x += this->GameTextSpeed[i] * this->GameTextDirection[i].x * TICK_TIME;
		this->GameTextRect[i].y += this->GameTextSpeed[i] * this->GameTextDirection[i].y * TICK_TIME;
	}
}
