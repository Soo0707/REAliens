/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>

#include "settingsManager.hpp"

#include "raylib.h"

#include "game.hpp"
#include "messageSystem.hpp"
#include "modifierSystem.hpp"

namespace GameInputSystem
{
	void HandleTickedInput(
			Game& game, MessageSystem& message_system, const ModifierSystem& modifier_system, 
			const SettingsManager& settings
			) noexcept;

	void HandleLeftClick(MessageSystem& message_system, const ModifierSystem& modifier_system, const Vector2 player_centre, const Camera2D camera) noexcept;
	void HandleRightClick(MessageSystem& message_system, const ModifierSystem& modifier_system, const Vector2 player_centre, const Camera2D camera) noexcept;
}
