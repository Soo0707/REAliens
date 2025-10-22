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

class Game
{
	public:
		Game(std::shared_ptr<GlobalDataWrapper> global_data);
		~Game();

		void HandleEssentialInput();
		void Draw(RenderTexture2D& virtual_canvas);
		void Update();

		std::shared_ptr<AssetManager> Assets;
		std::shared_ptr<GlobalDataWrapper> GlobalData;


		std::vector<Enemy> Enemies;

		std::vector<Projectile> Projectiles;
		
		std::vector<Xp> Xps;

		std::unique_ptr<Player> PlayerInstance;

	private:
		void SpawnEnemies();
		void LoopOverMap(Rectangle& m_obj);

		void DrawOverlay();


		void UpdateEnemies();
		void UpdateProjectiles();
		void UpdateXps();

		void HandleTickedInput();
		void HandleLeftClick();
		void HandleRightClick();

		size_t LastLMB = 0;
		bool CanLMB = true;

		size_t LastRMB = 0;
		bool CanRMB = true;

		float Accumulator = 0.0f;
		size_t CollectedXp = 0;

		size_t LastSpawn = 0;
		size_t SpawnTimeout = 1000;

		Camera2D Camera;
		Rectangle UpdateArea;
};
