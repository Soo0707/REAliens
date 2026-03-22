/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "cameraSystem.hpp"

#include "raylib.h"
#include "constants.hpp"
#include "modifiers.hpp"
#include "modifierSystem.hpp"

void CameraSystem::Update(const Vector2 player_centre, const ModifierSystem& modifier_system) noexcept
{
	this->UpdateArea.x = player_centre.x - REFERENCE_WIDTH / 2.0f;
	this->UpdateArea.y =  player_centre.y - REFERENCE_HEIGHT / 2.0f;
	
	this->Camera.target = player_centre;
	this->Camera.offset = { REFERENCE_WIDTH / 2.0f, REFERENCE_HEIGHT / 2.0f };

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
}

Rectangle CameraSystem::GetUpdateArea() const noexcept
{
	return this->UpdateArea;
}

Camera2D CameraSystem::GetCamera() const noexcept
{
	return this->Camera;
}
