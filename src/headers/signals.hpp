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
	PoisonTick,
	COUNT
};

enum class ModifierSystemSignal : uint8_t
{
	InsertLevelDebuff,
	RemoveLevelDebuff,
	COUNT
};
