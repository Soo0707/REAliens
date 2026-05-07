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
#include "assetManager.hpp"

enum class Item : uint8_t
{
	Xp,
	Turret,
	COUNT
};

struct ItemData
{
	const Color Colour;
	const uint16_t MaxCollisionHits;
	const TextureKey Texture;
	const bool EmitParticles;
};
