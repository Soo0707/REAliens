#pragma once

#include <cstdint>
#include <variant>

#include "raylib.h"

#include "stats.hpp"
#include "timers.hpp"
#include "projectileData.hpp"

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


struct CreateGameTexts
{
	size_t Creation;
	size_t Expiry;
	float X;
	float Y;
	float Speed;
	unsigned int Text;
	int Size;
	Color Colour;
};


struct CreateProjectile
{
	ProjectileType Type;
	Vector2 Direction;
	float X;
	float Y;
	float Speed;
	float Scale;
};

struct ProjectileHit
{
	size_t ProjectileIndex;
};

using ProjectileSystemCommand = std::variant<struct CreateProjectile, struct ProjectileHit>;


struct CreateXp
{
	float X;
	float Y;
	uint8_t Value;
};

struct KillXp
{
	size_t XpIndex;
};

using XpSystemCommand = std::variant<struct CreateXp, struct KillXp>;


struct RegisterTimer
{
	uint32_t Interval;
	bool Recurring;
	Timer Type;
};

struct DeregisterTimer
{
	Timer Type;
};

struct UpdateTimerInterval
{
	uint32_t NewInterval;
	Timer Type;
};

struct DecreaseTimerInterval
{
	int32_t Amount;
	int16_t Minimum;
	Timer Type;
};

using TimerSystemCommand = std::variant<struct RegisterTimer, struct DeregisterTimer, struct UpdateTimerInterval, struct DecreaseTimerInterval>;


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

using EnemySystemCommand = std::variant<struct DamageEnemy, struct EnemyLeAttacked>;


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

struct ApplySlide
{
	size_t Ticks;
};
using PlayerCommand = std::variant<struct DamagePlayer, struct IncreasePlayerHealth, struct SetPlayerDirection>;
