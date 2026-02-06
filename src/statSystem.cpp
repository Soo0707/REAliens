#include "statSystem.hpp"

#include <cstddef>
#include <mutex>

#include "messageSystem.hpp"
#include "commands.hpp"

void StatSystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	{
		std::lock_guard<std::mutex> lock(message_system.StatSystemMutex);
		message_system.StatSystemCommandsRead.swap(message_system.StatSystemCommandsWrite);
	}

	for (auto& command : message_system.StatSystemCommandsRead)
	{
		const size_t index = static_cast<size_t>(command.StatType);
		this->Stats[index] += static_cast<size_t>(command.Amount);
	}

	message_system.StatSystemCommandsRead.clear();
}

size_t StatSystem::GetStat(const Stat stat) const noexcept
{
	const size_t index = static_cast<size_t>(stat);

	return this->Stats[index];
}

void StatSystem::Reset() noexcept
{
	this->Stats = { 0 };
}
