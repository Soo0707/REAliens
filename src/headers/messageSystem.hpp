#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <variant>

#include <mutex>
#include <atomic>

#include "commands.hpp"
#include "signals.hpp"

struct MessageSystem
{
	MessageSystem();
	~MessageSystem() = default;

	void Reset() noexcept;

	std::mutex ParticleSystemMutex;
	std::vector<CreateParticles> ParticleSystemCommandsWrite;
	std::vector<CreateParticles> ParticleSystemCommandsRead;

	std::mutex ProjectileSystemMutex;
	std::vector<ProjectileSystemCommand> ProjectileSystemCommandsWrite;
	std::vector<ProjectileSystemCommand> ProjectileSystemCommandsRead;

	std::mutex XpSystemMutex;
	std::vector<XpSystemCommand> XpSystemCommandsWrite;
	std::vector<XpSystemCommand> XpSystemCommandsRead;

	std::mutex PlayerMutex;
	std::vector<PlayerCommand> PlayerCommandsWrite;
	std::vector<PlayerCommand> PlayerCommandsRead;

	std::mutex EnemySystemMutex;
	std::vector<EnemySystemCommand> EnemySystemCommandsWrite;
	std::vector<EnemySystemCommand> EnemySystemCommandsRead;

	std::mutex TimerSystemMutex;
	std::vector<TimerSystemCommand> TimerSystemCommandsWrite;
	std::vector<TimerSystemCommand> TimerSystemCommandsRead;

	std::mutex StatSystemMutex;
	std::vector<StatSystemCommand> StatSystemCommandsWrite;
	std::vector<StatSystemCommand> StatSystemCommandsRead;


	std::array<std::atomic<uint16_t>, static_cast<size_t>(EnemySystemSignal::COUNT)> EnemySystemSignals;
	std::array<std::atomic<uint16_t>, static_cast<size_t>(PlayerSignal::COUNT)> PlayerSignals;
	std::array<std::atomic<uint16_t>, static_cast<size_t>(ModifierSystemSignal::COUNT)> ModifierSystemSignals;
	std::array<std::atomic<uint16_t>, static_cast<size_t>(CollisionSystemSignal::COUNT)> CollisionSystemSignals;
};
