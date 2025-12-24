#pragma once

#include "settingsManager.hpp"
#include "assetManager.hpp"
#include "gameState.hpp"

namespace GameInputSystem
{
	void HandleTickedInput(GameState& game_state, const SettingsManager& settings, const AssetManager& assets) noexcept;
	void HandleLeftClick(GameState& game_state, const AssetManager& assets) noexcept;
	void HandleRightClick(GameState& game_state, const AssetManager& assets) noexcept;
}
