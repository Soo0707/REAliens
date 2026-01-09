#pragma once

#include <cstdint>

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

struct CreateXp
{
	float X;
	float Y;
	unsigned int Value;
};

enum class ModifierSystemCommandType : uint8_t
{
	ApplyMilk,
	ApplyGreenbull,
	ApplyMagnetism,
	ApplyAura,
	ApplyBuckshot,
	ApplyProjectile,
	ApplyLazer,
	ApplyLifeSteal,
	ApplyBabyOil,
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

	COUNT
};

struct ModifierSystemCommand
{
	ModifierSystemCommandType Type;
};

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

using TimerSystemCommand = std::variant<RegisterTimer, DeregisterTimer, UpdateTimerInterval>;

struct StatSystemCommand
{
	Stat StatType;
	uint32_t Amount;
};
