#include "timerSystem.hpp"

#include <cstddef>
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
		if (this->TimerActive[i] && this->Timers[i] <= ticks)
		{
			auto handler = this->TimeoutHandlers[i];

			(this->*handler)(message_system);

			if (this->TimerRecurring[i])
				this->Timers[i] = ticks + this->TimerInterval[i];
			else
				this->TimerActive[i] = false;
		}
	}
}

bool TimerSystem::GetTimerStatus(const Timer timer) const noexcept
{
	const size_t index = static_cast<size_t>(timer);

	return this->TimerActive[index];
}

size_t TimerSystem::GetTimer(const Timer timer) const noexcept
{
	const size_t index = static_cast<size_t>(timer);

	return this->Timers[index];
}

size_t TimerSystem::GetTimerInterval(const Timer timer) const noexcept
{
	const size_t index = static_cast<size_t>(timer);

	return this->TimerInterval[index];
}

void TimerSystem::Reset() noexcept
{
	this->Timers = { 0 };
	this->TimerInterval = { 0 };
	this->TimerRecurring = { 0 };
	this->TimerActive = { 0 };

	this->TimerInterval[static_cast<size_t>(Timer::AuraTick)] = static_cast<uint32_t>(SECONDS_TO_TICKS(2));
	this->TimerInterval[static_cast<size_t>(Timer::BallCountdown)] = static_cast<uint32_t>(SECONDS_TO_TICKS(30));
}

void TimerSystem::RegisterTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct RegisterTimer& data = std::get<struct RegisterTimer>(command);

	const size_t index = static_cast<size_t>(data.Type);

	this->Timers[index] = ticks + data.Interval;
	this->TimerInterval[index] = data.Interval;
	this->TimerRecurring[index] = data.Recurring;
	this->TimerActive[index] = true;
}

void TimerSystem::EnableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct EnableTimer& data = std::get<struct EnableTimer>(command);
	const size_t index = static_cast<size_t>(data.Type);

	this->TimerActive[index] = true;
	this->TimerRecurring[index] = data.Recurring;
}

void TimerSystem::DisableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const size_t index = static_cast<size_t>(std::get<struct DisableTimer>(command).Type);

	this->TimerActive[index] = false;
}

void TimerSystem::UpdateTimerInterval(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct UpdateTimerInterval& data = std::get<struct UpdateTimerInterval>(command);

	const size_t index = static_cast<size_t>(data.Type);
	this->TimerInterval[index] = data.NewInterval;
}

void TimerSystem::DecreaseTimerInterval(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct DecreaseTimerInterval& data = std::get<struct DecreaseTimerInterval>(command);

	const size_t index = static_cast<size_t>(data.Type);
	const int32_t original_interval = static_cast<int32_t>(this->TimerInterval[index]);

	if (original_interval - static_cast<int32_t>(data.Amount) >= static_cast<int32_t>(data.Minimum))
		this->TimerInterval[index] = original_interval - data.Amount;
	else
		this->TimerInterval[index] = data.Minimum;
}

void TimerSystem::GreenbullExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveGreenbull)]++;
}

void TimerSystem::MilkExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveMilk)]++;
}

void TimerSystem::PoisonTickHandler(MessageSystem& message_system) const noexcept
{
	std::lock_guard<std::mutex> lock(message_system.PlayerMutex);
	message_system.PlayerCommandsWrite.emplace_back(std::in_place_type<struct DamagePlayer>, 2.0f);
}

void TimerSystem::PoisonExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemovePoison)]++;

	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct DisableTimer>, Timer::PoisonTick);
	}
}

void TimerSystem::DrunkExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveDrunk)]++;
}

void TimerSystem::AussieExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveAussie)]++;
}

void TimerSystem::MagnetismExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveMagnetism)]++;
}

void TimerSystem::AuraTickHandler(MessageSystem& message_system) const noexcept
{
	message_system.CollisionSystemSignals[static_cast<size_t>(CollisionSystemSignal::AuraTick)]++;
}

void TimerSystem::EmitLocationParticlesHandler(MessageSystem& message_system) const noexcept
{
	message_system.EnemySystemSignals[static_cast<size_t>(EnemySystemSignal::EmitLocationParticles)]++;
}

void TimerSystem::SpawnEnemiesHandler(MessageSystem& message_system) const noexcept
{
	{
		std::lock_guard<std::mutex> lock(message_system.TimerSystemMutex);
		message_system.TimerSystemCommandsWrite.emplace_back(std::in_place_type<struct DisableTimer>, Timer::EmitLocationParticles);
	}

	message_system.EnemySystemSignals[static_cast<size_t>(EnemySystemSignal::SpawnEnemies)]++;
}

void TimerSystem::PlayerSlideExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::RemoveSlide)]++;
}

void TimerSystem::BallCountdownHandler(MessageSystem& message_system) const noexcept
{
	message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::SpawnBall)]++;
}
