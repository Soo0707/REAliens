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

	InsertLevelDebuff,
	RemoveLevelDebuff,

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
	PlayerDied,
	COUNT
};
