#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "timers.hpp"
#include "messageSystem.hpp"

class TimerSystem
{
	public:
		TimerSystem() = default;
		~TimerSystem() = default;
		
		void Update(MessageSystem& message_system, const size_t ticks) noexcept;

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

		void GreenbullExpireHandler(MessageSystem& message_system) const noexcept;
		void MilkExpireHandler(MessageSystem& message_system) const noexcept;
		void PoisonTickHandler(MessageSystem& message_system) const noexcept;
		void PoisonExpireHandler(MessageSystem& message_system) const noexcept;
		void DrunkExpireHandler(MessageSystem& message_system) const noexcept;
		void AussieExpireHandler(MessageSystem& message_system) const noexcept;
		void MagnetismExpireHandler(MessageSystem& message_system) const noexcept;
		void AuraTickHandler(MessageSystem& message_system) const noexcept;

		void EmitLocationParticlesHandler(MessageSystem& message_system) const noexcept;
		void SpawnEnemiesHandler(MessageSystem& message_system) const noexcept;

		void PlayerSlideExpireHandler(MessageSystem& message_system) const noexcept;
		void BallCountdownHandler(MessageSystem& message_system) const noexcept;

		void LMBHandler(MessageSystem& message_system) const noexcept;
		void RMBHandler(MessageSystem& message_system) const noexcept;
		void SlideHandler(MessageSystem& message_system) const noexcept;

		void RegisterTimer(const TimerSystemCommand& command, const size_t ticks) noexcept;
		void EnableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept;
		void DisableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept;
		void DecreaseTimerInterval(const TimerSystemCommand& command, const size_t ticks) noexcept;
		void TriggerNow(const TimerSystemCommand& command, const size_t ticks) noexcept;

		static constexpr std::array<void(TimerSystem::*)(const TimerSystemCommand& command, const size_t ticks) noexcept, 5> CommandHandlers = 
		{
			&RegisterTimer,
			&EnableTimer,
			&DisableTimer,
			&DecreaseTimerInterval,
			&TriggerNow
		};

		static constexpr std::array<void(TimerSystem::*)(MessageSystem&) const noexcept, static_cast<size_t>(Timer::COUNT)> TimeoutHandlers =
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
			&SpawnEnemiesHandler,
			&PlayerSlideExpireHandler,
			&BallCountdownHandler,
			&LMBHandler,
			&RMBHandler,
			&SlideHandler
		};
};
