/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <cstddef>

#include "stats.hpp"
#include "messageSystem.hpp"

class StatSystem
{
	public:
		StatSystem() = default;
		~StatSystem() = default;

		void Update(MessageSystem& message_system) noexcept;

		void ExecuteCommands(MessageSystem& message_system) noexcept;
		size_t GetStat(const Stat) const noexcept;

		void Reset() noexcept;

	private:
		std::array<size_t, static_cast<size_t>(Stat::COUNT)> Stats;
};
