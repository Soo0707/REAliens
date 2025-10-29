#pragma once

#include "game.hpp"

namespace GameInputSystem
{
	void HandleTickedInput(Game& game) noexcept;
	void HandleLeftClick(Game& game) noexcept;
	void HandleRightClick(Game& game) noexcept;
}
