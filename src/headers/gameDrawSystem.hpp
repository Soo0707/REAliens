/*
 * Copyright (C) 2026 Soo0707
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>

#include "game.hpp"
#include "assetManager.hpp"
#include "stringCache.hpp"
#include "modifierSystem.hpp"
#include "timerSystem.hpp"
#include "cameraSystem.hpp"

namespace GameDrawSystem
{
	void DrawGame(
			const Game& game, const CameraSystem& camera_system, const ModifierSystem& modifier_system,
			const AssetManager& assets
			) noexcept;

	void DrawLighting(const Game& game, const ModifierSystem& modifier_system) noexcept;

	void DrawOverlay(
			const Game& game, const TimerSystem& timer_system, const ModifierSystem& modifier_system,
			const StringCache& string_cache, const AssetManager& assets
			) noexcept;
}
