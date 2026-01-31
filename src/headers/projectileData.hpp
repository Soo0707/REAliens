#pragma once

#include <cstdint>

#include "raylib.h"
#include "constants.hpp"
#include "assetManager.hpp"

enum class ProjectileType : uint8_t
{
	Bullet,
	Lazer,
	Ball,

	COUNT
};

struct ProjectileData
{
	Color Colour;
	TextureKey Texture;
};
