#pragma once

#include <vector>
#include <memory>

#include "raylib.h"

#include "player.hpp"
#include "assetManager.hpp"
#include "globalDataWrapper.hpp"

#include "projectiles.hpp"
#include "enemy.hpp"
#include "xp.hpp"
#include "gameText.hpp"

class Game
{
	public:
		Game(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets);
		~Game();

		void HandleEssentialInput() noexcept;
		void Draw(RenderTexture2D& canvas) const noexcept;
		void Update() noexcept;

		void Reset() noexcept;

		std::shared_ptr<AssetManager> Assets;
		std::shared_ptr<GlobalDataWrapper> GlobalData;

		std::vector<Enemy> Enemies;
		std::vector<Projectile> Projectiles;
		std::vector<Xp> Xps;
		std::vector<GameText> GameTexts;

		std::unique_ptr<Player> PlayerInstance;

		Camera2D Camera;
		Rectangle UpdateArea;

		RenderTexture2D LightingLayer;
		RenderTexture2D GameLayer;
		
		size_t CollectedXp = 0;
		
		size_t LastLMB = 0;
		bool CanLMB = true;

		size_t LastRMB = 0;
		bool CanRMB = true;

		size_t LastLayerDown = 0;
		bool CanLayerDown = true;

		size_t LastLayerUp = 0;
		bool CanLayerUp = true;

	private:
		void UpdateEnemies() noexcept;
		void UpdateProjectiles() noexcept;
		void UpdateXps() noexcept;
		void UpdateGameTexts() noexcept;

		float Accumulator = 0.0f;
		size_t LastSpawn = 0;
};
