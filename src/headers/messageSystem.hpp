#pragma once

#include <vector>
#include <array>
#include <variant>

#include "commands.hpp"
#include "signals.hpp"

struct MessageSystem
{
	MessageSystem();
	~MessageSystem() = default;

	void Reset() noexcept;

	std::vector<CreateParticles> ParticleSystemCommands;
	std::vector<CreateGameTexts> GameTextSystemCommands;
	std::vector<ProjectileSystemCommand> ProjectileSystemCommands;
	std::vector<XpSystemCommand> XpSystemCommands;
	std::vector<PlayerCommand> PlayerCommands;
	std::vector<EnemySystemCommand> EnemySystemCommands;
	std::vector<ModifierSystemCommand> ModifierSystemCommands;
	std::vector<TimerSystemCommand> TimerSystemCommands;
	std::vector<StatSystemCommand> StatSystemCommands;

	std::array<bool, static_cast<size_t>(EnemySystemSignal::COUNT)> EnemySystemSignals;
	std::array<unsigned int, static_cast<size_t>(PlayerSignal::COUNT)> PlayerSignals;
};
