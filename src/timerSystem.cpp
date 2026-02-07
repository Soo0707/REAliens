#include "timerSystem.hpp"

#include <cstddef>
#include <atomic>
#include <mutex>

#include "timers.hpp"
#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"

void TimerSystem::ExecuteCommands(MessageSystem& message_system, const size_t ticks) noexcept
{
	{
		std::lock_guard lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsRead.swap(message_system.TimerSystemCommandsWrite);
	}

	for (auto const& command : message_system.TimerSystemCommandsRead)
	{
		const size_t index = command.index();
		auto handler = this->CommandHandlers[index];

		(this->*handler)(command, ticks);
	}

	message_system.TimerSystemCommandsRead.clear();
}

void TimerSystem::UpdateTimers(MessageSystem& message_system, const size_t ticks) noexcept
{
	for (size_t i = 0; i < static_cast<size_t>(Timer::COUNT); i++)
	{
		if (this->TimerActive[i] && ticks >= this->Timers[i])
		{
			auto handler = this->TimeoutHandlers[i];

			(this->*handler)(message_system);

			const bool timer_recurring = this->TimerRecurring[i].load(std::memory_order_acquire);
			const uint32_t timer_interval = this->TimerInterval[i].load(std::memory_order_acquire);

			if (timer_recurring)
				this->Timers[i].store(ticks + timer_interval, std::memory_order_release);
			else
				this->TimerActive[i].store(false, std::memory_order_release);
		}
	}
}

bool TimerSystem::GetTimerStatus(const Timer timer) const noexcept
{
	const size_t index = static_cast<size_t>(timer);

	return this->TimerActive[index].load(std::memory_order_acquire);
}

size_t TimerSystem::GetTimer(const Timer timer) const noexcept
{
	const size_t index = static_cast<size_t>(timer);

	return this->Timers[index].load(std::memory_order_acquire);
}

size_t TimerSystem::GetTimerInterval(const Timer timer) const noexcept
{
	const size_t index = static_cast<size_t>(timer);

	return this->TimerInterval[index].load(std::memory_order_acquire);
}

void TimerSystem::Reset() noexcept
{
	for (auto& value : this->Timers)
		value.store(0, std::memory_order_release);

	for (auto& value : this->TimerInterval)
		value.store(0, std::memory_order_release);

	for (auto& value : this->TimerRecurring)
		value.store(false, std::memory_order_release);

	for (auto& value : this->TimerActive)
		value.store(false, std::memory_order_release);

	this->TimerInterval[static_cast<size_t>(Timer::AuraTick)].store(
			static_cast<uint32_t>(SECONDS_TO_TICKS(2)),
			std::memory_order_release
			);

	this->TimerInterval[static_cast<size_t>(Timer::BallCountdown)].store(
			static_cast<uint32_t>(SECONDS_TO_TICKS(30)),
			std::memory_order_release
			);
}

void TimerSystem::RegisterTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct RegisterTimer& data = std::get<struct RegisterTimer>(command);

	const size_t index = static_cast<size_t>(data.Type);

	this->Timers[index].store(ticks + data.Interval, std::memory_order_release);
	this->TimerInterval[index].store(data.Interval, std::memory_order_release);
	this->TimerRecurring[index].store(data.Recurring, std::memory_order_release);
	this->TimerActive[index].store(true, std::memory_order_release);
}

void TimerSystem::EnableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct EnableTimer& data = std::get<struct EnableTimer>(command);
	const size_t index = static_cast<size_t>(data.Type);

	this->TimerActive[index].store(true, std::memory_order_release);
	this->TimerRecurring[index].store(data.Recurring, std::memory_order_release);
}

void TimerSystem::DisableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const size_t index = static_cast<size_t>(std::get<struct DisableTimer>(command).Type);

	this->TimerActive[index].store(false, std::memory_order_release);
}

void TimerSystem::DecreaseTimerInterval(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct DecreaseTimerInterval& data = std::get<struct DecreaseTimerInterval>(command);

	const size_t index = static_cast<size_t>(data.Type);
	const int32_t original_interval = static_cast<int32_t>(this->TimerInterval[index].load(std::memory_order_acquire));

	if (original_interval - static_cast<int32_t>(data.Amount) >= static_cast<int32_t>(data.Minimum))
		this->TimerInterval[index].store(original_interval - data.Amount, std::memory_order_release);
	else
		this->TimerInterval[index].store(data.Minimum, std::memory_order_release);
}

void TimerSystem::GreenbullExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveGreenbull)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::MilkExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveMilk)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::PoisonTickHandler(MessageSystem& message_system) const noexcept
{
	std::lock_guard<std::mutex> lock(message_system.PlayerMutex);
	message_system.PlayerCommandsWrite.emplace_back(std::in_place_type<struct DamagePlayer>, 2.0f);
}

void TimerSystem::PoisonExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemovePoison)].fetch_add(1, std::memory_order_acq_rel);

	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct DisableTimer>, Timer::PoisonTick);
	}
}

void TimerSystem::DrunkExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveDrunk)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::AussieExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveAussie)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::MagnetismExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveMagnetism)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::AuraTickHandler(MessageSystem& message_system) const noexcept
{
	message_system.CollisionSystemSignals[static_cast<size_t>(CollisionSystemSignal::AuraTick)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::EmitLocationParticlesHandler(MessageSystem& message_system) const noexcept
{
	message_system.EnemySystemSignals[static_cast<size_t>(EnemySystemSignal::EmitLocationParticles)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::SpawnEnemiesHandler(MessageSystem& message_system) const noexcept
{
	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct DisableTimer>, Timer::EmitLocationParticles);
	}

	message_system.EnemySystemSignals[static_cast<size_t>(EnemySystemSignal::SpawnEnemies)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::PlayerSlideExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::RemoveSlide)].fetch_add(1, std::memory_order_acq_rel);
}

void TimerSystem::BallCountdownHandler(MessageSystem& message_system) const noexcept
{
	message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::SpawnBall)].fetch_add(1, std::memory_order_acq_rel);
}
