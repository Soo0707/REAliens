#pragma once

#include <cstddef>

#include "raylib.h"


inline constexpr unsigned int REFERENCE_WIDTH = 1280;
inline constexpr unsigned int REFERENCE_HEIGHT = 720;

inline constexpr float TO_RAD = PI / 180.0f;
inline constexpr float TO_DEG = 180.0f / PI;

inline constexpr float TICK_TIME = (1.0/240.0);
inline constexpr float MAX_TICK_TIME = (1.0/5.0);

inline constexpr unsigned int TICK_RATE = 240;

inline constexpr unsigned int TEXTURE_TILE_SIZE = 64;

inline constexpr unsigned int PLAYER_TEXTURE_TILE_WIDTH = 35;
inline constexpr unsigned int PLAYER_TEXTURE_TILE_HEIGHT = 45;

[[nodiscard]] inline size_t SECONDS_TO_TICKS(unsigned int seconds)
{
	return seconds * TICK_RATE;
}
