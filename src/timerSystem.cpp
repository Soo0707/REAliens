/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "timerSystem.hpp"

#include <cstddef>

#include "timers.hpp"
#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"

void TimerSystem::Update(MessageSystem& message_system, const size_t ticks) noexcept
{
	this->ExecuteCommands(message_system, ticks);
	
	this->UpdateTimers(message_system, ticks);
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
		if (this->TimerActive[i] && ticks >= this->Timers[i])
		{
			auto handler = this->TimeoutHandlers[i];

			(this->*handler)(message_system);

			const bool timer_recurring = this->TimerRecurring[i];
			const uint32_t timer_interval = this->TimerInterval[i];

			if (timer_recurring)
				this->Timers[i] = ticks + timer_interval;
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

	this->TimerInterval[static_cast<size_t>(Timer::LMB)] = static_cast<uint32_t>(150);
	this->TimerActive[static_cast<size_t>(Timer::LMB)] = true;

	this->TimerInterval[static_cast<size_t>(Timer::RMB)] = static_cast<uint32_t>(450);
	this->TimerActive[static_cast<size_t>(Timer::RMB)] = true;

	this->TimerInterval[static_cast<size_t>(Timer::Slide)] = static_cast<uint32_t>(TICK_RATE);
	this->TimerActive[static_cast<size_t>(Timer::Slide)] = true;

	this->TimerInterval[static_cast<size_t>(Timer::UseItem)] = static_cast<uint32_t>(TICK_RATE);
	this->TimerActive[static_cast<size_t>(Timer::UseItem)] = true;

	this->TimerInterval[static_cast<size_t>(Timer::UpdateDuration)] = static_cast<uint32_t>(TICK_RATE);
	this->TimerActive[static_cast<size_t>(Timer::UpdateDuration)] = true;
	this->TimerRecurring[static_cast<size_t>(Timer::UpdateDuration)] = true;

	this->TimerInterval[static_cast<size_t>(Timer::UpdateMetrics)] = static_cast<uint32_t>(TICK_RATE / 2);
	this->TimerActive[static_cast<size_t>(Timer::UpdateMetrics)] = true;
	this->TimerRecurring[static_cast<size_t>(Timer::UpdateMetrics)] = true;
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
	this->Timers[index] = ticks + this->TimerInterval[index];
	this->TimerRecurring[index] = data.Recurring;
}

void TimerSystem::DisableTimer(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const size_t index = static_cast<size_t>(std::get<struct DisableTimer>(command).Type);

	this->TimerActive[index] = false;
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

void TimerSystem::TriggerNow(const TimerSystemCommand& command, const size_t ticks) noexcept
{
	const struct TriggerNow& data = std::get<struct TriggerNow>(command);

	const size_t index = static_cast<size_t>(data.Type);

	this->Timers[index] = ticks;
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
	message_system.PlayerCommands.emplace_back(std::in_place_type<struct DamagePlayer>, 2.0f);
}

void TimerSystem::PoisonExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemovePoison)]++;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct DisableTimer>, Timer::PoisonTick);
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

void TimerSystem::TariffsExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveTariffs)]++;
}

void TimerSystem::WeaknessExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveWeakness)]++;
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
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct DisableTimer>, Timer::EmitLocationParticles);

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

void TimerSystem::LMBHandler(MessageSystem& message_system) const noexcept
{
	message_system.GameSignals[static_cast<size_t>(GameSignal::EnableLMB)]++;
}

void TimerSystem::RMBHandler(MessageSystem& message_system) const noexcept
{
	message_system.GameSignals[static_cast<size_t>(GameSignal::EnableRMB)]++;
}

void TimerSystem::SlideHandler(MessageSystem& message_system) const noexcept
{
	message_system.GameSignals[static_cast<size_t>(GameSignal::EnableSlide)]++;
}

void TimerSystem::UseItemHandler(MessageSystem& message_system) const noexcept
{
	message_system.GameSignals[static_cast<size_t>(GameSignal::EnableUseItem)]++;
}

void TimerSystem::UpdateDurationHandler(MessageSystem& message_system) const noexcept
{
	message_system.GameSignals[static_cast<size_t>(GameSignal::UpdateDuration)]++;
}

void TimerSystem::SlideCameraExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.CameraSystemSignals[static_cast<size_t>(CameraSystemSignal::SlideCameraExpire)]++;
}

void TimerSystem::ReleaseCameraExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.CameraSystemSignals[static_cast<size_t>(CameraSystemSignal::ReleaseCameraExpire)]++;
}

void TimerSystem::AlcoholismExpireHandler(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::RemoveAlcoholism)]++;
}

void TimerSystem::UpdateMetricsHandler(MessageSystem& message_system) const noexcept
{
	message_system.GameSignals[static_cast<size_t>(GameSignal::UpdateMetrics)]++;
}
