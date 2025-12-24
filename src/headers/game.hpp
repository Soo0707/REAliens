#pragma once

#include <memory>

#include "raylib.h"

#include "assetManager.hpp"
#include "settingsManager.hpp"
#include "globalDataWrapper.hpp"
#include "gameState.hpp"

class Game
{
	public:
		Game( std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets,
			std::shared_ptr<SettingsManager> settings, std::shared_ptr<GameState> game_state);
		~Game();

		void Draw(RenderTexture2D& canvas) const noexcept;
		void TickedUpdate() noexcept;
		void Update() noexcept;

		void Reset() noexcept;

		std::shared_ptr<AssetManager> Assets;
		std::shared_ptr<SettingsManager> Settings;
		std::shared_ptr<GlobalDataWrapper> GlobalData;
		std::shared_ptr<GameState> m_GameState;

		RenderTexture2D LightingLayer;
		RenderTexture2D GameLayer;
		
	private:
		float Accumulator = 0.0f;
};
