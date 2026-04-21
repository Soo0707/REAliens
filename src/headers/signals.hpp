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
	SpawnTurret,

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
	ApplyGreed,
	ApplyAussie,
	ApplyPoison,
	ApplyDrunk,
	ApplyTariffs,
	ApplyWeakness,
	ApplyAlcoholism,

	RemoveMilk,
	RemoveGreenbull,
	RemoveMagnetism,
	RemoveAussie,
	RemovePoison,
	RemoveDrunk,
	RemoveTariffs,
	RemoveWeakness,
	RemoveAlcoholism,

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
	EnableUseItem,
	UpdateDuration,
	COUNT
};

enum class CameraSystemSignal : uint8_t
{
	SlideCameraExpire,
	ReleaseCameraExpire,
	COUNT
};

enum class InventorySystemSignal : uint8_t
{
	GrantItem,
	UseItem,
	COUNT
};
