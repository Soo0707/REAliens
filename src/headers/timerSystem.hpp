#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "timers.hpp"
#include "messageSystem.hpp"

class TimerSystem
{
	public:
		TimerSystem();
		~TimerSystem() = default;

		void ExecuteCommands(MessageSystem& message_system, const size_t ticks) noexcept;
		void UpdateTimers(MessageSystem& message_system, const size_t ticks) noexcept;
		
		void Reset() noexcept;
		
		bool GetTimerStatus(const Timer timer) const noexcept;
		size_t GetTimer(const Timer timer) const noexcept;
		size_t GetTimerInterval(const Timer timer) const noexcept;

	private:
		std::array<size_t, static_cast<size_t>(Timer::COUNT)> Timers;
		std::array<uint32_t, static_cast<size_t>(Timer::COUNT)> TimerInterval;
		std::array<bool, static_cast<size_t>(Timer::COUNT)> TimerRecurring;
		std::array<bool, static_cast<size_t>(Timer::COUNT)> TimerActive;

		void GreenbullExpireHandler(MessageSystem& message_system) noexcept;
		void MilkExpireHandler(MessageSystem& message_system) noexcept;
		void PoisonTickHandler(MessageSystem& message_system) noexcept;
		void PoisonExpireHandler(MessageSystem& message_system) noexcept;
		void DrunkExpireHandler(MessageSystem& message_system) noexcept;
		void AussieExpireHandler(MessageSystem& message_system) noexcept;
		void MagnetismExpireHandler(MessageSystem& message_system) noexcept;
		void AuraTickHandler(MessageSystem& message_system) noexcept;

		void EmitLocationParticlesHandler(MessageSystem& message_system) noexcept;
		void SpawnEnemiesHandler(MessageSystem& message_system)  noexcept;

		void RegisterTimer(const TimerSystemCommand& command, const size_t ticks) noexcept;
		void DeregisterTimer(const TimerSystemCommand& command, const size_t ticks) noexcept;
		void UpdateTimerInterval(const TimerSystemCommand& command, const size_t ticks) noexcept;

		static constexpr std::array<void(TimerSystem::*)(const TimerSystemCommand& command, const size_t ticks) noexcept, 3> CommandHandlers = 
		{
			&RegisterTimer,
			&DeregisterTimer,
			&UpdateTimerInterval
		};

		static constexpr std::array<void(TimerSystem::*)(MessageSystem& message_system) noexcept, static_cast<size_t>(Timer::COUNT)> TimeoutHandlers =
		{
			&GreenbullExpireHandler,
			&MilkExpireHandler,
			&PoisonTickHandler,
			&PoisonExpireHandler,
			&DrunkExpireHandler,
			&AussieExpireHandler,
			&MagnetismExpireHandler,
			&AuraTickHandler,
			&EmitLocationParticlesHandler,
			&SpawnEnemiesHandler
		};
};
