#include "statSystem.hpp"

#include <cstddef>

#include "messageSystem.hpp"
#include "commands.hpp"


void StatSystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto& command : message_system.StatSystemCommands)
	{
		const size_t index = static_cast<size_t>(command.StatType);
		this->Stats[index] += static_cast<size_t>(command.Amount);
	}

	message_system.StatSystemCommands.clear();
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
