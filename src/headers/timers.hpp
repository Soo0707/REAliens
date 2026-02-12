#pragma once

#include <cstdint>

enum class Timer : uint8_t
{
	GreenbullExpire,
	MilkExpire,

	PoisonTick,
	PoisonExpire,

	DrunkExpire,
	AussieExpire,
	MagnetismExpire,

	AuraTick,
	EmitLocationParticles,
	SpawnEnemies,

	PlayerSlideExpire,
	BallCountdown,

	LMB,
	RMB,
	Slide,

	COUNT
};
