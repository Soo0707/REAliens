#pragma once

#include <cstdint>

enum class State : uint8_t
{
	Game,
	SystemsReset,
	PowerupMenu,
	GameOverMenu,
	PauseMenu,
	MainMenu,
	GenerateGameOverStats,
	COUNT
};
