/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "raylib.h"


inline constexpr unsigned int REFERENCE_WIDTH = 1280;
inline constexpr unsigned int REFERENCE_HEIGHT = 720;

inline constexpr float TO_RAD = PI / 180.0f;
inline constexpr float TO_DEG = 180.0f / PI;

inline constexpr float TICK_TIME = (1.0/240.0);
inline constexpr float MAX_TICK_TIME = (1.0/5.0);

inline constexpr unsigned int TICK_RATE = 240;

inline constexpr uint8_t TILE_SIZE = 64;

inline constexpr unsigned int ENEMY_TEXTURE_TILE_SIZE = 64;
inline constexpr unsigned int PLAYER_TEXTURE_TILE_SIZE = 50;

[[nodiscard]] inline constexpr size_t SECONDS_TO_TICKS(unsigned int seconds)
{
	return seconds * TICK_RATE;
}

[[nodiscard]] inline constexpr size_t TICKS_TO_SECONDS(size_t ticks)
{
	return ticks / TICK_RATE;
}

constexpr Color CYAN = { 0, 243, 255, 255 };
constexpr Color TEAL = { 0, 250, 147, 255 };  
