/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "assetManager.hpp"

enum class EnemyType : uint8_t
{
	Australian,
	Drunkard,
	Pleb,
	Poison,
	Masochist,
	Orange,
	Tyrone,
	COUNT
};

struct EnemyAttackComponent
{
	size_t LastLeAttack;
	float Damage;
	bool CanLeAttack;
};

struct EnemyAnimationComponent
{
	size_t LastAnimationUpdate;
	uint8_t ImageIndex;
	uint8_t AnimationInterval;
};

struct EnemyData
{
	const TextureKey Texture;
	const float Damage;
	const float Speed;
	const float Health;
	const uint8_t AnimationInterval;
};
