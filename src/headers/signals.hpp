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

	COUNT
};

enum class ModifierSystemSignal : uint8_t
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

	InsertLevelDebuff,
	RemoveLevelDebuff,

	COUNT
};

enum class CollisionSystemSignal : uint8_t
{
	AuraTick,
	COUNT
};
