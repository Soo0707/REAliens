/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "raylib.h"
#include "modifierSystem.hpp"

class CameraSystem
{
	public:
		CameraSystem() = default;
		~CameraSystem() = default;

		Camera2D GetCamera() const noexcept;
		Rectangle GetUpdateArea() const noexcept;

		void Update(const Vector2 player_centre, const ModifierSystem& modifier_system) noexcept;
		void Reset(const Vector2 player_centre) noexcept;

	private:
		Camera2D Camera;
		Rectangle UpdateArea;
};
