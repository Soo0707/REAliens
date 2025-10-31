#pragma once

#include "game.hpp"
#include "raylib.h"

namespace GameDrawSystem
{
	void DrawGame(const Game& game) noexcept;
	void DrawLighting(const Game& game) noexcept;

	void DrawScreenLayer(const Game& game) noexcept;
	void DrawOverlay(const Game& game) noexcept;

	void DrawGreenbull(const Game& game) noexcept;
	void DrawMilk(const Game& game) noexcept;
	void DrawDrunk(const Game& game) noexcept;
	void DrawMagnetism(const Game& game) noexcept;
}
