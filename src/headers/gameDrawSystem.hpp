#pragma once

#include "gameState.hpp"
#include "assetManager.hpp"
#include "globalDataWrapper.hpp"
#include "raylib.h"

namespace GameDrawSystem
{
	void DrawScreenLayer(const GameState& game_state) noexcept;
	void DrawLighting(const GameState& game_state) noexcept;
	void DrawGame(const GameState& game_state, const AssetManager& assets) noexcept;

	void DrawOverlay(const GameState& game_state, const GlobalDataWrapper& global_data, const AssetManager& assets) noexcept;

	void DrawGreenbull(const GameState& game_state, const AssetManager& assets) noexcept;
	void DrawMilk(const GameState& game_state, const AssetManager& assets) noexcept;
	void DrawDrunk(const GameState& game_state, const AssetManager& assets) noexcept;
	void DrawMagnetism(const GameState& game_state, const AssetManager& assets) noexcept;
}
