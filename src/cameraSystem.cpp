/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "cameraSystem.hpp"

#include <cstddef>
#include <iostream>

#include "raylib.h"
#include "constants.hpp"
#include "modifiers.hpp"
#include "modifierSystem.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "timers.hpp"
#include "timerSystem.hpp"

void CameraSystem::Update(
		MessageSystem& message_system, const size_t ticks, const Vector2 player_centre,
		const ModifierSystem& modifier_system, const TimerSystem& timer_system
		) noexcept
{
	this->ExecuteCommands(message_system);
	this->PollSignals(message_system);

	if (!this->Release)
	{
		this->UpdateArea.x = player_centre.x - REFERENCE_WIDTH / 2.0f;
		this->UpdateArea.y =  player_centre.y - REFERENCE_HEIGHT / 2.0f;
		
		this->Camera.target = player_centre;
	}

	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };

	if (this->Sliding)
	{
		this->Camera.target.x += this->SlideOffset.x;
		this->Camera.target.y += this->SlideOffset.y;

		this->UpdateArea.x += this->SlideOffset.x;
		this->UpdateArea.y += this->SlideOffset.y;
		
		this->SlideOffset.x *= 0.9f;
		this->SlideOffset.y *= 0.9f;
	}

	if (modifier_system.EffectStatus(Effect::Earthquake))
	{
		const float shake_offset = static_cast<float>(GetRandomValue(-6, 6));

		this->Camera.offset.x += shake_offset;
		this->Camera.offset.y -= shake_offset;
	}

	this->Camera.rotation = modifier_system.EffectStatus(Effect::Aussie) * 180.0f;

	if (modifier_system.EffectStatus(Effect::Microscope))
		this->Camera.zoom = 2.0f;
	else
		this->Camera.zoom = 1.0f;
}

void CameraSystem::Reset(const Vector2 player_centre) noexcept
{
	this->Camera = { 0 };
	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };
	this->Camera.rotation = 0.0f;
    this->Camera.zoom = 1.0f;

	this->UpdateArea = {
		player_centre.x - (REFERENCE_WIDTH / 2.0f),
		player_centre.y - (REFERENCE_HEIGHT / 2.0f),
		static_cast<float>(REFERENCE_WIDTH),
		static_cast<float>(REFERENCE_HEIGHT)
	};

	this->Sliding = false;
	this->Release = false;
}

void CameraSystem::ExecuteCommands(MessageSystem& message_system) noexcept
{
	for (auto const& command : message_system.CameraSystemCommands)
	{
		const size_t index = command.index();
		auto handler = this->CommandHandlers[index];
		
		(this->*handler)(message_system, command);
	}

	message_system.CameraSystemCommands.clear();
}

void CameraSystem::PollSignals(MessageSystem& message_system) noexcept
{
	for (size_t i = 0, n = static_cast<size_t>(CameraSystemSignal::COUNT); i < n; i++)
	{
		if (message_system.CameraSystemSignals[i] > 0)
		{
			auto signal_handler = this->SignalHandlers[i];
			(this->*signal_handler)();
		}

		message_system.CameraSystemSignals[i] = 0;
	}
}

Rectangle CameraSystem::GetUpdateArea() const noexcept
{
	return this->UpdateArea;
}

Camera2D CameraSystem::GetCamera() const noexcept
{
	return this->Camera;
}

void CameraSystem::SlideCameraHandler(MessageSystem& message_system, const CameraSystemCommand& command) noexcept
{
	const SlideCamera& data = std::get<struct SlideCamera>(command);

	this->Sliding = true;
	this->SlideOffset = data.Offset;

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, data.Duration, false, Timer::SlideCameraExpire);
}

void CameraSystem::ReleaseCameraHandler(MessageSystem& message_system, const CameraSystemCommand& command) noexcept
{
	const ReleaseCamera& data = std::get<struct ReleaseCamera>(command);

	this->Release = true;
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct RegisterTimer>, data.Duration, false, Timer::ReleaseCameraExpire);
}

void CameraSystem::SlideCameraExpireHandler() noexcept
{
	this->Sliding = false;
}

void CameraSystem::ReleaseCameraExpireHandler() noexcept
{
	this->Release = false;
}
