#include "timerSystem.hpp"

#include <cstddef>

#include "timers.hpp"
#include "commands.hpp"
#include "signals.hpp"
#include "messageSystem.hpp"

TimerSystem::TimerSystem()
{

}

void TimerSystem::ExecuteCommands(MessageSystem& message_system, const size_t ticks) noexcept
{
	for (auto const& command : message_system.TimerSystemCommands)
	{
		const size_t index = command.index();
		auto handler = this->CommandHandlers[index];

		(this->*handler)(command, ticks);
	}

	message_system.TimerSystemCommands.clear();
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

void TimerSystem::DeregisterTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const size_t index = static_cast<size_t>(std::get<struct DeregisterTimer>(command).Type);

	this->TimerActive[index] = false;
}

void TimerSystem::UpdateTimerInterval(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct UpdateTimerInterval& data = std::get<struct UpdateTimerInterval>(command);

	const size_t index = static_cast<size_t>(data.Type);
	this->TimerInterval[index] = data.NewInterval;
}


void TimerSystem::GreenbullExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemoveGreenbull);
}

void TimerSystem::MilkExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemoveMilk);
}

void TimerSystem::PoisonTickHandler(MessageSystem& message_system) const noexcept
{
	const size_t index = static_cast<size_t>(PlayerSignal::PoisonTick);
	message_system.PlayerSignals[index] = 1;
}

void TimerSystem::PoisonExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemovePoison);
}

void TimerSystem::DrunkExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemoveDrunk);
}

void TimerSystem::AussieExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemoveAussie);
}

void TimerSystem::MagnetismExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemCommands.emplace_back(ModifierSystemCommandType::RemoveMagnetism);
}

void TimerSystem::AuraTickHandler(MessageSystem& message_system) const noexcept
{
	//TODO
}

