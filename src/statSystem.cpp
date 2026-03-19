/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "statSystem.hpp"

#include <cstddef>

#include "messageSystem.hpp"
#include "commands.hpp"

void StatSystem::Update(MessageSystem& message_system) noexcept
{
	this->ExecuteCommands(message_system);
}

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
