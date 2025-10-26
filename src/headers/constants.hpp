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

// for drawing
static constexpr Color CYAN = { 0, 243, 255, 255 };

static constexpr Rectangle XP_BACKGROUND = { 100, 680, 1080, 15 };
static constexpr Rectangle HEALTH_BACKGROUND = { 1060, 20, 200, 10 };

static constexpr Rectangle MAGNETISM_HALF_1 = { 1185, 40, 8, 15 };
static constexpr Rectangle MAGNETISM_HALF_2 = { 1192, 40, 7, 15 };

static constexpr Rectangle GREENBULL_SQUARE = { 1205, 40, 15, 15 };
static constexpr Rectangle MILK_SQUARE = { 1225, 40, 15, 15 };
static constexpr Rectangle DRUNK_SQUARE = { 1245, 40, 15, 15 };
