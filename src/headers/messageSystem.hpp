/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <variant>

#include "commands.hpp"
#include "signals.hpp"

struct MessageSystem
{
	MessageSystem();
	~MessageSystem() = default;

	void Reset() noexcept;

	std::vector<CreateParticles> ParticleSystemCommands;
	std::vector<ProjectileSystemCommand> ProjectileSystemCommands;
	std::vector<ItemSystemCommand> ItemSystemCommands;
	std::vector<PlayerCommand> PlayerCommands;
	std::vector<EnemySystemCommand> EnemySystemCommands;
	std::vector<TimerSystemCommand> TimerSystemCommands;
	std::vector<CameraSystemCommand> CameraSystemCommands;
	std::vector<StatSystemCommand> StatSystemCommands;
	std::vector<StateManagerCommand> StateManagerCommands;

	std::array<uint16_t, static_cast<size_t>(EnemySystemSignal::COUNT)> EnemySystemSignals;
	std::array<uint16_t, static_cast<size_t>(PlayerSignal::COUNT)> PlayerSignals;
	std::array<uint16_t, static_cast<size_t>(ModifierSystemSignal::COUNT)> ModifierSystemSignals;
	std::array<uint16_t, static_cast<size_t>(CollisionSystemSignal::COUNT)> CollisionSystemSignals;
	std::array<uint16_t, static_cast<size_t>(CameraSystemSignal::COUNT)> CameraSystemSignals;
	std::array<uint16_t, static_cast<size_t>(InventorySystemSignal::COUNT)> InventorySystemSignals;
	std::array<uint16_t, static_cast<size_t>(GameSignal::COUNT)> GameSignals;
};
