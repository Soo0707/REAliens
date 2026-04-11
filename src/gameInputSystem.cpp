/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gameInputSystem.hpp"

#include <variant>
#include <cstddef>

#include "raylib.h"
#include "raymath.h"
#include "settingsManager.hpp"

#include "signals.hpp"
#include "commands.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"
#include "player.hpp"
#include "cameraSystem.hpp"

void GameInputSystem::HandleShift(MessageSystem& message_system, const Vector2 player_direction) noexcept
{
	message_system.PlayerSignals[static_cast<size_t>(PlayerSignal::ApplySlide)]++;

	message_system.TimerSystemCommands.emplace_back(
			std::in_place_type<struct RegisterTimer>, TICK_RATE / 6,
			false, Timer::PlayerSlideExpire
			);

	message_system.CameraSystemCommands.emplace_back(
			std::in_place_type<struct SlideCamera>, Vector2Scale(player_direction, -10.0f), TICK_RATE / 2
			);

	message_system.CameraSystemCommands.emplace_back(std::in_place_type<struct ReleaseCamera>, TICK_RATE / 4);
	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, false, Timer::Slide);
}

void GameInputSystem::HandleLeftClick(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const Vector2 player_centre, const Camera2D camera
		) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
	const Vector2 centre_direction = Vector2Subtract(mouse_pos, player_centre);
	const float spread_angle = modifier_system.GetAttribute(Attribute::BuckshotSpread);
	const int buckshot = static_cast<int>((modifier_system.GetAttribute(Attribute::Buckshot) - 1) / 2);
	const float speed = modifier_system.GetAttribute(Attribute::BulletSpeed);

	for (int i = -buckshot; i <= buckshot; i++)
	{
		if (i == 0)
		{
			message_system.ProjectileSystemCommands.emplace_back(
					std::in_place_type<struct CreateProjectile>, ProjectileType::Bullet, centre_direction,
					player_centre.x, player_centre.y, speed
					);
			continue;
		}

		const Vector2 direction = Vector2Rotate(centre_direction, spread_angle * i);

		message_system.ProjectileSystemCommands.emplace_back(
				std::in_place_type<struct CreateProjectile>, ProjectileType::Bullet, direction,
				player_centre.x, player_centre.y, speed
				);
	}

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, false, Timer::LMB);
}

void GameInputSystem::HandleRightClick(
		MessageSystem& message_system, const ModifierSystem& modifier_system,
		const Vector2 player_centre, const Camera2D camera
		) noexcept
{
	const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
	const Vector2 direction = Vector2Subtract(mouse_pos, player_centre);
	const float speed = modifier_system.GetAttribute(Attribute::LazerSpeed);

	message_system.ProjectileSystemCommands.emplace_back(
			std::in_place_type<struct CreateProjectile>, ProjectileType::Lazer, direction,
			player_centre.x, player_centre.y, speed
			);

	message_system.TimerSystemCommands.emplace_back(std::in_place_type<struct EnableTimer>, false, Timer::RMB);
}
