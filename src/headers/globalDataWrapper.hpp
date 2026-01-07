#pragma once

#include "raylib.h"

#include <string>
#include <cstddef>
#include <unordered_map>

#include "constants.hpp"

enum class State
{
	Game,
	GameReset,
	PowerupMenu,

	PauseMenu,
	MainMenu,
	GameOverMenu,

	GenerateGameOverStats
};

enum class CachedString
{
	LevelText,
	UnclaimedPowerups,

	Duration,
	DamagePerSecond,

	AverageSpeed,

	EnemiesKilled,
	GameOverReason,
	LevelDebuff
};

struct GlobalDataWrapper
{
	GlobalDataWrapper() = default;
	~GlobalDataWrapper() = default;

	GlobalDataWrapper(const GlobalDataWrapper&) = delete;
	GlobalDataWrapper& operator=(const GlobalDataWrapper&) = delete;

	GlobalDataWrapper(const GlobalDataWrapper&&) = delete;
	GlobalDataWrapper& operator=(const GlobalDataWrapper&&) = delete;

	std::unordered_map<CachedString, std::string> CachedStrings;
	State ActiveState = State::MainMenu;

	bool Running = true;
	unsigned int UnclaimedPowerups;

	void Reset() noexcept;
};
