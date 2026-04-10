/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <cstddef>
#include <array>

#include "raylib.h"
#include "modifierSystem.hpp"
#include "messageSystem.hpp"
#include "timerSystem.hpp"

class CameraSystem
{
	public:
		CameraSystem() = default;
		~CameraSystem() = default;

		Camera2D GetCamera() const noexcept;
		Rectangle GetUpdateArea() const noexcept;

		void Update(
				MessageSystem& message_system, const size_t ticks, const Vector2 player_centre,
				const ModifierSystem& modifier_system, const TimerSystem& timer_system
				) noexcept;
		void Reset(const Vector2 player_centre) noexcept;

	private:
		void ExecuteCommands(MessageSystem& message_system) noexcept;
		void PollSignals(MessageSystem& message_system) noexcept;

		void SlideCameraHandler(MessageSystem& message_system, const CameraSystemCommand& command) noexcept;
		void ReleaseCameraHandler(MessageSystem& message_system, const CameraSystemCommand& command) noexcept;

		static constexpr std::array<void(CameraSystem::*)(MessageSystem&, const CameraSystemCommand&) noexcept, 2> CommandHandlers = {
			&CameraSystem::SlideCameraHandler,
			&CameraSystem::ReleaseCameraHandler
		};


		void SlideCameraExpireHandler() noexcept;
		void ReleaseCameraExpireHandler() noexcept;

		static constexpr std::array<void(CameraSystem::*)() noexcept, static_cast<size_t>(CameraSystemSignal::COUNT)> SignalHandlers = {
			&CameraSystem::SlideCameraExpireHandler,
			&CameraSystem::ReleaseCameraExpireHandler
		};

		Camera2D Camera;
		Rectangle UpdateArea;

		size_t SlideExpire;
		Vector2 SlideOffset;
		bool Sliding;
		bool Release;
};
