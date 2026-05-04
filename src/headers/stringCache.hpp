/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "raylib.h"

#include <string>
#include <array>
#include <cstddef>
#include <cstdint>

#include "constants.hpp"

enum class GameString : uint8_t
{
	LevelText,
	UnclaimedPowerups,
	Duration,
	DamagePerSecond,
	AverageSpeed,
	EnemiesKilled,
	CurrentEnemyCount,
	TotalEntities,

	COUNT
};

struct StringCache
{
	StringCache() = default;
	~StringCache() = default;

	StringCache(const StringCache&) = delete;
	StringCache& operator=(const StringCache&) = delete;

	StringCache(const StringCache&&) = delete;
	StringCache& operator=(const StringCache&&) = delete;
	
	void CacheString(std::string&& string, const GameString game_string) noexcept;
	void Reset() noexcept;

	std::array<std::string, static_cast<size_t>(GameString::COUNT)> Data;
};
