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
