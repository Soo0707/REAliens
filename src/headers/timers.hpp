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
	COUNT
};
