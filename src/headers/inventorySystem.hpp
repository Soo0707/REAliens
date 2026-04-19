/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <vector>
#include <array>

#include "signals.hpp"
#include "messageSystem.hpp"
#include "inventoryItem.hpp"

class InventorySystem
{
	public:
		InventorySystem();
		~InventorySystem() = default;

		void Update(MessageSystem& message_system) noexcept;
		void Reset() noexcept;
		bool IsEmpty() const noexcept;

	private:
		void PollSignals(MessageSystem& message_system) noexcept;

		void GrantItemHandler(MessageSystem& message_system) noexcept;
		void UseItemHandler(MessageSystem& message_system) noexcept;

		using SignalHandler = void(InventorySystem::*)(MessageSystem&) noexcept;

		static constexpr std::array<SignalHandler, static_cast<size_t>(InventorySystemSignal::COUNT)> SignalHandlers = {
			&InventorySystem::GrantItemHandler,
			&InventorySystem::UseItemHandler
		};


		void UseBomb(MessageSystem& message_system) const noexcept;
		void UseFreeze(MessageSystem& message_system) const noexcept;
		void UseLiquor(MessageSystem& message_system) const noexcept;
		void UseTurret(MessageSystem& message_system) const noexcept;

		using ItemHandler = void(InventorySystem::*)(MessageSystem&) const noexcept;

		static constexpr std::array<ItemHandler, static_cast<size_t>(InventoryItem::COUNT)> ItemHandlers = {
			&InventorySystem::UseBomb,
			&InventorySystem::UseFreeze,
			&InventorySystem::UseLiquor,
			&InventorySystem::UseTurret
		};

		std::vector<InventoryItem> InventoryItems;
};

