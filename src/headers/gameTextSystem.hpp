#pragma once

#include <vector>
#include <cstddef>

#include "raylib.h"
#include "gameText.hpp"

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
		void Draw(const size_t ticks, const Rectangle update_area) const noexcept;

	private:
		std::vector<GameText> GameTexts;
};
