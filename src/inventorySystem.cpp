/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "inventorySystem.hpp"

#include <vector>
#include <cstdint>

#include "raylib.h"

#include "inventoryItem.hpp"
#include "messageSystem.hpp"
#include "signals.hpp"

InventorySystem::InventorySystem()
{
	this->InventoryItems.reserve(16);
	this->Reset();
}

void InventorySystem::Reset() noexcept
{
	this->InventoryItems.clear();
}

void InventorySystem::Update(MessageSystem& message_system) noexcept
{
	this->PollSignals(message_system);
}

void InventorySystem::PollSignals(MessageSystem& message_system) noexcept
{
	for (size_t i = 0, n = static_cast<size_t>(InventorySystemSignal::COUNT); i < n; i++)
	{
		const size_t times = static_cast<size_t>(message_system.InventorySystemSignals[i]);

		for (size_t j = 0; j < times; j++)
		{
			auto handler = this->SignalHandlers[i];
			(this->*handler)(message_system);
		}

		message_system.InventorySystemSignals[i] = 0;
	}
}

bool InventorySystem::IsEmpty() const noexcept
{
	return this->InventoryItems.empty();
}

void InventorySystem::GrantItemHandler(MessageSystem& message_system) noexcept
{
	const InventoryItem item = static_cast<InventoryItem>(GetRandomValue(1, static_cast<int>(InventoryItem::COUNT) - 1));
	this->InventoryItems.emplace_back(item);
}

void InventorySystem::UseItemHandler(MessageSystem& message_system) noexcept
{
	if (this->IsEmpty())
		return;

	const size_t inventory_item_index = static_cast<size_t>(this->InventoryItems.back());
	this->InventoryItems.pop_back();

	auto handler = this->ItemHandlers[inventory_item_index];
	(this->*handler)(message_system);
}

void InventorySystem::UseRightsRemover(MessageSystem& message_system) const noexcept
{
	// TODO
	return;
}

void InventorySystem::UseEnemyFreeze(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyEnemyFreeze)]++;
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(10), false, Timer::EnemyFreezeExpire);
}

void InventorySystem::UseLiquor(MessageSystem& message_system) const noexcept
{
	message_system.ModifierSystemSignals[static_cast<size_t>(ModifierSystemSignal::ApplyAlcoholism)]++;
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, SECONDS_TO_TICKS(30), false, Timer::AlcoholismExpire);
}

void InventorySystem::UseTurret(MessageSystem& message_system) const noexcept
{
	message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::SpawnTurret)]++;
}
