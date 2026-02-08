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
	std::vector<XpSystemCommand> XpSystemCommands;
	std::vector<PlayerCommand> PlayerCommands;
	std::vector<EnemySystemCommand> EnemySystemCommands;
	std::vector<TimerSystemCommand> TimerSystemCommands;
	std::vector<StatSystemCommand> StatSystemCommands;

	std::array<uint16_t, static_cast<size_t>(EnemySystemSignal::COUNT)> EnemySystemSignals;
	std::array<uint16_t, static_cast<size_t>(PlayerSignal::COUNT)> PlayerSignals;
	std::array<uint16_t, static_cast<size_t>(ModifierSystemSignal::COUNT)> ModifierSystemSignals;
	std::array<uint16_t, static_cast<size_t>(CollisionSystemSignal::COUNT)> CollisionSystemSignals;
};
