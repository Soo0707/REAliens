/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

#include "raylib.h"
#include "constants.hpp"
#include "modifiers.hpp"
#include "assetManager.hpp"

enum class ProjectileType : uint8_t
{
	Bullet,
	Lazer,
	Ball,
	Glue,
	Plebifier,

	COUNT
};

struct ProjectileData
{
	const Color Colour;
	const TextureKey Texture;
	const Attribute SpeedKey;
	const Attribute PenetrationPowerKey;
};
