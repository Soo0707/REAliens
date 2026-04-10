/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <variant>

#include "raylib.h"

#include "stats.hpp"
#include "states.hpp"
#include "timers.hpp"
#include "projectileData.hpp"
#include "itemData.hpp"

struct CreateParticles
{
	size_t Creation;
	size_t Number;
	Vector2 Velocity;
	float X;
	float Y;
	unsigned int MinSize;
	unsigned int MaxSize;
	unsigned int MinLifetime;
	unsigned int MaxLifetime;
	unsigned int MaxSpeed;
	Color StartColour;
	Color EndColour;
};


struct CreateProjectile
{
	ProjectileType Type;
	Vector2 Direction;
	float X;
	float Y;
	float Speed;
};

struct ProjectileHit
{
	size_t ProjectileIndex;
};

using ProjectileSystemCommand = std::variant<struct CreateProjectile, struct ProjectileHit>;


struct CreateItem
{
	float X;
	float Y;
	Item Type;
};

struct CollidedWithItem
{
	size_t ItemIndex;
};

struct EnemyItemCollision
{
	size_t ItemIndex;
	size_t EnemyIndex;
};

using ItemSystemCommand = std::variant<struct CreateItem, struct CollidedWithItem, struct EnemyItemCollision>;


struct RegisterTimer
{
	uint32_t Interval;
	bool Recurring;
	Timer Type;
};

struct EnableTimer
{
	bool Recurring;
	Timer Type;
};

struct DisableTimer
{
	Timer Type;
};

struct DecreaseTimerInterval
{
	uint32_t Amount;
	uint32_t Minimum;
	Timer Type;
};

struct TriggerNow
{
	Timer Type;
};

using TimerSystemCommand = std::variant<
	struct RegisterTimer, struct EnableTimer, struct DisableTimer,
	struct DecreaseTimerInterval, struct TriggerNow
	>;


struct StatSystemCommand
{
	Stat StatType;
	uint32_t Amount;
};


struct DamageEnemy
{
	size_t EnemyIndex;
	float DamageAmount;
};

struct EnemyLeAttacked
{
	size_t EnemyIndex;
	size_t Ticks;
};

struct EnemyGotGlued
{
	size_t EnemyIndex;
};

using EnemySystemCommand = std::variant<struct DamageEnemy, struct EnemyLeAttacked, struct EnemyGotGlued>;


struct DamagePlayer
{
	float DamageAmount;
};

struct IncreasePlayerHealth
{
	float Amount;
};

struct SetPlayerDirection
{
	Vector2 Direction;
};

using PlayerCommand = std::variant<struct DamagePlayer, struct IncreasePlayerHealth, struct SetPlayerDirection>;


struct SlideCamera
{
	Vector2 Offset;
	uint32_t Duration;
};

struct ReleaseCamera
{
	uint32_t Duration;
};

using CameraSystemCommand = std::variant<struct SlideCamera, struct ReleaseCamera>;


struct SetState
{
	State NextState;
};

struct SetTerminate
{
	bool Terminate;
};

using StateManagerCommand = std::variant<struct SetState, struct SetTerminate>;
