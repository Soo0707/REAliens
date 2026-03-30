/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

enum class EnemySystemSignal : uint8_t
{
	EmitLocationParticles,
	SpawnEnemies,

	COUNT
};

enum class PlayerSignal : uint8_t
{
	IncreasePlotArmour,
	ApplySpeedBoots,

	ApplySlide,
	RemoveSlide,

	SpawnBall,
	DripGlue,

	COUNT
};

enum class ModifierSystemSignal : uint8_t
{
	ApplyMilk,
	ApplyGreenbull,
	ApplyMagnetism,
	ApplyAura,
	ApplyBuckshot,
	ApplyBullet,
	ApplyLazer,
	ApplyBall,
	ApplyLifeSteal,
	ApplyBabyOil,
	ApplyLuck,
	ApplyAussie,
	ApplyPoison,
	ApplyTrapped,
	ApplyDrunk,

	RemoveMilk,
	RemoveGreenbull,
	RemoveMagnetism,
	RemoveAussie,
	RemovePoison,
	RemoveTrapped,
	RemoveDrunk,

	IncrementCollectedXp,

	COUNT
};

enum class CollisionSystemSignal : uint8_t
{
	AuraTick,
	COUNT
};

enum class GameSignal : uint8_t
{
	EnableLMB,
	EnableRMB,
	EnableSlide,
	UpdateDuration,
	COUNT
};
