#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

#include "raylib.h"

#include "messageSystem.hpp"
#include "assetManager.hpp"

class GameTextSystem
{
	public:
		GameTextSystem();
		~GameTextSystem() = default;

		void Reset() noexcept;

		void ExecuteCommands(MessageSystem& message_system) noexcept;

		void UpdateGameText(const size_t ticks, const Rectangle update_area) noexcept;
		void Draw(const size_t ticks) const noexcept;

	private:
		void CheckVisibility(const Rectangle update_area) noexcept;

		void CreateGameText(
				const float x, const float y, const float speed, const unsigned int text, 
				const int size, const Color colour, const size_t creation, const size_t expiry
				) noexcept;

		void RemoveGameText(const size_t ticks) noexcept;
		void MoveGameText() noexcept;

		std::vector<uint8_t> GameTextIsVisible;
		std::vector<int> GameTextSize;
		std::vector<float> GameTextSpeed;

		std::vector<size_t> GameTextCreation;
		std::vector<size_t> GameTextExpiry;

		std::vector<std::string> GameTextText;

		std::vector<Color> GameTextColour;
		std::vector<Vector2> GameTextDirection;
		std::vector<Rectangle> GameTextRect;
};
