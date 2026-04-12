/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
	TariffsExpire,
	WeaknessExpire,
	AuraTick,
	EmitLocationParticles,

	SpawnEnemies,
	PlayerSlideExpire,
	BallCountdown,

	LMB,
	RMB,
	Slide,

	UpdateDuration,
	GlueCountdown,
	SlideCameraExpire,
	ReleaseCameraExpire,
	COUNT
};
