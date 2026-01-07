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

		void ExecuteCommands(MessageSystem& message_system) noexcept;
		size_t GetStat(const Stat) const noexcept;

		void Reset() noexcept;

	private:
		std::array<size_t, static_cast<size_t>(Stat::COUNT)> Stats;
};
